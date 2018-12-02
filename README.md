# nibble-chess
A tribute to chess programming community based on ideas taken from micro-Max by H.G. Muller

[![nibble-chess](https://github.com/maksimKorzh/nibble-chess/blob/master/nibble-chess.gif)]

# Overview

    A large chess programming community helped me a lot over last three years, now I want to give
    it something back. Nibble chess is a simple console based chess program which can be extended
    to UCI chess engine. It's not intended to be used by end users, but by beginner programmers
    who looks for some sort of resources to get started with. I wish I could have such program
    when just started to learn chess programming. The source is mostly based on H.G. Muller's ideas, as
    they are implemented in his micro-Max engine.

# Features

    - 0x88 board representation
    - three nested loops pseudo-legal movegenerator
    - material evaluation + positional scores(stored in right part of 0x88 board)
    - tricky move ordering technique: scoring move scores during move generation 
    - very basic alpha-beta search
    - quiescence search
    - very basic console-based interface

# Additional resources

    

# Useful resources
  http://home.hccnet.nl/h.g.muller/max-src2.html - H. G. Muller's micro-Max tutorial
  https://www.chessprogramming.org/Main_Page - chess programming wiki
  http://talkchess.com/forum3/index.php - chess programming forum
