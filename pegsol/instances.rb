# Adaption of Puzzle library from Solipeg 2.2
# (http://ourworld.compuserve.com/homepages/cade/psionsof.htm, changed all
# unmovable pegs to movable pegs)
# 
# Solipeg, a Classic Marble Puzzle Game for the Psion Series 3a, 3c, Siena
# Version 2.2 (and 2.2 Lite) Copyright (C) 1993, 1994, 1995, 1996 J Cade Roux
# 
# "The game Puzzle-Peg (Third Edition, 1924, Lubbers & Bell Mfg. Co.,Clinton,
# Iowa, USA, 50 cents) included a small booklet (23 pages),"Problems in
# Puzzle-Peg", which details over 100 different end-game-type problems which
# can be played on the standard English board, sent in by players of their
# earlier versions. It also contained adverts for other games of theirs. All
# 104 of the problems are included in this distribution - embedded in the
# library. The Lite version contains only the standard two starting positions,
# although an external library is provided which is comparable to the internal
# library."

$INVALID         = "X"
$OCCUPIED        = "*"
$OCCUPIED_TARGET = "+"
$FREE            = "o"
$FREE_TARGET     = "O"
$INDENT   = "        "

# 'X' denotes 'no valid position'
# '*' denotes 'valid position, occupied, not target position'
# '+' denotes 'valid position, occupied, target position'
# 'o' denotes 'valid position, free, not target position'
# 'O' denotes 'valid position, free, target position'
$config_105 =
  [
   %w(X X * * * X X),
   %w(X X * * * X X),
   %w(* * * * * * *),
   %w(* * * O * * *),
   %w(* * * * * * *),
   %w(X X * * * X X),
   %w(X X * * * X X)
  ] 

$europeanConfiguration =
  [
   %w(X X * * * X X),
   %w(X * * * * * X),
   %w(* * * * * * *),
   %w(* * * O * * *),
   %w(* * * * * * *),
   %w(X * * * * * X),
   %w(X X * * * X X)
  ]

$dummyConfiguration =
  [
   %w(* * O)
  ]

$squareConfiguration =
  [
   %w(* * * * *),
   %w(* * * * *),
   %w(* * * * *),
   %w(* * O * *),
   %w(* * * * *)
  ]

$config_001 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * * * o o),
   %w(o o o + * * o),
   %w(o o * * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_002 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * o * o o),
   %w(o * o O * o o),
   %w(o o o o o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_003 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * * o o o),
   %w(o o o + * o o),
   %w(o o * o o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_004 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * o * o o),
   %w(o o o O o * o),
   %w(o o o * * o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_005 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * * * o o),
   %w(o o * + * o o),
   %w(o o * * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_006 =
  [
   %w(X X o o o X X),
   %w(X X * * o X X),
   %w(o * * * * o o),
   %w(o * o O o o o),
   %w(o o * o * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_007 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o * * o o o o),
   %w(o o o O * * o),
   %w(o o * * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_008 =
  [
   %w(X X o o * X X),
   %w(X X o o * X X),
   %w(o o * * o * o),
   %w(o o * + o o o),
   %w(o o * o o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_009 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o o o * o o),
   %w(o o * + o * o),
   %w(o o * o * o o),
   %w(X X o o * X X),
   %w(X X o o o X X)
  ]

$config_010 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o * * o * o o),
   %w(o * o + o * o),
   %w(o o * o * o o),
   %w(X X o o * X X),
   %w(X X o o o X X)
  ]

$config_011 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o * o * * o o),
   %w(o o * + * o o),
   %w(o o o * o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_012 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o o * o o),
   %w(o * o + * o o),
   %w(o o * * * o o),
   %w(X X o o * X X),
   %w(X X o o o X X)
  ]

$config_013 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * * * * o),
   %w(o o * + * o o),
   %w(o o * o * o o),
   %w(X X o o * X X),
   %w(X X o o o X X)
  ]

$config_014 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o o * o o),
   %w(o o * + o o o),
   %w(o o o * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_015 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * * * o o),
   %w(o o * + o o o),
   %w(o o * o o o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_016 =
  [
   %w(X X o o o X X),
   %w(X X * o o X X),
   %w(o o * o o o o),
   %w(o o * + o o o),
   %w(o * * * o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_017 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * o * * o),
   %w(o o o + o o o),
   %w(o * * o * * o),
   %w(X X * * o X X),
   %w(X X o o o X X)
  ]

$config_018 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o * * * o),
   %w(o o * + * o o),
   %w(o o * o * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_019 =
  [
   %w(X X o o o X X),
   %w(X X o o * X X),
   %w(o o * * * o o),
   %w(o o * + * o o),
   %w(o o * * * * o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_020 =
  [
   %w(X X o o o X X),
   %w(X X o o * X X),
   %w(o o * * * o o),
   %w(o o * + * o o),
   %w(o o * * * * o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_021 =
  [
   %w(X X o o o X X),
   %w(X X * o o X X),
   %w(o o o * * o o),
   %w(o * o O o o o),
   %w(o o * * * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_022 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * * * o o),
   %w(o o o + * o o),
   %w(o o * * o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_023 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * o * * o),
   %w(o o * O o * o),
   %w(o o * o * o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_024 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * * * o o),
   %w(o o o O * * o),
   %w(o o * * * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_025 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * * * o o),
   %w(o o o O o o o),
   %w(o o * o * o o),
   %w(X X * * o X X),
   %w(X X o o o X X)
  ]

$config_026 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * o * o o),
   %w(o * * + o o o),
   %w(o * * o o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_027 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * * * o o),
   %w(o o * + * o o),
   %w(o o o * * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_028 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o * * o o),
   %w(o * o + * o o),
   %w(o o * * * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_029 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * o * o o),
   %w(o * o + o * *),
   %w(o o * o * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_030 =
  [
   %w(X X o * o X X),
   %w(X X o * o X X),
   %w(o o * * * o o),
   %w(* * * O * * *),
   %w(o o * * * o o),
   %w(X X o * o X X),
   %w(X X o * o X X)
  ]

$config_031 =
  [
   %w(X X o o o X X),
   %w(X X * * o X X),
   %w(* * o * o * *),
   %w(* * o + o * *),
   %w(* * o * o * o),
   %w(X X o * * X X),
   %w(X X o o * X X)
  ]

$config_032 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(* * * o * * o),
   %w(* * o O o o o),
   %w(* o o * * o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_033 =
  [
   %w(X X * o * X X),
   %w(X X * * * X X),
   %w(o o o o o o o),
   %w(o * o + o * *),
   %w(o o o o o o o),
   %w(X X * * * X X),
   %w(X X * o * X X)
  ]

$config_034 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(* * * o o o o),
   %w(* * * + o o o),
   %w(* * * o o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_035 =
  [
   %w(X X o o * X X),
   %w(X X o * * X X),
   %w(o o * o * o o),
   %w(o * * + * * *),
   %w(o o o o * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_036 =
  [
   %w(X X o * o X X),
   %w(X X * * * X X),
   %w(o * o o o * o),
   %w(* o o + o o *),
   %w(* * o o o * o),
   %w(X X * o * X X),
   %w(X X o * o X X)
  ]

$config_037 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o * * * * o *),
   %w(o * o O o * o),
   %w(* o * o * o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_038 =
  [
   %w(X X * o o X X),
   %w(X X * * o X X),
   %w(o o o * * o o),
   %w(o o o + * * o),
   %w(o o o o o * *),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_039 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(* * * o * * o),
   %w(* * * O o o o),
   %w(* * * o * * o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_040 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o * o * o),
   %w(* o * + * o *),
   %w(* * o o * * *),
   %w(X X o * o X X),
   %w(X X * * o X X)
  ]

$config_041 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(* * * * o o o),
   %w(* * * + o o o),
   %w(* * * * o o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_042 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * * * o o),
   %w(o * * + o * o),
   %w(* o * * * o o),
   %w(X X o o o X X),
   %w(X X o o * X X)
  ]

$config_043 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o o * * *),
   %w(o o o O o o o),
   %w(o * o o * * *),
   %w(X X * * o X X),
   %w(X X * o o X X)
  ]

$config_044 =
  [
   %w(X X o o * X X),
   %w(X X o o * X X),
   %w(o o * * o o o),
   %w(o * * + * * *),
   %w(o o * * * o o),
   %w(X X * * * X X),
   %w(X X o * o X X)
  ]

$config_045 =
  [
   %w(X X o o o X X),
   %w(X X o o * X X),
   %w(o o o o o * o),
   %w(o o * + o * o),
   %w(o o o * o * o),
   %w(X X * * o X X),
   %w(X X o o * X X)
  ]

$config_046 =
  [
   %w(X X * * o X X),
   %w(X X * * o X X),
   %w(o o o o o o o),
   %w(o o o O * * o),
   %w(o o o o o o o),
   %w(X X * * o X X),
   %w(X X o o o X X)
  ]

$config_047 =
  [
   %w(X X o * * X X),
   %w(X X o * * X X),
   %w(o o * * * * o),
   %w(* * * + o o o),
   %w(o o o * * o o),
   %w(X X o * * X X),
   %w(X X o * * X X)
  ]

$config_048 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * o * * o),
   %w(o * * O o o o),
   %w(o o * o * o o),
   %w(X X o o * X X),
   %w(X X o o o X X)
  ]

$config_049 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o * o o o o o),
   %w(o * * + o o o),
   %w(o * * o * o o),
   %w(X X * * o X X),
   %w(X X o o o X X)
  ]

$config_050 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o * o o o o o),
   %w(o * * + o o o),
   %w(o * * o * o o),
   %w(X X * * o X X),
   %w(X X o o o X X)
  ]

$config_051 =
  [
   %w(X X o o o X X),
   %w(X X * * o X X),
   %w(* * * o o * o),
   %w(o o * O * * *),
   %w(* * o * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_052 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * o * * *),
   %w(o * o O * * *),
   %w(o o o o * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_053 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o * o o * o o),
   %w(* * o + o o o),
   %w(o * * * * o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_054 =
  [
   %w(X X o o o X X),
   %w(X X * * o X X),
   %w(o * * o * * o),
   %w(o o o + o o o),
   %w(o o * o * o o),
   %w(X X o * * X X),
   %w(X X * o o X X)
  ]

$config_055 =
  [
   %w(X X o o o X X),
   %w(X X * o o X X),
   %w(* o * * * * o),
   %w(o * o + o o o),
   %w(o * * o o * *),
   %w(X X o o * X X),
   %w(X X o o * X X)
  ]

$config_056 =
  [
   %w(X X * o o X X),
   %w(X X * o * X X),
   %w(o o o * o o o),
   %w(o o o + o o o),
   %w(o * o o * * o),
   %w(X X * o o X X),
   %w(X X * o * X X)
  ]

$config_057 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o * * o o),
   %w(o o o + * * *),
   %w(o o o * * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_058 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * o * * o),
   %w(o * o + o o o),
   %w(o * * o * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_059 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o * * * o o),
   %w(o o * + * o o),
   %w(o o * * * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_060 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * o * * *),
   %w(o o o O o * *),
   %w(o * * o * * *),
   %w(X X o * * X X),
   %w(X X o * * X X)
  ]

$config_061 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * o o * o),
   %w(o o o O o * *),
   %w(o o * o o * o),
   %w(X X * o * X X),
   %w(X X * * o X X)
  ]

$config_062 =
  [
   %w(X X * * o X X),
   %w(X X * * o X X),
   %w(o o * * o o o),
   %w(o * o O * * o),
   %w(o o * * o o o),
   %w(X X * * o X X),
   %w(X X o o o X X)
  ]

$config_063 =
  [
   %w(X X o o o X X),
   %w(X X o * * X X),
   %w(o o * o * * *),
   %w(* * o O o * *),
   %w(* * * * * o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_064 =
  [
   %w(X X o o o X X),
   %w(X X * * o X X),
   %w(o o * * o * o),
   %w(o o * + * o o),
   %w(o o * * * * o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_065 =
  [
   %w(X X * o o X X),
   %w(X X * * * X X),
   %w(o * * o * o o),
   %w(o * o + * o o),
   %w(o * * o o * *),
   %w(X X * * * X X),
   %w(X X o o o X X)
  ]

$config_066 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o o * o o),
   %w(o o o O o * *),
   %w(* * o * o * *),
   %w(X X * * o X X),
   %w(X X o * * X X)
  ]

$config_067 =
  [
   %w(X X * * o X X),
   %w(X X * o o X X),
   %w(o o o * * o o),
   %w(o o o + * * *),
   %w(o o o o * o o),
   %w(X X * * o X X),
   %w(X X * o o X X)
  ]

$config_068 =
  [
   %w(X X * o o X X),
   %w(X X o * o X X),
   %w(o o * * * o o),
   %w(o * o O * * o),
   %w(o o * * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_069 =
  [
   %w(X X o * o X X),
   %w(X X * o * X X),
   %w(o * o * o * o),
   %w(* * * O * o *),
   %w(o * o * * * o),
   %w(X X * o * X X),
   %w(X X o * o X X)
  ]

$config_070 =
  [
   %w(X X o o * X X),
   %w(X X o * o X X),
   %w(o o * o o * *),
   %w(o * * O * * *),
   %w(o o * o o * *),
   %w(X X o * o X X),
   %w(X X o o * X X)
  ]

$config_071 =
  [
   %w(X X * * o X X),
   %w(X X * * o X X),
   %w(o o * * o o o),
   %w(* * * + * * *),
   %w(o o * * o o o),
   %w(X X * * o X X),
   %w(X X * * o X X)
  ]

$config_072 =
  [
   %w(X X o * o X X),
   %w(X X * * o X X),
   %w(o * * * o o o),
   %w(* * * + * * *),
   %w(o o o * * * o),
   %w(X X o * * X X),
   %w(X X o * o X X)
  ]

$config_073 =
  [
   %w(X X o o * X X),
   %w(X X o o * X X),
   %w(* o * * * * o),
   %w(o o * + * o o),
   %w(o * * o * o *),
   %w(X X o o * X X),
   %w(X X * o o X X)
  ]

$config_074 =
  [
   %w(X X * o * X X),
   %w(X X * * * X X),
   %w(* * o * o * *),
   %w(o * * + * * o),
   %w(* * o * o * *),
   %w(X X * * * X X),
   %w(X X * o * X X)
  ]

$config_075 =
  [
   %w(X X o o o X X),
   %w(X X * o o X X),
   %w(o * * o o o o),
   %w(* o * + o o o),
   %w(o * * o o o o),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_076 =
  [
   %w(X X o o o X X),
   %w(X X * o o X X),
   %w(o * * * o o o),
   %w(o * * + * * *),
   %w(o o o * * o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_077 =
  [
   %w(X X o o o X X),
   %w(X X * * o X X),
   %w(o * o o * o o),
   %w(o * o O o * o),
   %w(o * o o * o o),
   %w(X X * * o X X),
   %w(X X o o o X X)
  ]

$config_078 =
  [
   %w(X X o o o X X),
   %w(X X * o o X X),
   %w(o * o * * o o),
   %w(* * * O o o o),
   %w(o * o * o o o),
   %w(X X * o * X X),
   %w(X X o * o X X)
  ]

$config_079 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(* o o o o o o),
   %w(o * * O o * o),
   %w(o o o o o o *),
   %w(X X o * o X X),
   %w(X X * o o X X)
  ]

$config_080 =
  [
   %w(X X o * * X X),
   %w(X X o * * X X),
   %w(o * * o o * *),
   %w(o * * + * o o),
   %w(o * * o o * *),
   %w(X X * o o X X),
   %w(X X o o o X X)
  ]

$config_081 =
  [
   %w(X X o * o X X),
   %w(X X o * o X X),
   %w(o o o * o o o),
   %w(* * * + * * *),
   %w(o o o * o o o),
   %w(X X o * o X X),
   %w(X X o * o X X)
  ]

$config_082 =
  [
   %w(X X o * o X X),
   %w(X X o * o X X),
   %w(o o * o * o o),
   %w(o * o O o * *),
   %w(o o * o * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_083 =
  [
   %w(X X o * o X X),
   %w(X X o * o X X),
   %w(o o * o * o o),
   %w(* * o + o * *),
   %w(o o * o * o o),
   %w(X X o * o X X),
   %w(X X o * o X X)
  ]

$config_084 =
  [
   %w(X X o * o X X),
   %w(X X * * * X X),
   %w(* * o * o * *),
   %w(o * * + o * *),
   %w(* * * * o * *),
   %w(X X o * * X X),
   %w(X X o * o X X)
  ]

$config_085 =
  [
   %w(X X o o o X X),
   %w(X X * * * X X),
   %w(o * * o * * o),
   %w(o * o O o * o),
   %w(o * * o * * o),
   %w(X X * * * X X),
   %w(X X o o o X X)
  ]

$config_086 =
  [
   %w(X X o * o X X),
   %w(X X o * * X X),
   %w(o o * o * o o),
   %w(o * o O o * o),
   %w(o o * o * o o),
   %w(X X * * o X X),
   %w(X X o * o X X)
  ]

$config_087 =
  [
   %w(X X * * * X X),
   %w(X X * * o X X),
   %w(o * * o * o o),
   %w(o o * + * * o),
   %w(o o * * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_088 =
  [
   %w(X X o * o X X),
   %w(X X * * o X X),
   %w(o o o o o * o),
   %w(o * o O o o *),
   %w(* * * o o * o),
   %w(X X * * o X X),
   %w(X X o o * X X)
  ]

$config_089 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o * * * o * o),
   %w(* * o + o o o),
   %w(o * o o o * o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_090 =
  [
   %w(X X o o o X X),
   %w(X X * * o X X),
   %w(* * * o o * *),
   %w(* * o + * * *),
   %w(* * o * * * o),
   %w(X X * * * X X),
   %w(X X o * o X X)
  ]

$config_091 =
  [
   %w(X X o * o X X),
   %w(X X * * * X X),
   %w(o * o * * * o),
   %w(* * * O * * *),
   %w(o o * o o * o),
   %w(X X o * * X X),
   %w(X X o * o X X)
  ]

$config_092 =
  [
   %w(X X o * o X X),
   %w(X X o * o X X),
   %w(* * o * o * o),
   %w(o * * O * * *),
   %w(* * o * o * o),
   %w(X X o * o X X),
   %w(X X o * o X X)
  ]

$config_093 =
  [
   %w(X X o * * X X),
   %w(X X * o o X X),
   %w(o * o o * * *),
   %w(o o o O * * *),
   %w(o * o o * * *),
   %w(X X * o o X X),
   %w(X X o * * X X)
  ]

$config_094 =
  [
   %w(X X o o o X X),
   %w(X X * * * X X),
   %w(o * o o o * o),
   %w(o * o O o * o),
   %w(o * o o o * o),
   %w(X X * * * X X),
   %w(X X o o o X X)
  ]

$config_095 =
  [
   %w(X X * o o X X),
   %w(X X * * * X X),
   %w(o o o o o * o),
   %w(* * * + * * *),
   %w(o o o o o * o),
   %w(X X * o * X X),
   %w(X X * * o X X)
  ]

$config_096 =
  [
   %w(X X o o o X X),
   %w(X X o o * X X),
   %w(* o * * * o o),
   %w(o * * + o * o),
   %w(o o * * * o o),
   %w(X X * * o X X),
   %w(X X o o * X X)
  ]

$config_097 =
  [
   %w(X X o o o X X),
   %w(X X o o o X X),
   %w(o o o o * * *),
   %w(o o o O o * *),
   %w(o o o * o * *),
   %w(X X o * * X X),
   %w(X X * * * X X)
  ]

$config_098 =
  [
   %w(X X o o o X X),
   %w(X X * * * X X),
   %w(o * * * * o o),
   %w(* * * + * * *),
   %w(o * * * * o o),
   %w(X X * * * X X),
   %w(X X o o o X X)
  ]

$config_099 =
  [
   %w(X X o * * X X),
   %w(X X * * * X X),
   %w(o * o * * * *),
   %w(* * * + * * *),
   %w(o * o * * * *),
   %w(X X * * * X X),
   %w(X X * * * X X)
  ]

$config_100 =
  [
   %w(X X o o o X X),
   %w(X X * * * X X),
   %w(* * o o o * o),
   %w(* * o + o * o),
   %w(* * o o o * o),
   %w(X X * * * X X),
   %w(X X o o o X X)
  ]

$config_101 =
  [
   %w(X X o * o X X),
   %w(X X o * o X X),
   %w(o * * * o o o),
   %w(o * * + o o o),
   %w(o * * o o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_102 =
  [
   %w(X X o o o X X),
   %w(X X o * o X X),
   %w(o o * * * o o),
   %w(o * * O * * o),
   %w(o o * * * o o),
   %w(X X o * o X X),
   %w(X X o o o X X)
  ]

$config_103 =
  [
   %w(X X o * o X X),
   %w(X X o o o X X),
   %w(o o * * * o o),
   %w(o * * O * o *),
   %w(o o * o o o o),
   %w(X X o o o X X),
   %w(X X o o o X X)
  ]

$config_104 =
  [
   %w(X X o o o X X),
   %w(X X * * * X X),
   %w(o * * * * * o),
   %w(o * * O * * o),
   %w(o * * * * * o),
   %w(X X * * * X X),
   %w(X X o o o X X)
  ]

$config_names = [
                 "1 Novice",
                 "2 Novice",
                 "3 Novice",
                 "4 Going",
                 "5 After Dinner",
                 "6 Novice",
                 "7 Tricky",
                 "8 Puzzler",
                 "9 Chase",
                 "10 Dizzy Pin",
                 "11 Relief",
                 "12 Fool",
                 "13 Perfect Lady",
                 "14 Move right",
                 "15 Backward",
                 "16 Hard?",
                 "17 See-Saw",
                 "18 Nerves",
                 "19 Foot Race",
                 "20 Pup",
                 "21 7th inning",
                 "22 Primary (1)",
                 "23 Forget (2)",
                 "24 Me Not (2)",
                 "25 But easy (2)",
                 "26 Watch (2)",
                 "27 Algebra (3)",
                 "28 Derby (3)",
                 "29 DPP (4)",
                 "30 Gridley",
                 "31 $",
                 "32 Soft",
                 "33 Missouri",
                 "34 Last Nine",
                 "35 Four (3)",
                 "36 Mulberry",
                 "37 Herm",
                 "38 Slant (1)",
                 "39 Buck",
                 "40 De Roire",
                 "41 Start",
                 "42 Bee",
                 "43 Minnehaha (4)",
                 "44 Cyclone",
                 "45 Diagonal (5)",
                 "46 Looks easy",
                 "47 Raleigh",
                 "48 Saskatchewan",
                 "49 Spokane (1)",
                 "50 Pigs (2)",
                 "51 SC",
                 "52 Slow train",
                 "53 Pikes Peak",
                 "54 Cattle",
                 "55 Kansas City",
                 "56 Manhattan",
                 "57 Hard (2)",
                 "58 Doctor",
                 "59 73 Years (4)",
                 "60 Bunch",
                 "61 Stump",
                 "62 Advert",
                 "63 Shave Pig",
                 "64 Minnesota",
                 "65 Chock",
                 "66 Football",
                 "67 Banker (2)",
                 "68 From 21",
                 "69 Checker (1)",
                 "70 Spearhead",
                 "71 One unusual",
                 "72 Butterfly (12)",
                 "73 Krazy Kat (2)",
                 "74 At home",
                 "75 Little Folk",
                 "76 Six (6)",
                 "77 Heart (3)",
                 "78 Wear out",
                 "79 Dirty Diag (6)",
                 "80 Doctor II",
                 "81 Cross (6)",
                 "82 Hat",
                 "83 Star (4)",
                 "84 Scatter",
                 "85 California",
                 "86 Older (2)",
                 "87 Boola Boola",
                 "88 Thirteen",
                 "89 Shift Play (5)",
                 "90 Cranium (5)",
                 "91 Tune (5)",
                 "92 Peggy",
                 "93 Mushroom",
                 "94 Dog Pound (7)",
                 "95 Gopher (3)",
                 "96 Fun (7)",
                 "97 Simply awful",
                 "98 X-Mas Tree (5)",
                 "99 Hard",
                 "100 Century (7)",
                 "101 Left Hand",
                 "102 Squares",
                 "103 Canuck (3)",
                 "104 Puzzle Peg Jr.",
                 "Plain"
#                 "106 European configuration",
#                 "107 Dummy configuration",
#                 "106 Square configuration"
                ]

$all_configs = [
                $config_001,
                $config_002,
                $config_003,
                $config_004,
                $config_005,
                $config_006,
                $config_007,
                $config_008,
                $config_009,
                $config_010,
                $config_011,
                $config_012,
                $config_013,
                $config_014,
                $config_015,
                $config_016,
                $config_017,
                $config_018,
                $config_019,
                $config_020,
                $config_021,
                $config_022,
                $config_023,
                $config_024,
                $config_025,
                $config_026,
                $config_027,
                $config_028,
                $config_029,
                $config_030,
                $config_031,
                $config_032,
                $config_033,
                $config_034,
                $config_035,
                $config_036,
                $config_037,
                $config_038,
                $config_039,
                $config_040,
                $config_041,
                $config_042,
                $config_043,
                $config_044,
                $config_045,
                $config_046,
                $config_047,
                $config_048,
                $config_049,
                $config_050,
                $config_051,
                $config_052,
                $config_053,
                $config_054,
                $config_055,
                $config_056,
                $config_057,
                $config_058,
                $config_059,
                $config_060,
                $config_061,
                $config_062,
                $config_063,
                $config_064,
                $config_065,
                $config_066,
                $config_067,
                $config_068,
                $config_069,
                $config_070,
                $config_071,
                $config_072,
                $config_073,
                $config_074,
                $config_075,
                $config_076,
                $config_077,
                $config_078,
                $config_079,
                $config_080,
                $config_081,
                $config_082,
                $config_083,
                $config_084,
                $config_085,
                $config_086,
                $config_087,
                $config_088,
                $config_089,
                $config_090,
                $config_091,
                $config_092,
                $config_093,
                $config_094,
                $config_095,
                $config_096,
                $config_097,
                $config_098,
                $config_099,
                $config_100,
                $config_101,
                $config_102,
                $config_103,
                $config_104,
                $config_105
#                $europeanConfiguration,
#                $dummyConfiguration,
#                $squareConfiguration
               ]
