#include<bits/stdc++.h>
using namespace std;
map<string, int>reg, var, cnt; // reg-> register values, var-> variable names and values
map<int, string>command; // stores line number and command in the line
int cur = 0, num = -1; // cur-> no. of lines, num-> no. of cycles
map<string, int>res_reg, res_wrd; // res_reg -> names of reserved registers, res_wrd-> names of reserved words
map<int, char>hexa;  // stores hexadecimal char for 0-15
vector<vector<int> >memory(1024, vector<int>(1024));
vector<int>buffer(1024);
int cur_store = -1, row_delay, col_delay, row_up = 0;
ofstream outfile("output.txt");

vector<string>v;
string com;
string eraseTrail(string s)  // remove trailing whitespaces from string
{
	int low = -1, high = s.size();
	for (int i = 0; i < s.size(); ++i)
	{
		if (s[i] != ' ' && s[i] != '\t')
			break;
		low = i;
	}
	for (int i = s.size() - 1; i >= 0; --i)
	{
		if (s[i] != ' ' && s[i] != '\t')
			break;
		high = i;
	}
	return s.substr(low + 1, high - low - 1);
}
void isReg(string s, int line)  // checks if given string is a reserved register or not
{
	if (res_reg[s] == 0)
	{
		cout << "Invalid Register Name, error in instruction number: " << line;
		exit(0);
	}
}
void isVar(string s, int line)  // checks if given string is reserved keyword or not
{
	if (res_wrd[s] == 1)
	{
		cout << "The variable name is a reserved keyword, error in instruction number: " << line;
		exit(0);
	}

}
int str2int(string s, int line)  // converts string to int
{
	int ans = 0, i = 0;
	if (s[0] == '-')
	{
		i = 1;
		if (s.size() == 1)
		{
			cout << "Expected an integer, error in instruction number: " << line;
			exit(0);
		}
	}
	for (; i < s.size(); ++i)
	{
		int val = (s[i] - '0');
		if (val < 0 || val>9)
		{
			cout << "Expected an integer, error in instruction number: " << line;
			exit(0);
		}
		ans *= 10;
		ans += val;
	}
	if (s[0] == '-') {
		return -ans;
	}
	return ans;
}
vector<string> split(string s)  // used to split a string using the character delim
{
	string cur = "";
	s += ',';
	vector<string>ans;
	for (int i = 0; i < s.size(); ++i)
	{
		if (s[i] == ',')
		{
			ans.push_back(cur);
			cur = "";
		}
		else
			cur += s[i];
	}
	return ans;
}
void get()  // for output
{
	outfile << "Number of clock cycles: " << num << "\n";
	outfile << "Number of row buffer updates: " << row_up;
	cout << "Number of clock cycles: " << num << "\n";
	cout << "Number of row buffer updates: " << row_up;

}
void add(int line, int num)
{
	if (v.size() != 3)
	{
		cout << "add command needs 3 arguments, error in instruction number: " << line;
		exit(0);
	}
	isReg(v[0], line); isReg(v[1], line); isReg(v[2], line);
	reg[v[0]] = reg[v[1]] + reg[v[2]];
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n";
	outfile << "Modified Register: " << v[0] << " new value is: " << reg[v[0]] << "\n" << "\n";
}
void addi(int line, int num)
{
	if (v.size() != 3)
	{
		cout << "addi command needs 3 arguments, error in instruction number: " << line;
		exit(0);
	}
	isReg(v[0], line); isReg(v[1], line);
	int x = str2int(v[2], line);
	reg[v[0]] = reg[v[1]] + x;
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n";
	outfile << "Modified Register: " << v[0] << " new value is: " << reg[v[0]] << "\n" << "\n";
}
void sub(int line, int num)
{
	if (v.size() != 3)
	{
		cout << "sub command needs 3 arguments, error in instruction number: " << line;
		exit(0);
	}
	isReg(v[0], line); isReg(v[1], line); isReg(v[2], line);
	reg[v[0]] = reg[v[1]] - reg[v[2]];
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n";
	outfile << "Modified Register: " << v[0] << " new value is: " << reg[v[0]] << "\n" << "\n";
}
void mul(int line, int num)
{
	if (v.size() != 3)
	{
		cout << "mul command needs 3 arguments, error in instruction number: " << line;
		exit(0);
	}
	isReg(v[0], line); isReg(v[1], line); isReg(v[2], line);
	reg[v[0]] = reg[v[1]] * reg[v[2]];
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n";
	outfile << "Modified Register: " << v[0] << " new value is: " << reg[v[0]] << "\n" << "\n";
}
int beq(int line, int num)
{
	if (v.size() != 3)
	{
		cout << "beq command needs 3 arguments, error in instruction number: " << line;
		exit(0);
	}
	isReg(v[0], line); isReg(v[1], line);
	if (v[2][0] == '-')
	{
		cout << "3rd argument of beq cannot be negative, error in instruction number: " << line;
		exit(0);
	}
	int x = str2int(v[2], line);
	if (x % 4 != 0 || x >= pow(2, 19)) {
		cout << "Instruction doesn't exist, error in instruction number: " << line;
		exit(0);
	}
	x = x / 4;
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n" << "\n";
	if (reg[v[0]] == reg[v[1]])
	{
		if (x < 0 || x >= cur)
		{
			cout << "Instruction doesn't exist, error in instruction number: " << line;
			exit(0);
		}
		return x + 1;
	}
	return line;
}
void slt(int line, int num)
{
	if (v.size() != 3)
	{
		cout << "slt command needs 3 arguments, error in instruction number: " << line;
		exit(0);
	}
	isReg(v[0], line); isReg(v[1], line); isReg(v[2], line);
	if (reg[v[1]] < reg[v[2]])
		reg[v[0]] = 1;
	else
		reg[v[0]] = 0;
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n";
	outfile << "Modified Register: " << v[0] << " new value is: " << reg[v[0]] << "\n" << "\n";
}
int bne(int line, int num)
{
	if (v.size() != 3)
	{
		cout << "bne command needs 3 arguments, error in instruction number: " << line;
		exit(0);
	}
	if (v[2][0] == '-')
	{
		cout << "3rd argument of bne cannot be negative, error in instruction number: " << line;
		exit(0);
	}
	isReg(v[0], line); isReg(v[1], line);
	int x = str2int(v[2], line);
	if (x % 4 != 0 || x >= pow(2, 19)) {
		cout << "Instruction doesn't exist, error in instruction number: " << line;
		exit(0);
	}
	x = x / 4;
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n" << "\n";
	if (reg[v[0]] != reg[v[1]])
	{
		if (x < 0 || x >= cur)
		{
			cout << "Instruction doesn't exist, error in instruction number: " << line;
			exit(0);
		}
		return x + 1;
	}
	return line;
}
int j(int line, int num)
{
	if (v.size() != 1)
	{
		cout << "j command needs 1 argument, error in instruction number: " << line;
		exit(0);
	}
	if (v[0][0] == '-')
	{
		cout << "Argument of j cannot be negative, error in instruction number: " << line;
		exit(0);
	}
	int x = str2int(v[0], line);
	if (x % 4 != 0 || x >= pow(2, 19)) {
		cout << "Instruction doesn't exist, error in instruction number: " << line;
		exit(0);
	}
	x = x / 4;
	if (x < 0 || x >= cur)
	{
		cout << "Instruction doesn't exist, error in instruction number: " << line;
		exit(0);
	}
	outfile << "Clock Cycle " << num << "\n";
	outfile << "Executed Instruction: " << com << "\n" << "\n";
	return x + 1;
}

void execute(int line)
{
	line++;
	if (line > cur)
	{
		get();
		exit(0);
	}
	com = command[line - 1];
	if (com == "")
	{
		execute(line);
		exit(0);
	}
	int space = com.find(" ");
	string todo = com.substr(0, space), args = com.substr(space + 1, com.size() - space - 1);
	v = split(args);
	for (int i = 0; i < v.size(); ++i)
		v[i] = eraseTrail(v[i]);
	num++;
	if (num == 0)
	{
		if (v[0] == "main:")
			execute(line);
		else
		{
			cout << "Expected main label, error in instruction number: " << line;
			exit(0);
		}
	}
	if (line == cur)
	{
		if (v[0] == "exit:")
		{
			num--;
			execute(line);
		}
		else
		{
			cout << "Expected exit label, error in instruction number: " << line;
			exit(0);
		}
	}
	if (res_wrd[todo] == 0)
	{
		cout << "Invalid instruction, error in instruction number: " << line;
		exit(0);
	}
	cnt[todo]++;
	if (todo == "add")
		add(line, num);
	else if (todo == "addi")
		addi(line, num);
	else if (todo == "sub")
		sub(line, num);
	else if (todo == "mul")
		mul(line, num);
	else if (todo == "beq")
		line = beq(line, num);
	else if (todo == "bne")
		line = bne(line, num);
	else if (todo == "j")
		line = j(line, num);
	else if (todo == "slt")
		slt(line, num);
	else if (todo == "lw")
	{
		if (v.size() != 2)
		{
			cout << "lw command needs 2 arguments, error in instruction number: " << line;
			exit(0);
		}
		isReg(v[0], line); isVar(v[1], line);
		if (res_reg[v[1]] == 1)
		{
			cout << "Variable name is a register's name, error in instruction number: " << line;
			exit(0);
		}
		int cur = 0, i = 0;
		bool w = false;
		if (v[1][0] == '-')
		{
			w = true;
			++i;
		}
		for (; i < v[1].size(); ++i)
		{
			int zz = v[1][i] - '0';
			if (zz > 9 || zz < 0)
				break;
			cur *= 10;
			cur += zz;
		}
		cur = w ? -cur : cur;
		if (i != v[1].size())
		{
			if (v[1][i] != '(' || v[1][v[1].size() - 1] != ')')
			{
				cout << "Invalid argument to lw, error in instruction number: " << line;
				exit(0);
			}
			++i;
			string str = v[1].substr(i, v[1].size() - 1 - i);
			isReg(str, line);
			cur += reg[str];
		}
		cur += pow(2, 19);
		if (cur % 4 != 0 || cur < pow(2, 19) || cur >= pow(2, 20))
		{
			cout << "Invalid Memory access by lw, error in instruction number: " << line;
			exit(0);
		}
		int row_num = cur / 1024, col_num = cur % 1024;
		int last = cur_store, hj = num;
		outfile << "Clock Cycle " << hj << "\n";
		outfile << "Executed Instruction: " << com << "\n";
		outfile << "Activity on DRAM: DRAM request issued" << "\n" << "\n";
		++num;
		bool gh = true;
		string curreg = v[0];
		if (cur_store != row_num)
		{
			if (cur_store != -1)
			{
				if (gh)
				{
					for (int jk = 0; jk < row_delay; ++jk)
					{
						com = command[line];
						if (com == "exit:")
						{
							gh = false;
							break;
						}
						space = com.find(" ");
						todo = com.substr(0, space), args = com.substr(space + 1, com.size() - space - 1);
						v = split(args);
						for (int i = 0; i < v.size(); ++i)
							v[i] = eraseTrail(v[i]);
						for (int i = 0; i < v.size(); ++i)
						{
							if (v[i] == curreg)
							{
								gh = false;
								break;
							}
						}
						if (!gh)
							break;
						if (todo == "lw" || todo == "sw")
						{
							gh = false;
							break;
						}
						++line;
						if (todo == "add")
							add(line, num + jk);
						else if (todo == "addi")
							addi(line, num + jk);
						else if (todo == "sub")
							sub(line, num + jk);
						else if (todo == "mul")
							mul(line, num + jk);
						else if (todo == "beq")
							line = beq(line, num + jk);
						else if (todo == "bne")
							line = bne(line, num + jk);
						else if (todo == "j")
							line = j(line, num + jk);
						else if (todo == "slt")
							slt(line, num + jk);
						else
						{
							cout << "Invalid instruction, error in instruction number: " << line;
							exit(0);
						}
					}
				}

				memory[cur_store] = buffer;
				outfile << "Clock Cycle " << num << "-" << num + row_delay - 1 << "\n";
				outfile << "Executed Instruction: " << com << "\n";
				outfile << "Activity on DRAM: Writing from buffer to DRAM" << "\n" << "\n";
				num += row_delay;
			}
			if (gh)
			{
				for (int jk = 0; jk < row_delay; ++jk)
				{
					com = command[line];
					if (com == "exit:")
					{
						gh = false;
						break;
					}
					space = com.find(" ");
					todo = com.substr(0, space), args = com.substr(space + 1, com.size() - space - 1);
					v = split(args);
					for (int i = 0; i < v.size(); ++i)
						v[i] = eraseTrail(v[i]);
					for (int i = 0; i < v.size(); ++i)
					{
						if (v[i] == curreg)
						{
							gh = false;
							break;
						}
					}
					if (!gh)
						break;
					if (todo == "lw" || todo == "sw")
					{
						gh = false;
						break;
					}
					++line;
					if (todo == "add")
						add(line, num + jk);
					else if (todo == "addi")
						addi(line, num + jk);
					else if (todo == "sub")
						sub(line, num + jk);
					else if (todo == "mul")
						mul(line, num + jk);
					else if (todo == "beq")
						line = beq(line, num + jk);
					else if (todo == "bne")
						line = bne(line, num + jk);
					else if (todo == "j")
						line = j(line, num + jk);
					else if (todo == "slt")
						slt(line, num + jk);
					else
					{
						cout << "Invalid instruction, error in instruction number: " << line;
						exit(0);
					}
				}
			}

			outfile << "Clock Cycle " << num << "-" << num + row_delay - 1 << "\n";
			outfile << "Executed Instruction: " << com << "\n";
			outfile << "Activity on DRAM: Writing from DRAM to buffer: last row:: " << last << " cur row:: " << row_num << "\n" << "\n";
			cur_store = row_num;
			buffer = memory[cur_store];
			++row_up;
			num += row_delay;
		}
		if (gh)
		{
			for (int jk = 0; jk < col_delay; ++jk)
			{
				com = command[line];
				if (com == "exit:")
				{
					gh = false;
					break;
				}
				space = com.find(" ");
				todo = com.substr(0, space), args = com.substr(space + 1, com.size() - space - 1);
				v = split(args);
				for (int i = 0; i < v.size(); ++i)
					v[i] = eraseTrail(v[i]);
				for (int i = 0; i < v.size(); ++i)
				{
					if (v[i] == curreg)
					{
						gh = false;
						break;
					}
				}
				if (!gh)
					break;
				if (todo == "lw" || todo == "sw")
				{
					gh = false;
					break;
				}
				++line;
				if (todo == "add")
					add(line, num + jk);
				else if (todo == "addi")
					addi(line, num + jk);
				else if (todo == "sub")
					sub(line, num + jk);
				else if (todo == "mul")
					mul(line, num + jk);
				else if (todo == "beq")
					line = beq(line, num + jk);
				else if (todo == "bne")
					line = bne(line, num + jk);
				else if (todo == "j")
					line = j(line, num + jk);
				else if (todo == "slt")
					slt(line, num + jk);
				else
				{
					cout << "Invalid instruction, error in instruction number: " << line;
					exit(0);
				}
			}
		}
		reg[curreg] = buffer[col_num];
		outfile << "Clock Cycle " << num << "-" << num + col_delay - 1 << "\n";
		outfile << "Executed Instruction: " << com << "\n";
		outfile << "Modified Register: " << curreg << " new value is: " << reg[curreg] << "\n" << "\n";
		num += col_delay - 1;
	}
	else
	{
		if (v.size() != 2)
		{
			cout << "sw command needs 2 arguments, error in instruction number: " << line;
			exit(0);
		}
		isReg(v[0], line); isVar(v[1], line);
		if (res_reg[v[1]] == 1)
		{
			cout << "Variable name is a register's name, error in instruction number: " << line;
			exit(0);
		}
		int cur = 0, i = 0;
		bool w = false;
		if (v[1][0] == '-')
		{
			w = true;
			++i;
		}
		for (; i < v[1].size(); ++i)
		{
			int zz = v[1][i] - '0';
			if (zz > 9 || zz < 0)
				break;
			cur *= 10;
			cur += zz;
		}
		cur = w ? -cur : cur;
		if (i != v[1].size())
		{
			if (v[1][i] != '(' || v[1][v[1].size() - 1] != ')')
			{
				cout << "Invalid argument to sw, error in instruction number: " << line;
				exit(0);
			}
			++i;
			string str = v[1].substr(i, v[1].size() - 1 - i);
			isReg(str, line);
			cur += reg[str];
		}
		cur += pow(2, 19);
		if (cur % 4 != 0 || cur < pow(2, 19) || cur >= pow(2, 20))
		{
			cout << "Invalid Memory access by sw, error in instruction number: " << line;
			exit(0);
		}
		int row_num = cur / 1024, col_num = cur % 1024;
		int last = cur_store, hj = num, ss = reg[v[0]];
		outfile << "Clock Cycle " << hj << "\n";
		outfile << "Executed Instruction: " << com << "\n";
		outfile << "Activity on DRAM: DRAM request issued" << "\n" << "\n";
		++num;
		string com1 = com;
		bool gh = true;
		if (cur_store != row_num)
		{
			if (cur_store != -1)
			{
				if (gh)
				{
					for (int jk = 0; jk < row_delay; ++jk)
					{
						com = command[line];
						if (com == "exit:")
						{
							gh = false;
							break;
						}
						space = com.find(" ");
						todo = com.substr(0, space), args = com.substr(space + 1, com.size() - space - 1);
						v = split(args);
						for (int i = 0; i < v.size(); ++i)
							v[i] = eraseTrail(v[i]);
						if (todo == "lw" || todo == "sw")
						{
							gh = false;
							break;
						}
						++line;
						if (todo == "add")
							add(line, num + jk);
						else if (todo == "addi")
							addi(line, num + jk);
						else if (todo == "sub")
							sub(line, num + jk);
						else if (todo == "mul")
							mul(line, num + jk);
						else if (todo == "beq")
							line = beq(line, num + jk);
						else if (todo == "bne")
							line = bne(line, num + jk);
						else if (todo == "j")
							line = j(line, num + jk);
						else if (todo == "slt")
							slt(line, num + jk);
						else
						{
							cout << "Invalid instruction, error in instruction number: " << line;
							exit(0);
						}
					}
				}
				memory[cur_store] = buffer;
				outfile << "Clock Cycle " << num << "-" << num + row_delay - 1 << "\n";
				outfile << "Executed Instruction: " << com1 << "\n";
				outfile << "Activity on DRAM: Writing from buffer to DRAM" << "\n" << "\n";
				num += row_delay;
			}
			if (gh)
			{
				for (int jk = 0; jk < row_delay; ++jk)
				{
					com = command[line];
					if (com == "exit:")
					{
						gh = false;
						break;
					}
					space = com.find(" ");
					todo = com.substr(0, space), args = com.substr(space + 1, com.size() - space - 1);
					v = split(args);
					for (int i = 0; i < v.size(); ++i)
						v[i] = eraseTrail(v[i]);
					if (todo == "lw" || todo == "sw")
					{
						gh = false;
						break;
					}
					++line;
					if (todo == "add")
						add(line, num + jk);
					else if (todo == "addi")
						addi(line, num + jk);
					else if (todo == "sub")
						sub(line, num + jk);
					else if (todo == "mul")
						mul(line, num + jk);
					else if (todo == "beq")
						line = beq(line, num + jk);
					else if (todo == "bne")
						line = bne(line, num + jk);
					else if (todo == "j")
						line = j(line, num + jk);
					else if (todo == "slt")
						slt(line, num + jk);
					else
					{
						cout << "Invalid instruction, error in instruction number: " << line;
						exit(0);
					}
				}
			}
			outfile << "Clock Cycle " << num << "-" << num + row_delay - 1 << "\n";
			outfile << "Executed Instruction: " << com1 << "\n";
			outfile << "Activity on DRAM: Writing from DRAM to buffer: last row:: " << last << " cur row:: " << row_num << "\n" << "\n";
			buffer = memory[row_num];
			++row_up;
			cur_store = row_num;
			num += row_delay;
		}
		if (gh)
		{
			for (int jk = 0; jk < col_delay; ++jk)
			{
				com = command[line];
				if (com == "exit:")
				{
					gh = false;
					break;
				}
				space = com.find(" ");
				todo = com.substr(0, space), args = com.substr(space + 1, com.size() - space - 1);
				v = split(args);
				for (int i = 0; i < v.size(); ++i)
					v[i] = eraseTrail(v[i]);
				if (todo == "lw" || todo == "sw")
				{
					gh = false;
					break;
				}
				++line;
				if (todo == "add")
					add(line, num + jk);
				else if (todo == "addi")
					addi(line, num + jk);
				else if (todo == "sub")
					sub(line, num + jk);
				else if (todo == "mul")
					mul(line, num + jk);
				else if (todo == "beq")
					line = beq(line, num + jk);
				else if (todo == "bne")
					line = bne(line, num + jk);
				else if (todo == "j")
					line = j(line, num + jk);
				else if (todo == "slt")
					slt(line, num + jk);
				else
				{
					cout << "Invalid instruction, error in instruction number: " << line;
					exit(0);
				}
			}
		}
		buffer[col_num] = ss;
		outfile << "Clock Cycle " << num << "-" << num + col_delay - 1 << "\n";
		outfile << "Executed Instruction: " << com1 << "\n";
		outfile << "Modified Memory location: Memory at addresses " << cur << " - " << cur + 3 << " changed to " << ss << "\n" << "\n";
		++row_up;
		num += col_delay - 1;
	}
	execute(line);

}
int main()
{
	string filename;
	cin>>filename;
	cin >> row_delay >> col_delay;
	if (row_delay <= 0 || col_delay <= 0)
	{
		cout << "Row delay and column delay must be positive";
		exit(0);
	}
	ifstream input_file(filename);
	string line;
	while (getline(input_file, line))
	{
		line = eraseTrail(line);
		if (line != "")
		{
			command[cur] = line;
			++cur;
		}
	}
	for (int i = 0; i < 32; ++i)
	{
		string w = "$r";
		if (i / 10 > 0)
			w.push_back('0' + i / 10);
		w.push_back('0' + i % 10);
		res_reg[w] = 1;
		reg[w] = 0;
	}
	res_wrd["add"] = 1; res_wrd["sub"] = 1; res_wrd["addi"] = 1; res_wrd["mul"] = 1; res_wrd["bne"] = 1;
	res_wrd["beq"] = 1; res_wrd["j"] = 1; res_wrd["slt"] = 1; res_wrd["lw"] = 1; res_wrd["sw"] = 1;
	reg["$r30"] = pow(2, 19);
	for (int i = 0; i < 10; ++i) hexa[i] = ('0' + i);
	for (int i = 10; i < 16; ++i) hexa[i] = ('A' + (i - 10));
	int zzz = (1 << 17), yyy = cur - 2;
	if (yyy > zzz)
	{
		cout << "Memory Overflow";
		exit(0);
	}
	execute(0);
	outfile.close();

}
