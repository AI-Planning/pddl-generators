;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; The child-snack domain 2013
;;
;; This domain is for planning how to make and serve sandwiches for a group of
;; children in which some are allergic to gluten. There are two actions for
;; making sandwiches from their ingredients. The first one makes a sandwich and
;; the second one makes a sandwich taking into account that all ingredients are
;; gluten-free. There are also actions to put a sandwich on a tray, to move a tray
;; from one place to another and to serve sandwiches.
;; 
;; Problems in this domain define the ingredients to make sandwiches at the initial
;; state. Goals consist of having all kids served with a sandwich to which they
;; are not allergic.
;; 
;; Author: Raquel Fuentetaja and Tomás de la Rosa
;; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (domain child-snack)
(:requirements :typing :equality)
(:types child bread-portion content-portion sandwich tray place)
(:constants kitchen - place)

(:predicates (at_kitchen_bread ?b - bread-portion)
	     (at_kitchen_content ?c - content-portion)
     	     (at_kitchen_sandwich ?s - sandwich)
     	     (no_gluten_bread ?b - bread-portion)
       	     (no_gluten_content ?c - content-portion)
      	     (ontray ?s - sandwich ?t - tray)
       	     (no_gluten_sandwich ?s - sandwich)
	     (allergic_gluten ?c - child)
     	     (not_allergic_gluten ?c - child)
	     (served ?c - child)
	     (waiting ?c - child ?p - place)
             (at ?t - tray ?p - place)
	     (notexist ?s - sandwich)
  )

(:action make_sandwich_no_gluten 
	 :parameters (?s - sandwich ?b - bread-portion ?c - content-portion)
	 :precondition (and (at_kitchen_bread ?b)
			    (at_kitchen_content ?c)
			    (no_gluten_bread ?b)
			    (no_gluten_content ?c)
			    (notexist ?s))
	 :effect (and
		   (not (at_kitchen_bread ?b))
		   (not (at_kitchen_content ?c))
		   (at_kitchen_sandwich ?s)
		   (no_gluten_sandwich ?s)
                   (not (notexist ?s))
		   ))


(:action make_sandwich
	 :parameters (?s - sandwich ?b - bread-portion ?c - content-portion)
	 :precondition (and (at_kitchen_bread ?b)
			    (at_kitchen_content ?c)
                            (notexist ?s)
			    )
	 :effect (and
		   (not (at_kitchen_bread ?b))
		   (not (at_kitchen_content ?c))
		   (at_kitchen_sandwich ?s)
                   (not (notexist ?s))
		   ))


(:action put_on_tray
	 :parameters (?s - sandwich ?t - tray)
	 :precondition (and  (at_kitchen_sandwich ?s)
			     (at ?t kitchen))
	 :effect (and
		   (not (at_kitchen_sandwich ?s))
		   (ontray ?s ?t)))


(:action serve_sandwich_no_gluten
 	:parameters (?s - sandwich ?c - child ?t - tray ?p - place)
	:precondition (and
		       (allergic_gluten ?c)
		       (ontray ?s ?t)
		       (waiting ?c ?p)
		       (no_gluten_sandwich ?s)
                       (at ?t ?p)
		       )
	:effect (and (not (ontray ?s ?t))
		     (served ?c)))

(:action serve_sandwich
	:parameters (?s - sandwich ?c - child ?t - tray ?p - place)
	:precondition (and (not_allergic_gluten ?c)
	                   (waiting ?c ?p)
			   (ontray ?s ?t)
			   (at ?t ?p))
	:effect (and (not (ontray ?s ?t))
		     (served ?c)))

(:action move_tray
	 :parameters (?t - tray ?p1 ?p2 - place)
	 :precondition (and (at ?t ?p1))
	 :effect (and (not (at ?t ?p1))
		      (at ?t ?p2)))
			    

)