/*********************************************************************************\
;---------------------------------------------------------------------------------;
;        Copyright © 2018 Maksim Korzh <freesoft.for.people@gmail.com>            ;
;---------------------------------------------------------------------------------;
;     This work is free. You can redistribute it and/or modify it under the       ;
;      terms of the Do What The Fuck You Want To Public License, Version 2,       ;
;       as published by Sam Hocevar. See the COPYING file for more details.       ;
;---------------------------------------------------------------------------------;
'       THIS PROGRAM IS FREE SOFTWARE. IT COMES WITHOUT ANY WARRANTY, TO          ;
;        THE EXTENT PERMITTED BY APPLICABLE LAW. YOU CAN REDISTRIBUTE IT          ;
;       AND/OR MODIFY IT UNDER THE TERMS OF THE DO WHAT THE FUCK YOU WANT         ;
;          TO PUBLIC LICENCE, VERSION 2, AS PUBLISHED BY SAM HOCEVAR.             ;
;                SEE http://www.wtfpl.net/ FOR MORE DETAILS.                      ;
;---------------------------------------------------------------------------------;
\*********************************************************************************/

/*********************************************************************************\
;---------------------------------------------------------------------------------;
;                   DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE                   ;
;                           Version 2, December 2004                              ;
;                                                                                 ;
;        Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>                         ;
;                                                                                 ;
;        Everyone is permitted to copy and distribute verbatim or modified        ;
;        copies of this license document, and changing it is allowed as long      ;
;        as the name is changed.                                                  ;
;                                                                                 ;
;                   DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE                   ;
;          TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION        ;
;                                                                                 ;
;         0. You just DO WHAT THE FUCK YOU WANT TO.                               ;
;---------------------------------------------------------------------------------;
\*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>

//typedef unsigned long long ATTACK;

#define SETBIT(table, index) (table |= 1ULL << index)
#define GETBIT(table, index) ((table >> index) & 1U)


#define BOARD_SIZE 129
#define OFFBOARD 0x88
#define NO_SQ 128
#define TYPE 7
#define STEP 30

#define WP 9
#define WN 12
#define WB 13
#define WR 14
#define WQ 15
#define WK 11
#define BP 18
#define BN 20
#define BB 21
#define BR 22
#define BQ 23
#define BK 19

// 0000 0011 1000 0000 0000 0000 0000      Q    0xF3FFFFF
// 0000 0011 0000 0000 0000 0000 0000      R    0xF37FFFF
// 0000 0010 1000 0000 0000 0000 0000      B    0xF2FFFFF
// 0000 0010 0000 0000 0000 0000 0000      N    0xF27FFFF


#define startpos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//                                PRINT MACROS                                   //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#define PRINT_BOARD(board) \
for(int i = 0; i < NO_SQ; i++) \
printf(" %s", ((i & 8) && (i += 7)) ? "\n" : uniPce[board->pos[i] & 15]); \
printf("\n side %c\n", (board->side == 8) ? 'w' : 'b'); \
printf(" e.p. "); (board->ep == NO_SQ) ? printf("no") : PRINT_SQ(board->ep); \
printf("\n"); \
printf(" Castling: %c%c%c%c\n", \
board->castle & WKC ? 'K' : '-',  \
board->castle & WQC ? 'Q' : '-',  \
board->castle & BKC ? 'k' : '-',  \
board->castle & BQC ? 'q' : '-'); \
printf("\n\n");

#define PRINT_PIECE_LIST() \
while(*loopPce) { \
int piece = charPce[*loopPce]; \
for(int i = 0; i < board->pceNum[piece]; i++) { \
PRINT_PIECE(piece); PRINT_SQ(board->pieces[piece][i]); \
printf(" "); }*loopPce++; printf("\n"); } loopPce -= 12; printf("\n");

#define PRINT_ATTACKS(side) \
for(int sq = 0; sq < 128; sq++) { \
((sq & 8) && (sq += 7)) ? \
printf("\n") : printf(" %s", square_att(board, sq, side) ? "x" : "."); }

#define PRINT_ATTACK_TABLE(side) \
for(int sq = 0; sq < 128; sq++) { \
((sq & 8) && (sq += 7)) ? \
printf("\n") : printf(" %s", square_att_new(board, sq, side) ? "x" : "."); }



#define PRINT_PIECE(piece) printf("%s", uniPce[piece & 15])
#define PRINT_SQ(sq) printf("%c%c", ((sq & 7) + 'a'), (7 - (sq >> 4) + '1'))

#define PRINT_MOVE(from, to, prom) \
PRINT_SQ(from); PRINT_SQ(to); printf("%c", charPce[prom])

#define PRINT_MOVE_LIST(list) \
for(int moveNum = 0; moveNum < list->count; moveNum++) { \
PRINT_MOVE(GET_FROM(list->moves[moveNum].move), \
           GET_TO(list->moves[moveNum].move), \
           GET_PROM(list->moves[moveNum].move)); \
printf("\n"); } \
printf("count %d\n", list->count)

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//                                BOARD MACROS                                   //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#define FR2SQ(file, rank) (7 - rank) * 16 + file
#define RANK(sq) 7 - (sq >> 4) + 1

#define KING_SQ(side) \
board->pieces[charPce[(side == 8) ? 'K': 'k']][0]

#define IN_CHECK(side) \
square_att(board, \
board->pieces[charPce[((24 - side) == 8) ? 'K' : 'k']][0], side)

#define CASTLING \
type == K && rookSq & OFFBOARD && \
board->pos[rookSq = (fromSq | 7) - (dir >> 1 & 7)] &&  \
((step == 13 && board->castle & ((board->side == 8) ? WKC : BKC) && \
((board->side == 8) ? ((!board->pos[F1] && !board->pos[G1] && \
!square_att(board, E1, 24 - board->side) && \
!square_att(board, F1, 24 - board->side))):(( \
!board->pos[F8] && !board->pos[G8] && \
!square_att(board, E8, 24 - board->side) && \
!square_att(board, F8, 24 - board->side))))) || \
(step == 15 && board->castle & ((board->side == 8) ? WQC : BQC) && \
((board->side == 8) ? ((!board->pos[D1] && !board->pos[C1] && \
!board->pos[B1] && !square_att(board, E1, 24 - board->side) && \
!square_att(board, D1, 24 - board->side))):(( \
!board->pos[D8] && !board->pos[C8] && !board->pos[B8] && \
!square_att(board, E8, 24 - board->side) && \
!square_att(board, D8, 24 - board->side))))))

#define DOUBLE_PAWN_PUSH \
(type < K && 6 * board->side + (toSq & A1) == NO_SQ)

#define PAWN_PROMOTION \
board->side == 8 && RANK(fromSq) == 7 && type < 3 || \
board->side == 16 && RANK(fromSq) == 2 && type < 3

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//                                 MOVE MACROS                                   //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//                Move format                                                    //
//                                                                               //
//    0000 0000 0000 0000 0000 0111 1111      from                               //
//    0000 0000 0000 0011 1111 1000 0000      to                                 //
//    0000 0000 0111 1100 0000 0000 0000      cap                                //
//    0000 1111 1000 0000 0000 0000 0000      prom                               //
//    0001 0000 0000 0000 0000 0000 0000      ep                                 //
//    0010 0000 0000 0000 0000 0000 0000      ps                                 //
//    0100 0000 0000 0000 0000 0000 0000      cas                                //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#define SET_MOVE(from, to, cap, prom, ep, ps, cas) \
((from) | (to << 7) | (cap << 14) | (prom << 19) | \
(ep << 24) | (ps << 25) | (cas << 26))

#define GET_FROM(move) (move & 0x7F)
#define GET_TO(move) ((move >> 7) & 0x7F)
#define GET_CAP(move) ((move >> 14) & 0x1F)
#define GET_PROM(move) ((move >> 19) & 0x1F)
#define GET_EP(move) ((move >> 24) & 1)
#define GET_PS(move) ((move >> 25) & 1)
#define GET_CAS(move) ((move >> 26) & 1)

#define MOVE_PIECE(from, to, piece) \
for(int pceNum = 0; pceNum < board->pceNum[piece]; pceNum++) { \
if(board->pieces[piece][pceNum] == from) { \
board->pieces[piece][pceNum] = to; } }

#define ADD_PIECE(piece, sq) \
board->pieces[piece][board->pceNum[piece]++] = sq;

#define CLEAR_PIECE(piece, sq) \
int tempNum; for(int i = 0; i < board->pceNum[piece]; i++) { \
if(board->pieces[piece][i] == sq) {tempNum = i; } } \
board->pceNum[piece]--; \
board->pieces[piece][tempNum] = board->pieces[piece][board->pceNum[piece]];

/*********************************************************************************\
;---------------------------------------------------------------------------------;
;                              BOARD REPRESENTATION                               ;
;---------------------------------------------------------------------------------;
\*********************************************************************************/


enum { WKC = 1, WQC = 2, BKC = 4, BQC = 8 };
enum { all, caps };
enum { bP = 1, wP, K, N, B, R, Q};

enum
{
    A1 = 112, B1, C1, D1, E1, F1, G1, H1,
    A2 = 96,  B2, C2, D2, E2, F2, G2, H2,
    A3 = 80,  B3, C3, D3, E3, F3, G3, H3,
    A4 = 64,  B4, C4, D4, E4, F4, G4, H4,
    A5 = 48,  B5, C5, D5, E5, F5, G5, H5,
    A6 = 32,  B6, C6, D6, E6, F6, G6, H6,
    A7 = 16,  B7, C7, D7, E7, F7, G7, H7,
    A8 = 0,   B8, C8, D8, E8, F8, G8, H8
};

typedef struct
{
    int ep;
    int castle;
}
HISTORY;

typedef struct
{
    int pos[129];
    int side;
    int ep;
    int castle;
    
    int oldEp;
    int oldCastle;
    
    int pieces[24][10];
    int pceNum[24];
    
    //HISTORY history;
}
BOARD;

typedef struct
{
    int move;
    int score;
}
MOVE;

typedef struct
{
    MOVE moves[256];
    int count;
}
LIST;

typedef struct
{
    long nodes;
}
SEARCH;

char charPce[] = {
    [0] = ' ', [4] = 'n', [5] = 'b', [6] = 'r', [7] = 'q',
    ['P'] = 9, ['N'] = 12, ['B'] = 13, ['R'] = 14, ['Q'] = 15, ['K'] = 11,
    ['p'] = 18, ['n'] = 20, ['b'] = 21, ['r']= 22, ['q'] = 23, ['k'] = 19
};
    
char *uniPce[] = {
	".", "-", "\u265F", "\u265A", "\u265E", "\u265D", "\u265C", "\u265B", 
	"-", "\u2659", "-", "\u2654", "\u2658", "\u2657", "\u2656", "\u2655",  
};

char *loopPce = "PNBRQKpnbrqk";

static const int vectors[] = {
   16,  15,  17,   0,
  -16, -15, -17,   0,
    1,  16,  -1, -16,   0,
    1,  16,  -1, -16,  15, -15, 17, -17,  0,
   14, -14,  18, -18,  31, -31, 33, -33,  0, 
    3,  -1,  12,  21,  16,   7, 12, 0
};

static const int castling[128] = 
{
	 7, 15, 15, 15,  3, 15, 15, 11, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	13, 15, 15, 15, 12, 15, 15, 14, 8, 8, 8, 8, 8, 8, 8, 8
};

/*********************************************************************************\
;---------------------------------------------------------------------------------;
;                                MOVE GENERATION                                  ;
;---------------------------------------------------------------------------------;
\*********************************************************************************/

#define PAWN_ATTACK \
!((sq + dir) & OFFBOARD) && board->pos[sq + dir] == ((side == 8) ? WP : BP)

#define KNIGHT_ATTACK \
!((sq + dir) & OFFBOARD) && board->pos[sq + dir] == ((side == 8) ? WN : BN)

#define BISHOP_QUEEN_ATTACK \
(side == 8) ? (piece == WB || piece == WQ) : (piece == BB) || (piece == BQ) 

#define ROOK_QUEEN_ATTACK \
(side == 8) ? (piece == WR) || (piece == WQ) : (piece == BR) || (piece == BQ)

#define KING_ATTACK \
!((sq + dir) & OFFBOARD) && board->pos[sq + dir] ==  ((side == 8) ? WK : BK)

static int square_att_new(BOARD *board, int sq, int attackSide)
{
    //char *pce;
    int piece, type, step, dir, cap, toSq, shiftPawn;
    
    //(attackSide == 16) ? (pce = loopPce + 6) : (pce = loopPce);
    
    int nextPce = STEP;
    
    // loop over piece types
    while(vectors[++nextPce])
    {
        //if(attackSide == 8 && *pce == 'p') break;
        
        type = nextPce - STEP;//charPce[*pce++] & TYPE;  
        shiftPawn = 0;

        if(type == bP) shiftPawn = 1;
        if(type == wP) shiftPawn = -1;
        
        step = vectors[STEP + type + shiftPawn];
        
        // loop over step vectors
        while(dir = vectors[++step])
        {
            toSq = sq;
            // loop over squares
            do
            {
                toSq += dir;
                piece = board->pos[toSq];
                //if(!(piece & attackSide)) break;
               
                if(toSq & OFFBOARD) break;
               
                cap = board->pos[toSq];

                if(type < K && !(dir & 7) != !cap) break;
                if(piece & attackSide && (piece & TYPE) == type)
                {
                    //if(attackSide == 16) pce = loopPce;
                    return 1;
                }
                
                cap += type < B; 
            }
            
            while(!cap);
        }
    }    
    
    //if(attackSide == 16) pce = loopPce;
    
    return 0;
}

static int square_att(BOARD *board, int sq, int side)
{
    int dir, step, toSq, piece;
    
    // by pawns    
    step = (side == 8) ? 0 : 4;

    while(dir = vectors[++step]) if(PAWN_ATTACK) return 1;
    
    // by knights
    step = 21;

    while(dir = vectors[++step]) if(KNIGHT_ATTACK) return 1;
    
    // by bishops and queens
    step = 16;
    
    while(dir = vectors[++step])
    {
        toSq = sq + dir;
        while(!(toSq & OFFBOARD))
        {
            piece = board->pos[toSq];
            if(BISHOP_QUEEN_ATTACK) return 1;
            else if(piece) break;

            toSq += dir;
        }
    }
    
    // by rooks and queens
    step = 7;

    while(dir = vectors[++step])
    {
        toSq = sq + dir;
       
        while(!(toSq & OFFBOARD))
        {   
            piece = board->pos[toSq];
            
            if(ROOK_QUEEN_ATTACK) return 1;
            else if(piece) break;

            toSq += dir;
        }
    }
    
    // by kings
    step = 12;

    while(dir = vectors[++step]) if(KING_ATTACK) return 1;
    
    return 0;
}

static inline void take_back(BOARD* board, int move, int oldEp, int oldCastle)
{
    int fromSq = GET_FROM(move);
    int toSq = GET_TO(move);
    int piece = board->pos[toSq];
    int cap = GET_CAP(move);
    int prom = GET_PROM(move);
    int capSq = GET_EP(move) ? toSq ^ 16 : toSq;

    if(cap)
    {
        ADD_PIECE(cap, capSq);
    }
   
    board->pos[toSq] = 0;
    board->pos[fromSq] = piece;
    board->pos[capSq] = cap;
    
    MOVE_PIECE(toSq, fromSq, piece);
    
    if(prom)
    {
        int pawn = (board->side == 8) ? BP : WP;
        board->pos[fromSq] = pawn;
        CLEAR_PIECE(piece, fromSq);
        ADD_PIECE(pawn, fromSq);
    }
    
    if(GET_CAS(move))
    {
        switch(toSq)
        {
            case G1:
                board->pos[H1] = WR;
                board->pos[F1] = 0;
                MOVE_PIECE(F1, H1, WR);
                break;
                
            case C1:
                board->pos[A1] = WR;
                board->pos[D1] = 0;
                MOVE_PIECE(D1, A1, WR);
                break;
                
            case G8:
                board->pos[H8] = BR;
                board->pos[F8] = 0;
                MOVE_PIECE(F8, H8, BR);
                break;
                
            case C8:
                board->pos[A8] = BR;
                board->pos[D8] = 0;
                MOVE_PIECE(D8, A8, BR);
                break;
        }
    }
    
    board->ep = oldEp;
    board->castle = oldCastle;
    //board->ep = board->history.ep;//oldEp;
    //board->castle = board->history.castle;//oldCastle;
    
    board->side = 24 - board->side;
}

static inline int make_move(BOARD *board, int move)
{
    int oldEp = board->ep;
    int oldCastle = board->castle;

    board->oldCastle = board->castle;

    int fromSq = GET_FROM(move);
    int toSq = GET_TO(move);
    int cap = GET_CAP(move);
    int prom = GET_PROM(move);
    int capSq = GET_EP(move) ? toSq ^ 16 : toSq;
    int piece = board->pos[fromSq];

    if(cap)
    {
        CLEAR_PIECE(board->pos[capSq], capSq);
    }
    
    MOVE_PIECE(fromSq, toSq, piece);

    board->pos[fromSq] = 0;
    board->pos[capSq] = 0;
    board->pos[toSq] = piece;
    
    if(prom)
    {
        board->pos[toSq] = prom + board->side;
        ADD_PIECE(board->pos[toSq], toSq);
        CLEAR_PIECE((board->side == 8) ? WP : BP, toSq);
    }
    
    if(GET_CAS(move))
    {
        switch(toSq)
        {
            case G1:
                board->pos[H1] = 0;
                board->pos[F1] = WR;
                MOVE_PIECE(H1, F1, WR);
                break;
                
            case C1:
                board->pos[A1] = 0;
                board->pos[D1] = WR;
                MOVE_PIECE(A1, D1, WR);
                break;
                
            case G8:
                board->pos[H8] = 0;
                board->pos[F8] = BR;
                MOVE_PIECE(H8, F8, BR);
                break;
                
            case C8:
                board->pos[A8] = 0;
                board->pos[D8] = BR;
                MOVE_PIECE(A8, D8, BR);
                break;
        }
    }
    
    //board->oldEp = board->ep;
    board->ep = NO_SQ;
   
    if(GET_PS(move))
    {
        board->ep = (board->side == 8) ? (toSq + 16) : (toSq - 16);
       // board->oldEp = board->ep;
    }
    
    // update castling permission
	board->castle &= castling[fromSq];
    board-> castle &= castling[toSq];
    
    board->side = 24 - board->side;

    // move legality check
    if(IN_CHECK(board->side))
    {
        //take_move(board, copy);
        take_back(board, move, oldEp, oldCastle);
        return 0;
    }
      
    return 1;
}

static inline void add_move(int move, LIST *list)
{
    int prom = GET_PROM(move);
    
    if(!prom) 
    {
        list->moves[list->count].move = move;
        list->count++;
    }
    
    else if(prom)
    {
        list->moves[list->count].move = move & 0xF3FFFFF;
        list->count++;

        list->moves[list->count].move = move & 0xF37FFFF;
        list->count++;
        
        list->moves[list->count].move = move & 0xF2FFFFF;
        list->count++;
        
        list->moves[list->count].move = move & 0xF27FFFF;
        list->count++;
        
    }
    
    
}

static inline void generate_moves(BOARD *board, LIST *list, int moveType)
{
    int fromSq = 0, toSq, capSq, skipSq, rookSq;
    int piece, type, prom = 0, dir, cap;
    int score;
    char *pce;
    int step;
    
    list->count = 0;
    
    (board->side == 8) ? (pce = loopPce) : (pce = loopPce + 6);
    
    // loop over pieces
    while(*pce)
    {
        if(board->side == 8 && *pce == 'p') break;
    
        for(int pceNum = 0; pceNum < board->pceNum[charPce[*pce]]; pceNum++)
        {
            fromSq = board->pieces[charPce[*pce]][pceNum];
            piece = board->pos[fromSq];
            
            if(piece & board->side)
            {
                type = piece & TYPE;
                step = vectors[STEP + type];
                
                // loop over directions
                while(dir = vectors[++step])
                {
                    toSq = fromSq;
                    skipSq = rookSq = NO_SQ;
                    
                    // loop over target squares
                    do
                    {
                        toSq += dir;
                        capSq = toSq;
                        
                        if(toSq & OFFBOARD) break;
                        if(type < K && toSq == board->ep)
                            capSq = toSq ^ 16;
                        
                        cap = board->pos[capSq];

                        if((cap & 7) == K) { break;}
                        
                        if(cap & board->side || type < K && !(dir & 7) != !cap)
                            break;
                            
                        
                           
                        prom = 0;
                        
                        if(PAWN_PROMOTION) prom = Q;
                        
                        
                        int pawnStart = 0;
                        if (type < K && ((toSq - fromSq == 32) || (fromSq - toSq == 32)))// fr 2  to 4   fr 7  to 5 
                        {
                            pawnStart = 1;
                        }
                        
                        // add moves to move list
                        int move = SET_MOVE(fromSq, toSq, cap, prom, (capSq == toSq) ? 0 : 1, pawnStart, (!(rookSq & OFFBOARD) ? 1 : 0));
                        
                        if(moveType == caps && cap) add_move(move, list);
                        else if (moveType == all) add_move(move, list);
                        
                        cap += type < B;
                        
                        // castling
                        if (DOUBLE_PAWN_PUSH || CASTLING) 
                        {
                            cap--;
                            skipSq = toSq;
                        }
                    }
                    
                    while(!cap);
                }
            }
        }
        
        *pce++;
    }
}

/*********************************************************************************\
;---------------------------------------------------------------------------------;
;                                      UCI                                        ;
;---------------------------------------------------------------------------------;
\*********************************************************************************/

void parse_fen(BOARD *board, char *fen)
{
    for(int sq = 0; sq < BOARD_SIZE; sq++) board->pos[sq] = 0;
    for(int num = 0; num < 24; num++) board->pceNum[num] = 0;
    
    board->side = 0;
    board->ep = NO_SQ;
    board->castle = 0;

    int file = 0;
    int rank = 7;
    int piece;
    
    do
    {
        if(*fen >= 'A')
        {
            piece = charPce[*fen];
            board->pieces[piece][board->pceNum[piece]++] = FR2SQ(file, rank);
            board->pos[FR2SQ(file, rank)] = charPce[*fen];
            file++;
        }
        if(*fen >= '0' && *fen <= '9') file += *fen - '0';
        if(*fen == '/') { file = 0; rank--; }
    }
    
    while(*fen++ != ' ');
    
    (*fen++ == 'w') ? (board->side = 8) : (board->side = 16); *fen++;
    
    while(*fen != ' ')
	{
        switch(*fen)
        {
            case 'K': board->castle |= WKC; break;
            case 'Q': board->castle |= WQC; break;
            case 'k': board->castle |= BKC; break;
            case 'q': board->castle |= BQC; break;

            case '-': board->castle = 0;
        } fen++;
    }
	
	fen++;

	if(*fen != '-') board->ep = (7 - ((fen[1] - '0') - 1)) * 16 + (fen[0] - 'a');
}

/*********************************************************************************\
;---------------------------------------------------------------------------------;
;                                      PERFT                                      ;
;---------------------------------------------------------------------------------;
\*********************************************************************************/

static void perft(BOARD *board, int depth, int printDepth, SEARCH *search)
{
    if(!depth)
    {
        search->nodes++;
        return;
    }
    
    LIST list[1];
    generate_moves(board, list, all); //PRINT_MOVE_LIST(list);

    for(int moveNum = 0; moveNum < list->count; moveNum++)
    {
        int oldEp = board->ep;
        int oldCastle = board->castle;
        
        if(!make_move(board, list->moves[moveNum].move)) continue;
        
        PRINT_BOARD(board);
        //PRINT_PIECE_LIST();
        PRINT_ATTACK_TABLE(24 - board->side);
        //PRINT_ATTACK_BOARD(24 - board->side);
        getchar();
        
        long cumNodes = search->nodes;
        perft(board, depth - 1, printDepth, search);
        long oldNodes = search->nodes - cumNodes;
        
        
        take_back(board, list->moves[moveNum].move, oldEp, oldCastle);
        //take_move(board, copy);
        
            
        PRINT_BOARD(board);
        //PRINT_PIECE_LIST();
        PRINT_ATTACK_TABLE(24 - board->side);
        //PRINT_ATTACK_BOARD(24 - board->side);
        getchar();
        
        if(printDepth == depth)
        {
            printf("move %d ", moveNum + 1);
            PRINT_MOVE(GET_FROM(list->moves[moveNum].move), GET_TO(list->moves[moveNum].move), GET_PROM(list->moves[moveNum].move));
            printf("   old nodes %ld\n", oldNodes);
        }
        
    }
}

/*********************************************************************************\
;---------------------------------------------------------------------------------;
;                                      MAIN                                       ;
;---------------------------------------------------------------------------------;
\*********************************************************************************/

ATTACK init_attack_board(BOARD *board, ATTACK table, int side)
{
    int file, rank, index;

    for(int sq = 0; sq < 129; sq++)
    {
        if(!(sq & 0x88))
        {
            if(square_att_new(board, sq, side))
            {
                file = sq & 7;
                rank = sq >> 4;
                index = rank * 8 + file;
                SETBIT(table, index);
            }
        }
    }
    
    return table;
}

#define PRINT_ATTACK_BOARD(table, side)   \
for(int index = 0; index < 64; index++)   \
{                                         \
    if(!(index % 8)) printf("\n");        \
    printf(" %s", (GETBIT(table, index)) ? "x" : ".");\
} \
printf("\n\n")

int main()
{
    BOARD board[1];
    SEARCH search[1];
    search->nodes = 0;
    //parse_fen(board, startpos);
    parse_fen(board, "8/8/8/8/4Q3/8/8/8 w KQkq e6 0 1");
    //parse_fen(board, "8/8/8/3Pp3/8/n7/8/8 b KQkq e6 0 1");
    //parse_fen(board, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    //parse_fen(board, "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"); // white pawns
    //parse_fen(board, "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"); // black pawns
    //parse_fen(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "); // tricky
    //parse_fen(board, "r2k3r/p1pNqpb1/bn2pnp1/3P13/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "); // tricky
    
    //PRINT_BOARD(board); PRINT_PIECE_LIST();
    
    //int depth = 1;
    //perft(board, depth, depth, search);
    //printf("nodes %ld\n", search->nodes);

    ATTACK table = 0x0ULL;
    
    table = init_attack_board(board, table, 8);
    //printf("%llx\n", table);
    //init_attack_board(board, table, 16);

    //PRINT_ATTACK_TABLE(8);
    
    PRINT_ATTACK_BOARD(table, 8);
    
    //for(int sq = 0; sq < 128; sq++) { \
    //((sq & 8) && (sq += 7)) ? \
    //printf("\n") : printf(" %s", init_att_tables(board, sq, side) ? "x" : "."); }

    
    /*int move = SET_MOVE(G7, H8, BR, Q, 0, 0, 0);
    printf("%d %d %d %d %d %d %d\n", GET_FROM(move), GET_TO(move), GET_CAP(move), GET_PROM(move), GET_EP(move), GET_PS(move), GET_CAS(move));
    move = move & 0xF27FFFF;
    printf("%d %d %d %d %d %d %d\n", GET_FROM(move), GET_TO(move), GET_CAP(move), GET_PROM(move), GET_EP(move), GET_PS(move), GET_CAS(move));
    */
    
    
    /*for(int i = 0; i < 0xFFFFFF; i++)
    {
        
    }*/
      
    return 0;
}

