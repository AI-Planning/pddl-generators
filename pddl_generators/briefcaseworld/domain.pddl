(define (domain briefcase)
(:requirements :adl)
(:types portable location)
(:predicates (at ?y - portable ?x - location)
             (in ?x - portable)
             (is-at ?x - location))


(:action move
  :parameters (?m ?l - location)
  :precondition  (is-at ?m)
  :effect (and (is-at ?l) (not (is-at ?m))
		    (forall (?x - portable) (when (in ?x)
		      (and (at ?x ?l) (not (at ?x ?m)))))))

  (:action take-out
      :parameters (?x - portable)
      :precondition (in ?x)
      :effect (not (in ?x)))
      
  (:action put-in
      :parameters (?x - portable ?l - location)
      :precondition (and (not (in ?x)) (at ?x ?l) (is-at ?l))
      :effect (in ?x)))

