/*
* =====================================================================================
* 
*        Filename:  cpu.h
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  13.07.2005 23:17:20 CEST
*        Revision:  none
*        Compiler:  gcc
* 
*          Author:  Rainer Blessing 
*           Email:  rainer.blessing@gmail.com
* 
* =====================================================================================
*/
#ifndef CPU
#define CPU
#define USHORT unsigned short

class Cpu
{
public:
	Cpu();
	USHORT* getROM();
	unsigned char* getRAM();
	USHORT* getPC();
	unsigned char* getMode();
	string lookup();
	string lookup(USHORT opc);
	bool execute();
	void reset();
private:
	void sram();
	unsigned char RAM[262];
	USHORT ROM[4096];
	unsigned char SRAM[131072];
	unsigned char* indf;
	unsigned char* rtcc;
	USHORT pc;
	unsigned char* status;
	unsigned char* fsr;
	unsigned char* ra;
	unsigned char* rb;
	unsigned char* rc;
	unsigned char* rd;
	unsigned char* re;
	unsigned char* w;
	USHORT Stack[8];
	USHORT* sp;
	unsigned char mode;
	unsigned char option; //8 bit
	unsigned char Tris[5];
	int cycle;
	int cycle2;
	int prescaler;
};
#endif

