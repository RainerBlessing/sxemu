/*
* =====================================================================================
* 
*        Filename:  cpu.cpp
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  13.07.2005 23:17:59 CEST
*        Revision:  none
*        Compiler:  gcc
* 
*          Author:  Rainer Blessing (), rainer@theblessing.net
*         Company:  
* 
* =====================================================================================
*/
#include <string.h>
#include <sstream>
#include <string>
//#include "Windows.h"

using namespace std;
#include "cpu.h"

Cpu::Cpu ()
{
	rtcc = &RAM[1];
	status = &RAM[3];
	fsr = &RAM[4];
	ra = &RAM[5];
	rb = &RAM[6];
	rc = &RAM[7];
	rd = &RAM[8];
	re = &RAM[9];
	w = &RAM[261];
	reset();
}

void Cpu::reset()
{
	memset (RAM, 0, 262);
	memset (SRAM, 0, 131072*sizeof(unsigned char));
	cycle = 0;
	cycle2 = 0;
	prescaler = 0;
	sp = Stack;
	pc = 0xfff;
	mode = 0;
}

USHORT *
Cpu::getROM ()
{
	return ROM;
}

unsigned char *
Cpu::getRAM ()
{
	return RAM;
}

USHORT *
Cpu::getPC ()
{
	return &pc;
}

unsigned char *
Cpu::getMode ()
{
	return &mode;
}
string Cpu::lookup(USHORT opc)
{
	string
	op = "";
	ostringstream outs;
	USHORT
	mne = opc;
	if (mne == 0xfff)
		return "unused";

	if (mne >> 9 == 0x5)
	{				//jmp
		outs << "jmp " << hex << (mne & 0x1ff);
		op = outs.str();
		return op;
	}
	else if (mne >> 8 == 0xc)
	{				//mov w,lit
		outs << "mov w, #" << hex << (mne & 0xff);
		op = outs.str();

	}
	else if (mne >> 5 == 1)
	{				//mov fr,w
		outs << "mov " << hex << (mne & 0x1f) << ", w";
		op = outs.str();

	}
	else if (mne >> 8 == 5)
	{				//setb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "setb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();

	}
	else if (mne >> 5 == 0xf)
	{				//add fr,w
		outs << "add " << hex << (mne & 0x1f) << ", w";
		op = outs.str();

	}
	else if (mne >> 5 == 0xe)
	{				//add w,fr

		outs << "add w, " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 8 == 6)
	{				//snb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "snb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();

	}
	else if (mne >> 5 == 11)
	{				//and fr,w
		outs << "and " << hex << (mne & 0x1f) << ", w";
		op = outs.str();

	}
	else if (mne >> 5 == 10)
	{				//and w,fr
		outs << "and w, " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 8 == 0xe)
	{				//and w,#lit
		outs << "and w, #" << hex << (mne & 0xff);
		op = outs.str();

	}
	else if (mne >> 5 == 0x13)
	{				//not fr
		outs << "not " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 9)
	{				//or fr,w
		outs << "or " << hex << (mne & 0x1f) << ", w";
		op = outs.str();

	}
	else if (mne >> 8 == 0xd)
	{				//or w,#lit
		outs << "or w, #" << hex << (mne & 0xff);
		op = outs.str();

	}
	else if (mne >> 5 == 8)
	{				//or w,fr
		outs << "or w, " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 13)
	{				//xor fr,w
		outs << "xor " << hex << (mne & 0x1f) << ", w";
		op = outs.str();

	}
	else if (mne >> 5 == 12)
	{				//xor w,fr
		outs << "xor w, " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 8 == 0xf)
	{				//xor w,#lit
		outs << "xor w, #" << hex << (mne & 0xff);
		op = outs.str();

	}
	else if (mne >> 5 == 3)
	{				//clr fr
		outs << "clr " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne == 0x40)
	{				//clr w
		op = "clr w";
	}
	else if (mne == 4)
	{				//clr !WDT
		op = "clr !WDT";
	}
	else if (mne >> 5 == 7)
	{				//dec fr
		outs << "dec " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 8 == 7)
	{				//sb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "sb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();

	}
	else if (mne >> 5 == 0x17)
	{				//decsz fr
		outs << "decsz " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x15)
	{				//inc fr
		outs << "inc " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x1f)
	{				//incsz fr
		outs << "incsz " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x1b)
	{				//RL fr

		outs << "RL " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x19)
	{				//RR fr
		outs << "RR " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 5)
	{				//sub fr,w
		outs << "sub " << hex << (mne & 0x1f) << " w";
		op = outs.str();

	}
	else if (mne >> 5 == 0x1d)
	{				//swap fr
		outs << "swap " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 8 == 4)
	{				//clrb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "clrb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();

	}
	else if (mne >> 5 == 0x10)
	{				//mov w,fr
		outs << "mov w, " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x4)
	{				//mov w,fr-w
		outs << "mov w, " << hex << (mne & 0x1f) << "-w";
		op = outs.str();

	}
	else if (mne >> 5 == 0x12)
	{				//mov w,/fr (complement)
		outs << "mov w, /" << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x6)
	{				//mov w,--fr
		outs << "mov w, --" << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x14)
	{				//mov w,++fr
		outs << "mov w, ++" << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x1a)
	{				//mov w,<<fr
		outs << "mov w, <<" << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x18)
	{				//mov w,>>fr
		outs << "mov w, <<" << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne == 0x43)
	{				//TODO: mov m,w
		outs << "mov m, w" ;
		op = outs.str();

	}
	else if (mne == 0x42)
	{				//TODO: mov w,m
		outs << "mov w, m" << hex << (mne & 0x1f);
		op = outs.str();

	}

	else if (mne >> 5 == 0x16)
	{				//movsz w,--fr
		outs << "movsz w, --" << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 0x1e)
	{				//movsz w,++fr
		outs << "movsz w, ++" << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne == 0x43)
	{ //mov m,w
		outs << "mov m, w";
		op = outs.str();

	}
	else if (mne >> 4 == 0x5)
	{ //mov m,#lit
		outs << "mov m, #" << hex << (mne&0xf);
		op = outs.str();

	}
	else if (mne == 0x2)
	{ //mov !option,w
		outs << "mov !option, w";
		op = outs.str();

	}
	else if (mne > 4 && mne < 10)
	{
		outs << "mov !rx, w";
		op = outs.str();

	}
	else if (mne == 2)
	{ //mov !option,w
		outs << "not impl: mov !option, w";
		op = outs.str();

	}

	else if (mne >> 5 == 0x11)
	{				//test fr
		outs << "test " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne == 0)
	{				//nop
		outs << "nop";
		op = outs.str();

	}
	else if (mne >> 8 == 9)
	{ //call addr8
		outs << "call " << hex << ((*status & 0xe0) << 4 | mne & 0xff);
		op = outs.str();

	}
	else if (mne == 0xc)
	{ //ret
		outs << "ret";
		op = outs.str();
	}
	else if (mne == 0xd)
	{ //retp
		outs << "retp";
		op = outs.str();

	}
	else if (mne >> 3 == 3)
	{ //bank addr12
		outs << "bank " << hex << (mne&7);
		op = outs.str();

	}
	else if (mne >> 3 == 2)
	{ //page addr12
		outs << "page " << hex << (mne&7);
		op = outs.str();

	}
	else if (mne == 0x41)
	{ //IREAD
		outs << "IREAD";
		op = outs.str();
	}
	else if (mne == 0xe)
	{ //TODO: reti
		outs << "not impl: reti";
		op = outs.str();
	}
	else if (mne == 0xf)
	{ //TODO: retiw
		outs << "not impl: retiw";
		op = outs.str();
	}
	else if (mne >> 8 == 8)
	{ //retw #lit
		outs << "retw " << hex << (mne&0xff);
		op = outs.str();
	}
	else if (mne == 3)
	{ //TODO: SLEEP
		outs << "SLEEP";
		op = outs.str();
	}



	else
	{
		outs << "not impl " << hex << mne;
		op = outs.str();
	}
	return op;

}

string Cpu::lookup()
{
	string
	op = "";
	ostringstream outs;
	USHORT
	mne = ROM[pc];
	if (mne == 0xfff)
	{
		return "END";
	}
	if (mne >> 8 == 0xa)
	{				//jmp
		outs << "jmp " << hex << (mne & 0x1ff);
		op = outs.str();
		return op;
	}
	else if (mne >> 8 == 0xb)
	{				//jmp
		outs << "jmp " << hex << (mne & 0x1ff);
		op = outs.str();
		return op;
	}
	else if (mne >> 8 == 0xc)
	{				//mov w,lit
		outs << "mov w, #" << hex << (mne & 0xff);
		op = outs.str();
	}
	else if (mne >> 5 == 1)
	{				//mov fr,w
		outs << "mov " << hex << (mne & 0x1f) << ", w";
		op = outs.str();
	}
	else if (mne >> 8 == 5)
	{				//setb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "setb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();

	}
	else if (mne >> 5 == 0xf)
	{				//add fr,w
		outs << "add " << hex << (mne & 0x1f) << ", w";
		op = outs.str();
	}
	else if (mne >> 5 == 0xe)
	{				//add w,fr
		outs << "add w, " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 8 == 6)
	{				//snb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "snb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();
	}
	else if (mne >> 5 == 11)
	{				//and fr,w
		outs << "and " << hex << (mne & 0x1f) << ", w";
		op = outs.str();
	}
	else if (mne >> 5 == 10)
	{				//and w,fr
		outs << "and w, " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 8 == 0xe)
	{				//and w,#lit
		outs << "and w, #" << hex << (mne & 0xff);
		op = outs.str();
	}
	else if (mne >> 5 == 0x13)
	{				//not fr
		outs << "not " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 9)
	{				//or fr,w
		outs << "or " << hex << (mne & 0x1f) << ", w";
		op = outs.str();
	}
	else if (mne >> 8 == 0xd)
	{				//or w,#lit
		outs << "or w, #" << hex << (mne & 0xff);
		op = outs.str();
	}
	else if (mne >> 5 == 8)
	{				//or w,fr
		outs << "or w, " << hex << (mne & 0x1f);
		op = outs.str();

	}
	else if (mne >> 5 == 13)
	{				//xor fr,w
		outs << "xor " << hex << (mne & 0x1f) << ", w";
		op = outs.str();
	}
	else if (mne >> 5 == 12)
	{				//xor w,fr
		outs << "xor w, " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 8 == 0xf)
	{				//xor w,#lit
		outs << "xor w, #" << hex << (mne & 0xff);
		op = outs.str();
	}
	else if (mne >> 5 == 3)
	{				//clr fr
		outs << "clr " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne == 0x40)
	{				//clr w
		op = "clr w";
	}
	else if (mne == 4)
	{				//clr !WDT
		op = "clr !WDT";
	}
	else if (mne >> 5 == 7)
	{				//dec fr
		outs << "dec " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 8 == 7)
	{				//sb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "sb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();
	}
	else if (mne >> 5 == 0x17)
	{				//decsz fr
		outs << "decsz " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x15)
	{				//inc fr
		outs << "inc " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x1f)
	{				//incsz fr
		outs << "incsz " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x1b)
	{				//RL fr
		outs << "RL " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x19)
	{				//RR fr
		outs << "RR " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 5)
	{				//sub fr,w
		outs << "sub " << hex << (mne & 0x1f) << " w";
		op = outs.str();
	}
	else if (mne >> 5 == 0x1d)
	{				//swap fr
		outs << "swap " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 8 == 4)
	{				//clrb fr.bit
		int
		bit = (mne >> 5) & 7;
		outs << "clrb " << hex << (mne & 0x1f) << "." << bit;
		op = outs.str();
	}
	else if (mne >> 5 == 0x10)
	{				//mov w,fr
		outs << "mov w, " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x4)
	{				//mov w,fr-w
		outs << "mov w, " << hex << (mne & 0x1f) << "-w";
		op = outs.str();
	}
	else if (mne >> 5 == 0x12)
	{				//mov w,/fr (complement)
		outs << "mov w, /" << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x6)
	{				//mov w,--fr
		outs << "mov w, --" << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x14)
	{				//mov w,++fr
		outs << "mov w, ++" << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x1a)
	{				//mov w,<<fr
		outs << "mov w, <<" << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x18)
	{				//mov w,>>fr
		outs << "mov w, <<" << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x1c)
	{				//mov w,<>fr
		outs << "mov w, <>" << hex << (mne & 0x1f);
		op = outs.str();
	}
	//TODO: mov w,m
	else if (mne >> 5 == 0x16)
	{				//movsz w,--fr
		outs << "movsz w, --" << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne >> 5 == 0x1e)
	{				//movsz w,++fr
		outs << "movsz w, ++" << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne == 0x43)
	{ //mov m,w
		outs << "mov m, w";
		op = outs.str();
	}
	else if (mne >> 4 == 0x5)
	{ //mov m,#lit
		outs << "mov m, #" << hex << (mne&0xf);
		op = outs.str();
	}
	else if (mne == 0x2)
	{ //mov !option,w
		outs << "mov !option, w";
		op = outs.str();
	}
	else if (mne > 4 && mne < 10)
	{
		outs << "mov !rx, w";
		op = outs.str();
	}
	//mov !option,w
	else if (mne >> 5 == 0x11)
	{				//test fr
		outs << "test " << hex << (mne & 0x1f);
		op = outs.str();
	}
	else if (mne == 0)
	{				//nop
		outs << "nop";
		op = outs.str();
	}
	else if (mne >> 8 == 9)
	{ //call addr8
		outs << "call " << hex << ((*status & 0xe0) << 4 | mne & 0xff);
		op = outs.str();
	}
	else if (mne == 0xc)
	{ //ret
		outs << "ret";
		op = outs.str();
	}
	else if (mne == 0xd)
	{ //retp
		outs << "retp";
		op = outs.str();
	}
	else if (mne >> 8 == 0x8)
	{ //retw #lit
		outs << "retw " << hex << (mne&0xff);
		op = outs.str();
	}
	else if (mne >> 3 == 3)
	{ //bank addr12
		outs << "bank " << hex << (mne&7);
		op = outs.str();
	}
	else if (mne >> 3 == 2)
	{ //page addr12
		outs << "page " << hex << (mne&7);
		op = outs.str();
	}
	else if (mne == 0x41)
	{ //IREAD
		outs << "IREAD";
		op = outs.str();
	}
	else
	{
		outs << "not impl " << hex << mne;
		op = outs.str();
	}
	return op;
}

bool Cpu::execute ()
{
	USHORT mne = ROM[pc];
	USHORT bit;
	USHORT fr;
	USHORT a;
	prescaler += (cycle - cycle2);
	if (prescaler > (option&0x7))
	{
		RAM[1]++;
		prescaler = 0;
	}
	cycle2 = cycle;

	switch (mne >> 8)
	{
			case 0xa:     			//jmp 101k kkkk kkkk
			pc = ((*status & 0xe0) << 4) | (mne & 0x1ff);
			cycle += 3;
			return true;
			case 0xb:  			//jmp 101k kkkk kkkk
			pc = ((*status & 0xe0) << 4) | (mne & 0x1ff);
			cycle += 3;
			return true;
			case 0xc:     				//mov w,lit 1100 kkkk kkkk
			*w = (unsigned char)(mne & 0xff);
			cycle++;
			pc++;
			return true;
			case 0x5:     				//setb fr.bit 0111 bbbf ffff
			bit = (mne >> 5) & 7;
			fr = mne & 0x1f;
			if (fr == 0)
				fr = *fsr;
			else if (fr > 0xf)
				fr = (*fsr & 0xf0) | (fr & 0xf);

			RAM[fr] += 1 << bit;

			if (fr == 7)
			{
				sram();
			}

			cycle++;
			pc++;
			return true;
			case 0x6:     				//snb fr.bit 0110 bbbf ffff
			bit = (mne >> 5) & 7;
			fr = mne & 0x1f;
			if (fr == 0)
				fr = *fsr;
			else if (fr > 0xf)
				fr = (*fsr & 0xf0) | (fr & 0xf);

			if (!(RAM[fr] >> bit & 1))
			{
				pc++;	//skip if fr==0
				cycle++;
				while (ROM[pc] >> 3 == 2 || ROM[pc] >> 3 == 3)
				{
					pc++;	//skip if page/bank
					cycle++;
				}
			}
			cycle++;
			pc++;
			return true;
			case 0xe:     				//and w,#lit 1110 kkkk kkkk
			fr = mne & 0xff;
			*w = *w & fr;
			if (*w == 0) //set zero
				*status |= 1 << 2;
			else
			{
				*status &= ~(1 << 2);
			}
			cycle++;
			pc++;
			return true;
			case 0xd:     			//or w,#lit 1101 kkkk kkkk
			fr = mne & 0xff;
			*w = *w | fr;
			if (*w == 0)
				*status |= 1 << 2; //set zero
			else
			{
				USHORT cmp = 1 << 2;
				*status &= ~cmp;
			}
			cycle++;
			pc++;
			return true;
			case 0xf:     	//xor w,#lit 1111 kkkk kkkk
			*w ^= mne & 0xff;
			if (*w == 0)
				*status |= 1 << 2; //set zero
			else
			{
				*status &= ~(1 << 2);
			}
			cycle++;
			pc++;
			return true;
			case 0x7:     //sb fr.bit 0111 bbbf ffff
			fr = mne & 0x1f;
			if (fr == 0)
				fr = *fsr;
			else if (fr > 0xf)
				fr = (*fsr & 0xf0) | (fr & 0xf);

			bit = (mne >> 5) & 7;
			if (((RAM[fr] >> bit) & 1) == 1)
			{
				pc++;	//skip if fr==0
				cycle++;
				while (ROM[pc] >> 3 == 2 || ROM[pc] >> 3 == 3)
				{
					pc++;	//skip if page/bank
					cycle++;
				}
			}
			cycle++;
			pc++;
			return true;
			case 0x4:     		//clrb fr.bit 0100 bbbf ffff
			bit = (mne >> 5) & 7;
			fr = mne & 0x1f;
			if (fr == 0)
				fr = *fsr;
			else if (fr > 0xf)
				fr = (*fsr & 0xf0) | (fr & 0xf);

			RAM[fr] &= (~ (1 << bit));

			if (fr == 7)
			{
				sram();
			}

			cycle++;
			pc++;
			return true;
			case 0x9:     //call addr8 1001 kkkk kkkk
			*sp = pc + 1;
			if (sp == Stack + 8)
			{
				sp = Stack;
			}
			else sp++;
			pc = (*status & 0xe0) << 4 | mne & 0xff;
			cycle += 3;
			return true;
			case 0x8:     //retw #lit 1000 kkkk kkkk
			if (sp == Stack)
			{
				sp = Stack + 8;
			}
			else sp--;
			pc = ((*status & 0xe0) << 4) | *sp;
			*w = mne & 0xff;
			cycle += 3;
			return true;
			default:
			switch (mne >> 5)
			{
					case 0x1:     				//mov fr,w 0000 001f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] = *w;
					cycle++;
					pc++;
					return true;
					case 0xf:     				//add fr,w 0001 111f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);
					RAM[fr] > *w ? a = RAM[fr] : a = *w;
					if (a >> 4 < ((*w) + RAM[fr]) >> 4)
					{
						*status |= 1 << 1;	//set DC
					}
					else
					{
						*status &= ~(1 << 1);
					}
					a = RAM[fr] + *w;
					if (a > 0xff)
						*status |= 1;	//set carry
					else
					{
						*status &= ~1;
					}
					RAM[fr] = a & 0xff;

					if (RAM[fr] == 0)
						*status |= 1 << 2;	// set zero
					else
					{
						*status &= ~(1 << 2);
					}

					cycle++;
					pc++;
					return true;
					case 0xe:     				//add w,fr 0001 110f fff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] > *w ? a = RAM[fr] : a = *w;
					if (a >> 4 < ((*w) + RAM[fr]) >> 4)
						*status |= 1 << 1;	//set DC
					else
					{
						*status &= ~(1 << 1);
					}
					a = *w + RAM[fr];
					if (a > 0xff)
						*status |= 1;	//set carry
					else
					{
						USHORT cmp = 1;
						*status &= ~cmp;
					}
					*w = a & 0xff;

					if (*w == 0)
						*status |= 1 << 2;	// set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0xb:     				//and fr,w 0001 011f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);
					RAM[fr] = RAM[fr] & *w;
					if (RAM[fr] == 0)
						*status |= 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0xa:     				//and w,fr 0001 010f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = *w & RAM[fr];
					if (*w == 0)
						*status |= 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x13:     					//not fr 0010 011f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] = ~RAM[fr];
					if (RAM[fr] == 0)
						*status |= 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x9:     				//or fr,w	 001 001f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] |= *w;
					if (RAM[fr] == 0)
						*status |= 1 << 2;
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x8:     				//or w,fr 0001 000f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w |= RAM[fr];
					if (*w == 0)
						*status = 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0xd:     			//xor fr,w 0001 101f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] ^= *w;
					if (RAM[fr] == 0)
						*status |= 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0xc:     				//xor w,fr 0001 100f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w ^= RAM[fr];
					if (*w == 0)
						*status |= 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x3:     		//clr fr 0000 011f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] = 0;
					*status |= 1 << 2;
					cycle++;
					pc++;
					return true;
					case 0x7:     			//dec fr 0000 111f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] -= 1;
					if (RAM[fr] == 0)
						*status |= 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x17:     			//decsz fr 0010 111f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] -= 1;
					if (RAM[fr] == 0)
					{
						*status |= 1 << 2; //set zero
						pc++;	//skip if fr==0
						cycle++;
						while (ROM[pc] >> 3 == 2 || ROM[pc] >> 3 == 3)
						{
							pc++;	//skip if page/bank
							cycle++;
						}
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x15:     				//inc fr 0010 101f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr]++;

					if (RAM[fr] == 0)
						*status |= 1 << 2; //set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x1f:     			//incsz fr 0011 111f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] ++;
					if (RAM[fr] == 0)
					{
						*status |= 1 << 2; //set zero
						pc++;	//skip if fr==0
						cycle++;
						while (ROM[pc] >> 3 == 2 || ROM[pc] >> 3 == 3)
						{
							pc++;	//skip if page/bank
							cycle++;
						}
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x1b:     		//RL fr	0011 011f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					if (RAM[fr] >> 7 == 1)
					{
						a = (RAM[fr] << 1) | (*status & 1);
						*status |= 1; //set carry
					}
					else
					{
						a = (RAM[fr] << 1) | (*status & 1);
						*status &= ~1;
					}
					RAM[fr] = a & 0xff;
					cycle++;
					pc++;
					return true;
					case 0x19:     					//RR fr 0011 001f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					if (RAM[fr]&0x1 == 0x1)
					{
						RAM[fr] = (RAM[fr] >> 1) | ((*status & 1) << 7);
						*status |= 1; //set carry
					}
					else
					{
						RAM[fr] = (RAM[fr] >> 1) | ((*status & 1) << 7);
						*status &= ~1;
					}
					cycle++;
					pc++;
					return true;
					case 0x5:     					//sub fr,w 0000 101f ffff
					//TODO: complement of carry flag ...
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					RAM[fr] > *w ? a = RAM[fr] : a = *w;
					if (a >> 4 < ((*w) + RAM[fr]) >> 4)
						*status |= 1 << 1;	//set DC
					else
					{
						*status &= ~(1 << 1);
					}
					if (RAM[fr] >= *w)
						*status |= 1;	//set carry
					else
					{
						*status &= ~1;
					}
					RAM[fr] -= *w;

					if (*w == 0)
						*status |= 1 << 2;	// set zero
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x1d:     			//swap fr 0011 101f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					static int l;
					l = RAM[fr] & 0xf;
					RAM[fr] = (RAM[fr] >> 4) | (l << 4);
					cycle++;
					pc++;
					return true;
					case 0x10:     				//mov w,fr 0010 000f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = RAM[fr];
					if (*w == 0)
					{
						*status |= 1 << 2;	// set zero
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x4:     				//mov w,fr-w 0000 100f ffff
					//TODO: CF, complement ...
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					if (RAM[fr] >= *w)
						*status |= 1;	//set carry
					else
					{
						*status &= ~1;
					}
					*w = RAM[fr] - *w;

					if (*w == 0)
					{
						*status |= 1 << 2;	// set zero
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x12:     			//mov w,/fr (complement) 0010 010f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = ~RAM[fr];
					if (*w == 0)
					{
						*status |= 1 << 2;	// set zero
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x6:     					//mov w,--fr 0000 110f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = RAM[fr] - 1;
					if (*w == 0)
					{
						*status |= 1 << 2;	// set zero
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x14:     	//mov w,++fr 0010 100f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = RAM[fr] + 1;
					if (*w == 0)
					{
						*status |= 1 << 2;	// set zero
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x1a:     		//mov w,<<fr 0011 010f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					a = (RAM[fr] << 1) & (*status & 1);
					if (a > 0xff == 0)
						*status |= 1; //set carry
					else
					{
						*status &= ~1;
					}
					*w = a & 0xff;
					cycle++;
					pc++;
					return true;
					case 0x18:     				//mov w,>>fr 0011 000f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					a = (RAM[fr] >> 1) & (*status & 1) << 7;
					if (a > 0xff)
						*status |= 1; //set carry
					else
					{
						*status &= ~1;
					}
					*w = a & 0xff;
					cycle++;
					pc++;
					return true;
					case 0x1c:     					//mov w,<>fr 0011 100f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = (RAM[fr] >> 4) & ((RAM[fr] & 0xf) << 4);
					cycle++;
					pc++;
					return true;

					case 0x16:     			//movsz w,--fr 0010 110f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = RAM[fr] - 1;
					if (*w == 0)
					{
						*status |= 1 << 2;	// set zero
						pc++;	//skip if fr==0
						cycle++;
						while (ROM[pc] >> 3 == 2 || ROM[pc] >> 3 == 3)
						{
							pc++;	//skip if page/bank
							cycle++;
						}
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x1e:     	//movsz w,++fr 0011 110f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					*w = RAM[fr] + 1;
					if (*w == 0)
					{
						*status |= 1 << 2;	// set zero
						cycle++;
						pc++;	//skip if fr==0
						cycle++;
						while (ROM[pc] >> 3 == 2 || ROM[pc] >> 3 == 3)
						{
							pc++;	//skip if page/bank
							cycle++;
						}
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					case 0x11:     	//test fr 0010 001f ffff
					fr = mne & 0x1f;
					if (fr == 0)
						fr = *fsr;
					else if (fr > 0xf)
						fr = (*fsr & 0xf0) | (fr & 0xf);

					if (RAM[fr] == 0)
					{
						*status |= 1 << 2;	// set zero
					}
					else
					{
						*status &= ~(1 << 2);
					}
					cycle++;
					pc++;
					return true;
					default:
					if (mne == 0x40)
					{				//clr w 0000 011f ffff
						*w = 0;
						*status |= 1 << 2;
						cycle++;

					}
					else if (mne == 4)
					{				//clr !WDT 0000 0000 0100
						//TODO
						//TO=1
						//PD=1
						cycle++;

					}
					else if (mne == 0x42)
					{ //mov w,m 0000 0100 0010
						*w = mode;
						cycle++;
					}
					else if (mne == 0x43)
					{ //mov m,w 0000 0100 0011
						mode = *w;
						cycle++;
					}

					else if (mne >> 4 == 0x5)
					{ //mov m,#lit 0000 0101 kkkk
						mode = mne & 0xf;
						cycle++;
					}
					else if (mne == 0x2)
					{ //mov !option,w 0000 0000 0010
						option = *w;
						cycle++;
					}
					else if (mne > 0x4 && mne < 0xa)
					{
						//mov !rx,w
						if (mode == 0xf)
						{
							Tris[mne - 5] = *w;
						}
						option = *w;
						cycle++;
					}
					else if (mne == 0)
					{				//nop
						cycle++;
					}
					else if (mne == 0xc)
					{ // ret 0000 0000 1100
						if (sp == Stack)
						{
							sp = Stack + 8;
						}
						else sp--;
						pc = (*status & 0xe0) << 4 | *sp;
						cycle += 3;
					}
					else if (mne == 0xd)
					{ //retp 0000 0000 1101
						if (sp == Stack)
						{
							sp = Stack + 8;
						}
						else sp--;
						pc = *sp;
						USHORT page = *sp >> 4;
						RAM[3] = page | (RAM[3] & 0x1f);
						cycle += 3;
						return true;
					}
					else if (mne >> 3 == 3)
					{ //bank addr12 0000 0001 1nnn
						*fsr = ((mne & 7) << 4) | (*fsr & 0xf);
						cycle++;
					}
					else if (mne >> 3 == 2)
					{ //page addr12 0000 0001 0nnn
						*status = ((mne & 7) << 5) | (*status & 0x1f);
						cycle++;
					}
					else if (mne == 0x41)
					{ //IREAD 0000 0100 0001
						a = ((mode & 0xf) << 8) | *w;
						a = ROM[a] & 0xfff;
						mode = (mode & 0xf0) | (a >> 8);

						*w = a & 0xff;
						cycle += 3;
					}
					/*else if (mne>> == 0x41)
					{ //not w 0010 011f ffff
						*w = ~0xff;
						if (*w == 0)
							*status |= 1 << 2;
						else
						{
							USHORT cmp = 1 << 2;
							*status &= ~cmp;
						}
						cycle++;
					}*/


					//TODO: reti
					//TODO: retiw
					//TODO: SLEEP


					else
					{
						cycle++;
					}
					pc++;

					return true;
			}
	}

}
void Cpu::sram()
{
	static USHORT page = 0;
	static USHORT address;
	static bool active = false;
	if ((*rc >> 4)&1 == 1)
	{
		page <<= 1; //12 bit
		page |= (*rc >> 5) & 1;
	}
	if (((*rc >> 6)&1) == 0)
	{ // chipenable=0
		address = ((*ra >> 7) & 1) << 16 | page << 4 | *rc & 0xf;

		if (*rc >> 7&1 == 0)
		{ //write
			SRAM[address] = *rd;
		}
		else
		{ //read
			RAM[11] = SRAM[address];
		}
		active = true;
	}
	if (((*rc >> 6)&1) == 1 && active == true)
	{ // chipenable=1
		page = 0;
		active = false;
	}
}
