(define (domain spider)
    (:requirements :typing :conditional-effects :disjunctive-preconditions :action-costs)
    (:types
        cardposition - object
        deal - cardposition
	positioninplay - cardposition
        card - positioninplay
        tableau - positioninplay
    )

    (:constants
        discard - cardposition
    )

    (:predicates
        (on ?c1 - card ?c2 - cardposition)
        (current-deal ?d - deal)
 ; static predicates
        (CAN-CONTINUE-GROUP ?c1 - card ?c2 - cardposition)
        (CAN-BE-PLACED-ON ?c1 - card ?c2 - card)
        (IS-ACE ?c - card)
        (IS-KING ?c - card)
        (NEXT-DEAL ?d ?nd - deal)
        (TO-DEAL ?c - card ?p - tableau ?d - deal)
 ; derived predicates
        (clear ?c - positioninplay)
        (movable ?c - card)
	(is-in-block ?x - card ?y - card)
        (in-tableau ?c - card ?t - tableau)
    )
    
    (:functions (total-cost) - number)


(:derived (clear ?x - positioninplay)
	  (not (exists (?y - card) (on ?y ?x)))
)


(:derived (movable ?y - card)
	   (or (clear ?y)
	       (exists (?x - card) (and
	                     (on ?x ?y)
			     (movable ?x)
			     (CAN-CONTINUE-GROUP ?x ?y))))
)

(:derived (is-in-block ?y - card ?x - card)
          (or (= ?x ?y) 
	      (and (on ?x ?y) (CAN-CONTINUE-GROUP ?x ?y))
	      (exists (?z - card)  (and (on ?x ?z) (CAN-CONTINUE-GROUP ?x ?z)  (is-in-block ?y ?z)))
	  )
)

(:derived (in-tableau ?c - positioninplay ?t - tableau)
          (or (= ?c ?t)
	      (on ?c ?t)
	      (exists (?x - card)  (and (on ?c ?x) (in-tableau ?x ?t)))
	   )
)


    (:action perform-deal
        :parameters (?d ?nd - deal )
        :precondition (and
		(current-deal ?d)

	)
        :effect (and
            (not (current-deal ?d))
	    (forall (?nd - deal) (when (NEXT-DEAL ?d ?nd) (current-deal ?nd)))
	    (forall (?c - card ?totableau - tableau ?to - playposition)
               (when (and
	                 (TO-DEAL ?c ?totableau ?d)
			 (clear ?to)
			 (in-tableau ?to ?totableau)
			 )
		     (and
			  (on ?c ?to)
			  (not (on ?c ?d))
		     )
	       )
	    )
            (increase (total-cost) 1)
        )
    )

    (:action move-card
        :parameters (?c - card ?from - positioninplay ?to - positioninplay)
        :precondition (and
            (movable ?c)
            (clear ?to)
            (CAN-BE-PLACED-ON ?c ?to)
            (on ?c ?from)
        )
        :effect (and
            (not (on ?c ?from))
            (on ?c ?to)
            (increase (total-cost) 1)
        )
    )
    
    (:action collect-deck
        :parameters (?ace - card ?king - card)
        :precondition (and
            (clear ?ace)
            (IS-ACE ?ace)
    	    (IS-KING ?king)
    	    (is-in-block ?king ?ace)
        )
        :effect (and
    	     (increase (total-cost) 1)
   	     (forall (?c - cardposition ?c2 - cardposition)
    	         (when (and (is-in-block ?c ?ace) (on ?c ?c2))
		 (and 
			   (not (on ?c ?c2))
    	     	 	   (on ?c discard)
    	     	       )
    	     	  )
		)
             )
	     )
)
