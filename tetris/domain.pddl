; IPC8 Domain: Tetris
; Author: Mauro Vallati

(define (domain tetris)
(:requirements :typing :equality :negative-preconditions :action-costs)
  (:types  
	one_square two_straight right_l - pieces
	position 	
	)

  (:predicates
    (clear ?xy - position)
    (connected ?x - position ?y - position )
    (at_square ?element - one_square ?xy - position)
    (at_two ?element - two_straight ?xy - position ?xy2 - position)
    (at_right_l ?element - right_l ?xy - position ?xy2 - position ?xy3 - position)

  )
(:functions (total-cost) - number)

;; move a 1 square piece
(:action move_square
  :parameters (?xy_initial - position ?xy_final - position ?element - one_square )
  :precondition (and 
		(clear ?xy_final) 
		(at_square ?element ?xy_initial) 
		(connected ?xy_initial ?xy_final)
		(connected ?xy_final ?xy_initial)  
		)
  :effect (and  
		(clear ?xy_initial)
		(at_square ?element ?xy_final)
		(not (clear ?xy_final))
		(not (at_square ?element ?xy_initial) )
                (increase (total-cost) 1)
		)
)


;; move a straight 2 square piece (this include rotation)
(:action move_two
  :parameters (?xy_initial1 - position ?xy_initial2 - position ?xy_final - position ?element - two_straight )
  :precondition (and 
		(clear ?xy_final) 
		(at_two ?element ?xy_initial1 ?xy_initial2) 
		(connected ?xy_initial2 ?xy_final) 
		)
  :effect (and  
		(clear ?xy_initial1)
		(at_two ?element ?xy_initial2 ?xy_final)
		(not (clear ?xy_final))
		(not (at_two ?element ?xy_initial1 ?xy_initial2) )
                (increase (total-cost) 2)
		)
)

;; moving a piece "L" to the right
(:action move_l_right
  :parameters (?xy_initial1 ?xy_initial2 ?xy_initial3 ?xy_final ?xy_final2 ?xy_between_final - position ?element - right_l )
  :precondition (and 
		(clear ?xy_final)
		(clear ?xy_final2) 
		(at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3) 
		(connected ?xy_initial1 ?xy_final)
		(connected ?xy_initial3 ?xy_final2) 
		(connected ?xy_initial3 ?xy_final) 
		(connected ?xy_final ?xy_between_final) 
		(connected ?xy_final2 ?xy_between_final) 
		(not (= ?xy_final ?xy_final2))
		(not (= ?xy_between_final ?xy_initial3))
		(not (connected ?xy_initial1 ?xy_final2))
		)
  :effect (and  
		(clear ?xy_initial2)
		(clear ?xy_initial1)
		(at_right_l ?element ?xy_final ?xy_initial3 ?xy_final2) 
		(not (clear ?xy_final))
		(not (clear ?xy_final2))
		(not (at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3))
                (increase (total-cost) 3)
		)
)

;; moving a piece "L" to the left
(:action move_l_left
  :parameters (?xy_initial1 ?xy_initial2 ?xy_initial3 ?xy_final ?xy_final2 - position ?element - right_l )
  :precondition (and 
		(clear ?xy_final)
		(clear ?xy_final2) 
		(at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3) 
		(connected ?xy_initial1 ?xy_final)
		(connected ?xy_initial2 ?xy_final2) 
		(connected ?xy_final2 ?xy_final) 
		(not (= ?xy_final ?xy_final2))
		)
  :effect (and  
		(clear ?xy_initial3)
		(clear ?xy_initial1)
		(at_right_l ?element ?xy_final ?xy_final2 ?xy_initial2) 
		(not (clear ?xy_final))
		(not (clear ?xy_final2))
		(not (at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3))
                (increase (total-cost) 3)
		)
)

;; moving a piece "L" up
(:action move_l_up
  :parameters (?xy_initial1 ?xy_initial2 ?xy_initial3 ?xy_final ?xy_final2 ?xy_between_final - position ?element - right_l )
  :precondition (and 
		(clear ?xy_final)
		(clear ?xy_final2) 
		(at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3) 
		(connected ?xy_initial1 ?xy_final)
		(connected ?xy_initial3 ?xy_final2) 
		(connected ?xy_initial1 ?xy_final2) 
		(connected ?xy_final ?xy_between_final) 
		(connected ?xy_final2 ?xy_between_final) 
		(not (= ?xy_final ?xy_final2))
		(not (= ?xy_between_final ?xy_initial1))
		(not (connected ?xy_initial3 ?xy_final))
		)
  :effect (and  
		(clear ?xy_initial2)  
		(clear ?xy_initial3)
		(at_right_l ?element ?xy_final ?xy_initial1 ?xy_final2) 
		(not (clear ?xy_final))
		(not (clear ?xy_final2))
		(not (at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3))
                (increase (total-cost) 3)
		)
)

;; moving a piece "L" down
(:action move_l_down
  :parameters (?xy_initial1 ?xy_initial2 ?xy_initial3 ?xy_final ?xy_final2 - position ?element - right_l )
  :precondition (and 
		(clear ?xy_final)
		(clear ?xy_final2) 
		(at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3) 
		(connected ?xy_initial2 ?xy_final)
		(connected ?xy_initial3 ?xy_final2) 
		(connected ?xy_final2 ?xy_final) 
		(not (= ?xy_final ?xy_final2))
		)
  :effect (and  
		(clear ?xy_initial3)
		(clear ?xy_initial1)
		(at_right_l ?element ?xy_initial2 ?xy_final ?xy_final2) 
		(not (clear ?xy_final))
		(not (clear ?xy_final2))
		(not (at_right_l ?element ?xy_initial1 ?xy_initial2 ?xy_initial3))
	        (increase (total-cost) 3)
		)
)




)
