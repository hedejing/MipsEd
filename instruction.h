#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "headers.h"
#include <set>
#include <bitset>

namespace Mips{
	bool IsRIns(const string target);

	bool IsIIns(const string target);

	bool IsJIns(const string target);

	void init_encode_reg(map<string, string> &reg2code);

	string dec2bin(int dec, unsigned num);

	string hex2bin(const string hex);

	bitset<32> signedRS(bitset<32> target, unsigned val);

	int bit2int(bitset<32> target);

	bool IsDecimal(string word);

	bool Is32bitsHex(string word);

	string dec2hex2(int dec);

	string dec2hex8(unsigned dec);
}
#endif