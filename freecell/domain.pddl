;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (domain freecell)
  (:requirements :strips :typing)  
  (:types card colnum cellnum num suit)
  (:predicates (on ?c1 - card ?c2 - card)
	       (incell ?c - card)
	       (clear ?c - card)
	       (cellspace ?n - cellnum)
	       (colspace ?n - colnum)
	       (home ?c - card)
	       (bottomcol ?c - card)
	       (canstack ?c1 - card ?c2 - card)
	       (suit ?c - card ?s - suit)
	       (value ?c - card ?v - num)
	       (successor ?n1 - num ?n0 - num)
	       (colsuccessor ?n1 - colnum ?n0 - colnum)
	       (cellsuccessor ?n1 - cellnum ?n0 - cellnum)
 )

;;; Move card between columns. Two versions dependent on whether or
;;; not we generate a new stack.

  (:action move
	   :parameters (?card ?oldcard ?newcard - card)
	   :precondition (and (clear ?card) 
			      (clear ?newcard)
			      (canstack ?card ?newcard)
			      (on ?card ?oldcard))
	   :effect (and
		    (on ?card ?newcard)
		    (clear ?oldcard)
		    (not (on ?card ?oldcard))
		    (not (clear ?newcard))))

  (:action move-b
	   :parameters (?card ?newcard - card ?cols ?ncols - colnum)
	   :precondition (and (clear ?card) 
			      (bottomcol ?card)
			      (clear ?newcard)
			      (canstack ?card ?newcard)
			      (colspace ?cols)
			      (colsuccessor ?ncols ?cols))
	   :effect (and (on ?card ?newcard)
			(colspace ?ncols)
			(not (bottomcol ?card))
			(not (clear ?newcard))
			(not (colspace ?cols))))

;; Send a card to a freecell. Two versions dependent on whether or not
;; we generate a new stack.

  (:action sendtofree 
	   :parameters (?card ?oldcard - card ?cells ?ncells - cellnum)
	   :precondition (and
			  (clear ?card) 
			  (on ?card ?oldcard)
			  (cellspace ?cells)
			  (cellsuccessor ?cells ?ncells))
	   :effect (and
		    (incell ?card) 
		    (clear ?oldcard)
		    (cellspace ?ncells)
		    (not (on ?card ?oldcard))
		    (not (clear ?card))
		    (not (cellspace ?cells))))

  (:action sendtofree-b 
	   :parameters (?card - card ?cells ?ncells - cellnum ?cols ?ncols - colnum)
	   :precondition (and (clear ?card) 
			      (bottomcol ?card)
			      (cellspace ?cells)
			      (cellsuccessor ?cells ?ncells)
			      (colspace ?cols)
			      (colsuccessor ?ncols ?cols))
	   :effect (and
		    (incell ?card)
		    (colspace ?ncols)
		    (cellspace ?ncells)
		    (not (bottomcol ?card))
		    (not (clear ?card))
		    (not (colspace ?cols))
		    (not (cellspace ?cells))))

;; Send a card a new column

(:action sendtonewcol
	 :parameters (?card ?oldcard - card ?cols ?ncols - colnum)
	 :precondition (and
			(clear ?card)
			(colspace ?cols)
			(colsuccessor ?cols ?ncols)
			(on ?card ?oldcard))
	 :effect (and
		  (bottomcol ?card) 
		  (clear ?oldcard)
		  (colspace ?ncols)
		  (not (on ?card ?oldcard))
		  (not (colspace ?cols))))

;;Send a card home

(:action sendtohome
	 :parameters (?card ?oldcard - card ?suit - suit ?vcard - num
			    ?homecard - card ?vhomecard - num)
	 :precondition (and
			(clear ?card) 
			(on ?card ?oldcard)
			(home ?homecard)
			(suit ?card ?suit)
			(suit ?homecard ?suit)
			(value ?card ?vcard)
			(value ?homecard ?vhomecard)
			(successor ?vcard ?vhomecard))
	 :effect (and (home ?card)
		      (clear ?oldcard)
		      (not (on ?card ?oldcard))
                      (not (home ?homecard))
		      (not (clear ?card))))

(:action sendtohome-b
	 :parameters (?card - card ?suit - suit  ?vcard - num
			    ?homecard - card
			    ?vhomecard - num
			    ?cols ?ncols - colnum)
	 :precondition (and (clear ?card) 
			    (bottomcol ?card)
			    (home ?homecard)
			    (suit ?card ?suit)
			    (suit ?homecard ?suit)
			    (value ?card ?vcard)
			    (value ?homecard ?vhomecard)
			    (successor ?vcard ?vhomecard)
			    (colspace ?cols)
			    (colsuccessor ?ncols ?cols))
	 :effect (and (home ?card)
		      (colspace ?ncols)
		      (not (home ?homecard))
		      (not (clear ?card))
		      (not (bottomcol ?card))
		      (not (colspace ?cols))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Move cards in free cell
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(:action homefromfreecell
	 :parameters (?card - card ?suit - suit ?vcard - num
			    ?homecard - card ?vhomecard - num
			    ?cells ?ncells - cellnum)
    ;;Send a card home from a free cell.
	 :precondition (and 
			(incell ?card)
			(home ?homecard) 
			(suit ?card ?suit)
			(suit ?homecard ?suit)
			(value ?card ?vcard)
			(value ?homecard ?vhomecard)
			(successor ?vcard ?vhomecard)
			(cellspace ?cells)
			(cellsuccessor ?ncells ?cells))
	 :effect (and
		  (home ?card)
		  (cellspace ?ncells)
		  (not (incell ?card))
		  (not (cellspace ?cells))
		  (not (home ?homecard))))

(:action colfromfreecell
	 :parameters (?card ?newcard -  card ?cells ?ncells - cellnum)
	 :precondition (and  (incell ?card)
			     (clear ?newcard)
			     (canstack ?card ?newcard)
			     (cellspace ?cells)
			     (cellsuccessor ?ncells ?cells))
	 :effect (and (cellspace ?ncells)
		      (clear ?card)
		      (on ?card ?newcard)
		      (not (incell ?card))
		      (not (cellspace ?cells))
		      (not (clear ?newcard))))

(:action newcolfromfreecell
	 :parameters (?card - card ?cols ?ncols - colnum ?cells ?ncells - cellnum)
	 :precondition (and (incell ?card)
			    (colspace ?cols)
			    (cellspace ?cells)
			    (colsuccessor ?cols ?ncols)
			    (cellsuccessor ?ncells ?cells))
	 :effect (and (bottomcol ?card) 
		      (clear ?card)
		      (colspace ?ncols)
		      (cellspace ?ncells)
		      (not (incell ?card))
		      (not (colspace ?cols))
		      (not (cellspace ?cells)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
