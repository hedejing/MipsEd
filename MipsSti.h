#ifndef _MIPSSTI_H_
#define _MIPSSTI_H_

#include "file.h"
#include "memory.h"
#include "register.h"
#include "instruction.h"

namespace Mips{

	class MipsSti{
	public:
		MipsSti();
		~MipsSti() {}

        void prepare(ifstream& is);
		map<string, bitset<32>> run(ifstream &is);
        map<string, bitset<32>> step(unsigned& address);
		map<unsigned, string> get_allo_text();
		map<unsigned, int>	get_allo_data();
		void coe(string in_path, string out_path);
		void binary(string path);
        Text text;

	private:
		Data data;
		Register regs;
		map<string, int> labels;
		Code *code;
	};
}


#endif
