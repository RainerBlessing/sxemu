/*
* SxKey.h, part of gSXprog
*
* Copyright 2001-2003 Eric Smith <eric@brouhaha.com>
*
* $Id: SxKey.h,v 1.5 2003/11/18 13:59:42 garlick Exp $
*/

#ifndef SXKEY_H
#define SXKEY_H

#include <string>

#define SX_52_MODE(dev)		(((dev) & 0xfff) == 2)  /* SX48/52 */
#define SX_28_MODE(dev)		(((dev) & 0xfff) != 2)  /* SX18/28 */

#define SXKEY_TYPE_NEW 		0       /* SX-Key Rev. E, Rev. F, SX-Blitz */
#define SXKEY_TYPE_OLD 		1       /* SX-Key Rev. C */

#define SX_MAX_ROM_SIZE 	4096
#define SX_ID_SIZE 		16

/* hex file offsets */
#define ID_LOC 			0x1000
#define FUSE_LOC 		0x1010
#define FUSEX_LOC 		0x1011

/* fusex bits for trimming internal RCA oscillator (modified if --trim) */
#define IRCTRIM0            0x100
#define IRCTRIM1            0x200
#define IRCTRIM2            0x800

/* --clockon limits */
#define MIN_CLOCK   0.4
#define MAX_CLOCK   120.
/* 20 MHz reference clock */
#define REF_CLOCK   20.

#define SXKEY_ECHO_TIMEOUT 	100

#define SXKEY_NEW_TIMEOUT                       (100) /* dflt SX-Key response timeout */
#define SXKEY_NEW_PROG_FUSE_TIMEOUT(n)  (100 + SX_NEW_FUSE_PROG_MS*(n))
#define SXKEY_NEW_PROG_CODE_TIMEOUT(n)  (100 + SX_NEW_CODE_PROG_MS*(n))

/* Programming times */
#define SX_NEW_ERASE_MS                    1000    /* 1...65536 */
#define SX_NEW_FUSE_PROG_MS                 250     /* 1...255 */
#define SX_NEW_CODE_PROG_MS                 100     /* 1...255 */

/* Firmware Revisions */
#define SXKEY_REV_E                     1
#define SXKEY_REV_F                     2
#define SXKEY_BLITZ                     3

/* SX-Key commands */
#define SXKEY_NEW_CMD_END                   0
#define SXKEY_NEW_CMD_ERASE                 1
#define SXKEY_NEW_CMD_PROGRAM               2
#define SXKEY_NEW_CMD_READ                  3
#define SXKEY_NEW_CMD_RESET                 4
#define SXKEY_NEW_CMD_DEBUG_POLL            5
#define SXKEY_NEW_CMD_DEBUG_RECV            6
#define SXKEY_NEW_CMD_DEBUG_SEND            7
#define SXKEY_NEW_CMD_CLOCK_ENABLE          8
#define SXKEY_NEW_CMD_CLOCK_DISABLE         9


#define SXKEY_DEFAULT_DEVICE    "/dev/ttyS0"

#define ARRAY_SIZE 		(SX_MAX_ROM_SIZE + SX_ID_SIZE + 2 + 256)
typedef struct
{
	u16 device;
	u16 rom[ ARRAY_SIZE ];
}
sx_t;

#define SXKEY_T_INIT {              \
                                    .cmd_read = NULL,               \
                                    .cmd_read_fuses = NULL,         \
                                    .cmd_prog = NULL,               \
                                    .cmd_erase = NULL,              \
                                    .cmd_clock_on = NULL,           \
                                    .cmd_clock_off = NULL,          \
                                    .cmd_reset = NULL,              \
                                    .fini = NULL,                   \
                                    .fw_rev = -1,                   \
                                    .fd = -1,                       \
                                    .speed = 0,                     \
                                    .device = SXKEY_DEFAULT_DEVICE, \
                     }
#define speed_t unsigned int

#define ABS(i)  ((i) < 0 ? -(i) : (i))
#define SXKEY_CLK_HZ_TARGET     4000000
#define SXKEY_CLK_HZ(rsp)       (1266035968L/(rsp))

class trim_t
{
public:
	u16 fusex[ 8 ];
	u16 freq[ 8 ];
};


class Fusebits
{
public:
	void clear()
	{
		f_turbo = 1; //sx28
		f_sync = 1;
		f_irc = 1;
		f_div = 0;
		f_ifbd = 1;
		f_xtl = 1; //sx52
		f_cp = 1;
		f_wdte = 0;
		f_osc = 3;
		fx_pins = 1; //sx28
		fx_optionx = 0; //sx28
		fx_sleep = 1; //sx52
		fx_cf = 1;
		fx_bor = 3;
		fx_bp = 3; //sx28
		fx_drt = 1; //sx52
		device = 1; // 0=sx28, 1=sx52
		//u16 deviceString;
	};


	bool f_turbo; //sx28
	bool f_sync;
	bool f_irc;
	char f_div;
	bool f_ifbd;
	bool f_xtl; //sx52
	bool f_cp;
	bool f_wdte;
	char f_osc;

	bool fx_pins; //sx28
	bool fx_optionx; //sx28
	bool fx_sleep; //sx52
	bool fx_cf;
	int fx_bor;
	char fx_bp; //sx28
	char fx_drt; //sx52
	bool device; // 0=sx28, 1=sx52
	u16 deviceString;
};

using std::string;
class SxKey
{
public:
	SxKey();
	SxKey( string device, speed_t bit_rate );
	~SxKey();
	bool sxopen();
	void sxclose();
	bool init();
	Fusebits* getFusebits();
	void saveFusebits();
	char* print();
	void save( char *fn );
	void load( char *fn );
	int  programMemoryProlog( bool trim, bool idskip );
	bool program_20( int i );
	bool programMemoryEpilog( bool idskip );
	int readMemoryProlog();
	bool readMemory( int i );
	bool readMemoryEpilog();
private:
	bool reconnect();
	void unsetkeys();
	void setkeys( u8 c1, u8 c2 );
	bool send_byte( u8 c );
	bool serial_write( u8 c );
	bool serial_read( int ms_timeout, u8 * c );
	u8 encrypt( u8 p );
	u8 decrypt( u8 c );
	void munge_keys();
	bool serial_flush();
	bool send_str( char *s );
	bool expect_str( int ms_timeout, char *s );
	bool get_byte( int ms_timeout, u8 * c );
	bool read_fuses();
	bool get_word( int ms_timeout, u16 * w );
	bool check_error( int ms_timeout );
	bool read_prolog();
	bool read_epilog();
	bool reset();
	void sx52_show_fuse();
	void sx28_show_fuse();

	bool read_id();
	int sx_rom_size();
	void prog_size( int *size, bool idskip );
	bool clock_off();
	bool trim_oscillator( u16 *fusex );
	bool erase();
	bool prog_fusex( u16 fusex, u16 *freq );
	bool prog_prolog();
	bool prog_epilog( u16 *freq );
	bool send_word( u16 w );
	void show_fuse_bits();
	bool prog_verify( bool idskip );
	void sx52_save_fuse();
	void sx28_save_fuse();

	string device;
	int fd;
	speed_t speed;
	bool _crypto_enabled;
	/* values used in stream encryption */
	u8 _crypto_key_1;
	u8 _crypto_key_2;
	u8 fw_rev;
	sx_t sx;
	Fusebits fusebits;
	char *dump;
} ;

#endif

/*
 * em:Local Variables: 
 * em:indent-tabs-mode:nil
 * em:c-basic-offset:4
 * em:End:
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
