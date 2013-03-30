(define (domain mprime-strips)
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
       (not-equal ?l1 ?l2)

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

   (:action donate
      :parameters (?l1 ?l2 - location ?f11 ?f12 ?f13 ?f21 ?f22 - fuel)
      :precondition (and (not-equal ?l1 ?l2)
                         (has-fuel ?l1 ?f11)
                         (fuel-neighbor ?f12 ?f11)
                         (fuel-neighbor ?f13 ?f12)
                         (has-fuel ?l2 ?f21)
                         (fuel-neighbor ?f21 ?f22))
      :effect (and (not (has-fuel ?l1 ?f11))
                   (has-fuel ?l1 ?f12)
                   (not (has-fuel ?l2 ?f21))
                   (has-fuel ?l2 ?f22)))

)
