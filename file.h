#ifndef _FILE_H_
#define _FILE_H_

#include "headers.h"
#include <fstream>
#include <sstream>
#include <queue>
#include "memory.h"
#include <qmessagebox.h>
#include <qstring.h>

namespace Mips{
	class Line{
	public:
		Line(string sentence);
		~Line();
		bool Islabeled();
		void print(); //for test
		string front();
		string pop_front();
		deque<string> getwords();
        
	private:
		deque<string> words;
		bool haslabel;
	};

	class Code{
	public:
		Code();
        ~Code() {}
		Code(ifstream &is);
		void print();
		void load(Text &text, Data &data, map<string, int> &labels);
	private:
		vector<Line> lines;
	};

	void generate_bin(ofstream &out, const map<unsigned, string> &text_code, const map<unsigned, string> &data_code);

	void coe2asm(ifstream &in, ofstream &out);

	void bin2asm(ifstream &in, ofstream &out);

	void bin2coe(ifstream &in, ofstream &out);

	void Error(unsigned line_num);
}
#endif
