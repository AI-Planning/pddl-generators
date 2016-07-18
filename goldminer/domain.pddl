;; Gold Miner domain

(define (domain gold-miner-typed)
(:requirements :typing)
(:types LOC)

(:predicates
        (robot-at ?x - LOC)
        (bomb-at ?x - LOC )
        (laser-at ?x - LOC)
        (soft-rock-at ?x - LOC)
        (hard-rock-at ?x - LOC)
        (gold-at ?x - LOC)
        (arm-empty)
        (holds-bomb)
                (holds-laser)
        (holds-gold)
        (clear ?x - LOC)
        (connected ?x - LOC ?y - LOC)
)


; move to an adjacent empty grid location
(:action move
  :parameters (?x - LOC ?y - LOC)
  :precondition (and (robot-at ?x) (connected ?x ?y) (clear ?y))
  :effect (and (robot-at ?y) (not (robot-at ?x)))
)

(:action pickup-laser
  :parameters (?x - LOC)
  :precondition (and (robot-at ?x) (laser-at ?x) (arm-empty))
  :effect (and (not (arm-empty)) (holds-laser) (not (laser-at ?x)) )
)

;have to be over the bomb location to pick the bomb
(:action pickup-bomb
  :parameters (?x - LOC)
  :precondition (and (robot-at ?x) (bomb-at ?x) (arm-empty))
  :effect (and (not (arm-empty)) (holds-bomb))
)

(:action putdown-laser
  :parameters (?x - LOC)
  :precondition (and (robot-at ?x) (holds-laser))
  :effect (and (arm-empty) (not (holds-laser)) (laser-at ?x))
)

;bomb an adjacent location that has soft-rock
(:action detonate-bomb
  :parameters (?x - LOC ?y - LOC)
  :precondition (and (robot-at ?x) (holds-bomb)
                     (connected ?x ?y) (soft-rock-at ?y))
  :effect (and (not (holds-bomb)) (arm-empty) (clear ?y) (not (soft-rock-at ?y)))
)

(:action fire-laser
  :parameters (?x - LOC ?y - LOC)
  :precondition (and (robot-at ?x) (holds-laser)
                     (connected ?x ?y))
  :effect (and (clear ?y) (not (soft-rock-at ?y)) (not (gold-at ?y))
               (not (hard-rock-at ?y)))
)

;mine gold !
;the robot has to be over the gold location to pick it up
(:action pick-gold
  :parameters (?x - LOC)
  :precondition (and (robot-at ?x) (arm-empty) (gold-at ?x))
  :effect (and (not (arm-empty)) (holds-gold))
)
)
