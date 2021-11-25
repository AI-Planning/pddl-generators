;; variable number of robots, variable number of grippers per robot
(define (domain gripper-ex-typed)
   (:requirements :strips :typing)
   (:types room ball gripper robot)
   (:predicates (at-robot ?r - robot ?a - room)
		(at-ball ?b - ball ?r - room)
		(has ?r - robot ?g - gripper)
		(free ?g - gripper)
		(carry ?b - ball ?g - gripper))

   (:action move
       :parameters  (?from - room ?to - room ?robot - robot)
       :precondition (at-robot ?robot ?from)
       :effect (and  (at-robot ?robot ?to)
		     (not (at-robot ?robot ?from))))



   (:action pick
       :parameters (?ball - ball ?room - room ?robot - robot ?gripper - gripper)
       :precondition  (and (at-ball ?ball ?room) (at-robot ?robot ?room) (has ?robot ?gripper) (free ?gripper))
       :effect (and (carry ?ball ?gripper)
		    (not (at-ball ?ball ?room))
		    (not (free ?gripper))))


   (:action drop
       :parameters  (?ball - ball ?room - room ?robot - robot ?gripper - gripper)
       :precondition  (and (at-robot ?robot ?room) (has ?robot ?gripper) (carry ?ball ?gripper))
       :effect (and (at-ball ?ball ?room)
		    (free ?gripper)
		    (not (carry ?ball ?gripper)))))

