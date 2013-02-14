/*
* hex_file.c, part of gSXprog
*
* Copyright 2001-2003 Eric Smith <eric@brouhaha.com>
*
* $Id: hex_file.c,v 1.5 2003/11/18 13:59:42 garlick Exp $
*/


#include <stdio.h>
#include <termios.h>
#include "hex_file.h"

int HexFile::parse_hex_digit( char **p )
{
	int r = -1;
	if ( ( ( **p ) >= '0' ) && ( ( **p ) <= '9' ) )
		r = **p - '0';
	else if ( ( ( **p ) >= 'A' ) && ( ( **p ) <= 'F' ) )
		r = 10 + **p - 'A';
	else if ( ( ( **p ) >= 'a' ) && ( ( **p ) <= 'f' ) )
		r = 10 + **p - 'a';
	( *p ) ++;
	return ( r );
}

int HexFile::parse_hex_digits( char **p, int digits, u8 * checksum )
{
	int r = 0;
	int d;
	while ( digits-- )
	{
		d = parse_hex_digit( p );
		if ( d < 0 )
			return ( -1 );
		r = ( r << 4 ) + d;
		if ( checksum && !( digits & 1 ) )
			( *checksum ) += ( r & 0xff );
	}
	return ( r );
}

bool HexFile::readFile( char *fn, sx_t * sx )
{
	bool r = 0;
	FILE *f = NULL;
	char *p;
	char buf[ 100 ];
	int i;
	u8 checksum;
	int byte_count;
	int address;
	int record_type;
	int byte;

	f = fopen( fn, "r" );
	if ( !f )
	{
		perror( fn );
		goto error;
	}

	sx->rom[ FUSEX_LOC ] = 0xfff;
	sx->rom[ FUSE_LOC ] = 0xfff;

	for ( i = 0; i < SX_MAX_ROM_SIZE; i++ )
		sx->rom[ i ] = 0xfff;

	for ( i = 0; i < SX_ID_SIZE; i++ )
		sx->rom[ ID_LOC + i ] = 0xfff;

	while ( fgets( &buf[ 0 ], sizeof( buf ), f ) )
	{
		checksum = 0;
		p = &buf[ 0 ];
		while ( ( *p ) && ( ( *p ) != ':' ) )
			p++;
		if ( *p != ':' )
		{
			fprintf( stderr, "warning: spurious line in hex file\n"
			       );
			continue;
		}
		p++;
		byte_count = parse_hex_digits( &p, 2, &checksum );
		if ( byte_count < 0 )
		{
			fprintf( stderr,
			         "error reading hex file: bogus byte count\n"
			       );
			goto error;
		}
		address = parse_hex_digits( &p, 4, &checksum );
		if ( address < 0 )
		{
			fprintf( stderr, "error reading hex file: bogus address\n"
			       );
			goto error;
		}
		record_type = parse_hex_digits( &p, 2, &checksum );
		if ( record_type < 0 )
		{
			fprintf( stderr,
			         "error reading hex file: bogus record type\n"
			       );
			goto error;
		}
		if ( record_type > 1 )
		{
			/* Ignore expected extra gpasm data, but warn about others */
			if ( ( record_type == 0x04 ) && ( byte_count == 0x02 ) &&
			        ( address == 0x0000 ) )
				continue;
			fprintf( stderr,
			         "warning: unknown record type %02x in hex file\n",
			         record_type );
			continue;
		}
		if ( ( record_type == 1 ) && ( byte_count != 0 ) )
		{
			fprintf( stderr,
			         "error reading hex file: end record with data bytes\n"
			       );
			goto error;
		}
		for ( i = 0; i < byte_count; i++ )
		{
			byte = parse_hex_digits( &p, 2, &checksum );
			if ( byte < 0 )
			{
				fprintf( stderr,
				         "error reading hex file: bogus data byte for addr %04x\n",
				         address + i );
				goto error;
			}
			if ( ( address + i ) > ( ARRAY_SIZE * 2 ) )
			{
				fprintf( stderr,
				         "error reading hex file: address %04x out of boundsx\n",
				         address + i );
				goto error;
			}
			if ( ( address + i ) & 1 )
				sx->rom[ ( address + i ) / 2 ] =
				    ( sx->rom[ ( address + i ) / 2 ] & 0x00ff )
				    + ( byte << 8 );
			else
				sx->rom[ ( address + i ) / 2 ] =
				    ( sx->rom[ ( address + i ) / 2 ] & 0xff00 )
				    + ( byte );
		}
		byte = parse_hex_digits( &p, 2, &checksum );
		if ( byte < 0 )
		{
			fprintf( stderr, "error reading hex file: bogus checksum\n" );
			goto error;
		}
		if ( checksum != 0 )
		{
			fprintf( stderr,
			         "error reading hex file: incorrect checksum\n" );
			goto error;
		}
		if ( byte_count == 0 )
			break;              /* logical end of file */
	}

	r = 1;

error:
	if ( f )
		fclose( f );
	return ( r );
}


void HexFile::writeByte( FILE * f, u8 byte, u8 * checksum )
{
	fprintf( f, "%02X", byte );
	if ( checksum )
		*checksum += byte;
}

bool HexFile::writeFile( char *fn, sx_t * sx )
{
	FILE *f = NULL;
	int r = 0;
	int i, j;
	u8 checksum;

	f = fopen( fn, "w" );
	if ( !f )
	{
		perror( fn );
		goto error;
	}
	for ( i = 0; i < SX_MAX_ROM_SIZE; i += 8 )
	{
		checksum = 0;
		fprintf( f, ":" );
		writeByte( f, 0x10, &checksum );     /* byte count */
		writeByte( f, ( i * 2 ) >> 8, &checksum );     /* address high */
		writeByte( f, ( i * 2 ) & 0xff, &checksum );   /* address low */
		writeByte( f, 0x00, &checksum );     /* record type */
		for ( j = i; j < ( i + 8 ); j++ )
		{
			writeByte( f, sx->rom[ j ] & 0xff, &checksum );
			writeByte( f, sx->rom[ j ] >> 8, &checksum );
		}
		writeByte( f, 256 - checksum, NULL );
		fprintf( f, "\n" );
	}
	for ( i = ID_LOC; i < ID_LOC + SX_ID_SIZE; i += 8 )
	{
		checksum = 0;
		fprintf( f, ":" );
		writeByte( f, 0x10, &checksum );     /* byte count */
		writeByte( f, ( i * 2 ) >> 8, &checksum );     /* address high */
		writeByte( f, ( i * 2 ) & 0xff, &checksum );   /* address low */
		writeByte( f, 0x00, &checksum );     /* record type */
		for ( j = i; j < ( i + 8 ); j++ )
		{
			writeByte( f, sx->rom[ j ] & 0xff, &checksum );
			writeByte( f, sx->rom[ j ] >> 8, &checksum );
		}
		writeByte( f, 256 - checksum, NULL );
		fprintf( f, "\n" );
	}

	checksum = 0;
	fprintf( f, ":" );
	writeByte( f, 0x04, &checksum ); /* byte count */
	writeByte( f, ( FUSE_LOC * 2 ) >> 8, &checksum );  /* address high */
	writeByte( f, ( FUSE_LOC * 2 ) & 0xff, &checksum );        /* address low */
	writeByte( f, 0x00, &checksum ); /* record type */
	for ( j = FUSE_LOC; j <= FUSEX_LOC; j++ )
	{
		writeByte( f, sx->rom[ j ] & 0xff, &checksum );
		writeByte( f, sx->rom[ j ] >> 8, &checksum );
	}
	writeByte( f, 256 - checksum, NULL );
	fprintf( f, "\n" );

	checksum = 0;
	fprintf( f, ":" );
	writeByte( f, 0x00, &checksum ); /* byte count */
	writeByte( f, 0x00, &checksum ); /* address high */
	writeByte( f, 0x00, &checksum ); /* address low */
	writeByte( f, 0x01, &checksum ); /* record type */
	writeByte( f, 256 - checksum, NULL );
	fprintf( f, "\n" );

	r = 1;

error:
	if ( f )
		fclose( f );
	return ( r );
}

/*
 * em:Local Variables: 
 * em:indent-tabs-mode:nil
 * em:c-basic-offset:4
 * em:End:
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
