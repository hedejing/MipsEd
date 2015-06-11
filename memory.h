#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "headers.h"
#include "register.h"
#include "instruction.h"
#include <deque>
#include <iomanip>
#include <fstream>

namespace Mips{
	class Text{
	public:
		Text();
		~Text();
		unsigned pc;//program counter

		void setaddr(int _addr);
		void push_back(deque<string> line);
		void encode(const map<string, int>& labels); //encode lines to machine code
		void Allo(deque<string> line, map<string, int> &labels);
		void print();
		void print_code();
		map<unsigned, string> get_code();
		map<unsigned, string> get_hex_code();
        Register run(const map<string, int>& labels, map<unsigned, int>& value);
        Register step(const map<string,int>& labels, map<unsigned, int>& value, unsigned& address);
        map<unsigned, deque<string>> allo_text;  //allocate address for each line in text

	private:
		unsigned addr;
		map<unsigned, string> machine_code;		//transform text into machine code
	};

	class Data{
	public:
		Data();
		~Data();
		unsigned gp; //global pointer/the

		void setaddr(int _addr);
		void exec(deque<string> line, map<string, int> &labels);
		void encode();
		void print();//for tests
		void print_code();
		map<unsigned, string> get_code();
		map<unsigned, int> get_value();
		bool empty();
	private:
		unsigned addr;
		map<unsigned, int> value;
		map<unsigned, string> machine_code;
	};

}
#endif
