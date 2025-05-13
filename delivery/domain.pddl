
(define (domain delivery)
    (:requirements :typing :equality :negative-preconditions)
    (:types
        cell - object
        locatable - object
        package - locatable
        truck - locatable
    )

    (:predicates
        (at ?x1 - locatable ?x2 - cell)
        (carrying ?x1 - truck ?x2 - package)
        (empty ?x1 - truck)
        (adjacent ?x1 - cell ?x2 - cell)
    )

    (:action pick-package
     :parameters (?t - truck ?p - package ?x - cell)
     :precondition (and (at ?p ?x) (at ?t ?x) (empty ?t))
     :effect (and
        (not (at ?p ?x))
        (not (empty ?t))
        (carrying ?t ?p))
    )

    (:action drop-package
     :parameters (?t - truck ?p - package ?x - cell)
     :precondition (and (at ?t ?x) (carrying ?t ?p))
     :effect (and
        (empty ?t)
        (not (carrying ?t ?p))
        (at ?p ?x))
    )

    (:action move
     :parameters (?t - truck ?from - cell ?to - cell)
     :precondition (and (adjacent ?from ?to) (at ?t ?from) (not (= ?from ?to)))
     :effect (and
        (not (at ?t ?from))
        (at ?t ?to))
    )
)
