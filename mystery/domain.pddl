(define (domain mystery-strips)
   (:types space fuel
           location vehicle cargo)
   (:predicates
       (at ?v ?l)
       (conn ?l1 ?l2)
       (has-fuel ?l ?f)
       (fuel-neighbor ?f1 ?f2)
       (in ?c ?v)
       (has-space ?v ?s)
       (space-neighbor ?s1 ?s2)

)



   (:action move
       :parameters (?v - vehicle ?l1 ?l2 - location ?f1 ?f2 - fuel)
       :precondition (and (at ?v ?l1)
                          (conn ?l1 ?l2)
                          (has-fuel ?l1 ?f1)
                          (fuel-neighbor ?f2 ?f1))
       :effect (and (not (at ?v ?l1))
                    (at ?v ?l2)
                    (not (has-fuel ?l1 ?f1))
                    (has-fuel ?l1 ?f2)))

   (:action load
       :parameters (?c - cargo ?v - vehicle ?l - location ?s1 ?s2 - space)
       :precondition (and (at ?c ?l)
                          (at ?v ?l)
                          (has-space ?v ?s1)
			  (space-neighbor ?s2 ?s1))
       :effect (and (not (at ?c ?l))
                    (in ?c ?v)
                    (not (has-space ?v ?s1))
                    (has-space ?v ?s2)))

   (:action unload
       :parameters (?c - cargo ?v - vehicle ?l - location ?s1 ?s2 - space)
       :precondition (and (in ?c ?v)
                          (at ?v ?l)
			  (has-space ?v ?s1)
                          (space-neighbor ?s1 ?s2))
       :effect (and (not (in ?c ?v))
                    (at ?c ?l)
                    (not (has-space ?v ?s1))
                    (has-space ?v ?s2)))

)
