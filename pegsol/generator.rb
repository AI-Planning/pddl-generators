#!/usr/bin/ruby

require 'ftools'
require 'instances.rb'

class PegSolitaire

  def costFunction1(row, col)
    return 1
  end
  
  def costFunction2(row, col)
    dist = (@target["row"] - row).abs + (@target["col"] - col).abs + 1
    cost = dist * (dist + 1) / 2
    return cost
  end

  def costFunction3(row, col)
    dist = 5 - (@target["row"] - row).abs - (@target["col"] - col).abs
    cost = dist * (dist + 1) / 2
    if(cost < 1) then
      puts "Warning: For position #{posName(row, col)}, the computed cost for costFunction3 is less than 1, namely #{cost}."
    end
    return cost
  end

  def sumOfCostFunctionValues(costFunction)
    result = 0
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isOccupied(row, col)) then
          result += costFunction.call(row, col)
        end
      end
    end
    return result
  end

  def initialize(config,name)
    @config = config
    @name = name
    identifyTarget(config)
    identifySolvable(name)
    countPegs(config)
  end
  
  def identifyTarget(config)
    targetsFound = 0
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isTarget(row, col)) then
          @target = { "row" => row, "col" => col}
          targetsFound += 1
        end
      end
    end
    if targetsFound != 1 then
      puts "Error: The configuration #{config} contains #{targetsFound} target positions."
      puts "       There must be exactly one target in a valid problem instance."
      exit(1)
    end
  end

  def identifySolvable(name)
    @numDiagonalMoves = 0
    if not name.include?("(") then return end
    @numDiagonalMoves = name.split("(")[1].split(")")[0].to_i
  end

  def countPegs(config)
    @numPegs = 0
    (0...config.length).each do | row |
      (0...config[row].length).each do | col |
        if(isOccupied(row, col)) then
          @numPegs += 1
        end
      end
    end
  end

  def isValid(row, col)
    return (isOccupied(row, col) or isFree(row, col))
  end

  def isOccupied(row, col)
    return (@config[row][col] == $OCCUPIED || @config[row][col] == $OCCUPIED_TARGET)
  end

  def isFree(row, col)
    return (@config[row][col] == $FREE || @config[row][col] == $FREE_TARGET)
  end

  def isTarget(row, col)
    return (@config[row][col] == $FREE_TARGET || @config[row][col] == $OCCUPIED_TARGET)
  end

  def startsValidHorizontalLine(row, col)
    if @config[row].length < col+3 then return false end
    return(isValid(row,col) &&
           isValid(row,col+1) &&
           isValid(row,col+2))
  end

  def startsValidVerticalLine(row, col)
    if @config.length < row+3 then return false end
    return(isValid(row,col) &&
           isValid(row+1,col) &&
           isValid(row+2,col))
  end

  def posName(row, col)
    "pos-#{row}-#{col}"
  end

  def printPosObjects(stream)
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isValid(row, col)) then
          stream.printf("#{$INDENT}%s - location\n", posName(row, col))
        end
      end
    end
  end

  def printInLinePredicate(stream)
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(startsValidHorizontalLine(row, col)) then
          fst = posName(row, col)
          snd = posName(row, col+1)
          trd = posName(row, col+2)
          stream.printf("#{$INDENT}(IN-LINE %s %s %s)\n", fst, snd, trd)
          stream.printf("#{$INDENT}(IN-LINE %s %s %s)\n", trd, snd, fst)
        end
        if(startsValidVerticalLine(row, col)) then
          fst = posName(row, col)
          snd = posName(row+1, col)
          trd = posName(row+2, col)
          stream.printf("#{$INDENT}(IN-LINE %s %s %s)\n", fst, snd, trd)
          stream.printf("#{$INDENT}(IN-LINE %s %s %s)\n", trd, snd, fst)
        end
      end
    end
  end

  def printInitOccupiedPredicate(stream)
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isOccupied(row, col)) then
          stream.printf("#{$INDENT}(occupied %s)\n", posName(row, col))
        end
      end
    end
  end

  def printInitFreePredicate(stream)
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isFree(row, col)) then
          stream.printf("#{$INDENT}(free %s)\n", posName(row, col))
        end
      end
    end
  end

  def printGoalOccupiedPredicate(stream)
    stream.printf("#{$INDENT}(occupied %s)\n", posName(@target["row"], @target["col"]))
  end

  def printGoalFreePredicate(stream)
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isValid(row, col) and (row != @target["row"] or col != @target["col"])) then
          stream.printf("#{$INDENT}(free %s)\n", posName(row, col))
        end
      end
    end
  end
  
  def printPreferenceFreePredicate(stream)
    goalCount = 1
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isValid(row, col)) then
          stream.printf("#{$INDENT}(preference g%d (free %s))\n", goalCount, posName(row, col))
          goalCount+=1
        end
      end
    end
  end

  def printTargetFunctionNetBenefit(stream, costFunction)
    goalCount = 1
    (0...@config.length).each do | row |
      (0...@config[row].length).each do | col |
        if(isValid(row, col)) then
          stream.printf("#{$INDENT}(* (is-violated g%d) #{costFunction.call(row, col)})\n", goalCount)
          goalCount+=1
        end
      end
    end
  end

  def printCopyrightNoticeAndInstanceName(stream, linePrefix)
    stream.print("#{linePrefix}The following problem is taken from the problem library of Solipeg 2.2:\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}Solipeg, a Classic Marble Puzzle Game for the\n")
    stream.print("#{linePrefix}Psion Series 3a, 3c and Siena\n")
    stream.print("#{linePrefix}Version 2.2 (and 2.2 Lite)\n")
    stream.print("#{linePrefix}Copyright (C) 1993, 1994, 1995, 1996 J Cade Roux\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}This program is free software; you can redistribute it and/or modify\n")
    stream.print("#{linePrefix}it under the terms of the GNU General Public License as published by\n")
    stream.print("#{linePrefix}the Free Software Foundation; either version 2 of the License, or\n")
    stream.print("#{linePrefix}(at your option) any later version.\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}This program is distributed in the hope that it will be useful,\n")
    stream.print("#{linePrefix}but WITHOUT ANY WARRANTY; without even the implied warranty of\n")
    stream.print("#{linePrefix}MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n")
    stream.print("#{linePrefix}GNU General Public License for more details.\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}You should have received a copy of the GNU General Public License\n")
    stream.print("#{linePrefix}along with this program; if not, write to the Free Software\n")
    stream.print("#{linePrefix}Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}(see http://ourworld.compuserve.com/homepages/cade/psionsof.htm,\n")
    stream.print("#{linePrefix}http://ourworld.compuserve.com/homepages/cade/solsrc22.zip)\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}The Solipeg problem library in turn is derived from the booklet\n")
    stream.print("#{linePrefix}\"Problems in Puzzle-Peg\" included in the game Puzzle-Peg (Third\n")
    stream.print("#{linePrefix}Edition, 1924, Lubbers & Bell Mfg. Co.,Clinton, Iowa, USA).\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}Original problem number and name: \"#{@name.split("(")[0].strip()}\"\n")
  end

  def prettyPrintBoard(stream, linePrefix, leadingLine)
    if leadingLine then stream.print leadingLine end
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}Number of pegs:                  #{@numPegs}\n")
    stream.print("#{linePrefix}Number of diagonal moves needed: #{@numDiagonalMoves}\n")
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}Problem description:\n")
    stream.print("#{linePrefix}\"*\" denotes \"occupied\"\n")
    stream.print("#{linePrefix}\"o\" denotes \"free\"\n")
    stream.print("#{linePrefix}\n")
    @config.each do | line |
      stream.print(linePrefix)
      line.each do | column |
        if(column == $OCCUPIED || column == $OCCUPIED_TARGET)
          stream.print("* ")
        else
          if(column == $FREE || column == $FREE_TARGET)
            stream.print("o ")
          else
            if(column == $INVALID)
              stream.print("  ")
            end
          end
        end
      end
      stream.print "\n"
    end
    stream.print("#{linePrefix}\n")
    stream.print("#{linePrefix}Target position = (#{@target["row"]},#{@target["col"]})\n")
    stream.print("#{linePrefix}\n")
  end

  def printDomainTemporal(stream)
    stream.printf(";; Peg Solitaire domain\n")
    stream.printf("\n")
    stream.printf("(define (domain pegsolitaire-temporal)\n")
#    stream.printf("    (:requirements :typing :durative-actions :action-costs)\n")
    stream.printf("    (:requirements :typing :durative-actions)\n")
    stream.printf("    (:types location - object)\n")
    stream.printf("    (:predicates\n")
    stream.printf("        (IN-LINE ?x ?y ?z - location)\n")
    stream.printf("        (occupied ?l - location)\n")
    stream.printf("        (free ?l - location)\n")
    stream.printf("    )\n")
    stream.printf("\n")
    stream.printf("    (:durative-action jump\n")
    stream.printf("     :parameters (?from - location ?over - location ?to - location)\n")
    stream.printf("     :duration (= ?duration 1)\n")
    stream.printf("     :condition (and \n")
    stream.printf("                    (over all (IN-LINE ?from ?over ?to))\n")
    stream.printf("                    (at start (occupied ?from))\n")
    stream.printf("                    (at start (occupied ?over))\n")
    stream.printf("                    (at start (free ?to))\n")
    stream.printf("                )\n")
    stream.printf("     :effect (and\n")
    stream.printf("                 (at start (not (occupied ?from)))\n")
    stream.printf("                 (at start (not (occupied ?over)))\n")
    stream.printf("                 (at start (not (free ?to)))\n")
    stream.printf("                 (at end (free ?from))\n")
    stream.printf("                 (at end (free ?over))\n")
    stream.printf("                 (at end (occupied ?to))\n")
    stream.printf("             )\n")
    stream.printf("    )\n")
    stream.printf(")\n")
  end


  def printProblemTemporal(stream, probNum, instance)
    instance.printCopyrightNoticeAndInstanceName(stream, ";; ")
    instance.prettyPrintBoard(stream, ";; ", "")
    stream.printf("(define (problem pegsolitaire-temporal-#{probNum})\n")
    stream.printf("    (:domain pegsolitaire-temporal)\n")
    stream.printf("    (:objects\n")
    printPosObjects(stream)
    stream.printf("    )\n")
    stream.printf("    (:init\n")
    printInLinePredicate(stream)
    printInitFreePredicate(stream)
    printInitOccupiedPredicate(stream)
    stream.printf("    )\n")
    stream.printf("    (:goal (and\n")
    printGoalFreePredicate(stream)
    printGoalOccupiedPredicate(stream)
    stream.printf("           )\n")
    stream.printf("    )\n")
    stream.printf("    (:metric minimize (total-time))\n")
    stream.printf(")\n")
  end

  def printDomainNetBenefit(stream)
    stream.printf(";; Peg Solitaire domain\n")
    stream.printf("\n")
    stream.printf("(define (domain pegsolitaire-netbenefit)\n")
#    stream.printf("    (:requirements :typing :durative-actions :goal-utilities :action-costs)\n")
    stream.printf("    (:requirements :typing :durative-actions :goal-utilities)\n")
    stream.printf("    (:types location - object)\n")
    stream.printf("    (:predicates\n")
    stream.printf("        (IN-LINE ?x ?y ?z - location)\n")
    stream.printf("        (occupied ?l - location)\n")
    stream.printf("        (free ?l - location)\n")
    stream.printf("    )\n")
    stream.printf("\n")
    stream.printf("    (:action jump\n")
    stream.printf("     :parameters (?from - location ?over - location ?to - location)\n")
    stream.printf("     :precondition (and \n")
    stream.printf("                       (IN-LINE ?from ?over ?to)\n")
    stream.printf("                       (occupied ?from)\n")
    stream.printf("                       (occupied ?over)\n")
    stream.printf("                       (free ?to)\n")
    stream.printf("                   )\n")
    stream.printf("     :effect (and\n")
    stream.printf("                 (not (occupied ?from))\n")
    stream.printf("                 (not (occupied ?over))\n")
    stream.printf("                 (not (free ?to))\n")
    stream.printf("                 (free ?from)\n")
    stream.printf("                 (free ?over)\n")
    stream.printf("                 (occupied ?to)\n")
    stream.printf("             )\n")
    stream.printf("    )\n")
    stream.printf(")\n")
  end


  def printProblemNetBenefit(stream, probNum, instance, costFunction)
    instance.printCopyrightNoticeAndInstanceName(stream, ";; ")
    instance.prettyPrintBoard(stream, ";; ", "")
    stream.printf("(define (problem pegsolitaire-netbenefit-#{probNum})\n")
    stream.printf("    (:domain pegsolitaire-netbenefit)\n")
    stream.printf("    (:objects\n")
    printPosObjects(stream)
    stream.printf("    )\n")
    stream.printf("    (:init\n")
    printInLinePredicate(stream)
    printInitFreePredicate(stream)
    printInitOccupiedPredicate(stream)
    stream.printf("    )\n")
    stream.printf("    (:goal (and\n")
    printPreferenceFreePredicate(stream)
    stream.printf("           )\n")
    stream.printf("    )\n")
    stream.printf("    (:metric maximize (- #{sumOfCostFunctionValues(costFunction)} (+\n")
    printTargetFunctionNetBenefit(stream, costFunction)
    stream.printf("    )))\n")
    stream.printf(")\n")
  end

  def printDomainSequential(stream)
    stream.printf(";; Peg Solitaire sequential domain\n")
    stream.printf("\n")
    stream.printf("(define (domain pegsolitaire-sequential)\n")
    stream.printf("    (:requirements :typing :action-costs)\n")
    stream.printf("    (:types location - object)\n")
    stream.printf("    (:predicates\n")
    stream.printf("        (IN-LINE ?x ?y ?z - location)\n")
    stream.printf("        (occupied ?l - location)\n")
    stream.printf("        (free ?l - location)\n")
    stream.printf("        (move-ended)\n")
    stream.printf("        (last-visited ?l - location)\n")
    stream.printf("    )\n")
    stream.printf("    (:functions (total-cost) - number)\n")
    stream.printf("\n")
    stream.printf("    (:action jump-new-move\n")
    stream.printf("     :parameters (?from - location ?over - location ?to - location)\n")
    stream.printf("     :precondition (and \n")
    stream.printf("                       (move-ended)\n")
    stream.printf("                       (IN-LINE ?from ?over ?to)\n")
    stream.printf("                       (occupied ?from)\n")
    stream.printf("                       (occupied ?over)\n")
    stream.printf("                       (free ?to)\n")
    stream.printf("                   )\n")
    stream.printf("     :effect (and\n")
    stream.printf("                 (not (move-ended))\n")
    stream.printf("                 (not (occupied ?from))\n")
    stream.printf("                 (not (occupied ?over))\n")
    stream.printf("                 (not (free ?to))\n")
    stream.printf("                 (free ?from)\n")
    stream.printf("                 (free ?over)\n")
    stream.printf("                 (occupied ?to)\n")
    stream.printf("                 (last-visited ?to)\n")
    stream.printf("                 (increase (total-cost) 1)\n")
    stream.printf("             )\n")
    stream.printf("    )\n")
    stream.printf("\n")
    stream.printf("    (:action jump-continue-move\n")
    stream.printf("     :parameters (?from - location ?over - location ?to - location)\n")
    stream.printf("     :precondition (and \n")
    stream.printf("                       (last-visited ?from)\n")
    stream.printf("                       (IN-LINE ?from ?over ?to)\n")
    stream.printf("                       (occupied ?from)\n")
    stream.printf("                       (occupied ?over)\n")
    stream.printf("                       (free ?to)\n")
    stream.printf("                   )\n")
    stream.printf("     :effect (and\n")
    stream.printf("                 (not (occupied ?from))\n")
    stream.printf("                 (not (occupied ?over))\n")
    stream.printf("                 (not (free ?to))\n")
    stream.printf("                 (free ?from)\n")
    stream.printf("                 (free ?over)\n")
    stream.printf("                 (occupied ?to)\n")
    stream.printf("                 (not (last-visited ?from))\n")
    stream.printf("                 (last-visited ?to)\n")
    stream.printf("             )\n")
    stream.printf("    )\n")
    stream.printf("\n")
    stream.printf("    (:action end-move\n")
    stream.printf("     :parameters (?loc - location)\n")
    stream.printf("     :precondition (last-visited ?loc)\n")
    stream.printf("     :effect (and\n")
    stream.printf("                 (move-ended)\n")
    stream.printf("                 (not (last-visited ?loc))\n")
    stream.printf("             )\n")
    stream.printf("    )\n")
    stream.printf(")\n")
  end

  def printProblemSequential(stream, probNum, instance)
    instance.printCopyrightNoticeAndInstanceName(stream, ";; ")
    instance.prettyPrintBoard(stream, ";; ", "")
    stream.printf("(define (problem pegsolitaire-sequential-#{probNum})\n")
    stream.printf("    (:domain pegsolitaire-sequential)\n")
    stream.printf("    (:objects\n")
    printPosObjects(stream)
    stream.printf("    )\n")
    stream.printf("    (:init\n")
    stream.printf("#{$INDENT}(= (total-cost) 0)\n")
    stream.printf("#{$INDENT}(move-ended)\n")
    printInLinePredicate(stream)
    printInitFreePredicate(stream)
    printInitOccupiedPredicate(stream)
    stream.printf("    )\n")
    stream.printf("    (:goal (and\n")
    printGoalFreePredicate(stream)
    printGoalOccupiedPredicate(stream)
    stream.printf("           )\n")
    stream.printf("    )\n")
    stream.printf("    (:metric minimize (total-cost))\n")
    stream.printf(")\n")
  end

end



def pad(counter, maxCounter)
  ("0"*(Math.log10(maxCounter).ceil-counter.to_s.length))+counter.to_s
end

def processConfig(configName, configNum, maxNum)
  pegSolitaire = PegSolitaire.new(configName, $config_names[configNum-1])

  File.makedirs("tempo-sat")
  domainFile = File.new("tempo-sat/domain.pddl", "w")
  pegSolitaire.printDomainTemporal(domainFile)
  problemFile = File.new("tempo-sat/p#{pad(configNum,maxNum)}.pddl", "w")
  pegSolitaire.printProblemTemporal(problemFile, pad(configNum,maxNum), pegSolitaire)
  
  File.makedirs("seq-sat")
  domainFile = File.new("seq-sat/domain.pddl", "w")
  pegSolitaire.printDomainSequential(domainFile)
  problemFile = File.new("seq-sat/p#{pad(configNum,maxNum)}.pddl", "w")
  pegSolitaire.printProblemSequential(problemFile, pad(configNum,maxNum), pegSolitaire)
  
  File.makedirs("netben-opt")
  domainFile = File.new("netben-opt/domain.pddl", "w")
  pegSolitaire.printDomainNetBenefit(domainFile)
  problemFile1 = File.new("netben-opt/p#{pad(configNum,maxNum)}-1.pddl", "w")
  pegSolitaire.printProblemNetBenefit(problemFile1, pad(configNum,maxNum) + "-1", pegSolitaire, pegSolitaire.method(:costFunction1))
  problemFile2 = File.new("netben-opt/p#{pad(configNum,maxNum)}-2.pddl", "w")
  pegSolitaire.printProblemNetBenefit(problemFile2, pad(configNum,maxNum) + "-2", pegSolitaire, pegSolitaire.method(:costFunction2))
  problemFile3 = File.new("netben-opt/p#{pad(configNum,maxNum)}-3.pddl", "w")
  pegSolitaire.printProblemNetBenefit(problemFile3, pad(configNum,maxNum) + "-3", pegSolitaire, pegSolitaire.method(:costFunction3))

  pegSolitaire.prettyPrintBoard($stdout, "", "Problem p#{pad(configNum,maxNum)}.pddl:\n")
  print("\n")
end

configNum = 1
$all_configs.each do | config |
  processConfig(config,configNum,$all_configs.length)
  configNum+=1
end 
