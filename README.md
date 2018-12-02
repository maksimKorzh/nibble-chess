# nibble-chess
A tribute to chess programming community based on ideas taken from micro-Max by H.G. Muller

![nibble-chess](https://github.com/maksimKorzh/nibble-chess/blob/master/nibble-chess.gif)

# Overview

    A large chess programming community helped me a lot over last three years, now I want to give
    it something back. Nibble chess is a simple console based chess program which can be extended
    to UCI chess engine. It's not intended to be used by end users, but by beginner programmers
    who looks for some sort of resources to get started with. I wish I could have such program
    when just started to learn chess programming. The source is mostly based on H.G. Muller's ideas, as
    they are implemented in his micro-Max engine.

# Features

    - single source file of about 300 lines of C code
    - 0x88 board representation
    - three nested loops pseudo-legal move generator
    - material evaluation + positional scores(stored in right part of 0x88 board)
    - tricky move ordering technique: scoring move during move generation 
    - very basic alpha-beta search
    - quiescence search
    - user move validation
    - console-based interface

# Additional resources

    There are various code snippets available in res/ folder:
    
        - pretty fast move generator/perft tester with piece lists
        - reimplementation of micro-Max's move generator in NASM assembly
        - FEN parser for nibble chess
        - very basic and raw implementation of UCI loop for nibble-chess
        - single file chess project template of my own design

# Compile
    
    on linux:
        gcc -Ofast nibble-chess.c -o nibble-chess
    
    on windows:
        gcc.exe -std=c99 nibble-chess.c -o nibble-chess.exe && wine nibble-chess.exe
    
    If you're on windows please compile nibble-chess(ASCII).c file due to I shamely
    don't know how to print unicode chars under windows, sorry about that.

# Contributions

    Any forks and contributions are appriciated. Feel free to contact me on any questions
    via talkchess.com forum, opening issues on github or by email <freesoft.for.people@gmail.com>

# Contributors

    Roland Chastain - kindly pointed out windows compilation details and reported unicode chars output issue
 
# Useful resources

http://home.hccnet.nl/h.g.muller/max-src2.html - H. G. Muller's micro-Max tutorial

https://www.chessprogramming.org/Main_Page - chess programming wiki

http://talkchess.com/forum3/index.php - chess programming forum
