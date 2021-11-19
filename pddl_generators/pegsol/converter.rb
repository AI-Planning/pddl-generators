#!/usr/bin/ruby

config_names = [
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
]

#  'a' == UNUSABLE
#  'b' == EMPTY
#  'c' == PEG
configs = [
           "aabbbaaaabcbaabbcccbbbbbcccbbbcccbbaabcbaaaabbbaa",
           "aabbbaaaabcbaabbcbcbbbcbbcbbbbbbbbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabbccbbbbbbccbbbbcbbbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabbcbcbbbbbbbcbbbbccbbaacbbaaaabbbaa",
           "aabbbaaaabcbaabbcccbbbbcccbbbbcccbbaabcbaaaabbbaa",
           "aabbbaaaaccbaabccccbbbcbbbbbbbcbcbbaabcbaaaabbbaa",
           "aabbbaaaabbbaabccbbbbbbbbccbbbcccbbaabcbaaaabbbaa",
           "aabbcaaaabbcaabbccbcbbbccbbbbbcbbbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabbbbcbbbbccbcbbbcbcbbaabbcaaaabbbaa",
           "aabbbaaaabbbaabccbcbbbcbcbcbbbcbcbbaabbcaaaabbbaa",
           "aabbbaaaabbbaabcbccbbbbcccbbbbbcbbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbbbcbbbcbccbbbbcccbbaabbcaaaabbbaa",
           "aabbbaaaabbbaabbccccbbbcccbbbbcbcbbaabbcaaaabbbaa",
           "aabbbaaaabbbaabbbbcbbbbccbbbbbbccbbaabcbaaaabbbaa",
           "aabbbaaaabbbaabbcccbbbbccbbbbbcbbbbaacbbaaaabbbaa",
           "aabbbaaaacbbaabbcbbbbbbccbbbbcccbbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbcbccbbbbcbbbbccbccbaaccbaaaabbbaa",
           "aabbbaaaabbbaabbbcccbbbcccbbbbcbcbbaabcbaaaabbbaa",
           "aabbbaaaabbcaabbcccbbbbcccbbbbccccbaabcbaaaabbbaa",
           "aabbbaaaabbcaabbcccbbbbcccbbbbccccbaabcbaaaabbbaa",
           "aabbbaaaacbbaabbbccbbbcbbbbbbbcccbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabbcccbbbbbccbbbbccbbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbcbccbbbcbbcbbbcbcbbaacbbaaaabbbaa",
           "aabbbaaaabbbaabbcccbbbbbbccbbbcccbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbcccbbbbbbbbbbbcbcbbaaccbaaaabbbaa",
           "aabbbaaaabbbaabbcbcbbbcccbbbbccbbbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbcccbbbbcccbbbbbccbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbbccbbbcbccbbbbcccbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbcbcbbbcbcbccbbcbcbbaabbbaaaabbbaa",
           "aabcbaaaabcbaabbcccbbcccbcccbbcccbbaabcbaaaabcbaa",
           "aabbbaaaaccbaaccbcbccccbcbccccbcbcbaabccaaaabbcaa",
           "aabbbaaaabbbaacccbccbccbbbbbcbbccbbaacbbaaaabbbaa",
           "aacbcaaaacccaabbbbbbbbcbcbccbbbbbbbaacccaaaacbcaa",
           "aabbbaaaabbbaacccbbbbccccbbbcccbbbbaabbbaaaabbbaa",
           "aabbcaaaabccaabbcbcbbbccccccbbbbcbbaabbbaaaabbbaa",
           "aabcbaaaacccaabcbbbcbcbbcbbcccbbbcbaacbcaaaabcbaa",
           "aabbbaaaabbbaabccccbcbcbbbcbcbcbcbbaacbbaaaabbbaa",
           "aacbbaaaaccbaabbbccbbbbbcccbbbbbbccaabbbaaaabbbaa",
           "aabbbaaaabbbaacccbccbcccbbbbcccbccbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbbcbcbcbcccbcccbbcccaabcbaaaaccbaa",
           "aabbbaaaabcbaaccccbbbccccbbbccccbbbaabcbaaaabbbaa",
           "aabbbaaaabcbaabbcccbbbcccbcbcbcccbbaabbbaaaabbcaa",
           "aabbbaaaabbbaabbbbcccbbbbbbbbcbbcccaaccbaaaacbbaa",
           "aabbcaaaabbcaabbccbbbbccccccbbcccbbaacccaaaabcbaa",
           "aabbbaaaabbcaabbbbbcbbbccbcbbbbcbcbaaccbaaaabbcaa",
           "aaccbaaaaccbaabbbbbbbbbbbccbbbbbbbbaaccbaaaabbbaa",
           "aabccaaaabccaabbccccbccccbbbbbbccbbaabccaaaabccaa",
           "aabbbaaaabcbaabbcbccbbccbbbbbbcbcbbaabbcaaaabbbaa",
           "aabbbaaaabbbaabcbbbbbbcccbbbbccbcbbaaccbaaaabbbaa",
           "aabbbaaaabbbaabcbbbbbbcccbbbbccbcbbaaccbaaaabbbaa",
           "aabbbaaaaccbaacccbbcbbbcbcccccbccbbaabcbaaaabbbaa",
           "aabbbaaaabcbaabbcbcccbcbbcccbbbbcbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabcbbcbbccbcbbbbccccbbaacbbaaaabbbaa",
           "aabbbaaaaccbaabccbccbbbbcbbbbbcbcbbaabccaaaacbbaa",
           "aabbbaaaacbbaacbccccbbcbcbbbbccbbccaabbcaaaabbcaa",
           "aacbbaaaacbcaabbbcbbbbbbcbbbbcbbccbaacbbaaaacbcaa",
           "aabbbaaaabbbaabbbccbbbbbccccbbbccbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabbcbccbbcbcbbbbccbcbbaabbbaaaabbbaa",
           "aabbbaaaabbbaabbcccbbbbcccbbbbcccbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabbcbcccbbbbbccbccbcccaabccaaaabccaa",
           "aabbbaaaabcbaabbcbbcbbbbbbccbbcbbcbaacbcaaaaccbaa",
           "aaccbaaaaccbaabbccbbbbcbbccbbbccbbbaaccbaaaabbbaa",
           "aabbbaaaabccaabbcbcccccbbbcccccccbbaacbbaaaabbbaa",
           "aabbbaaaaccbaabbccbcbbbcccbbbbccccbaabbbaaaabbbaa",
           "aacbbaaaacccaabccbcbbbcbccbbbccbbccaacccaaaabbbaa",
           "aabbbaaaabbbaabbbbcbbbbbbbccccbcbccaaccbaaaabccaa",
           "aaccbaaaacbbaabbbccbbbbbccccbbbbcbbaaccbaaaacbbaa",
           "aacbbaaaabcbaabbcccbbbcbbccbbbcccbbaabcbaaaabbbaa",
           "aabcbaaaacbcaabcbcbcbcccbcbcbcbcccbaacbcaaaabcbaa",
           "aabbcaaaabcbaabbcbbccbccbcccbbcbbccaabcbaaaabbcaa",
           "aaccbaaaaccbaabbccbbbcccccccbbccbbbaaccbaaaaccbaa",
           "aabcbaaaaccbaabcccbbbcccccccbbbcccbaabccaaaabcbaa",
           "aabbcaaaabbcaacbccccbbbcccbbbccbcbcaabbcaaaacbbaa",
           "aacbcaaaacccaaccbcbccbcccccbccbcbccaacccaaaacbcaa",
           "aabbbaaaacbbaabccbbbbcbccbbbbccbbbbaacbbaaaabbbaa",
           "aabbbaaaacbbaabcccbbbbccccccbbbccbbaabbbaaaabbbaa",
           "aabbbaaaaccbaabcbbcbbbcbbbcbbcbbcbbaaccbaaaabbbaa",
           "aabbbaaaacbbaabcbccbbcccbbbbbcbcbbbaacbcaaaabcbaa",
           "aabbbaaaabcbaacbbbbbbbccbbcbbbbbbbcaabcbaaaacbbaa",
           "aabccaaaabccaabccbbccbccccbbbccbbccaacbbaaaabbbaa",
           "aabcbaaaabcbaabbbcbbbcccccccbbbcbbbaabcbaaaabcbaa",
           "aabcbaaaabcbaabbcbcbbbcbbbccbbcbcbbaabcbaaaabbbaa",
           "aabcbaaaabcbaabbcbcbbccbcbccbbcbcbbaabcbaaaabcbaa",
           "aabcbaaaacccaaccbcbccbcccbccccccbccaabccaaaabcbaa",
           "aabbbaaaacccaabccbccbbcbbbcbbccbccbaacccaaaabbbaa",
           "aabcbaaaabccaabbcbcbbbcbbbcbbbcbcbbaaccbaaaabcbaa",
           "aacccaaaaccbaabccbcbbbbccccbbbcccbbaabcbaaaabbbaa",
           "aabcbaaaaccbaabbbbbcbbcbbbbccccbbcbaaccbaaaabbcaa",
           "aabbbaaaabcbaabcccbcbccbcbbbbcbbbcbaabcbaaaabbbaa",
           "aabbbaaaaccbaacccbbccccbccccccbcccbaacccaaaabcbaa",
           "aabcbaaaacccaabcbcccbcccbcccbbcbbcbaabccaaaabcbaa",
           "aabcbaaaabcbaaccbcbcbbccbcccccbcbcbaabcbaaaabcbaa",
           "aabccaaaacbbaabcbbcccbbbbcccbcbbcccaacbbaaaabccaa",
           "aabbbaaaacccaabcbbbcbbcbbbcbbcbbbcbaacccaaaabbbaa",
           "aacbbaaaacccaabbbbbcbcccccccbbbbbcbaacbcaaaaccbaa",
           "aabbbaaaabbcaacbcccbbbcccbcbbbcccbbaaccbaaaabbcaa",
           "aabbbaaaabbbaabbbbcccbbbbbccbbbcbccaabccaaaacccaa",
           "aabbbaaaacccaabccccbbcccccccbccccbbaacccaaaabbbaa",
           "aabccaaaacccaabcbcccccccccccbcbccccaacccaaaacccaa",
           "aabbbaaaacccaaccbbbcbccbcbcbccbbbcbaacccaaaabbbaa",
           "aabcbaaaabcbaabcccbbbbcccbbbbccbbbbaabbbaaaabbbaa",
           "aabbbaaaabcbaabbcccbbbccbccbbbcccbbaabcbaaaabbbaa",
           "aabcbaaaabbbaabbcccbbbccbcbcbbcbbbbaabbbaaaabbbaa",
           "aabbbaaaacccaabcccccbbccbccbbcccccbaacccaaaabbbaa"
]


def pad(counter, maxCounter)
  ("0"*(Math.log10(maxCounter).ceil-counter.to_s.length))+counter.to_s
end

def translateChar(char, row, col)
  if(row == 3 and col == 3) then
    if(char == "a") then
      "X"
    else if(char == "b") then
           "O"
         else if(char == "c") then
                "+"
              end
         end
    end
  else 
    if(char == "a") then
      "X"
    else if(char == "b") then
           "o"
         else if(char == "c") then
                "*"
              end
         end
    end
  end
end

maxCounter = 104
counter = 1

configs.each do | config |
  printf("$config_#{pad(counter,maxCounter)} =\n")
  printf("  [\n")
  0.upto 6 do | line |
    c1 = translateChar(config[(line*7)+0..(line*7)+0], line, 0)
    c2 = translateChar(config[(line*7)+1..(line*7)+1], line, 1)
    c3 = translateChar(config[(line*7)+2..(line*7)+2], line, 2)
    c4 = translateChar(config[(line*7)+3..(line*7)+3], line, 3)
    c5 = translateChar(config[(line*7)+4..(line*7)+4], line, 4)
    c6 = translateChar(config[(line*7)+5..(line*7)+5], line, 5)
    c7 = translateChar(config[(line*7)+6..(line*7)+6], line, 6)
    comma = ","
    if line == 6 then comma = "" end
    printf("   %%w(%s %s %s %s %s %s %s)%s\n", c1, c2, c3, c4, c5, c6, c7, comma)
  end
  printf("  ]\n\n")
  counter+=1
end

printf("$config_names = [\n")
config_names.each do | name |
  comma = ","
  printf("                 \"%s\",\n", name)
end
printf("                ]\n")
printf("\n")


counter = 1
printf("$all_configs = [\n")
configs.each do | config |
  printf("    $config_#{pad(counter,maxCounter)},\n")
  counter+=1
end
printf("               ]\n")
printf("\n")

