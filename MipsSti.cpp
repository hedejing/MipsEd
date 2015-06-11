#include "MipsSti.h"

namespace Mips{
	MipsSti::MipsSti()
	{
		Text text;
		Data data;
		Register regs;
		map<string, int> labels;
	}

    void MipsSti::prepare(ifstream& is)
    {
        code = new Code(is);
        code->load(text, data, labels);
        text.encode(labels);
		text.pc = text.allo_text.begin()->first;
        data.encode();
    }

	map<string, bitset<32>> MipsSti::run(ifstream &is)
	{
		code = new Code(is);
		code->load(text, data, labels);
		text.encode(labels);
		data.encode();
		regs = text.run(labels, data.get_value());
		return regs.get_regs();
	}

    map<string,bitset<32>> MipsSti::step(unsigned& address)
    {
        regs = text.step(labels, data.get_value(),address);
        return regs.get_regs();
    }

	map<unsigned, string> MipsSti::get_allo_text()
	{
		return text.get_hex_code();
	}

	map<unsigned, int>	MipsSti::get_allo_data()
	{
		return data.get_value();
	}

	void MipsSti::binary(string path)
	{
		ofstream out(path);
		generate_bin(out, text.get_code(), data.get_code());
	}

	void MipsSti::coe(string in_path,string out_path)
	{
		ofstream out(out_path);
		ifstream in(in_path);
		out << "memory_initialization_radix=16;" << endl;
		out << "memory_initialization_vector = " << endl;
		bin2coe(in,out);
	}



}
