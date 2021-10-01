(define (domain spider)
    (:requirements :typing :conditional-effects :action-costs :negative-preconditions)
    (:types
        cardposition - object
	card_or_tableau - cardposition
        card - card_or_tableau
        tableau - card_or_tableau
        deal - cardposition
    )

    (:constants
        discard - cardposition
    )

; Each deal, deal-i, is represented by a list of cards where the card going to pile-0 is
;   clear and the last card is placed on deal-i.  Moreover, TO-DEAL stores the information
;   of on which deal each card must be placed.  TO-DEAL and on have redundant
;   information. This is done this way because TO-DEAL is static, reducing the number of
;   instantiations of the deal-card schema. On is used on top of TO-DEAL for cards in the
;   deal so that cards are always somewhere.

    (:predicates
        (on ?c1 - card ?c2 - cardposition)
        (clear ?c - cardposition)
        (in-play ?c - card)
        (current-deal ?d - deal)
        ;
        ; static predicates
        (CAN-CONTINUE-GROUP ?c1 - card ?c2 - cardposition)
        (CAN-BE-PLACED-ON ?c1 - card ?c2 - card)
        (IS-ACE ?c - card)
        (IS-KING ?c - card)
        (NEXT-DEAL ?d ?nd - deal)
        (TO-DEAL ?c - card ?p - tableau ?d - deal ?next - cardposition)
        ;
        ; control flags for dealing
        (currently-dealing)
        ;
        ; control flags for collecting cards
        (currently-collecting-deck)
        (collect-card ?c - cardposition)
        ;
        ; derived predicates
        (part-of-tableau ?c - cardposition ?t - tableau)
        (movable ?c - card)
        ;
        ; control flags for derived predicates
        (currently-updating-unmovable)
        (make-unmovable ?c - card)
        (currently-updating-movable)
        (make-movable ?c - cardposition)
        (currently-updating-part-of-tableau)
        (make-part-of-tableau ?c - card ?t - tableau)
    )
    (:functions (total-cost) - number)

    (:action start-dealing
        :parameters ()
        :precondition (and
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-collecting-deck))
            (not (currently-dealing))
        )
        :effect (and
            (currently-dealing)
            (increase (total-cost) 1)
        )
    )

    (:action deal-card
        :parameters (?c - card  ?from - cardposition ?fromdeal - deal ?to - card ?totableau - tableau)
        :precondition (and
            (currently-dealing)
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-collecting-deck))
            (current-deal ?fromdeal)
            (TO-DEAL ?c ?totableau ?fromdeal ?from)
            (clear ?c)
            (on ?c ?from)
            (part-of-tableau ?to ?totableau)
            (clear ?to)
        )
        :effect (and
            (not (on ?c ?from))
            (on ?c ?to)
            (not (clear ?to))
            (clear ?from)
            (in-play ?c)
            (part-of-tableau ?c ?totableau)
            (movable ?c)
            (when
                (not (CAN-CONTINUE-GROUP ?c ?to))
                (and (currently-updating-unmovable) (make-unmovable ?to))
            )
        )
    )

    (:action finish-dealing
        :parameters (?d ?nd - deal)
        :precondition (and
            (currently-dealing)
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-collecting-deck))
            (current-deal ?d)
            (clear ?d)
            (NEXT-DEAL ?d ?nd)
        )
        :effect (and
            (not (currently-dealing))
            (not (current-deal ?d))
            (current-deal ?nd)
        )
    )

    (:action move-to-card
        :parameters (?c - card ?from - card_or_tableau ?to - card ?totableau - tableau)
        :precondition (and
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-collecting-deck))
            (not (currently-updating-part-of-tableau))
            (not (currently-dealing))
            (movable ?c)
            (in-play ?c)
            (clear ?to)
            (in-play ?to)
            (part-of-tableau ?to ?totableau)
            (CAN-BE-PLACED-ON ?c ?to)
            (on ?c ?from)
        )
        :effect (and
            (not (on ?c ?from))
            (on ?c ?to)
            (not (clear ?to))
            (clear ?from)
            (when
                (not (CAN-CONTINUE-GROUP ?c ?from))
                (and (currently-updating-movable) (make-movable ?from))
            )
            (when
                (not (CAN-CONTINUE-GROUP ?c ?to))
                (and (currently-updating-unmovable) (make-unmovable ?to))
            )
            (currently-updating-part-of-tableau)
            (make-part-of-tableau ?c ?totableau)
            (increase (total-cost) 1)
        )
    )

    (:action move-to-tableau
        :parameters (?c - card ?from - card_or_tableau ?to - tableau)
        :precondition (and
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-collecting-deck))
            (not (currently-updating-part-of-tableau))
            (not (currently-dealing))
            (movable ?c)
            (in-play ?c)
            (clear ?to)
            (on ?c ?from)
        )
        :effect (and
            (not (on ?c ?from))
            (on ?c ?to)
            (not (clear ?to))
            (clear ?from)
            (when
                (not (CAN-CONTINUE-GROUP ?c ?from))
                (and (currently-updating-movable) (make-movable ?from))
            )
            (currently-updating-part-of-tableau)
            (make-part-of-tableau ?c ?to)
            (increase (total-cost) 1)
        )
    )

    (:action change-tableau-and-continue
        :parameters (?c - card ?tbefore - tableau ?t - tableau ?next - card)
        :precondition (and
            (currently-updating-part-of-tableau)
            (not (currently-collecting-deck))
            (on ?next ?c)
            (make-part-of-tableau ?c ?t)
            (part-of-tableau ?c ?tbefore)
        )
        :effect (and
            (not (part-of-tableau ?c ?tbefore))
            (part-of-tableau ?c ?t)
            (not  (make-part-of-tableau ?c ?t))
            (make-part-of-tableau ?next ?t)
        )
    )

    (:action change-tableau-and-stop
        :parameters (?c - card ?tbefore - tableau ?t - tableau)
        :precondition (and
            (currently-updating-part-of-tableau)
            (not (currently-collecting-deck))
            (make-part-of-tableau ?c ?t)
            (part-of-tableau ?c ?tbefore)
            (clear ?c)
        )
        :effect (and
            (not (part-of-tableau ?c ?tbefore))
            (part-of-tableau ?c ?t)
            (not  (make-part-of-tableau ?c ?t))
            (not (currently-updating-part-of-tableau))
        )
    )

    (:action make-unmovable-and-continue
        :parameters (?c - card ?nextcard - card)
        :precondition (and
            (currently-updating-unmovable)
            (not (currently-collecting-deck))
            (not (currently-updating-part-of-tableau))
            (on ?c ?nextcard)
            (make-unmovable ?c)
            (CAN-CONTINUE-GROUP ?c ?nextcard)
            (movable ?nextcard)
        )
        :effect (and
            (not (movable ?c))
            (make-unmovable ?nextcard)
            (not (make-unmovable ?c))
        )
    )

    (:action make-unmovable-and-stop-at-tableau
        :parameters (?c - card ?t - tableau)
        :precondition (and
            (currently-updating-unmovable)
            (not (currently-collecting-deck))
            (not (currently-updating-part-of-tableau))
            (on ?c ?t)
            (make-unmovable ?c)
        )
        :effect (and
            (not (movable ?c))
            (not (make-unmovable ?c))
            (not (currently-updating-unmovable))
        )
    )

    (:action make-unmovable-and-stop-at-unmovable
        :parameters (?c - card ?nextcard - card)
        :precondition (and
            (currently-updating-unmovable)
            (not (currently-collecting-deck))
            (not (currently-updating-part-of-tableau))
            (on ?c ?nextcard)
            (make-unmovable ?c)
            (not (movable ?nextcard))
        )
        :effect (and
            (not (movable ?c))
            (not (make-unmovable ?c))
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
        )
    )

    (:action make-movable-and-continue
        :parameters (?c - card ?nextcard - card)
        :precondition (and
            (currently-updating-movable)
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-collecting-deck))
            (on ?c ?nextcard)
            (make-movable ?c)
            (CAN-CONTINUE-GROUP ?c ?nextcard)
        )
        :effect (and
            (make-movable ?nextcard)
            (not  (make-movable ?c))
            (movable ?c)
        )
    )

    (:action make-movable-ignore-pile
        :parameters (?p - tableau)
        :precondition (and
            (currently-updating-movable)
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-collecting-deck))
            (make-movable ?p)
        )
        :effect (and
            (not (make-movable ?p))
            (not (currently-updating-movable))
        )
    )

    (:action make-movable-and-stop
        :parameters (?c - card ?nextcard - cardposition)
        :precondition (and
            (currently-updating-movable)
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-collecting-deck))
            (on ?c ?nextcard)
            (make-movable ?c)
            (not (CAN-CONTINUE-GROUP ?c ?nextcard))
        )
        :effect (and
            (not (make-movable ?c))
            (not (currently-updating-movable))
            (movable ?c)
        )
    )

    (:action start-collecting-deck
        :parameters (?c - card)
        :precondition (and
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-collecting-deck))
            (not (currently-dealing))
            (clear ?c)
            (IS-ACE ?c)
            (in-play ?c)
        )
        :effect (and
            (currently-collecting-deck)
            (collect-card ?c)
            (increase (total-cost) 1)
        )
    )

    (:action collect-card
        :parameters (?c - card ?nextcard - cardposition ?p - tableau)
        :precondition (and
            (currently-collecting-deck)
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-dealing))
            (collect-card ?c)
            (on ?c ?nextcard)
            (in-play ?c)
            (part-of-tableau ?c ?p)
            (CAN-CONTINUE-GROUP ?c ?nextcard)
        )
        :effect (and
            (not (on ?c ?nextcard))
            (on ?c discard)
            (clear ?nextcard)
            (not (in-play ?c))
            (not (part-of-tableau ?c ?p))
            (not (movable ?c))
            (not (collect-card ?c))
            (collect-card ?nextcard)
        )
    )

    (:action finish-collecting-deck
        :parameters (?c - card ?nextcard - cardposition ?p - tableau)
        :precondition (and
            (currently-collecting-deck)
            (not (currently-updating-movable))
            (not (currently-updating-unmovable))
            (not (currently-updating-part-of-tableau))
            (not (currently-dealing))
            (on ?c ?nextcard)
            (collect-card ?c)
            (IS-KING ?c)
            (in-play ?c)
            (part-of-tableau ?c ?p)
        )
        :effect (and
            (not (on ?c ?nextcard))
            (on ?c discard)
            (clear ?nextcard)
            (not (in-play ?c))
            (not (part-of-tableau ?c ?p))
            (not (movable ?c))
            (not (collect-card ?c))
            (not (currently-collecting-deck))
            (make-movable ?nextcard)
            (currently-updating-movable)
        )
    )

)
