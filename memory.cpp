#include "memory.h"

namespace Mips{
//Text
Text::Text() {}

void Text::setaddr(int _addr) { addr = _addr; }

Text::~Text() {}

void Text::Allo(deque<string> line, map<string, int> &labels)
{
	string word;
	word = line.front();
	if (IsRIns(word) || IsIIns(word) || IsJIns(word))
	{
		allo_text.insert({ addr, line });
		addr += 4;
		return;
	}
	else //with label
	{
		labels.insert({ word, addr>>2 });
		line.pop_front(); //pop out the label
		allo_text.insert({ addr, line });
		addr += 4;
		return;
	}
}

void Text::print_code()
{
	for (auto i : machine_code)
	{
		cout << i.second << endl;
	}
}

void Text::encode(const map<string, int>& labels)
{
	map<string, string> reg2code;
	init_encode_reg(reg2code);
	for (auto line : allo_text)
	{
		deque<string> words;
		unsigned addr;
		words = line.second;
		addr = line.first;
		string op;
		op = words[0];
		if (IsRIns(op))
		{
			string code("000000");
			string rd,rs,rt,shamt,funct;
			if (op == "sll" || op == "srl" || op == "sra")
			{
				try{
					rd = reg2code[words[1]];
					rt = reg2code[words[2]];
					int val = stoi(words[3], nullptr);
					shamt = dec2bin(val, 5);
				}
				catch (const out_of_range &e){
					fstream err("ERROR");
					err << addr << "error: "<< e.what() << endl;
				}
				rs = "00000";
				if (op == "sll") funct = "000000";
				if (op == "srl") funct = "000010";
				if (op == "sra") funct = "000011";
			}
			if (op == "sllv" || op == "srlv" || op == "srav")
			{
				rd = reg2code[words[1]];
				rt = reg2code[words[2]];
				rs = reg2code[words[3]];
				shamt = "00000";
				if (op == "sllv") funct = "000100";
				if (op == "srlv") funct = "000110";
				if (op == "srav") funct = "000111";
			}
			if (op == "jr")
			{
				rd = "00000";
				rt = "00000";
				rs = reg2code[words[1]];
				shamt = "00000";
				funct = "001000";
			}
			if (op == "jalr")
			{
				rd = reg2code[words[1]];
				rs = reg2code[words[2]];
				rt = "00000";
				shamt = "00000";
				funct = "001001";
			}
			if (op == "syscall")
			{
				rd = rs = rt = shamt = "00000";
				funct = "001100";
			}
			if (op == "mfhi")
			{
				rd = reg2code[words[1]];
				rs = rt = shamt = "00000";
				funct = "010000";
			}
			if (op == "mthi")
			{
				rs = reg2code[words[1]];
				rt = rd = shamt = "00000";
				funct = "010001";
			}
			if (op == "mflo")
			{
				rd = reg2code[words[1]];
				rs = rt = shamt = "00000";
				funct = "010010";	//18
			}
			if (op == "mtlo")
			{
				rs = reg2code[words[1]];
				rt = rd = shamt = "00000";
				funct = "010011";	//19
			}
			if (op == "mult"||op=="multu"||op == "div"||op=="divu")
			{
				rs = reg2code[words[1]];
				rt = reg2code[words[2]];
				rd = shamt = "00000";
				if (op == "mult") funct = "011000";	//24
				if (op == "multu") funct = "011001";//25
				if (op == "div") funct = "011010";	//26
				if (op == "divu") funct = "011011"; //27
			}
			if (op == "add"||op=="addu"||op=="sub"||op=="subu"||op=="and"||op=="or"||op=="xor"||op=="nor"||op=="slt"||op=="sltu")
			{
				rd = reg2code[words[1]];
				rs = reg2code[words[2]];
				rt = reg2code[words[3]];
				shamt = "00000";
				if (op == "add") funct = "100000";	//32
				if (op == "addu") funct = "100001";	//33
				if (op == "sub") funct = "100010";	//34
				if (op == "subu") funct = "100011";	//35
				if (op == "and") funct = "100100";	//36
				if (op == "or") funct = "100101";	//37
				if (op == "xor") funct = "100110";	//38
				if (op == "nor") funct = "100111";	//39
				if (op == "slt") funct = "101010";	//42
				if (op == "sltu") funct = "101011";	//43
			}
			code = code + rs + rt + rd + shamt + funct;
			machine_code.insert({ addr, code });
			continue;
		}
		if (IsJIns(op))
		{
			string code;
			if (op == "j") code = "000010";
			if (op == "jal") code = "000011";
			string address;
			auto iter = labels.find(words[1]);
			if (iter != labels.end())
			{
				unsigned pc_plus4_hi4 = (addr + 4) & 0xf000;
				bitset<30> pc_plus4_hi4_bit(pc_plus4_hi4);
				unsigned target = (iter->second<<2);
				int bias = target - pc_plus4_hi4_bit.to_ulong();
				address=dec2bin((bias>>2),26);          //pending
			}
			code = code + address;
			//else error
			machine_code.insert({ addr, code });
			continue;
		}
		if (IsIIns(op))
		{
			string code,rs, rt, imm;
			if (op == "beq" || op == "bne")
			{
				if (op == "beq") code = "000100";
				if (op == "bne") code = "000101";
				rs = reg2code[words[1]];
				rt = reg2code[words[2]];
				auto iter = labels.find(words[3]);
				if (iter != labels.end())
				{
					unsigned pc_plus4 = addr + 4;
					unsigned target = iter->second;
					int bias = target - (pc_plus4 >>2);
					imm = dec2bin(bias, 16);
				}
				//else error
			}
			if (op == "blez" || op == "bgtz" || op == "bltz" || op == "bgez")
			{
				if (op == "blez") 
				{
					code = "000110";
					rt = "00000";
				}
				if (op == "bgtz")
				{
					code = "000111";
					rt = "00000";
				}
				if (op == "bltz")
				{
					code = "000001";
					rt = "00000";
				}
				if (op == "bgez")
				{
					code = "000001";
					rt = "00001";
				}
				rs = reg2code[words[1]];
				auto iter = labels.find(words[2]);
				if (iter != labels.end())
				{
					unsigned pc_plus4 = addr + 4;
					unsigned target = iter->second;
					int bias = target - (pc_plus4>>2);
					imm = dec2bin(bias , 16);
				}
			}
			if (op == "addi" || op == "addiu" || op == "slti" || op == "sltiu" || op == "andi" || op == "ori" || op == "xori"||op == "la")
			{
				if (op == "addi"||op=="la") code = "001000";
				if (op == "addiu") code = "001001";
				if (op == "slti") code = "001010";
				if (op == "sltiu") code = "001011";
				if (op == "andi") code = "001100";
				if (op == "ori") code = "001101";
				if (op == "xori") code = "001110";
				rt = reg2code[words[1]];
				if (op == "la")
				{
					rs = reg2code[words[1]];
					auto iter = labels.find(words[2]);
					if (iter != labels.end())
					{
						unsigned long address = iter->second<<2;
						bitset<16> imm_bit(address);
						imm = imm_bit.to_string();
					}
					continue;
				}
				rs = reg2code[words[2]];
				string str = words[3];
				if (str[0] == '0'&&str[1] == 'x')
				{
					int value = stoi(str, nullptr, 16); //if it is a hexadecimal number
					imm = dec2bin(value, 16);
				}
				else
				{
					int value = stoi(str, nullptr,10);   //if it is a decimal number
					imm = dec2bin(value, 16);
				}
				code = code + rs + rt + imm;
				machine_code.insert({ addr, code });
				continue;
			}
			if (op == "lui")
			{
				code = "001111";
				rt = reg2code[words[1]];
				rs = "00000";
				imm = hex2bin(words[2]);
			}
			if (op == "lb" || op == "lh" || op == "lw" || op == "lbu" || op == "lhu" || op == "sb" || op == "sh" || op == "sw")
			{
				if (op == "lb") code = "100000";
				if (op == "lh") code = "100001";
				if (op == "lw") code = "100010";
				if (op == "lbu") code = "100100";
				if (op == "lhu") code = "100101";
				if (op == "sb") code = "101000";
				if (op == "sh") code = "101001";
				if (op == "sw") code = "101011";
				rt = reg2code[words[1]];				// for example : lb $rt,imm($rs)
				string imm_reg = words[2];
				auto first_par = imm_reg.find_first_of("(");
				string imm_str = imm_reg.substr(0, first_par); //get the immidate part
				rs = imm_reg.substr(first_par + 1);
				rs.pop_back();							//get the rs part
				rs = reg2code[rs];
				int value = stoi(imm_str, nullptr);
				imm = dec2bin(value, 16);
			}
			code = code + rs + rt + imm;
			machine_code.insert({ addr, code });
			continue;
		}

	}
}

Register Text::run(const map<string, int>& labels, map<unsigned, int>& value)
{
	Register regs;
	auto start = allo_text.begin();
	unsigned int pc = start->first;

	while (allo_text[pc].size()>0)
		regs = step(labels, value, pc);

	return regs;
}

Register Text::step(const map<string, int>& labels, map<unsigned, int>& value,unsigned& address)
{
    static Register regs;
	//reset the registers at the beginning of program
	if (address == allo_text.begin()->first)
	{
		regs.reset();
	}

    auto words = allo_text[address];
	pc = address;
    regs["$zero"].reset();						//reset $zero to 0
    if (words.size() == 0) return regs;
    string op;
    op = words[0];

	try{
		if (IsRIns(op))
		{
			if (op == "sll" || op == "srl" || op == "sra")
			{
				bitset<32> &rd = regs[words[1]];
				bitset<32> rt = regs[words[2]];
				int val = stoi(words[3], nullptr);

				if (op == "sll") rd = rt << val;
				if (op == "srl") rd = rt >> val;
				if (op == "sra") rd = signedRS(rt, val);
				pc += 4;

			}
			if (op == "sllv" || op == "srlv" || op == "srav")
			{
				bitset<32> &rd = regs[words[1]];
				bitset<32> rt = regs[words[2]];
				bitset<32> rs = regs[words[3]];
				unsigned long val = rs.to_ulong();
				if (op == "sllv") rd = rt << val;
				if (op == "srlv") rd = rt >> val;
				if (op == "srav") rd = signedRS(rt, val);
				pc += 4;

			}
			if (op == "jr")
			{
				bitset<32> rs = regs[words[1]];
				pc = rs.to_ulong();

			}
			if (op == "jalr")
			{
				bitset<32> &rd = regs[words[1]];
				bitset<32> rs = regs[words[2]];
				bitset<32> temp(pc + 4);
				rd = temp;
				pc = rs.to_ulong();

			}
			if (op == "syscall")		//pending!!!!!!!!!!!
			{

			}
			if (op == "mfhi")
			{
				bitset<32> &rd = regs[words[1]];
				rd = regs["HI"];
				pc += 4;

			}
			if (op == "mthi")
			{
				regs["HI"] = regs[words[1]];
				pc += 4;

			}
			if (op == "mflo")
			{
				bitset<32> &rd = regs[words[1]];
				rd = regs["LO"];
				pc += 4;

			}
			if (op == "mtlo")
			{
				regs["LO"] = regs[words[1]];
				pc += 4;

			}
			if (op == "mult" || op == "multu" || op == "div" || op == "divu")
			{
				unsigned long HI_val;
				unsigned long LO_val;
				bitset<32> rs = regs[words[1]];
				bitset<32> rt = regs[words[2]];
				if (op == "mult")	//24
				{
					long long oprand = bit2int(rs);
					long long oper = bit2int(rt);
					long long ret = oprand*oper;
					HI_val = ret >> 32;
					LO_val = ret;
				}
				if (op == "multu")//25
				{
					unsigned long long oprand = rs.to_ullong();
					unsigned long long oper = rt.to_ullong();
					unsigned long long ret = oper*oprand;
					HI_val = ret >> 32;
					LO_val = ret;
				}
				if (op == "div") 	//26
				{
					long long oprand = bit2int(rs);
					long long oper = bit2int(rt);
					LO_val = oprand / oper;
					HI_val = oprand % oper;
				}
				if (op == "divu")  //27
				{
					unsigned long long oprand = rs.to_ullong();
					unsigned long long oper = rt.to_ullong();
					LO_val = oprand / oper;
					HI_val = oprand % oper;
				}
				bitset<32> HI(HI_val);
				bitset<32> LO(LO_val);
				regs["HI"] = HI;
				regs["LO"] = LO;
				pc += 4;

			}
			if (op == "add" || op == "addu" || op == "sub" || op == "subu" || op == "and" || op == "or" || op == "xor" || op == "nor" || op == "slt" || op == "sltu")
			{
				bitset<32> &rd = regs[words[1]];
				bitset<32> rs = regs[words[2]];
				bitset<32> rt = regs[words[3]];
				if (op == "add") //32
				{
					long oprand = bit2int(rs);
					long oper = bit2int(rt);
					bitset<32> ret(oprand + oper);
					rd = ret;
				}
				if (op == "addu") 	//33
				{
					unsigned long oprand = rs.to_ulong();
					unsigned long oper = rt.to_ulong();
					bitset<32> ret(oprand + oper);
					rd = ret;
				}
				if (op == "sub") 	//34
				{
					long oprand = bit2int(rs);
					long oper = bit2int(rt);
					bitset<32> ret(oprand - oper);
					rd = ret;
				}
				if (op == "subu") //35
				{
					unsigned long oprand = rs.to_ulong();
					unsigned long oper = rt.to_ulong();
					bitset<32> ret(oprand - oper);
					rd = ret;
				}
				if (op == "and") //36
				{
					rd = rs & rt;
				}
				if (op == "or") //37
				{
					rd = rs | rt;
				}
				if (op == "xor") //38
				{
					rd = rs^rt;
				}
				if (op == "nor") //39
				{
					rd = rs | rt;
					rd.flip();
				}
				if (op == "slt") //42
				{
					long oprand = bit2int(rs);
					long oper = bit2int(rt);
					rd.reset();
					if (oprand < oper)
						rd.set(0);
				}
				if (op == "sltu")//43
				{
					unsigned long oprand = rs.to_ulong();
					unsigned long oper = rt.to_ulong();
					rd.reset();
					if (oprand < oper)
						rd.set(0);
				}
				pc += 4;

			}
		}
		if (IsJIns(op))
		{
			if (op == "j")
			{
				auto iter = labels.find(words[1]);
				if (iter != labels.end())
				{
					pc = iter->second << 2;        //pending

				}
				//else error
			}
			if (op == "jal")
			{
				auto iter = labels.find(words[1]);
				if (iter != labels.end())
				{
					bitset<32> ra_bit(pc + 4);
					regs["$ra"] = ra_bit;
					pc = (iter->second << 2);        //pending

				}
				//else error
			}
		}
		if (IsIIns(op))
		{
			if (op == "beq" || op == "bne")
			{
				bitset<32> rs = regs[words[1]];
				bitset<32> rt = regs[words[2]];
				if (op == "beq")
				{
					if (rs == rt)
					{
						auto iter = labels.find(words[3]);
						if (iter != labels.end())
							pc = iter->second << 2;
						//else error

					}
					else
					{
						pc += 4;

					}

				}
				if (op == "bne")
				{
					if (rs != rt)
					{
						auto iter = labels.find(words[3]);
						if (iter != labels.end())
							pc = iter->second << 2;
						//else error

					}
					else
					{
						pc += 4;

					}
				}
			}
			if (op == "blez" || op == "bgtz" || op == "bltz" || op == "bgez")
			{
				bitset<32> rs = regs[words[1]];
				if (op == "blez")
				{
					if (rs[31] == 1 || rs.to_ulong() == 0)
					{
						auto iter = labels.find(words[2]);
						if (iter != labels.end())
							pc = iter->second << 2;

					}
					else
					{
						pc += 4;

					}
				}
				if (op == "bgtz")
				{
					if (rs[31] == 0 && rs.to_ulong() != 0)
					{
						auto iter = labels.find(words[2]);
						if (iter != labels.end())
							pc = iter->second << 2;

					}
					else
					{
						pc += 4;

					}
				}
				if (op == "bltz")
				{
					if (rs[31] == 1 && rs.to_ulong() != 0)
					{
						auto iter = labels.find(words[2]);
						if (iter != labels.end())
							pc = iter->second << 2;

					}
					else
					{
						pc += 4;

					}
				}
				if (op == "bgez")
				{
					if (rs[31] == 0 || rs.to_ulong() == 0)
					{
						auto iter = labels.find(words[2]);
						if (iter != labels.end())
							pc = iter->second << 2;

					}
					else
					{
						pc += 4;

					}
				}
			}
			if (op == "la" || op == "addi" || op == "addiu" || op == "slti" || op == "sltiu" || op == "andi" || op == "ori" || op == "xori")
			{
				bitset<32> &rt = regs[words[1]];
				unsigned long ret_val = 0;
				if (op == "la")
				{
					auto iter = labels.find(words[2]);
					if (iter != labels.end())
					{
						int imm = iter->second << 2;
						bitset<32> ret(imm);
						rt = ret;
						pc += 4;

					}
					//else error
				}
				bitset<32> rs = regs[words[2]];
				if (op == "addi")
				{
					int imm = stoi(words[3], nullptr, 0);
					ret_val = bit2int(rs) + imm;
				}
				if (op == "addiu")
				{
					unsigned long imm = stoi(words[3], nullptr, 0);
					ret_val = rs.to_ulong() + imm;
				}
				if (op == "slti")
				{
					int imm = stoi(words[3], nullptr, 0);
					int rs_val = bit2int(rs);
					ret_val = rs_val < imm;
				}
				if (op == "sltiu")
				{
					unsigned long imm = stoi(words[3], nullptr, 0);
					ret_val = rs.to_ulong() < imm;
				}
				if (op == "andi")
				{
					unsigned long imm = stoi(words[3], nullptr, 0);
					ret_val = rs.to_ulong() & imm;
				}
				if (op == "ori")
				{
					unsigned long imm = stoi(words[3], nullptr, 0);
					ret_val = rs.to_ulong() | imm;
				}
				if (op == "xori")
				{
					unsigned long imm = stoi(words[3], nullptr, 0);
					ret_val = rs.to_ulong() ^ imm;
				}
				bitset<32> ret(ret_val);
				rt = ret;
				pc += 4;

			}
			if (op == "lui")
			{
				bitset<32> &rt = regs[words[1]];
				unsigned long imm = stoi(words[2], nullptr, 16);
				imm = imm << 16;
				bitset<32> ret(imm);
				rt = ret;
				pc += 4;

			}
			if (op == "lb" || op == "lh" || op == "lw" || op == "lbu" || op == "lhu" || op == "sb" || op == "sh" || op == "sw")
			{
				// for example : lb $rt,imm($rs)
				string::size_type sz;
				int imm = stoi(words[2], &sz);
				string imm_reg = words[2];
				string rs_str = imm_reg.substr(sz + 1);
				rs_str.pop_back();							//get the rs part
				bitset<32> rs = regs[rs_str];
				int ret_val;
				if (op == "lb")
				{
					bitset<32> &rt = regs[words[1]];
					auto iter = value.find(rs.to_ulong() + imm);
					if (iter != value.end())
						ret_val = iter->second;
					//else error
					bitset<32> ret(ret_val);
					if (ret_val >= 0x80) //sign extends
					{
						for (unsigned i = 8; i < 32; i++)
							ret[i] = 1;
					}
					rt = ret;
					pc += 4;

				}
				if (op == "lh")
				{
					bitset<32> &rt = regs[words[1]];
					int front_ret, back_ret;
					auto iter = value.find(rs.to_ulong() + imm);
					if (iter != value.end())
						front_ret = iter->second;
					//else error
					iter = value.find(rs.to_ulong() + imm + 1);
					if (iter != value.end())
						back_ret = iter->second;
					//else error
					ret_val = (front_ret << 8) + back_ret;
					bitset<32> ret(ret_val);
					if (ret_val >= 0x8000)
					{
						for (unsigned i = 16; i < 32; i++)
							ret[i] = 1;
					}
					rt = ret;
					pc += 4;

				}
				if (op == "lw")
				{
					bitset<32> &rt = regs[words[1]];
					int ret1, ret2, ret3, ret4;
					auto iter1 = value.find(rs.to_ulong() + imm);
					if (iter1 != value.end())
						ret1 = iter1->second;
					//else error
					auto iter2 = value.find(rs.to_ulong() + imm + 1);
					if (iter2 != value.end())
						ret2 = iter2->second;
					//else error
					auto iter3 = value.find(rs.to_ulong() + imm + 2);
					if (iter3 != value.end())
						ret3 = iter3->second;
					//else error
					auto iter4 = value.find(rs.to_ulong() + imm + 3);
					if (iter4 != value.end())
						ret4 = iter4->second;
					//else error
					unsigned long val = (ret1 << 24) + (ret2 << 16) + (ret3 << 8) + ret4;
					bitset<32> ret(val);
					rt = ret;
					pc += 4;

				}
				if (op == "lbu")
				{
					bitset<32> &rt = regs[words[1]];
					auto iter = value.find(rs.to_ulong() + imm);
					if (iter != value.end())
						ret_val = iter->second;
					//else error
					bitset<32> ret(ret_val);
					rt = ret;
					pc += 4;

				}
				if (op == "lhu")
				{
					bitset<32> &rt = regs[words[1]];
					int front_ret, back_ret;
					auto iter1 = value.find(rs.to_ulong() + imm);
					if (iter1 != value.end())
						front_ret = iter1->second;
					//else error
					auto iter2 = value.find(rs.to_ulong() + imm + 1);
					if (iter2 != value.end())
						back_ret = iter2->second;
					//else error
					ret_val = (front_ret << 8) + back_ret;
					bitset<32> ret(ret_val);
					pc += 4;
					rt = ret;

				}
				if (op == "sb")
				{
					bitset<32> rt = regs[words[1]];
					bitset<32> tar(0x000000ff);
					tar = rt & tar;
					unsigned address = rs.to_ulong() + imm;
					unsigned long target = tar.to_ulong();
					value[address] = target;
					pc += 4;

				}
				if (op == "sh")
				{
					bitset<32> rt = regs[words[1]];
					bitset<32> tar(0x0000ffff);
					tar = rt & tar;
					unsigned address = rs.to_ulong() + imm;
					bitset<32> front_bit;
					front_bit = tar >> 8;
					bitset<32> back_bit(0x000000ff);
					back_bit = tar & back_bit;
					unsigned long front_target = front_bit.to_ulong();
					unsigned long back_target = back_bit.to_ulong();
					value[address] = front_target;
					value[address + 1] = back_target;
					pc += 4;

				}
				if (op == "sw")
				{
					bitset<32> rt = regs[words[1]];
					unsigned address = rs.to_ulong() + imm;
					bitset<32> byte4;
					byte4 = rt >> 24;
					bitset<32> byte3(0x00ff0000);
					byte3 = (rt & byte3) >> 16;
					bitset<32> byte2(0x0000ff00);
					byte2 = (rt & byte2) >> 8;
					bitset<32> byte1(0x000000ff);
					byte1 = (rt & byte1);
					value[address] = byte4.to_ulong();
					value[address + 1] = byte3.to_ulong();
					value[address + 2] = byte2.to_ulong();
					value[address + 3] = byte1.to_ulong();
					pc += 4;

				}
			}
		}
		}
		catch (out_of_range& e)
		{
			cout << address << ":";
			for (auto iter : words)
				cout << iter << " " ;
			cout << endl;
			system("pause");
		}
	
		address = pc;
    return regs;
}

void Text::print()
{
	for (map<unsigned, deque<string>>::iterator iter = allo_text.begin(); iter != allo_text.end(); ++iter)
	{
		std::cout << hex << iter->first << ":";
		for (unsigned i = 0; i < iter->second.size(); i++)
			cout << iter->second[i] << " ";
		cout << endl;
	}
}
//Data
Data::Data() { }

void Data::setaddr(int _addr){ addr = _addr; }

void Data::print()
{
	for (map<unsigned, int>::iterator iter = value.begin(); iter != value.end();)
	{
		//cout << hex << iter->first << ":";
		cout << hex << iter->second << " ";
		iter++;
		cout << hex << iter->second << " ";
		iter++;
		cout << hex << iter->second << " ";
		iter++;
		cout << hex << iter->second << " ";
		iter++;
		cout << endl;
	}
		
}

void Data::exec(deque<string> line, map<string, int> &labels)
{
	string word;
	word = line.front();
	line.pop_front();

	if (word == "db" || word == "DB")
	{
		while (!line.empty())
		{
			word = line.front();
			line.pop_front();        //get front element and pop it out
			if (word[0] == '\'')     //if it is a string or a character
			{
				//error if no another '\''
				bool hasend = true;
				unsigned i = 1;
				for (; word[i] != '\''&&i<word.size(); ++i)
				{
					value.insert({ addr, int(word[i]) });
					addr++;
				}
				if (i >= word.size() ) hasend = false;
				continue;
			}
			if (word[0] == '0' && word[1] == 'x')	//if it is a hexidecimal number
			{
				value.insert({ addr, stoi(word, nullptr, 16) });
				addr++;
				continue;
			}
			if (IsDecimal(word))//if it is a decimal number //supported???
			{
				string::size_type sz;
				int val = stoi(word, &sz);
				//if (sz != word.size() || val > 0xff || val < 0)  error  //check if decimal
				value.insert({ addr, val });
				addr++;
				continue;
			}

			//error
		}
		return;
	}
	if (word == "dw" || word == "DW")
	{
		while (!line.empty())
		{
			word = line.front();
			line.pop_front();
			if (word[0] == '\'')     //if it is a string or a character
			{
				//error if no another '\''
				bool hasend = true;
				unsigned i = 1;
				for (; word[i] != '\''&&i<word.size(); i++)
				{
					value.insert({ addr, int(word[i]) });
					addr++;
				}
				if (i % 2 == 0)			//if the string length is odd or it's a character
				{
					value.insert({ addr, 0 });//insert 0x00
					addr++;
				}
				if (i >= word.size()) hasend = false;
				continue;
			}
			if (word[0] == '0' && word[1] == 'x')	//if it is a hexidecimal number
			{
				string::size_type sz;
				int val = stoi(word, &sz, 16);
				//if (sz != 6) error
				int front = (val & 0xff00)>>8;
				int back = val & 0x00ff;
				value.insert({ addr, back });
				addr++;
				value.insert({ addr, front });		//back first, then front
				addr++;
				continue;
			}
			if (IsDecimal(word))//if it is a decimal number  //supported???
			{
				string::size_type sz;
				int val = stoi(word, &sz);
				//if (sz != word.size() || val < 0xffff)  error  //check if decimal
				value.insert({ addr, val });
				addr++;
				continue;
			}

			//error
		}
		return;										// end dw
	}
	if (word == "dd" || word == "DD")
	{
		while (!line.empty())
		{
			word = line.front();
			line.pop_front();
			if (word[0] == '\'')     //if it is a string or a character
			{
				//error if no another '\''
				bool hasend = true;
				unsigned i = 1;
				for (; word[i] != '\''&&i<word.size(); i++)
				{
					value.insert({ addr, int(word[i]) });
					addr++;
				}
				if (i % 4 != 1)			//if the string length is odd or it's a character
				{
					for (; i % 4 != 1;i++)
					value.insert({ addr, 0 });//insert 0x00
					addr++;
				}
				if (i >= word.size()) hasend = false;
				continue;
			}
			if (word[0] == '0' && word[1] == 'x')	//if it is a hexidecimal number
			{
				string::size_type sz;
				int val = stoi(word, &sz, 16);
				//if (sz != 10) error
				int part1 = val & 0x000000ff;
				int part2 = (val & 0x0000ff00)>>8;
				int part3 = (val & 0x00ff0000)>>16;
				int part4 = (val & 0xff000000)>>24;
				value.insert({ addr, part1 });
				addr++;
				value.insert({ addr, part2 });      
				addr++;
				value.insert({ addr, part3 });
				addr++;
				value.insert({ addr, part4 });
				addr++;
				continue;
			}
			if (IsDecimal(word))//if it is a decimal number  //supported???
			{
				string::size_type sz;
				int val = stoi(word, &sz);
				//if (sz != word.size() || val < 0xffffffff)  error  //check if decimal
				value.insert({ addr, val });
				addr++;
				continue;
			}

			//error
		}
		return;
	}
	
	//*************label for spaces definition***********

	string target;
	target = word; //label number of name of constant defined in equ/EQU
	word = line.front();
	line.pop_front();
	if (word == "EQU" || word == "equ")
	{
		int constant = stoi(line.front(),nullptr);
		line.pop_front();
		//if (!line.empty()) error
		auto ret = labels.insert({ target, constant });	//insert target and the constant
		if (!ret.second)  //uf target is already defined, update it
			ret.first->second = constant;
		//don't increase addr
		return;
	}
	if (word == "RESB" || word == "resb")
	{
		int constant = stoi(line.front(), nullptr);
		line.pop_front();
		//if (!line.empty()) error
		auto ret = labels.insert({ target, addr });   //insert target and address
		//if (!ret.second) error
		addr = addr + constant;

		return;
	}
	if (word == "RESW" || word == "resw")
	{
		int constant = stoi(line.front(), nullptr);
		line.pop_front();
		//if (!line.empty()) error
		auto ret = labels.insert({ target, addr });   //insert target and address
		//if (!ret.second) error
		addr = addr + 2 * constant;

		return;
	}
	if (word == "RESD" || word == "resd")
	{
		int constant = stoi(line.front(), nullptr);
		line.pop_front();
		//if (!line.empty()) error
		auto ret = labels.insert({ target, addr });   //insert target and address
		//if (!ret.second) error
		addr = addr + 4 * constant;

		return;
	}
	if (word == "DD" || word == "dd")
	{
		auto ret = labels.insert({ target, addr });   //insert target and address
		//if (!ret.second) error
		while (!line.empty())
		{
			unsigned val = stoul(line.front(), nullptr, 16);
			line.pop_front();
			int part4 = val & 0x000000ff;
			int part3 = (val & 0x0000ff00) >> 8;
			int part2 = (val & 0x00ff0000) >> 16;
			int part1 = (val & 0xff000000) >> 24;
			value.insert({ addr, part1 });
			addr++;
			value.insert({ addr, part2 });
			addr++;
			value.insert({ addr, part3 });
			addr++;
			value.insert({ addr, part4 });
			addr++;
		}
	}
}

Data::~Data() {}

void Data::encode()
{
	for (map<unsigned, int>::iterator iter = value.begin(); iter != value.end();)
	{
		unsigned address;
		address = iter->first;
		string code;
		code = hex2bin(dec2hex2(iter->second));
		iter++;
		code += hex2bin(dec2hex2(iter->second));
		iter++;
		code += hex2bin(dec2hex2(iter->second));
		iter++;
		code += hex2bin(dec2hex2(iter->second));
		iter++;
		machine_code.insert({ address, code });
	}
}

void Data::print_code()
{
	for (auto i : machine_code)
	{
		cout << i.second << endl;
	}
}

map<unsigned, string> Data::get_code()
{
	if (!empty())
	return machine_code;
}

map<unsigned, string> Text::get_code()
{
	return machine_code;
}

string bit4_2hex(const string binstr)
{
	bitset<4> bit4(binstr);
	unsigned val = bit4.to_ulong();
	switch (val)
	{
	case 0:return "0";
	case 1:return "1";
	case 2:return "2";
	case 3:return "3";
	case 4:return "4";
	case 5:return "5";
	case 6:return "6";
	case 7:return "7";
	case 8:return "8";
	case 9:return "9";
	case 10:return "A";
	case 11:return "B";
	case 12:return "C";
	case 13:return "D";
	case 14:return "E";
	case 15:return "F";
	}
}

map<unsigned, string> Text::get_hex_code()
{
	map<unsigned, string> hex_code;
	for (auto line : machine_code)
	{
		unsigned address = line.first;
		string bin_code = line.second;
		string ret;
		if (bin_code.size()==32)
		for (unsigned i = 0; i < 32; i = i + 4)
		{
			string bit4 = bin_code.substr(i, 4);
			ret += bit4_2hex(bit4);
		}
		hex_code.insert({ address, ret });
	}
	return hex_code;
}


map<unsigned, int> Data::get_value()
{
	return value;
}

bool Data::empty()
{
	return value.empty();
}
}
