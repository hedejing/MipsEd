#include "register.h"
namespace Mips{
	Register::Register()
	{
		//The constant value 0
		regs.insert({ "$zero", 0 });
		regs.insert({ "$at", 0 });
		//Values for results and expression evalution v0~v3
		regs.insert({ "$v0", 0 });
		regs.insert({ "$v1", 0 });
		//Arguments a0~a3
		regs.insert({ "$a0", 0 });
		regs.insert({ "$a1", 0 });
		regs.insert({ "$a2", 0 });
		regs.insert({ "$a3", 0 });
		//Temporaries t0~t7
		regs.insert({ "$t0", 0 });
		regs.insert({ "$t1", 0 });
		regs.insert({ "$t2", 0 });
		regs.insert({ "$t3", 0 });
		regs.insert({ "$t4", 0 });
		regs.insert({ "$t5", 0 });
		regs.insert({ "$t6", 0 });
		regs.insert({ "$t7", 0 });
		//Saved s0~s7
		regs.insert({ "$s0", 0 });
		regs.insert({ "$s1", 0 });
		regs.insert({ "$s2", 0 });
		regs.insert({ "$s3", 0 });
		regs.insert({ "$s4", 0 });
		regs.insert({ "$s5", 0 });
		regs.insert({ "$s6", 0 });
		regs.insert({ "$s7", 0 });
		//More temporaries t8~t9
		regs.insert({ "$t8", 0 });
		regs.insert({ "$t9", 0 });
		//reserved for the operating system
		regs.insert({ "$k0", 0 });
		regs.insert({ "$k1", 0 });
		//Global pointer	
		regs.insert({ "$gp", 0 });
		//Stack pointer		
		regs.insert({ "$sp", 0 });
		//Frame pointer		
		regs.insert({ "$fp", 0 });
		//Return address	
		regs.insert({ "$ra", 0 });
		//Special registers
		regs.insert({ "HI", 0 });
		regs.insert({ "LO", 0 });
	}

	bitset<32> &Register::operator[](string name)
	{
		if (name == "$0" || name == "R0" || name == "r0") name = "$zero";
		if (name == "R1" || name == "r1") name = "$at";
		if (name == "R2" || name == "r2") name = "$v0";
		if (name == "R3" || name == "r3") name = "$v1";
		if (name == "R4" || name == "r4") name = "$a0";
		if (name == "R5" || name == "r5") name = "$a1";
		if (name == "R6" || name == "r6") name = "$a2";
		if (name == "R7" || name == "r7") name = "$a3";
		if (name == "R8" || name == "r8") name = "$t0";
		if (name == "R9" || name == "r9") name = "$t1";
		if (name == "R10" || name == "r10") name = "$t2";
		if (name == "R11" || name == "r11") name = "$t3";
		if (name == "R12" || name == "r12") name = "$t4";
		if (name == "R13" || name == "r13") name = "$t5";
		if (name == "R14" || name == "r14") name = "$t6";
		if (name == "R15" || name == "r15") name = "$t7";
		if (name == "R16" || name == "r16") name = "$s0";
		if (name == "R17" || name == "r17") name = "$s1";
		if (name == "R18" || name == "r18") name = "$s2";
		if (name == "R19" || name == "r19") name = "$s3";
		if (name == "R20" || name == "r20") name = "$s4";
		if (name == "R21" || name == "r21") name = "$s5";
		if (name == "R22" || name == "r22") name = "$s6";
		if (name == "R23" || name == "r23") name = "$s7";
		if (name == "R24" || name == "r24") name = "$t8";
		if (name == "R25" || name == "r25") name = "$t9";
		if (name == "R26" || name == "r26") name = "$k0";
		if (name == "R27" || name == "r27") name = "$k1";
		if (name == "R28" || name == "r28") name = "$gp";
		if (name == "R29" || name == "r29") name = "$sp";
		if (name == "R30" || name == "r30") name = "$fp";
		if (name == "R31" || name == "r31") name = "$ra";

		return regs[name];
	}

	void Register::print()
	{
		for (map<string, bitset<32>>::iterator iter = regs.begin(); iter != regs.end(); iter++)
			cout << iter->first << ":" << iter->second.to_ulong() << endl;
	}

	map<string,bitset<32>> Register::get_regs()
	{
		return regs;
	}

	void Register::reset()
	{
		Register newone;
		regs = newone.get_regs();
	}
}
