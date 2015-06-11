#include "instruction.h"

namespace Mips{
	bool IsRIns(const string target)
	{
		string RIns[] = { "add", "addu", "sub", "subu", "and", "or", "xor", "nor", "sll", "srl", "sra", "sllv", "srlv", "srav", "slt", "sltu", "jr", "jalr", "syscall", "mfhi", "mthi", "mflo", "mtlo", "mult", "multu", "div", "divu" };
		set<string> RInsSet(RIns, RIns + 27);
		if (RInsSet.find(target) != RInsSet.end())
			return true;
		else
			return false;
	}

	bool IsIIns(const string target)
	{
		string IIns[] = { "addi", "addiu", "ori", "andi", "xori", "lui", "slti", "sltiu", "lw", "lb", "lbu", "lh", "lhu", "sw", "sh", "sb", "beq", "bne", "bgez", "bgezal", "bgtz", "blez", "bltz", "bltzal", "la" };
		set<string> IInsSet(IIns, IIns + 25);
		if (IInsSet.find(target) != IInsSet.end())
			return true;
		else
			return false;
	}

	bool IsJIns(const string target)
	{
		if (target == "j" || target == "jal")
			return true;
		else
			return false;
	}

	void init_encode_reg(map<string, string> &reg2code)
	{
		//The constant value 0
		reg2code.insert({ "$0", "00000" });
		reg2code.insert({ "$zero", "00000" });
		reg2code.insert({ "$at", "00001" });
		//Values for results and expression evalution v0~v3
		reg2code.insert({ "$v0", "00010" });
		reg2code.insert({ "$v1", "00011" });
		//Arguments a0~a3
		reg2code.insert({ "$a0", "00100" });
		reg2code.insert({ "$a1", "00101" });
		reg2code.insert({ "$a2", "00110" });
		reg2code.insert({ "$a3", "00111" });
		//Temporaries t0~t7
		reg2code.insert({ "$t0", "01000" });
		reg2code.insert({ "$t1", "01001" });
		reg2code.insert({ "$t2", "01010" });
		reg2code.insert({ "$t3", "01011" });
		reg2code.insert({ "$t4", "01100" });
		reg2code.insert({ "$t5", "01101" });
		reg2code.insert({ "$t6", "01110" });
		reg2code.insert({ "$t7", "01111" });
		//Saved s0~s7
		reg2code.insert({ "$s0", "10000" });
		reg2code.insert({ "$s1", "10001" });
		reg2code.insert({ "$s2", "10010" });
		reg2code.insert({ "$s3", "10011" });
		reg2code.insert({ "$s4", "10100" });
		reg2code.insert({ "$s5", "10101" });
		reg2code.insert({ "$s6", "10110" });
		reg2code.insert({ "$s7", "10111" });
		//More temporaries t8~t9
		reg2code.insert({ "$t8", "11000" });
		reg2code.insert({ "$t9", "11001" });
		//reserved for the operating system
		reg2code.insert({ "$k0", "11010" });
		reg2code.insert({ "$k1", "11011" });
		//Global pointer	    
		reg2code.insert({ "$gp", "11100" });
		//Stack pointer		    
		reg2code.insert({ "$sp", "11101" });
		//Frame pointer		    
		reg2code.insert({ "$fp", "11110" });
		//Return address	    
		reg2code.insert({ "$ra", "11111" });
	}

	string dec2bin(int dec, unsigned num)
	{
		string ret;
		bool flag = true;
		if (dec < 0)
		{
			dec = -dec;
			flag = false;
		}
		while (dec > 0)
		{
			if (dec % 2 == 0)
				ret.push_back('0');
			else
				ret.push_back('1');
			dec = dec / 2;
		}

		while (ret.size() < num)
			ret.push_back('0');
		reverse(ret.begin(), ret.end());
		if (flag == true)
			return ret;
		if (flag == false && num == 16)
		{
			bitset<16> bit(ret);
			bit.flip();
			unsigned long r = bit.to_ulong();
			r = r + 1;
			bitset<16> bit2(r);
			return bit2.to_string();
		}
		//	else error

	}

	string hex2bin(const string hex)
	{
		string bin;
		unsigned start;
		if (hex[0] == '0'&&hex[1] == 'x')
			start = 2;
		else
			start = 0;
		string tempstr = hex.substr(start, hex.size()-start);
		while (tempstr.size() < 4)
		{
			tempstr = "0" + tempstr;
		}
		for (unsigned i = 0; i < tempstr.size(); i++)
		{
			string str;
			switch (tempstr[i])
			{
			case'0':str = "0000"; break;
			case'1':str = "0001"; break;
			case'2':str = "0010"; break;
			case'3':str = "0011"; break;
			case'4':str = "0100"; break;
			case'5':str = "0101"; break;
			case'6':str = "0110"; break;
			case'7':str = "0111"; break;
			case'8':str = "1000"; break;
			case'9':str = "1001"; break;
			case'a':case'A':str = "1010"; break;
			case'b':case'B':str = "1011"; break;
			case'c':case'C':str = "1100"; break;
			case'd':case'D':str = "1101"; break;
			case'e':case'E':str = "1110"; break;
			case'f':case'F':str = "1111"; break;
			default:break;
			}
			bin = bin + str;
		}
		return bin;
	}

	bitset<32> signedRS(bitset<32> target, unsigned val)
	{
		bitset<32> ret;
		ret = target >> val;
		if (target[31] == 1)
		{
			while (val--)
				ret.flip(31 - val);
		}
		return ret;
	}

	int bit2int(bitset<32> target)
	{
		if (target[31] == 1)
		{
			bitset<32> all1(0xffffffff);
			unsigned long tar = target.to_ulong();
			return tar - 1 - all1.to_ulong();
		}
		else
			return target.to_ulong();
	}

	bool IsDecimal(string word)
	{
		string::size_type sz;
		int val = stoi(word, &sz);
		stringstream ss;
		string test;
		ss << val;
		ss >> test;
		if (test == word)
			return true;
		else
			return false;
	}

	bool Is32bitsHex(string word)
	{
		if (word.size() != 8)          //check if 8 digits
			return false;
		unsigned i = 0;
		for (; i < word.size(); i++)   //check if all hexadecimal digits
			if (!((word[i] <= 'z'&&word[i] >= 'a') || (word[i] <= 'Z'&&word[i] >= 'A')))
				break;
		if (i < word.size())
			return false;

		return true;
	}

	string dec2hex2(int dec)
	{
		string code;
		stringstream ss;
		if (dec > 0x0f)
			ss << hex << dec;
		else
			ss << '0' << hex << dec;
		ss >> code;
		return code;
	}

	string dec2hex8(unsigned dec)
	{
		string ret;
		stringstream ss;
		ss << std::hex << dec;
		ss >> ret;

		while (ret.size() != 8)
		{
			ret = "0" + ret;
		}

		return ret;
	}
}