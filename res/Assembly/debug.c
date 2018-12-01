/*************************************************************************
;------------------------------------------------------------------------;
;    Copyright © 2018 Maksim Korzh <freesoft.for.people@gmail.com>       ;
;------------------------------------------------------------------------;
; This work is free. You can redistribute it and/or modify it under the  ;
;  terms of the Do What The Fuck You Want To Public License, Version 2,  ;
;   as published by Sam Hocevar. See the COPYING file for more details.  ;
;------------------------------------------------------------------------;
;   THIS PROGRAM IS FREE SOFTWARE. IT COMES WITHOUT ANY WARRANTY, TO     ;
;    THE EXTENT PERMITTED BY APPLICABLE LAW. YOU CAN REDISTRIBUTE IT     ;
;   AND/OR MODIFY IT UNDER THE TERMS OF THE DO WHAT THE FUCK YOU WANT    ;
;      TO PUBLIC LICENCE, VERSION 2, AS PUBLISHED BY SAM HOCEVAR.        ;
;            SEE http://www.wtfpl.net/ FOR MORE DETAILS.                 ;
;------------------------------------------------------------------------;
*************************************************************************/

#include "data.h"

/*************************************************************************
;------------------------------------------------------------------------;
;                         DEBUGGING FUNCTIONS                            ;
;------------------------------------------------------------------------;
*************************************************************************/

void parse_fen(char *fen)
{
    for(int sq = 0; sq < BRD_SIZE; sq++) board[sq] = 0;

    int file = 0; int rank = 7;
    
    do
    {
        if(*fen >= 'A') board[(7 - rank) * 16 + file++] = pieces[*fen];
        if(*fen >= '0' && *fen <= '9') file += *fen - '0';
        if(*fen == '/') { file = 0; rank--; }
    }
    
    while(*fen++ != ' ');
    
    (*fen++ == 'w') ? (side = 8) : (side = 16); *fen++;
    
    do
	{
		//TODO set virgin bits on corner pieces
	}
	
	while(*fen++ != ' ');

	if(*fen != '-') ep = (7 - ((fen[1] - '0') - 1)) * 16 + (fen[0] - 'a');
}


void print_sq(int sq)
{
    printf("%c%c", ((sq & 7) + 'a'), (7 - (sq >> 4) + '1'));
}

void print_mv(int src, int dst, char prm)
{
    print_sq(src);
    print_sq(dst);
    printf("%c\n", pieces[prm]);
}

void print_pce(int pce)
{
    printf("%s\n", unipieces[pce & 15]);
}

void print_brd()
{
    for(int sq = 0; sq < BRD_SIZE; sq++)
        printf(" %s", ((sq & 8) && (sq += 7)) ? "\n" : unipieces[board[sq] & 15]);

    printf("\n side %c\n", (side == 8) ? 'w' : 'b');
    printf(" e.p. ");
    (ep == 128) ? printf("no") : print_sq(ep);
    printf("\n\n");
}
                                                       
void print_ln() { printf("\n"); }
void print_num(int num) { printf("%d\n", num); }
void print_chr(int chr) { printf("%c\n", chr); }
void print_str(char *str) { printf("%s\n", str); }


