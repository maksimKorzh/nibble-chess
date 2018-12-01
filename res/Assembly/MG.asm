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
                                                                         ;
%include "macros.asm"                                                    ;
                                                                         ;
;------------------------------------------------------------------------;
section .text                                                            ;
    global main                      ; must be declared to link with gcc ;

;------------------------------------------------------------------------;
                                                                         ;
generate_moves:                                 ; GENERATE MOVES ROUTINE ;                                     
    push ebp                                                    ; CREATE ;
    mov ebp, esp                                                 ; STACK ;
    push 0                                           ; initialize CAP_SQ ;
    push 0                                          ; initialize ROOK_SQ ;
    push 0                                          ; initialize SKIP_SQ ;
    mov FROM_SQ, 0                            ; initialize source square ;
                                                                         ;
    .loop_brd:                                 ; LOOP OVER BOARD SQUARES ;
        test FROM_SQ, VALID_SQ              ; is source square on board? ;
        jne .skip_sq                        ; if not then go to .skip_sq ;
        mov PIECE, BRD(FROM_SQ)             ; otherwise initialize piece ;
        test PIECE, SIDE                   ; is it piece of moving side? ;
        je .end_brd                         ; if not then go to .end_brd ;
        and PIECE, 7                      ; otherwise extract piece type ;
        mov DIR, PIECE                  ; assign piece type to direction ;
        add DIR, 30                         ; initialize vectors' offset ;
        mov STEP, VEC(DIR)                      ; initialize step vector ;
                                                                         ;
    .loop_dir:                              ; LOOP OVER PIECE DIRECTIONS ;
        mov TO_SQ, FROM_SQ                    ; initialize target square ;
        mov ROOK_SQ, NO_SQ                   ; assign 128 to rook square ;
        mov SKIP_SQ, NO_SQ                   ; assign 128 to skip square ;
                                                                         ;
    .loop_sq:                                 ; LOOP OVER TARGET SQUARES ;
        add TO_SQ, VEC(STEP)              ; step into next target square ;
        mov CAP_SQ, TO_SQ                    ; initialize capture square ;
        test TO_SQ, VALID_SQ                ; is target square on board? ;
        jne .end_dir                     ; if not then break out of loop ;
        mov CAP, BRD(TO_SQ)                  ; initialize captured piece ;
        cmp EP, 128                     ; if en passant square offboard? ;
        jne .ep_exists                     ; if not the go to .ep_exists ;
        jmp .tests                              ; otherwise go to .tests ;
                                                                         ;
    .ep_exists:                                     
        push TO_SQ
        mov TO_SQ, EP
        cmp BRD(TO_SQ), 0
        jne .castle_cond1
        pop TO_SQ
        jmp .tests
        
    .castle_cond1:
        pop TO_SQ
        push TO_SQ
        sub TO_SQ, EP
        cmp TO_SQ, 2
        jl .castle_cond2 
        pop TO_SQ    
        jmp .tests
        
    .castle_cond2:
        pop TO_SQ
        push EP
        sub EP, TO_SQ
        cmp EP, 2
        jl .ep_king_capture
        pop EP
        jmp .tests
        
    .ep_king_capture:
        pop EP
        jmp .return
    
    .tests:    
        test CAP, SIDE                             ; captured own piece? ;
        jne .end_dir                      ; if so then break out of loop ;
        cmp PIECE, 3                                     ; is it a pawn? ;        
        jl .is_pawn                          ; if so then go to .is_pawn ;
        
    .is_legal:                                  ; check if move is legal ;
        and CAP, 7                         ; extract captured piece type ;
        cmp CAP, 3                           ; if captured piece is king ;
        je .return                                  ; then go to .return ;

    .make_move:                                    ; otherwise make move ;
        PRINT_MOVE FROM_SQ, TO_SQ, 0
        inc Nodes
        
        cmp PIECE, 5                             ; is it a slider piece? ;            
        jl .fake_capture               ; if not then go to .fake_capture ;
        jmp .end_sq                            ; otherwise continue loop ;
    
    .is_pawn:                                        ; now piece is pawn ;
        TEST VEC(STEP), 7              ; is pawn going straight forward? ;
        je .pawn_quiet                    ; if so then go to .pawn_quiet ;
        jne .pawn_cap                        ; otherwise go to .pawn_cap ;

    .pawn_quiet:                            ; pawn goes straight forward ;
        cmp CAP, 0                             ; is target square empty? ;
        je .is_legal                        ; if so then go to .is_legal ;
        jmp .end_dir                       ; otherwise break out of loop ;
        
    .pawn_cap:                                    ; pawn goes diagonally ;
        cmp CAP, 0                             ; is target square empty? ;
        jne .is_legal                      ; if not then go to .is_legal ;
        cmp TO_SQ, EP                       ; if target square equals ep ;
        je .set_cap_sq                          ; then go to .set_cap_sq ;
        jmp .end_dir                       ; otherwise break out of loop ;

    .set_cap_sq:
        push TO_SQ
        xor TO_SQ, 16
        mov CAP_SQ, TO_SQ
        pop TO_SQ
        jmp .is_legal

    .double_push:                        ; pawn goes two squares forward ;
        PUSH_DATA
        mov eax, SIDE       ; 6 * S + (v.t & 112) == 128 ;
        mov ebx, 6
        mul ebx
        push TO_SQ
        and TO_SQ, 112
        add TO_SQ, eax
        cmp TO_SQ, 128
        je .pawn_ep
        pop TO_SQ        
        
        POP_DATA
        jmp .end_sq                                      ; go to .end_sq ;
    
    .castling:
        pop PIECE                       ; retreive piece type from stack ;
        cmp STEP, 13                       ; if king is moving king side ;
        je .KQkq                                      ; then go to .KQkq ;
        cmp STEP, 15                      ; if king is moving queen side ;
        je .KQkq                                      ; then go to .KQkq ;
        jmp .end_sq                        ; otherwise break out of loop ;
        
    .KQkq:
        test ROOK_SQ, VALID_SQ                    ; is ROOK_SQ offboard? ;
        je .end_sq                       ; if not then break out of loop ;
        push FROM_SQ
        push VEC(STEP)
        shr VEC(STEP), 1
        and VEC(STEP), 7
        or FROM_SQ, 7
        sub FROM_SQ, VEC(STEP)
        mov ROOK_SQ, FROM_SQ
        push BRD(FROM_SQ)
        and BRD(FROM_SQ), 32
        jne .jmp_sq1
        pop BRD(FROM_SQ)
        pop VEC(STEP)
        pop FROM_SQ
        jmp .end_sq
        
    .jmp_sq1:
        pop BRD(FROM_SQ)
        pop VEC(STEP)
        pop FROM_SQ
        push FROM_SQ
        mov FROM_SQ, ROOK_SQ
        xor FROM_SQ, 1
        cmp BRD(FROM_SQ), 0
        je .jmp_sq2
        pop FROM_SQ
        jmp .end_sq
        
    .jmp_sq2:
        mov FROM_SQ, ROOK_SQ
        xor FROM_SQ, 2
        cmp BRD(FROM_SQ), 0
        je .king_ep        
        pop FROM_SQ    
        jmp .end_sq
        
    .fake_capture:                        ; fake capture for non-sliders ;
        mov CAP, 1                 ; to stop looping over target squares ;
        cmp PIECE, 3                                  ; if piece is pawn ;
        jl .double_push                             ; go to .double_push ;
        push PIECE                            ; save piece type in stack ;
        mov PIECE, BRD(FROM_SQ)                       ; initialize piece ;
        and PIECE, -25              ; strip side bit, leaving virgin bit ;
        cmp PIECE, 35                                 ; if piece is king ;
        je .castling                              ; then go to .castling ;
        pop PIECE                        ; otherwise retrieve piece type ;
        jmp .end_sq                        ; otherwise break out of loop ;
    
    .king_ep:
        mov CAP, 0
        pop FROM_SQ
        mov SKIP_SQ, TO_SQ
        jmp .end_sq
    
    .pawn_ep:                                    ; set en passant square ;
        mov CAP, 0                          ; give piece extra iteration ;
        pop TO_SQ                    ; retreive target square from stack ;
        POP_DATA
        mov SKIP_SQ, TO_SQ          ; set search arg EP to target square ;
            
    .end_sq:
        cmp CAP, 0                          ; if didn't capture anything ;
        je .loop_sq                      ; continue looping over squares ;
        
    .end_dir: 
        inc STEP
        cmp VEC(STEP), 0
        jne .loop_dir
        jmp .end_brd

    .skip_sq:
        add FROM_SQ, 7                            ; skip offboard square ;
    
    .end_brd:
        inc FROM_SQ                             ; increment square count ;
        cmp FROM_SQ, 128                    ; test if board is exhausted ;
        jne .loop_brd                           ; LOOP OVER BOARD PIECES ;
    
    .return:
        mov esp, ebp                                           ; DESTROY ;
        pop ebp                                                  ; STACK ;
        ret                                                     ; return ;

main:
    PARSE_FEN trickyPos
    PRINT_BOARD
    GENERATE_MOVES Side, Ep, 0, 0, 5
    NUM Nodes

    ret
    
    
    
    
    
