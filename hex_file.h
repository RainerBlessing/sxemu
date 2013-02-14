/*
* hex_file.c, part of gSXprog
*
* Copyright 2001-2003 Eric Smith <eric@brouhaha.com>
*
* $Id: hex_file.h,v 1.4 2003/11/18 13:59:42 garlick Exp $
*/

#ifndef HEXFILE_H
#define HEXFILE_H
#include <stdint.h>
typedef uint16_t u16;
typedef uint8_t u8;
typedef uint32_t u32;

#include "sxkey.h"


class HexFile
{
public:
	HexFile()
	{}
	;
	bool readFile( char *fn, sx_t * sx );
	bool writeFile( char *fn, sx_t * sx );
private:
	void writeByte( FILE * f, u8 byte, u8 * checksum );
	int parse_hex_digit( char** );
	int parse_hex_digits( char **p, int digits, u8 * checksum );
};


#endif

/*
 * em:Local Variables: 
 * em:indent-tabs-mode:nil
 * em:c-basic-offset:4
 * em:End:
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
