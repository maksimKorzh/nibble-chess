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

;------------------------------------------------------------------------;
;                            data.h variables                            ;
;------------------------------------------------------------------------;

extern startpos, trickyPos, testpos
extern board, side, ep, vectors, weights, nodes
    
    
;------------------------------------------------------------------------;
;                            data.h functions                            ;
;------------------------------------------------------------------------;

extern print_ln, print_num, print_chr, print_str
extern print_brd, print_pce, print_mv, print_sq
extern parse_fen
    
    
;------------------------------------------------------------------------;
;                               MCP macros                               ;
;------------------------------------------------------------------------;

%define VALID_SQ 0x88
%define NO_SQ 128

%define Side dword [side]
%define Ep dword [ep]
%define Nodes dword [nodes]

%define SIDE dword [ebp + 8]                     ; search arg1 reference ;
%define EP dword [ebp + 12]                      ; search arg2 reference ;
%define ALPHA dword [ebp + 16]                   ; search arg3 reference ;
%define BETA dword [ebp + 20]                    ; search arg4 reference ;
%define DEPTH dword [ebp + 24]                   ; search arg5 reference ;
%define CAP_SQ dword [ebp - 4]                   ; search var1 reference ;
%define ROOK_SQ dword [ebp - 8]                  ; search var2 reference ;
%define SKIP_SQ dword [ebp - 12]                 ; search var3 reference ;

%define FROM_SQ ecx
%define PIECE eax
%define DIR ebx
%define STEP edx
%define TO_SQ esi
%define CAP edi
    
%define BRD(sq) dword [board + sq * 4]           ; board index reference ;
%define VEC(sq) dword [vectors + sq * 4]       ; vectors index reference ;


;------------------------------------------------------------------------;
;                          NASM functions calls                          ;
;------------------------------------------------------------------------;

%macro GENERATE_MOVES 5
    push %5
    push %4
    push %3
    push %2
    push %1
    call generate_moves
    add esp, 20
%endmacro 


;------------------------------------------------------------------------;
;                           C functions calls                            ;
;------------------------------------------------------------------------;

%macro PARSE_FEN 1
    push %1
    call parse_fen
    add esp, 4
%endmacro

%macro PRINT_BOARD 0
    PUSH_DATA
    call print_brd
    POP_DATA
%endmacro

%macro PRINT_SQ 1
    PUSH_DATA
    push %1
    call print_sq
    add esp, 4
    POP_DATA
%endmacro

%macro PRINT_MOVE 3
    PUSH_DATA
    push %3
    push %2
    push %1
    call print_mv
    add esp, 12
    POP_DATA
%endmacro

%macro FSH 0
    PUSH_DATA
    call print_ln
    POP_DATA
%endmacro

%macro NUM 1
    PUSH_DATA
    push %1
    call print_num
    add esp, 4
    POP_DATA
%endmacro

%macro CHR 1
    PUSH_DATA
    push %1
    call print_chr
    add esp, 4
    POP_DATA
%endmacro

%macro STR 1
    PUSH_DATA
    push %1
    call print_str
    add esp, 4
    POP_DATA
%endmacro

%macro PCE 1
    PUSH_DATA
    push %1
    call print_pce
    add esp, 4
    POP_DATA
%endmacro

%macro PUSH_DATA 0
    push eax
    push ebx
    push ecx
    push edx
%endmacro

%macro POP_DATA 0
    pop edx
    pop ecx
    pop ebx
    pop eax
%endmacro



