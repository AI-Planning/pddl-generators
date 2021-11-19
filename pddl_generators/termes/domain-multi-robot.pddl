(define (domain termes)
    (:requirements :typing :negative-preconditions)
    (:types
        numb - object
        position - object
        robot - object
    )

    (:predicates
        (height ?p - position ?h - numb)
        (at ?r - robot ?p - position)
        (has-block ?r - robot)
        (occupied ?p - position)
        (SUCC ?n1 - numb ?n2 - numb)
        (NEIGHBOR ?p1 - position ?p2 - position)
        (IS-DEPOT ?p - position)
    )

    (:action move
        :parameters (?r - robot ?from - position ?to - position ?h - numb)
        :precondition (and
            (at ?r ?from)
            (not (occupied ?to))
            (NEIGHBOR ?from ?to)
            (height ?from ?h)
            (height ?to ?h)
        )
        :effect (and
            (not (at ?r ?from))
            (at ?r ?to)
            (not (occupied ?from))
            (occupied ?to)
        )
    )

    (:action move-up
        :parameters (?r - robot ?from - position ?hfrom - numb ?to - position ?hto - numb)
        :precondition (and
            (at ?r ?from)
            (not (occupied ?to))
            (NEIGHBOR ?from ?to)
            (height ?from ?hfrom)
            (height ?to ?hto)
            (SUCC ?hto ?hfrom)
        )
        :effect (and
            (not (at ?r ?from))
            (at ?r ?to)
            (not (occupied ?from))
            (occupied ?to)
        )
    )

    (:action move-down
        :parameters (?r - robot ?from - position ?hfrom - numb ?to - position ?hto - numb)
        :precondition (and
            (at ?r ?from)
            (not (occupied ?to))
            (NEIGHBOR ?from ?to)
            (height ?from ?hfrom)
            (height ?to ?hto)
            (SUCC ?hfrom ?hto)
        )
        :effect (and
            (not (at ?r ?from))
            (at ?r ?to)
            (not (occupied ?from))
            (occupied ?to)
        )
    )

    (:action place-block
        :parameters (?r - robot ?rpos - position ?bpos - position ?hbefore - numb ?hafter - numb)
        :precondition (and
            (at ?r ?rpos)
            (not (occupied ?bpos))
            (NEIGHBOR ?rpos ?bpos)
            (height ?rpos ?hbefore)
            (height ?bpos ?hbefore)
            (SUCC ?hafter ?hbefore)
            (has-block ?r)
        )
        :effect (and
            (not (height ?bpos ?hbefore))
            (height ?bpos ?hafter)
            (not (has-block ?r))
        )
    )

    (:action remove-block
        :parameters (?r - robot ?rpos - position ?bpos - position ?hbefore - numb ?hafter - numb)
        :precondition (and
            (at ?r ?rpos)
            (not (occupied ?bpos))
            (NEIGHBOR ?rpos ?bpos)
            (height ?rpos ?hafter)
            (height ?bpos ?hbefore)
            (SUCC ?hbefore ?hafter)
            (not (has-block ?r))
        )
        :effect (and
            (not (height ?bpos ?hbefore))
            (height ?bpos ?hafter)
            (has-block ?r)
        )
    )

    (:action create-block
        :parameters (?r - robot ?p - position)
        :precondition (and
            (at ?r ?p)
            (not (has-block ?r))
            (IS-DEPOT ?p)
        )
        :effect (and
            (has-block ?r)
        )
    )

    (:action destroy-block
        :parameters (?r - robot ?p - position)
        :precondition (and
            (at ?r ?p)
            (has-block ?r)
            (IS-DEPOT ?p)
        )
        :effect (and
            (not (has-block ?r))
        )
    )


)
