#include "file.h"
namespace Mips{

	bool str_cut(string& const sentence)
	{
		if (sentence.empty() || (sentence[0] == '/'&&sentence[1] == '/'))
			return false;										//if the whole line is just a commment
		string sentence_cut;
		for (unsigned i = 0; i < sentence.size(); i++)
		{
			if ((i < sentence.size() - 1) && sentence[i] == '/' && sentence[i + 1] == '/')
				break;
			sentence_cut.push_back(sentence[i]);
		}
		auto endpos = sentence_cut.find_last_not_of(" #\t\f\v");
		if (endpos == string::npos)								//if this is a blank line
			return false;
		sentence_cut.erase(endpos + 1);

		auto startpos = sentence_cut.find_first_not_of(" ");
		string sentence_cut2 = sentence_cut.substr(startpos);		//to deal with the commend in the middle of line
		sentence = sentence_cut2;
		return true;
	}

	Code::Code(ifstream &is)
	{
		string sentence;
		while (getline(is, sentence))
		{
			if (str_cut(sentence) == false)
				continue;

			while (sentence.find('\\') == (sentence.size() - 1)) //deal with '\'s
			{
				sentence.pop_back();								//pop out the '\'
				string next_sentence;
				getline(is, next_sentence);
				while (!str_cut(next_sentence))
				{
					getline(is, next_sentence);
				}
				sentence = sentence + next_sentence;
			}

			if (sentence.find(':') == (sentence.size() - 1))       //if start a newline after data district declaration
			{
				string next_sentence;
				getline(is, next_sentence);
				while (!str_cut(next_sentence))
				{
					getline(is, next_sentence);
				}
				sentence = sentence + next_sentence;
				while (sentence.find('\\') == (sentence.size() - 1))
				{
					sentence.pop_back();
					getline(is, next_sentence);
					while (!str_cut(next_sentence))
					{
						getline(is, next_sentence);
					}
					sentence = sentence + next_sentence;
				}
			}

			lines.push_back(sentence);                         //a line parser
		}
	}

	void Code::print()
	{
		for (unsigned i = 0; i < lines.size(); ++i)
			lines[i].print();
	}

	void Code::load(Text &text, Data &data, map<string, int> &labels)
	{
		for (unsigned i = 0; i < lines.size(); ++i)
		{
			try
			{
				if (lines[i].Islabeled())
				{

					string label;
					label = lines[i].pop_front();
					if (label == "#BaseAddre")
					{
						unsigned text_start = stoi(lines[i].pop_front(), nullptr, 16);	//transform string into a base 16 unsigned number
						text.setaddr(text_start);										//set it as start address of text
						if (i < lines.size())
							i++;
						//else error 
						for (; i < lines.size(); i++)
						{
							if (lines[i].Islabeled() && lines[i].front() == "#DataAddre")
							{
								i--;
								break;
							}//break when encount DataAddre
							else if (lines[i].Islabeled() && lines[i].front() == "#BaseAddre")
							{
								lines[i].pop_front();
								text_start = stoi(lines[i].pop_front(), nullptr, 16);
								text.setaddr(text_start);										//set it as start address of text
								continue;
							}
							text.Allo(lines[i].getwords(), labels);							//store the encode instruction in text
						}
					}
					if (label == "#DataAddre")
					{
						unsigned data_start = stoi(lines[i].pop_front(), nullptr, 16);	//transform string into a base 16 unsigned number
						data.setaddr(data_start);										//set it as start address of data
						if (i < lines.size())
							i++;
						//else error 
						for (; i < lines.size(); i++)
						{
							if (lines[i].Islabeled() && lines[i].front() == "#BaseAddre")
							{
								i--;
								break; //break when encount BaseAddre
							}
							if (lines[i].Islabeled() && lines[i].front() == "#DataAddre")
							{
								lines[i].pop_front();
								data_start = stoi(lines[i].pop_front(), nullptr, 16);
								data.setaddr(data_start);
								continue;
							}
							data.exec(lines[i].getwords(), labels);							//execute the instruction for data initialization or space allocation
						}
					}
				}
				//if it's a normal label
			}
			catch (exception ex)
			{
				Error(i);
			}
			//else error
		}
	}

	Line::~Line() {}

	bool Line::Islabeled()
	{
		return haslabel;
	}

	string Line::front()
	{
		return words.front();
	}

	//get front and pop it out
	string Line::pop_front()
	{
		string ret = words.front();
		words.pop_front();
		return ret;
	}

	deque<string> Line::getwords()
	{
		return words;
	}

    void Error(unsigned line_num)
    {
		string str;
		stringstream ss;
		ss << line_num;
		ss >> str;
		str = "Error in line" + str;
		QString qstr = QString::fromStdString(str);
		QMessageBox::warning(NULL, "ERROR", qstr);
    }

	Line::Line(string sentence_cut)
	{
		string word;
		//vector<string> tempwords;
		haslabel = false;
		if (sentence_cut.find(":") != string::npos)
			haslabel = true;

		//while (line >> word)
		//{
		//	tempwords.push_back(word);;
		//}

		////check ','
		//unsigned i = 1;
		//for (; i < tempwords.size() - 1; i++) //skip the first one and the last one
		//{
		//	//assert(tempwords[i].back != ',');
		//}
		////check ';'
		////assert(tempwords[i].back != ';');

		//drop all the ',' and ';'in the sentence
		for (unsigned j = 0; j < sentence_cut.size(); ++j)
		{

			if (sentence_cut[j] == ',' || sentence_cut[j] == ';' || sentence_cut[j] == ':' || sentence_cut[j] == '\\')
				sentence_cut[j] = ' ';
		}
		//construct the line and parse
		istringstream line(sentence_cut);

		//really read in
		while (line >> word)
		{
			words.push_back(word);
		}
	}

	void Line::print()
	{
		for (unsigned i = 0; i < words.size(); i++)
			cout << words[i] << " | ";
		cout << haslabel << endl;
	}

	void generate_bin(ofstream &out, const map<unsigned, string> &text_code, const map<unsigned, string> &data_code)
	{
		auto iter = data_code.end();
		iter--;
		unsigned max = iter->first;
		for (unsigned i = 0; i < max; i = i + 4)
		{
			auto t = text_code.find(i);
			auto d = data_code.find(i);
			if (t != text_code.end())
			{
				out << t->second;
				continue;
			}
			if (d != data_code.end())
			{
				out << d->second;
				continue;
			}
			out << "00000000000000000000000000000000";
		}
	}

	string code2reg(unsigned reg_val)
	{
		switch (reg_val)
		{
		case 0: return "$zero";
		case 1:	return "$at";
		case 2: return "$v0";
		case 3: return "$v1";

		case 4: return "$a0";
		case 5: return "$a1";
		case 6: return "$a2";
		case 7: return "$a3";

		case 8: return "$t0";
		case 9: return "$t1";
		case 10: return "$t2";
		case 11: return "$t3";
		case 12: return "$t4";
		case 13: return "$t5";
		case 14: return "$t6";
		case 15: return "$t7";

		case 16: return "$s0";
		case 17: return "$s1";
		case 18: return "$s2";
		case 19: return "$s3";
		case 20: return "$s4";
		case 21: return "$s5";
		case 22: return "$s6";
		case 23: return "$s7";

		case 24: return "$t8";
		case 25: return "$t9";

		case 26: return "$k0";
		case 27: return "$k1";

		case 28: return "$gp";
		case 29: return "$sp";
		case 30: return "$fp";
		case 31: return "$ra";
		default: return "";
		}
	}

	string bin2dec(string bin)
	{
		string dec;
		stringstream ss;
		unsigned long value;
		if (bin[0] == '1')
		{
			value = 0xffff - stoi(bin, nullptr, 2) + 1;
			ss << value;
			ss >> dec;
			dec = "-" + dec;
		}
		else
		{
			value = stoi(bin, nullptr, 2);
			ss << value;
			ss >> dec;
		}
		return dec;
	}

	string bin2hex(string bin)
	{
		string hexstr;
		stringstream ss;
		ss << hex << stoi(bin, nullptr, 2);
		ss >> hexstr;
		while(hexstr.size() < 4)
		{
			hexstr = "0" + hexstr;
		}
		hexstr = "0x" + hexstr;
		return hexstr;
	}

	string code2ins(const string line)
	{
		string opcode = line.substr(0, 6);
		unsigned op_val = stoi(opcode, nullptr, 2);

		string op;
		string ins;
		if (op_val == 0)	//R type
		{
			string rs, rt, rd, shamt, funct;
			rs = line.substr(6, 5);
			rt = line.substr(11, 5);
			rd = line.substr(16, 5);
			shamt = line.substr(21, 5);
			funct = line.substr(26, 6);
			unsigned rs_val = stoi(rs, nullptr, 2);
			unsigned rt_val = stoi(rt, nullptr, 2);
			unsigned rd_val = stoi(rd, nullptr, 2);
			rs = code2reg(rs_val);
			rt = code2reg(rt_val);
			rd = code2reg(rd_val);
			unsigned funct_val = stoi(funct, nullptr, 2);

			switch (funct_val)
			{
			case 0:case 2: case 3:
			{
				if (funct_val == 0) op = "sll";
				if (funct_val == 2) op = "srl";
				if (funct_val == 3) op = "sra";
				shamt = bin2dec(shamt);
				ins = op + " " + rd + ", " + rt + ", " + shamt;
				return ins;
			}
			case 4:case 6:case 7:
			{
				if (funct_val == 4) op = "sllv";
				if (funct_val == 6) op = "srlv";
				if (funct_val == 7) op = "srav";
				ins = op + " " + rd + ", " + rt + ", " + rs;
				return ins;
			}
			case 8:
			{
				ins = "jr " + rs;
				return ins;
			}
			case 9:
			{
				ins = "jalr " + rd + ", " + rs;
				return ins;
			}
			case 16:case 18:
			{
				if (funct_val == 16) op = "mfhi";
				if (funct_val == 18) op = "mflo";
				ins = op + " " + rd;
				return ins;
			}
			case 17:case 19:
			{
				if (funct_val == 17) op = "mthi";
				if (funct_val == 19) op = "mtlo";
				ins = op + " " + rs;
				return ins;
			}
			case 24:case 25:case 26:case 27:
			{
				switch (funct_val)
				{
				case 24:op = "mult"; break;
				case 25:op = "multu"; break;
				case 26:op = "div"; break;
				case 27:op = "divu"; break;
				}
				ins = op + " " + rs + ", " + rt;
				return ins;
			}
			default:
			{
				switch (funct_val)
				{
				case 32:op = "add"; break;
				case 33:op = "addu"; break;
				case 34:op = "sub"; break;
				case 35:op = "subu"; break;
				case 36:op = "and"; break;
				case 37:op = "or"; break;
				case 38:op = "xor"; break;
				case 39:op = "nor"; break;
				case 42:op = "slt"; break;
				case 43:op = "sltu"; break;
				}
				ins = op + " " + rd + ", " + rs + ", " + rt;
				return ins;
			}
			}
		}
		else if (op_val == 2 || op_val == 3) //J type
		{
			string address;
			if (op_val == 2) op = "j";
			if (op_val == 3) op = "jal";
			address = line.substr(6, 26);
			stringstream ss;
			ss << hex << stoi(address,nullptr,2);
			ss >> address;
			while (address.size() < 4)
			{
				address = "0" + address;
			}
			address = " 0x"+address;
			ins = op + address;
			return ins;
		}
		else
		{
			string rs, rt, imm;
			rs = line.substr(6, 5);
			rt = line.substr(11, 5);
			imm = line.substr(16, 16);
			unsigned rs_val = stoi(rs, nullptr, 2);
			unsigned rt_val = stoi(rt, nullptr, 2);
			rs = code2reg(rs_val);
			rt = code2reg(rt_val);
			switch (op_val){
			case 4:case 5:
			{
				if (op_val == 4) op = "beq";
				if (op_val == 5) op = "bne";
				//imm
				imm = bin2hex(imm);
				ins = op + " " + rs + ", " + rt + ", " + imm;
				return ins;
			}
			case 6:case 7:
			{
				if (op_val == 6) op = "blez";
				if (op_val == 7) op = "bgtz";
				//imm
				imm = bin2hex(imm);
				ins = op + " " + rs + ", " + imm;
				return ins;
			}
			case 8:case 9:case 10:case 11:case 12:case 13:case 14:
			{
				switch (op_val)
				{
				case 8:op = "addi"; imm = bin2dec(imm); break;
				case 9:op = "addiu"; imm = bin2dec(imm); break;
				case 10:op = "slti"; imm = bin2dec(imm); break;
				case 11:op = "sltiu"; imm = bin2dec(imm); break;
				case 12:op = "andi"; imm = bin2hex(imm); break;
				case 13:op = "ori"; imm = bin2hex(imm); break;
				case 14:op = "xori"; imm = bin2hex(imm); break;
				}
				ins = op + " " + rt + ", " + rs + ", " + imm;
				return ins;
			}
			case 15:
			{
				//imm
				imm = bin2hex(imm);
				ins = "lui " + rt + ", " + imm;
				return ins;
			}
			case 32:case 33:case 35:case 36:case 37:case 40:case 41:case 43:
			{
				switch (op_val)
				{
				case 32:op = "lb"; break;
				case 33:op = "lh"; break;
				case 35:op = "lw"; break;
				case 36:op = "lbu"; break;
				case 37:op = "lhu"; break;
				case 40:op = "sb"; break;
				case 41:op = "sh"; break;
				case 43:op = "sw"; break;
				}
				imm = bin2dec(imm);
				ins = op + " " + rt + ", " + imm + "(" + rs + ")";
				return ins;
			}
			}
		}
	}
	//only consider that BaseAddre is 0000 0000 and base address declartion is occurred once
	string dec2str(unsigned in)
	{
		stringstream ss;
		ss << in;
		string str;
		ss >> str;
		return str;
	}

	void bin2asm(ifstream &in, ofstream &out)
	{
		string bin_str;
		getline(in, bin_str);
		out << "#BaseAddre:00000000" << endl;
		map<unsigned, unsigned> labels;
		map<unsigned, unsigned> labels_back;
		unsigned linenum = 0;
		int labelnum = 0;
		for (unsigned i = 0; i < bin_str.size(); i = i + 32)
		{
			string line;
			line = bin_str.substr(i, 32);
			if (line == "00000000000000000000000000000000")
			{
				linenum++;
				continue;
			}
			string ins = code2ins(line);
			// j jal beq bne 
			int diff;
			if (ins[0] == 'j'&&ins[1] == 'a'&&ins[2] == 'l' ||ins[0]=='j'&&ins[1]==' ')	//j & jal
			{
				labelnum++;
				string address = line.substr(6, 26);
				if (address[0] == '1')
				{
					while (address.size() < 32)
						address = "1" + address;
				}
				diff = int(stoul(address, nullptr, 2));
				if (labels.find(diff) == labels.end())
				{
					labels.insert({ diff, labelnum });
					labels_back.insert({ linenum, labelnum });
				}
				else
				{
					labelnum--;
					labels_back.insert({ linenum, labels[diff] });
				}
			}
			if (ins[0] == 'b'&&ins[1] == 'e'&&ins[2] == 'q' || ins[0] == 'b'&&ins[1] == 'n'&&ins[2] == 'e' || 
				ins[0] == 'b'&&ins[1] == 'l'&&ins[2] == 'e'&&ins[3] == 'z' || ins[0] == 'b'&&ins[1] == 'g'&&ins[2] == 't'&&ins[3] == 'z')
			{
				labelnum++;
				string address = line.substr(16, 16);
				if (address[0] == '1')
				{
					while (address.size() < 32)
						address = "1" + address;
				}
				diff = int(stoul(address, nullptr, 2));
				if (labels.find(linenum + 1 + diff) == labels.end())
				{
					labels.insert({ linenum + 1 + diff, labelnum });
					labels_back.insert({ linenum, labelnum });
				}
				else
				{
					labelnum--;
					labels_back.insert({ linenum, labels[linenum + 1 + diff] });
				}
			}
			
			linenum++;
		}
		linenum = 0;
		labelnum = 0;
		stringstream ss;
		for (unsigned i = 0; i < bin_str.size(); i = i + 32)
		{
			string line;
			line = bin_str.substr(i, 32);
			string ins;
			ins = code2ins(line);
			if (line == "00000000000000000000000000000000")
			{
				ins = "add $0, $0, $0"; //do nothing
			}
			if (ins[0] == 'j'&&ins[1] == 'a'&&ins[2] == 'l')	//j & jal
			{
				labelnum = labels_back[linenum];
				ins = "jal L" + dec2str(labelnum);
			}
			if (ins[0] == 'j'&&ins[1] == ' ')
			{
				labelnum = labels_back[linenum];
				ins = "j L" + dec2str(labelnum);
			}
			if (ins[0] == 'b'&&ins[1] == 'e'&&ins[2] == 'q' || ins[0] == 'b'&&ins[1] == 'n'&&ins[2] == 'e')
			{
				labelnum = labels_back[linenum];
				auto found = ins.find_last_of(' ');
				ins = ins.substr(0, found) + " L" + dec2str(labelnum);
			}
			if (ins[0] == 'b'&&ins[1] == 'l'&&ins[2] == 'e'&&ins[3] == 'z' || ins[0] == 'b'&&ins[1] == 'g'&&ins[2] == 't'&&ins[3] == 'z')
			{
				labelnum = labels_back[linenum];
				auto found = ins.find_last_of(' ');
				ins = ins.substr(0, found) + " L" + dec2str(labelnum);
			}
			if (labels.find(linenum) != labels.end())
				out << "L" << labels[linenum] << " : "<< "\t" << ins << endl;
			else
				out << "\t\t" << ins << endl;
			linenum++;
		}
	}
	//only consider that BaseAddre is 0000 0000 and base address declartion is occurred once
	void bin2coe(ifstream &in, ofstream &out)
	{
		string bin_str;
		getline(in, bin_str);
		unsigned cnt = 0;
		string hex_str;
		for (unsigned i = 0; i < bin_str.size(); i = i + 32)
		{
			string line;
			line = bin_str.substr(i, 32);
			if (line == "00000000000000000000000000000000")
			{
				continue;
			}
			stringstream ss;
			ss << hex << stoi(line, nullptr, 2);
			ss >> line;
			while (line.size() < 8)
			{
				line = "0" + line;
			}
			if (cnt % 4 == 0)
				hex_str = line + ", ";
			else
				hex_str = hex_str + line + ", ";

			if (cnt % 4 == 3)
				out <<hex_str<< endl;
			cnt++;
		}
		if (hex_str.size() > 0)
		{
			unsigned size = hex_str.size();
			hex_str[size - 2] = ';';
			out << hex_str << endl;

		}
	}
	//only consider radix 16
	void coe2asm(ifstream &in, ofstream &out)
	{
		string coe_str;
		getline(in, coe_str);
        //getline(in, coe_str);
		getline(in, coe_str);	//skip the first two line
		ofstream bin("temp.bin");
		unsigned cnt = 0;
		while (getline(in, coe_str))
		{
			cnt++;
			stringstream ss;
			ss << coe_str;
			string hex_str;
			while (ss >> hex_str)
			{
				
				auto sz = hex_str.size();
				if (sz != 9)
				{
					Error(cnt);
					return;
				}
				else
				{
					hex_str.pop_back();
					string hex_str1 = hex_str.substr(0, 4);
					string hex_str2 = hex_str.substr(4, 4);
					bin << hex2bin(hex_str1) << hex2bin(hex_str2);
				}
			}
		}
		bin.close();
		ifstream bin_in("temp.bin");
		bin2asm(bin_in, out);
	}
}
