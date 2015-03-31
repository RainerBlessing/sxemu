using namespace std;
#include    <unistd.h>
#include 	<pthread.h>
#include	<iostream>
#include 	<curses.h>
#include	"filehandler.h"
#include 	<string.h>
#include	"cpu.h"

/*-----------------------------------------------------------------------------
 *  function main
 *-----------------------------------------------------------------------------*/

void dumpMem(char *dump, USHORT* mem, USHORT size);
void dumpRAM(char *dump, unsigned char* mem, USHORT size);
void screen();
bool keys();
void run();

Cpu cpu;
WINDOW *mainwin, *romwin, *ramwin, *sidewin;
string impl = "";
unsigned char* romptr;
unsigned char* ramptr;
USHORT* pc;
unsigned char memrow[46];
unsigned char ramrow[53];
int romr, ramr;
char awin; // tab changes window focus
unsigned char* RAM;
USHORT* ROM;
int row, col;
int cursorrow;
bool breakpoint[4096];
bool running = false;
pthread_t walkthread;

void *run(void *Arg)
{
	while (running == true)
	{
		romr = 35 * (*pc);
		cursorrow = romr / 35;
		if (cpu.execute() == false )
		{
			running = false;
			break;
		}
		if (*(char*)Arg == 'w' )usleep(500000);
		if (breakpoint[*pc] == true)
		{
			running = false;
			break;
		}
	}
	return NULL;
}

int
main (int argc, char *argv[])
{
	FileHandler fh;

	RAM = cpu.getRAM();
	ROM = cpu.getROM();
	if (argc < 2)
	{
		printf("Usage: sxemu <hexfile>\n");
		printf("e.g. \"./sxemu file.hex\"\n");
		return EXIT_FAILURE;
	}
	fh.readFile (argv[1], ROM);
	if ((mainwin = initscr ()) == NULL)
	{
		fprintf (stderr, "Error initialising ncurses.\n");
		exit (EXIT_FAILURE);
	}

	noecho();
	start_color();
	curs_set(0);
	nodelay(stdscr, true);
	keypad (mainwin, TRUE);
	getmaxyx(stdscr, row, col);
	romwin = subwin (mainwin, row, 27, 0, 0);
	ramwin = subwin (mainwin, row / 2, 54, row / 2 + 1, 27);
	sidewin = subwin (mainwin, row / 2, 54, 0, 27);

	cursorrow = 0;
	ramr = romr = 0;
	//romptr = new char[(4096 * 2 / 8) * (32 + 8 + 5) + 1];
	romptr = new unsigned char[4096 * 35];
	dumpMem((char*)romptr, ROM, 4096);
	ramptr = new unsigned char[(262 * 2 / 8) * (48 + 5 + 8) + 1];
	memrow[45] = '\0';
	ramrow[53] = '\0';
	pc = cpu.getPC();
	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_RED, COLOR_BLACK);

	string impl = "";
	while (1)
	{
		screen();
		if (keys() == false)break;
	}
	delete[] ramptr;
	delete[] romptr;

	delwin (romwin);
	delwin (ramwin);
	delwin (sidewin);
	delwin (mainwin);
	endwin();
	running = false;
	if (walkthread != 0)pthread_join( walkthread, 0);
	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

void screen()
{
	int row, col;
	impl = cpu.lookup();
	getmaxyx(stdscr, row, col);
	erase();
	mvwprintw(ramwin, 1, 1, "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
	dumpRAM((char*)ramptr, cpu.getRAM(), 262);
	for (int j = 0;j < row - 2;j++)
	{
		memcpy(memrow, romptr + romr + j*35, 35);
		mvwprintw (romwin, 1 + j, 1, "%s", memrow);
		if (romr / 35 + j == cursorrow)mvchgat(1 + j, 6 , 20, A_REVERSE, 0, NULL);
		if (romr / 35 + j == *pc)
		{
			mvchgat(1 + j, 0 , 4, A_REVERSE, 1, NULL);
		}
		if (breakpoint[romr / 35 + j] == true)
		{
			mvchgat(1 + j, 5 , 1, A_REVERSE, 2, NULL);
		}
	}
	for (int j = 0;j < row / 2 - 3;j++)
	{
		memcpy(ramrow, ramptr + ramr + j*53, 52);
		mvwprintw (ramwin, 2 + j, 1, "%s", ramrow);
	}
	if (awin == 0)
	{
		wattron(romwin, A_BOLD);
	}
	else
	{
		wattron(ramwin, A_BOLD);
	}
	box (romwin, 0, 0);
	box (ramwin, 0, 0);
	box (sidewin, 0, 0);
	wattroff(romwin, A_BOLD);
	wattroff(ramwin, A_BOLD);
	mvwprintw(sidewin, 1, 1, "INDF %02X", RAM[RAM[4]]);
	mvwprintw(sidewin, 2, 1, "RTCC %02X", RAM[1]);
	mvwprintw(sidewin, 3, 1, "PC   %03X", *pc);
	mvwprintw(sidewin, 4, 1, "ST   %02X", RAM[3]);
	mvwprintw(sidewin, 5, 1, "FSR  %02X", RAM[4]);
	mvwprintw(sidewin, 6, 1, "RA   %02X", RAM[5]);
	mvwprintw(sidewin, 7, 1, "RB   %02X", RAM[6]);
	mvwprintw(sidewin, 8, 1, "RC   %02X", RAM[7]);
	mvwprintw(sidewin, 9, 1, "RD   %02X", RAM[8]);
	mvwprintw(sidewin, 10, 1, "RE   %02X", RAM[9]);
	mvwprintw(sidewin, 11, 1, "W    %02X", RAM[261]);
	mvwprintw(sidewin, 1, 20, "Mode %02X", *cpu.getMode());
	mvwprintw(sidewin, 2, 20, "OPC  %03X", ROM[*pc]);
	int j = 0;
	for (int i = 0;i < 12;i++)
	{
		int k = ROM[*pc] >> i & 1;
		mvwprintw(sidewin, 2, 30 + 15 - (j++), "%d", k);
		if ((i + 1) % 4 == 0)
			mvwprintw(sidewin, 13, 1 + 15 - (j++), " ");
		mvwprintw(sidewin, 15, 1, "%s", impl.c_str());
	}
	refresh();
}

bool keys()
{
	int ch = getch();
	switch (ch)
	{
			case KEY_UP:
			if (awin == 0)
			{
				if (cursorrow > 0)
				{
					cursorrow--;
					if (cursorrow*35 < romr)romr -= 35;
				}
			}
			else ramr = ramr - 53;
			break;
			case KEY_PPAGE:
			if (awin == 0)
			{
				romr = romr - (row - 2) * 35;
				cursorrow = romr / 35;
			}
			else
				ramr = ramr - (row / 2 - 2) * 53;

			break;
			case KEY_DOWN:
			if (awin == 0)
			{
				if (cursorrow < 4095)
				{
					cursorrow++;
					if (cursorrow*35 >= romr + (row - 2) * 35)romr += 35;
				}
			}
			else
				ramr = ramr + 53;
			break;
			case KEY_NPAGE:
			if (awin == 0)
			{
				romr = romr + (row - 2) * 35;
				cursorrow = romr / 35;
			}
			else ramr = ramr + (row / 2 - 2) * 53;
			break;
			case 10:      // enter
			ch = 0;
			break;
			case 32:      // space
			if (running == false)
			{
				if (cpu.lookup() == "END")
				{
					cpu.reset();
				}
				cpu.execute();
				romr = 35 * (*pc);
				cursorrow = romr / 35;
			}
			break;
			case 9:      //tab
			if (awin == 0)awin = 1;
			else awin = 0;
			break;
			case 'r':
			if (cpu.lookup() == "END")
			{
				cpu.reset();
			}
			if (running == false)
			{
				running = true;
				char m = 'r';
				pthread_create( &walkthread, NULL, &run, &m);
			}
			else running = false;
			break;
			case 'R':
			running = false;
			cpu.reset();
			break;
			case 'w':
			if (cpu.lookup() == "END")
			{
				cpu.reset();
			}
			if (running == false)
			{
				running = true;
				char m = 'w';
				pthread_create( &walkthread, NULL, &run, &m);
			}
			else running = false;
			break;
			case 'b':
			if (breakpoint[cursorrow] != true)
				breakpoint[cursorrow] = true;
			else
				breakpoint[cursorrow] = false;
			break;
	}
	if (ch == 0) return false;
	if (romr < 0)
	{
		romr = 0;
		cursorrow = 0;
	}
	if (ramr < 0)ramr = 0;
	if ((romr / 35 + row - 2) > 4096)romr = (4096 - row + 2) * 35;
	if ((ramr / 53 + row / 2 - 2) > 32)ramr = (32 - row / 2 + 2) * 53;
	return true;
}

void dumpMem(char *dump, USHORT* mem, USHORT size)
{
	int i, j, k;
	k = 0;
	j = 35;
	for (i = 0; i < size; i ++)
	{
		sprintf (dump + k, "%03X: ", i);
		k += 5;
		string cmd = cpu.lookup(mem[i] & 0xfff);
		sprintf (dump + k, "%s", cmd.c_str() );
		k += cmd.length();
		sprintf (dump + j, "\n");
		k = j;
		j += 35;
	}
}

void dumpRAM(char *dump, unsigned char* mem, USHORT size)
{
	int i, j, k;
	k = 0;

	for (i = 0; i < size; i += 16)
	{
		sprintf (dump + k, "%03X: ", i);
		k += 5;
		for (j = i; j < (i + 16); j++)
		{
			sprintf (dump + k, "%02X ", mem[j] & 0xff);
			k += 3;
		}
		sprintf (dump + k - 1, "\n");
	}
}
