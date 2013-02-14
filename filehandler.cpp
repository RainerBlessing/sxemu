/*
* =====================================================================================
* 
*        Filename:  filehandler.cpp
* 
*     Description:  adapts to sx_t
* 
*         Version:  1.0
*         Created:  14.07.2005 22:55:31 CEST
*        Revision:  none
*        Compiler:  gcc
* 
*          Author:  Rainer Blessing (), rainer@theblessing.net
*         Company:  
* 
* =====================================================================================
*/

#include "filehandler.h"

bool FileHandler::readFile( char *fn, u16 * rom )
{
	sx_t sx;
	HexFile::readFile(fn, &sx);
	memcpy(rom, sx.rom, 4096*2);
	return true;
}

