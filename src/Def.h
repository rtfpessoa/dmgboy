#ifndef __DEF_H__
#define __DEF_H__

typedef unsigned char BYTE;
typedef unsigned short WORD;

#define MAX_LCD_MODE_0 204    //201-207
#define MAX_LCD_MODE_1 4560    //456 * 10 = 4560
#define MAX_LCD_MODE_2 80     //77-83
#define MAX_LCD_MODE_3 172    //169-175

#define BIT0(value)	((value) & 0x01)
#define BIT1(value)	((value) & 0x02)
#define BIT2(value)	((value) & 0x04)
#define BIT3(value)	((value) & 0x08)
#define BIT4(value)	((value) & 0x10)
#define BIT5(value)	((value) & 0x20)
#define BIT6(value)	((value) & 0x40)
#define BIT7(value)	((value) & 0x80)

#define BITS01(value)	((value) & 0x03)
#define BITS23(value)	((value) & 0x0C)
#define BITS45(value)	((value) & 0x30)
#define BITS67(value)	((value) & 0xC0)

#define P1		0xFF00
#define SB		0xFF01
#define SC		0xFF02
#define DIV		0xFF04
#define TIMA	0xFF05
#define TMA		0xFF06
#define TAC		0xFF07
#define IF		0xFF0F
#define NR10	0xFF10
#define NR11	0xFF11
#define NR12	0xFF12
#define NR13	0xFF13
#define NR14	0xFF14
#define NR21	0xFF16
#define NR22	0xFF17
#define NR23	0xFF18
#define NR24	0xFF19
#define NR30	0xFF1A
#define NR31	0xFF1B
#define NR32	0xFF1C
#define NR33	0xFF1D
#define NR34	0xFF1E
#define NR41	0xFF20
#define NR42	0xFF21
#define NR43	0xFF22
#define NR44	0xFF23
#define NR50	0xFF24
#define NR51	0xFF25
#define NR52	0xFF26
#define LCDC	0xFF40
#define STAT	0xFF41
#define SCY		0xFF42
#define SCX		0xFF43
#define LY		0xFF44
#define LYC		0xFF45
#define DMA		0xFF46
#define BGP		0xFF47
#define OBP0	0xFF48
#define OBP1	0xFF49
#define WY		0xFF4A
#define WX		0xFF4B
#define IE		0xFFFF

#endif