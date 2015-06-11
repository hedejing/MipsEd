#ifndef _REGISTER_H_
#define _REGISTER_H_

#include "headers.h"
#include <bitset>

namespace Mips{
	class Register{
	public:
		Register();
        ~Register() {}
		bitset<32>& operator[](string name);
		void reset();
		void print();
		map<string, bitset<32>> get_regs();
	private:
		map<string, bitset<32>> regs;
	};
}
#endif
