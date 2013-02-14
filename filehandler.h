/*
* =====================================================================================
* 
*        Filename:  filehandler.h
* 
*     Description:  adapts to sx_t
* 
*         Version:  1.0
*         Created:  14.07.2005 22:53:03 CEST
*        Revision:  none
*        Compiler:  gcc
* 
*          Author:  Rainer Blessing 
*         Company:  
*           Email:  rainer@theblessing.net
* 
* =====================================================================================
*/
#ifndef SIMHEXFILE_H
#define SIMHEXFILE_H

#include "hex_file.h"

class FileHandler: HexFile
{
public:
	bool readFile( char *fn, u16 * sx );
};
#endif
