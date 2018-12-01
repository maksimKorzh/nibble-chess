#include <stdio.h>
#include <stdlib.h>

#define BRD_SIZE 128
#define BUF_SIZE 2400

u_int8_t input[BUF_SIZE];

u_int8_t startpos[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
u_int8_t trickyPos[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
u_int8_t checkPos[] = "7k/8/6K1/4Q3/8/8/8/8 b - - 2 2 "; // check
u_int8_t matePos[] = "3Q2k/8/6K1/8/8/8/8/8 b - - 2 2 "; // mate
u_int8_t checkmate[] = "7k/8/6K1/3Q4/8/8/8/8 w - - 2 2 "; // checkmate
u_int8_t stalemate[] = "7k/8/5PK1/8/8/8/8/8 w - - 2 1 "; // stalemate
u_int8_t whitePawns[] = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1 "; // white pawns
u_int8_t tricky1[] = "r3k2r/p1ppqPb1/bn3np1/4N3/4P3/2p2Q1p/PPPBBPPP/R3K2R b KQkq - 0 2 "; // tricky d5e6
u_int8_t tricky2[] = "r3k2r/p1ppqpb1/Bn2pnp1/1N1PN3/1p2P3/5Q2/PPPB1PpP/R3K2R b KQkq - 1 2 "; // tricky - promotions
u_int8_t tricky3[] = "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq a3 1 1 "; // tricky e2b5
u_int8_t tricky4[] = "r4k1r/p1pNqpb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "; // tricky e5d7
u_int8_t testpos[] = "8/8/8/8/3Pp3/8/8/8 b KQkq d3 0 1"; //"2kr4/5N2/P7/8/8/8/8/8 w KQkq d8 0 1 ";//

u_int32_t board[BRD_SIZE + 1];
u_int32_t side, ep = 128, nodes;

u_int8_t pieces[] = {
    [0] = ' ', [4] = 'n', [5] = 'b', [6] = 'r', [7] = 'q',
    ['P'] = 9, ['N'] = 12, ['B'] = 13, ['R'] = 46, ['Q'] = 15, ['K'] = 43,
    ['p'] = 18, ['n'] = 20, ['b'] = 21, ['r']= 54, ['q'] = 23, ['k'] = 51    
};
    
char *unipieces[] = {
	".", "-", "\u265F", "\u265A", "\u265E", "\u265D", "\u265C", "\u265B", 
	"-", "\u2659", "-", "\u2654", "\u2658", "\u2657", "\u2656", "\u2655",  
};

int32_t vectors[] = {
   15,  16,  17,   0,
  -15, -16, -17,   0,
    1,  16,  -1, -16,   0,
    1,  16,  -1, -16,  15, -15, 17, -17,  0,
   14, -14,  18, -18,  31, -31, 33, -33,  0, 
    4,   0,  13,  22,  17,   8, 13
};


u_int32_t weights[] = { 0, 0, -100, 0, -300, -350, -500, -900, 0, 100, 0, 0, 300, 350, 500, 900 };
