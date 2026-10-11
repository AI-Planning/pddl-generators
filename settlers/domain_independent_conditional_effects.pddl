	(define (domain civ)
    (:requirements :typing :negative-preconditions :conditional-effects)

    (:types store level resource - object
            place vehicle - store
	    atleastlevel - level
    )

    (:constants stone ore timber coal wood iron - resource
    		l1 l2 l4 - atleastlevel
                l0 l3 l5 l6 l7 l8 l9 l10 - level)

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

        (available-atleast ?r - resource ?s - store ?l - level)
	(has-space ?v - vehicle)

	; ?r_old < ?al <= ?rold + ?r_added
	(add-atleast  ?r_old - level ?r_added - atleastlevel ?al - atleastlevel)
	
	; ?r_old - r_consumed < ?al <= ?rold
	(del-atleast  ?r_old - level ?r_consumed - atleastlevel ?al - atleastlevel) 
    )

    (:functions (total-cost))

    ;; A.1: Loading and unloading.
    ; resource[place] -= 1, resource[vehicle] += 1, space[vehicle] -= 1\
    (:action load
     :parameters (?v - vehicle ?p - place ?r - resource)
     :precondition (and (is-at ?v ?p) (available-atleast ?r ?p l1) (has-space ?v))

     :effect (and (forall (?rpold ?rpnew - level) (when (and (available ?r ?p ?rpold) (difference ?rpold l1 ?rpnew)) (and  (available ?r ?p ?rpnew) (not (available ?r ?p ?rpold)))))
     		  (forall (?rpold - level ?al - atleastlevel) (when (and (available ?r ?p ?rpold) (del-atleast  ?rpold l1 ?al)) (not (available-atleast ?r ?p ?al))))

     	     	  (forall (?rvold ?rvnew  - level) (when (and (available ?r ?v ?rvold) (difference ?rvnew l1 ?rvold)) (and (not (available ?r ?v ?rvold)) (available ?r ?v ?rvnew))))
		  (forall (?rvold - level ?al - atleastlevel) (when (and (available ?r ?v ?rvold) (add-atleast ?rvold l1 ?al)) (available-atleast ?r ?v ?al)))
		  
		  (forall (?sold ?snew - level) (when (and (space-in ?v ?sold) (difference ?sold l1 ?snew)) (and (not (space-in ?v ?sold))  (space-in ?v ?snew))))
  		  (forall (?sold - level) (when (and (space-in ?v ?sold) (= ?sold l1)) (not (has-space ?v))))
	  
		  (increase (labour) 1))
     )
     
     ; resource[place] += 1, resource[vehicle] -= 1, space[vehicle] += 1
    (:action unload
     :parameters (?v - vehicle ?p - place ?r - resource)
     :precondition (and (is-at ?v ?p) (available-atleast ?r ?v l1) )
     :effect (and
     (forall (?rpold ?rpnew - level) (when (and (available ?r ?p ?rpold)  (difference ?rpnew l1 ?rpold)) (and (not (available ?r ?p ?rpold)) (available ?r ?p ?rpnew))))
     (forall (?rpold - level ?al - atleastlevel) (when (and (available ?r ?p ?rpold) (add-atleast ?rpold l1 ?al)) (available-atleast ?r ?p ?al)))

     (forall (?rvold ?rvnew  - level) (when (and (available ?r ?v ?rvold) (difference ?rvold l1 ?rvnew) ) (and (not (available ?r ?v ?rvold)) (available ?r ?v ?rvnew))))
     (forall (?rvold - level ?al - atleastlevel) (when (and (available ?r ?v ?rvold) (del-atleast  ?rvold l1 ?al)) (not (available-atleast ?r ?v ?al))))

     (forall (?sold ?snew - level) (when (and (space-in ?v ?sold)  (difference ?snew l1 ?sold)) (and  (not (space-in ?v ?sold))   (space-in ?v ?snew))))
     (has-space ?v)

     (increase (labour) 1))
    )



    ;; A.2: Moving vehicles.
    ;; Moving trains and ships consumes coal, which has to be
    ;; loaded in the vehicle.

    (:action move-cart
     :parameters (?v - vehicle ?p1 ?p2 - place)
     :precondition (and (is-cart ?v) (connected-by-land ?p1 ?p2) (is-at ?v ?p1) )
     :effect (and (not (is-at ?v ?p1)) (is-at ?v ?p2) (increase (labour) 2))
    )
    

     ; coal[vehicle] -= 1
     ; space[vehicle] += 1

    (:action move-train
     :parameters (?v - vehicle ?p1 ?p2 - place)
     :precondition (and (is-train ?v) (connected-by-rail ?p1 ?p2) (is-at ?v ?p1)  (available-atleast coal ?v l1) )
     :effect (and 
     	     (not (is-at ?v ?p1))
	     (is-at ?v ?p2)
     	     (forall (?cold ?cnew - level) (when (and (available coal ?v ?cold) (difference ?cold l1 ?cnew)) (and (not (available coal ?v ?cold)) (available coal ?v ?cnew))))
	     (forall (?cold - level ?al - atleastlevel) (when (and (available coal ?v ?cold) (del-atleast  ?cold l1 ?al)) (not (available-atleast coal ?v ?al))))

	     (forall (?sold ?snew - level) (when (and (space-in ?v ?sold)  (difference ?snew l1 ?sold)) (and  (not (space-in ?v ?sold))   (space-in ?v ?snew))))
	     (has-space ?v)

	     (increase (pollution) 1))
    )

    ; coal[vehicle] -= 2
    ; space[vehicle] += 2
    (:action move-ship
     :parameters (?v - vehicle ?p1 ?p2 - place)
     :precondition (and (is-ship ?v) (connected-by-sea ?p1 ?p2) (is-at ?v ?p1)  (available-atleast coal ?v l2))
     :effect (and
             (not (is-at ?v ?p1))
	     (is-at ?v ?p2) 
     	     (forall (?cold ?cnew - level) (when (and (available coal ?v ?cold) (difference ?cold l2 ?cnew)) (and (not (available coal ?v ?cold)) (available coal ?v ?cnew))))
	     (forall (?cold - level ?al - atleastlevel) (when (and (available coal ?v ?cold) (del-atleast  ?cold l2 ?al)) (not (available-atleast coal ?v ?al))))

	     (forall (?sold ?snew - level) (when (and (space-in ?v ?sold)  (difference ?snew l2 ?sold)) (and  (not (space-in ?v ?sold))   (space-in ?v ?snew))))
	     (has-space ?v)

	     (increase (pollution) 2))
    )
    
    ;; B.1: Building structures.
    ; timber [place] -= 1
    (:action build-coal-stack
     :parameters (?p - place)
     :precondition (and (not (has-coal-stack ?p)) (available-atleast timber ?p l1))
     :effect (and
     	     (has-coal-stack ?p)
	     (forall (?told ?tnew - level) (when (and (available timber ?p ?told) (difference ?told l1 ?tnew)) (and (not (available timber ?p ?told)) (available timber ?p ?tnew))))
	     (forall (?told - level ?al - atleastlevel) (when (and (available timber ?p ?told) (del-atleast  ?told l1 ?al)) (not (available-atleast timber ?p ?al))))

	     (increase (labour) 2))
    )

    ; timber [place] -= 2
    (:action build-sawmill
     :parameters (?p - place)
     :precondition (and (not (has-sawmill ?p)) (available-atleast timber ?p l2))
     :effect (and
     	     (has-sawmill ?p)
     	     (forall (?told ?tnew - level) (when (and (available timber ?p ?told) (difference ?told l2 ?tnew)) (and (not (available timber ?p ?told)) (available timber ?p ?tnew))))
	     (forall (?told - level ?al - atleastlevel) (when (and (available timber ?p ?told) (del-atleast  ?told l2 ?al)) (not (available-atleast timber ?p ?al))))

	     (increase (labour) 2))
    )

    ; wood [place] -= 2
    ; stone [place] -= 2
    (:action build-ironworks
     :parameters (?p - place)
     :precondition (and (not (has-ironworks ?p))  (available-atleast wood ?p l2) (available-atleast stone ?p l2))
     :effect (and
          (has-ironworks ?p)	
     	  (forall (?sold ?snew - level) (when (and (available stone ?p ?sold) (difference ?sold l2 ?snew)) (and (not (available stone ?p ?sold)) (available stone ?p ?snew))))
	  (forall (?sold - level ?al - atleastlevel) (when (and (available stone ?p ?sold) (del-atleast  ?sold l2 ?al)) (not (available-atleast stone ?p ?al))))

	  (forall (?wold ?wnew - level) (when (and (available wood ?p ?wold) (difference ?wold l2 ?wnew)) (and (not (available wood ?p ?wold)) (available wood ?p ?wnew))))
	  (forall (?wold - level ?al - atleastlevel) (when (and (available wood ?p ?wold) (del-atleast  ?wold l2 ?al)) (not (available-atleast wood ?p ?al))))
	  
	  (increase (labour) 3))
    )

    ; wood [place] -= 2
    ; stone [place] -= 2
    (:action build-docks
     :parameters (?p - place)
     :precondition (and (by-coast ?p) (not (has-docks ?p))  (available-atleast wood ?p l2) (available-atleast stone ?p l2)) 
     :effect (and
     (has-docks ?p)
     (forall (?wold ?wnew - level) (when (and (available wood ?p ?wold) (difference ?wold l2 ?wnew)) (and (not (available wood ?p ?wold)) (available wood ?p ?wnew))))
     (forall (?wold - level ?al - atleastlevel) (when (and (available wood ?p ?wold) (del-atleast  ?wold l2 ?al)) (not (available-atleast wood ?p ?al))))

     (forall (?sold ?snew - level) (when (and (available stone ?p ?sold) (difference ?sold l2 ?snew)) (and (not (available stone ?p ?sold)) (available stone ?p ?snew))))
     (forall (?sold - level ?al - atleastlevel) (when (and (available stone ?p ?sold) (del-atleast  ?sold l2 ?al)) (not (available-atleast stone ?p ?al))))

     (increase (labour) 2))
    )

    ; iron [place] -= 2
    ; stone [place] -= 2
    (:action build-wharf
     :parameters (?p - place)
     :precondition (and (has-docks ?p) (not (has-wharf ?p)) (available-atleast iron ?p l2) (available-atleast stone ?p l2))
     :effect (and
     (has-wharf ?p) 
     (forall (?iold ?inew - level) (when (and (available iron ?p ?iold) (difference ?iold l2 ?inew)) (and (not (available iron ?p ?iold)) (available iron ?p ?inew))))
     (forall (?iold - level ?al - atleastlevel) (when (and (available iron ?p ?iold) (del-atleast  ?iold l2 ?al)) (not (available-atleast iron ?p ?al))))

     (forall (?sold ?snew - level) (when (and (available stone ?p ?sold) (difference ?sold l2 ?snew)) (and (not (available stone ?p ?sold)) (available stone ?p ?snew))))
     (forall (?sold - level ?al - atleastlevel) (when (and (available stone ?p ?sold) (del-atleast  ?sold l2 ?al)) (not (available-atleast stone ?p ?al))))

     (increase (labour) 2))
    )

    ; iron [place] -= 1
    ; wood [place] -= 1
    (:action build-rail
     :parameters (?p1 ?p2 - place)
     :precondition (and (connected-by-land ?p1 ?p2) (not (connected-by-rail ?p1 ?p2)) (available-atleast iron ?p1 l1) (available-atleast wood ?p1 l1))
     :effect (and
     	     (connected-by-rail ?p1 ?p2)
     	     (forall (?wold ?wnew - level) (when (and (available wood ?p1 ?wold)  (difference ?wold l1 ?wnew)) (and (not (available wood ?p1 ?wold))  (available wood ?p1 ?wnew))))
             (forall (?wold - level ?al - atleastlevel) (when (and (available wood ?p1 ?wold) (del-atleast  ?wold l1 ?al)) (not (available-atleast wood ?p1 ?al))))

             (forall (?iold ?inew - level) (when (and (available iron ?p1 ?iold) (difference ?iold l1 ?inew))  (and (not (available iron ?p1 ?iold)) (available iron ?p1 ?inew))))
             (forall (?iold - level ?al - atleastlevel) (when (and (available iron ?p1 ?iold) (del-atleast  ?iold l1 ?al)) (not (available-atleast iron ?p1 ?al))))

     	     (increase (labour) 2))
    )

    ; wood [place] -= 1
    ; stone [place] -= 1
    ; housing [place] += 1 (we do not need available-atleast housing because it's not a precondition of any action)
    (:action build-house
     :parameters (?p - place)
     :precondition (and (available-atleast wood ?p l1) (available-atleast stone ?p l1))
     :effect (and
     (forall (?sold ?snew - level) (when (and (available stone ?p ?sold) (difference ?sold l1 ?snew)) (and (not (available stone ?p ?sold)) (available stone ?p ?snew))))
     (forall (?sold - level ?al - atleastlevel) (when (and (available stone ?p ?sold) (del-atleast  ?sold l1 ?al)) (not (available-atleast stone ?p ?al))))

     (forall (?wold ?wnew - level) (when (and (available wood ?p ?wold) (difference ?wold l1 ?wnew)) (and (not (available wood ?p ?wold)) (available wood ?p ?wnew))))
     (forall (?wold - level ?al - atleastlevel) (when (and (available wood ?p ?wold) (del-atleast  ?wold l1 ?al)) (not (available-atleast wood ?p ?al))))

     (forall (?hold ?hnew - level) (when (and (housing ?p ?hold) (difference ?hnew l1 ?hold)) (and (not (housing ?p ?hold)) (housing ?p ?hnew))))
    ))


    ;; B.2: Building vehicles.
    ; timber [place] -= 1
    (:action build-cart
     :parameters (?p - place ?v - vehicle)
     :precondition (and (potential ?v)  (available-atleast timber ?p l1))
     :effect (and
     	     (forall (?told ?tnew - level) (when (and (available timber ?p ?told) (difference ?told l1 ?tnew)) (and (not (available timber ?p ?told)) (available timber ?p ?tnew))))
    	     (forall (?told - level ?al - atleastlevel) (when (and (available timber ?p ?told) (del-atleast  ?told l1 ?al)) (not (available-atleast timber ?p ?al))))

     	     (is-at ?v ?p)
	     (is-cart ?v)
	     (not (potential ?v))
	     (space-in ?v l1)
     	     (has-space ?v)
	     (increase (labour) 1))
    )


    ; iron [place] -= 2
    (:action build-train
     :parameters (?p - place ?v - vehicle)
     :precondition (and (potential ?v) (available-atleast iron ?p l2))
     :effect (and
     	     (forall (?iold ?inew - level) (when (and (available iron ?p ?iold) (difference ?iold l2 ?inew)) (and (not (available iron ?p ?iold)) (available iron ?p ?inew))))
	     (forall (?iold - level ?al - atleastlevel) (when (and (available iron ?p ?iold) (del-atleast  ?iold l2 ?al)) (not (available-atleast iron ?p ?al))))

     	     (is-at ?v ?p)
	     (is-train ?v)
	     (not (potential ?v))
	     (space-in ?v l5)
    	     (has-space ?v)
	     (increase (labour) 2))
    )

    ; iron [place] -= 4
    (:action build-ship
     :parameters (?p - place ?v - vehicle)
     :precondition (and (has-wharf ?p) (potential ?v) (available-atleast iron ?p l4))
     :effect (and
     	     (forall (?iold ?inew - level) (when (and (available iron ?p ?iold) (difference ?iold l4 ?inew)) (and (not (available iron ?p ?iold)) (available iron ?p ?inew))))
     	     (forall (?iold - level ?al - atleastlevel) (when (and (available iron ?p ?iold) (del-atleast  ?iold l4 ?al)) (not (available-atleast iron ?p ?al))))

     	     (is-at ?v ?p)
	     (is-ship ?v)
	     (not (potential ?v))
	     (space-in ?v l10)
	     (has-space ?v)
	     (increase (labour) 3))
    )

    ;; C.1: Refining resources.

    ; timber [place] -= 1
    ; coal [place] += 1
    (:action burn-coal
     :parameters (?p - place)
     :precondition (and (has-coal-stack ?p)  (available-atleast timber ?p l1))
     :effect (and
     (forall (?told ?tnew - level) (when (and (available timber ?p ?told) (difference ?told l1 ?tnew)) (and (not (available timber ?p ?told))  (available timber ?p ?tnew))))
     (forall (?told - level ?al - atleastlevel) (when (and (available timber ?p ?told) (del-atleast  ?told l1 ?al)) (not (available-atleast timber ?p ?al))))

     (forall (?cold ?cnew - level) (when (and (available coal ?p ?cold) (difference ?cnew l1 ?cold)) (and  (not (available coal ?p ?cold)) (available coal ?p ?cnew))))
     (forall (?cold - level ?al - atleastlevel) (when (and (available coal ?p ?cold) (add-atleast ?cold l1 ?al)) (available-atleast coal ?p ?al)))

     (increase (pollution) 1)) 
    )

    ; timber [place] -= 1
    ; wood [place] += 1
    (:action saw-wood
     :parameters (?p - place)
     :precondition (and (has-sawmill ?p)  (available-atleast timber ?p l1))
     :effect (and
     (forall (?told ?tnew - level) (when (and (available timber ?p ?told) (difference ?told l1 ?tnew)) (and (not (available timber ?p ?told)) (available timber ?p ?tnew))))
     (forall (?told - level ?al - atleastlevel) (when (and (available timber ?p ?told) (del-atleast  ?told l1 ?al)) (not (available-atleast timber ?p ?al))))

     (forall (?wold ?wnew - level) (when (and (available wood ?p ?wold)  (difference ?wnew l1 ?wold)) (and (not (available wood ?p ?wold))  (available wood ?p ?wnew))))
     (forall (?wold - level ?al - atleastlevel) (when (and (available wood ?p ?wold) (add-atleast ?wold l1 ?al)) (available-atleast wood ?p ?al)))

    ))

    ; coal [place] -= 2
    ; ore [place] -= 1
    ; iron [place] += 1
    (:action make-iron
     :parameters (?p - place)
     :precondition (and (has-ironworks ?p) (available-atleast coal ?p l2) (available-atleast ore ?p l1))
     :effect (and
     (forall (?cold ?cnew - level) (when (and (available coal ?p ?cold) (difference ?cold l2 ?cnew)) (and (not (available coal ?p ?cold)) (available coal ?p ?cnew))))
     (forall (?cold - level ?al - atleastlevel) (when (and (available coal ?p ?cold) (del-atleast  ?cold l2 ?al)) (not (available-atleast coal ?p ?al))))

     (forall (?oold ?onew - level) (when (and (available ore ?p ?oold) (difference ?oold l1 ?onew)) (and (not (available ore ?p ?oold)) (available ore ?p ?onew))))
     (forall (?oold - level ?al - atleastlevel) (when (and (available ore ?p ?oold) (del-atleast  ?oold l1 ?al)) (not (available-atleast ore ?p ?al))))

     (forall (?iold ?inew - level) (when (and (available iron ?p ?iold)  (difference ?inew l1 ?iold)) (and (not (available iron ?p ?iold)) (available iron ?p ?inew))))
     (forall (?iold - level ?al - atleastlevel) (when (and (available iron ?p ?iold) (add-atleast ?iold l1 ?al)) (available-atleast iron ?p ?al)))

     (increase (pollution) 2)))
)
