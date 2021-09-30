(define (domain civ)
    (:requirements :typing :negative-preconditions)

    (:types store level resource - object
            place vehicle - store
    )

    (:constants stone ore timber coal wood iron - resource
                l0 l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 - level)

    (:predicates
        (connected-by-land ?p1 - place ?p2 - place)
        (connected-by-rail ?p1 - place ?p2 - place)
        (connected-by-sea ?p1 - place ?p2 - place)
        (by-coast ?p - place)

        (has-coal-stack ?p - place)
        (has-sawmill ?p - place)
        (has-ironworks ?p - place)
        (has-docks ?p - place)
        (has-wharf ?p - place)

        (potential ?v - vehicle)
        (is-cart ?v - vehicle)
        (is-train ?v - vehicle)
        (is-ship ?v - vehicle)
        (is-at ?v - vehicle ?p - place)

        (space-in ?v - vehicle ?l - level)

        (available ?r - resource ?s - store ?l - level)

        (housing ?p - place ?l - level)

        (difference ?l1 ?l2 ?l3 - level)
    )

    (:functions (total-cost))


    ;; A.1: Loading and unloading.

    (:action load
     :parameters (?v - vehicle ?p - place ?r - resource ?rpold ?rpnew ?rvold ?rvnew ?sold ?snew - level)
     :precondition (and (is-at ?v ?p) (available ?r ?p ?rpold) (available ?r ?v ?rvold) (space-in ?v ?sold) (difference ?rpold l1 ?rpnew) (difference ?rvnew l1 ?rvold) (difference ?sold l1 ?snew))
     :effect (and (not (available ?r ?p ?rpold)) (not (available ?r ?v ?rvold)) (not (space-in ?v ?sold)) (available ?r ?p ?rpnew) (available ?r ?v ?rvnew) (space-in ?v ?snew) (increase (labour) 1)))

    (:action unload
     :parameters (?v - vehicle ?p - place ?r - resource ?rpold ?rpnew ?rvold ?rvnew ?sold ?snew - level)
     :precondition (and (is-at ?v ?p) (available ?r ?p ?rpold) (available ?r ?v ?rvold) (space-in ?v ?sold) (difference ?rpnew l1 ?rpold) (difference ?rvold l1 ?rvnew) (difference ?snew l1 ?sold))
     :effect (and (not (available ?r ?p ?rpold)) (not (available ?r ?v ?rvold)) (not (space-in ?v ?sold)) (available ?r ?p ?rpnew) (available ?r ?v ?rvnew) (space-in ?v ?snew) (increase (labour) 1)))



    ;; A.2: Moving vehicles.
    ;; Moving trains and ships consumes coal, which has to be
    ;; loaded in the vehicle.

    (:action move-cart
     :parameters (?v - vehicle ?p1 ?p2 - place)
     :precondition (and (is-cart ?v) (connected-by-land ?p1 ?p2) (is-at ?v ?p1))
     :effect (and (not (is-at ?v ?p1)) (is-at ?v ?p2) (increase (labour) 2))
    )

    (:action move-train
     :parameters (?v - vehicle ?p1 ?p2 - place ?cold ?cnew - level ?sold ?snew - level)
     :precondition (and (is-train ?v) (connected-by-rail ?p1 ?p2) (is-at ?v ?p1) (available coal ?v ?cold) (difference ?cold l1 ?cnew) (space-in ?v ?sold) (difference ?snew l1 ?sold))
     :effect (and (not (space-in ?v ?sold)) (space-in ?v ?snew) (not (is-at ?v ?p1)) (is-at ?v ?p2) (not (available coal ?v ?cold)) (available coal ?v ?cnew) (increase (pollution) 1))
    )

    (:action move-ship
     :parameters (?v - vehicle ?p1 ?p2 - place ?cold ?cnew - level ?sold ?snew - level)
     :precondition (and (is-ship ?v) (connected-by-sea ?p1 ?p2) (is-at ?v ?p1) (available coal ?v ?cold) (difference ?cold l2 ?cnew) (space-in ?v ?sold) (difference ?snew l2 ?sold))
     :effect (and (not (space-in ?v ?sold)) (space-in ?v ?snew) (not (is-at ?v ?p1)) (is-at ?v ?p2) (not (available coal ?v ?cold)) (available coal ?v ?cnew) (increase (pollution) 2))
    )



    ;; B.1: Building structures.

    (:action build-coal-stack
     :parameters (?p - place ?told ?tnew - level)
     :precondition (and (not (has-coal-stack ?p)) (available timber ?p ?told) (difference ?told l1 ?tnew))
     :effect (and (has-coal-stack ?p) (not (available timber ?p ?told)) (available timber ?p ?tnew) (increase (labour) 2))
    )

    (:action build-sawmill
     :parameters (?p - place ?told ?tnew - level)
     :precondition (and (not (has-sawmill ?p)) (available timber ?p ?told) (difference ?told l2 ?tnew))
     :effect (and (has-sawmill ?p) (not (available timber ?p ?told)) (available timber ?p ?tnew) (increase (labour) 2))
    )

    (:action build-ironworks
     :parameters (?p - place ?sold ?snew ?wold ?wnew - level)
     :precondition (and (not (has-ironworks ?p)) (available stone ?p ?sold) (available wood ?p ?wold) (difference ?sold l2 ?snew) (difference ?wold l2 ?wnew))
     :effect (and (has-ironworks ?p) (not (available stone ?p ?sold)) (not (available wood ?p ?wold)) (available stone ?p ?snew) (available wood ?p ?wnew) (increase (labour) 3))
    )

    (:action build-docks
     :parameters (?p - place ?sold ?snew ?wold ?wnew - level)
     :precondition (and (by-coast ?p) (not (has-docks ?p)) (available stone ?p ?sold) (available wood ?p ?wold) (difference ?sold l2 ?snew) (difference ?wold l2 ?wnew))
     :effect (and (has-docks ?p) (not (available stone ?p ?sold)) (not (available wood ?p ?wold)) (available stone ?p ?snew) (available wood ?p ?wnew) (increase (labour) 2))
    )

    (:action build-wharf
     :parameters (?p - place ?iold ?inew ?sold ?snew - level)
     :precondition (and (has-docks ?p) (not (has-wharf ?p)) (available stone ?p ?sold) (available iron ?p ?iold) (difference ?sold l2 ?snew) (difference ?iold l2 ?inew))
     :effect (and (has-wharf ?p) (not (available stone ?p ?sold)) (not (available iron ?p ?iold)) (available stone ?p ?snew) (available iron ?p ?inew) (increase (labour) 2))
    )

    (:action build-rail
     :parameters (?p1 ?p2 - place ?iold ?inew ?wold ?wnew - level)
     :precondition (and (connected-by-land ?p1 ?p2) (not (connected-by-rail ?p1 ?p2)) (available wood ?p1 ?wold) (available iron ?p1 ?iold) (difference ?wold l1 ?wnew) (difference ?iold l1 ?inew))
     :effect (and (connected-by-rail ?p1 ?p2) (not (available wood ?p1 ?wold)) (not (available iron ?p1 ?iold)) (available wood ?p1 ?wnew) (available iron ?p1 ?inew) (increase (labour) 2))
    )

    (:action build-house
     :parameters (?p - place ?sold ?snew ?wold ?wnew ?hold ?hnew - level)
     :precondition (and (housing ?p ?hold) (available stone ?p ?sold) (available wood ?p ?wold) (difference ?sold l1 ?snew) (difference ?wold l1 ?wnew) (difference ?hnew l1 ?hold))
     :effect (and (not (housing ?p ?hold)) (not (available stone ?p ?sold)) (not (available wood ?p ?wold)) (housing ?p ?hnew) (available stone ?p ?snew) (available wood ?p ?wnew))
    )



    ;; B.2: Building vehicles.

    (:action build-cart
     :parameters (?p - place ?v - vehicle ?told ?tnew - level)
     :precondition (and (potential ?v) (available timber ?p ?told) (difference ?told l1 ?tnew))
     :effect (and (not (available timber ?p ?told)) (available timber ?p ?tnew) (is-at ?v ?p) (is-cart ?v) (not (potential ?v)) (space-in ?v l1) (increase (labour) 1))
    )

    (:action build-train
     :parameters (?p - place ?v - vehicle ?iold ?inew - level)
     :precondition (and (potential ?v) (available iron ?p ?iold) (difference ?iold l2 ?inew))
     :effect (and (not (available iron ?p ?iold)) (available iron ?p ?inew) (is-at ?v ?p) (is-train ?v) (not (potential ?v)) (space-in ?v l5) (increase (labour) 2))
    )

    (:action build-ship
     :parameters (?p - place ?v - vehicle ?iold ?inew - level)
     :precondition (and (has-wharf ?p) (potential ?v) (available iron ?p ?iold) (difference ?iold l4 ?inew))
     :effect (and (not (available iron ?p ?iold)) (available iron ?p ?inew) (is-at ?v ?p) (is-ship ?v) (not (potential ?v)) (space-in ?v l10) (increase (labour) 3))
    )



    ;; C.1: Refining resources.

    (:action burn-coal
     :parameters (?p - place ?told ?tnew ?cold ?cnew - level)
     :precondition (and (has-coal-stack ?p) (available timber ?p ?told) (available coal ?p ?cold) (difference ?told l1 ?tnew) (difference ?cnew l1 ?cold))
     :effect (and (not (available timber ?p ?told)) (not (available coal ?p ?cold)) (available timber ?p ?tnew) (available coal ?p ?cnew) (increase (pollution) 1))
    )

    (:action saw-wood
     :parameters (?p - place ?told ?tnew ?wold ?wnew - level)
     :precondition (and (has-sawmill ?p) (available timber ?p ?told) (available wood ?p ?wold) (difference ?told l1 ?tnew) (difference ?wnew l1 ?wold))
     :effect (and  (not (available timber ?p ?told)) (not (available wood ?p ?wold)) (available timber ?p ?tnew) (available wood ?p ?wnew))
    )

    (:action make-iron
     :parameters (?p - place ?cold ?cnew ?oold ?onew ?iold ?inew - level)
     :precondition (and (has-ironworks ?p) (available ore ?p ?oold) (available coal ?p ?cold) (available iron ?p ?iold) (difference ?oold l1 ?onew) (difference ?cold l2 ?cnew) (difference ?inew l1 ?iold))
     :effect (and (not (available ore ?p ?oold)) (not (available coal ?p ?cold)) (not (available iron ?p ?iold)) (available ore ?p ?onew) (available coal ?p ?cnew) (available iron ?p ?inew)  (increase (pollution) 2)))
)
