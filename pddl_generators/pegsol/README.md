Adaption of Puzzle library from Solipeg 2.2
(http://ourworld.compuserve.com/homepages/cade/psionsof.htm, changed all
unmovable pegs to movable pegs)

Solipeg, a Classic Marble Puzzle Game for the Psion Series 3a, 3c, Siena
Version 2.2 (and 2.2 Lite) Copyright (C) 1993, 1994, 1995, 1996 J Cade Roux

"The game Puzzle-Peg (Third Edition, 1924, Lubbers & Bell Mfg. Co.,Clinton,
Iowa, USA, 50 cents) included a small booklet (23 pages),"Problems in
Puzzle-Peg", which details over 100 different end-game-type problems which
can be played on the standard English board, sent in by players of their
earlier versions. It also contained adverts for other games of theirs. All
104 of the problems are included in this distribution - embedded in the
library. The Lite version contains only the standard two starting positions,
although an external library is provided which is comparable to the internal
library."




# pddl-generators memo

The original code was probably written for ruby 1.8.
The code was fixed for ruby 2.x.
This ruby code reproduces instances takes from the book.

`ruby converter.rb` seems to generate a template for `instances.rb`.
`ruby -I. generator.rb` will generate problem files.


A new randomized implementation (generate.py) will generate a new board
which is not guaranteed to be solvable.
It generates a random board state and has a customizable board size and a corner drop off size.
