    (define (domain civ)
    (:requirements :typing :negative-preconditions :conditional-effects)

    (:types store stone_level ore_level timber_level coal_level wood_level iron_level resource space_level housing_level - object
            place vehicle - store
            stone_atleastlevel - stone_level
            ore_atleastlevel - ore_level
            timber_atleastlevel - timber_level
            coal_atleastlevel - coal_level
            wood_atleastlevel - wood_level
            iron_atleastlevel - iron_level)

    (:constants 
            spl0 spl1 spl2 spl3 spl4 spl5 spl6 spl7 spl8 spl9 spl10 - space_level
            hl0 hl1 hl2 hl3 hl4 hl5 hl6 hl7 hl8 hl9 hl10 - housing_level
            sl1 sl2 sl4 - stone_atleastlevel
            sl0 sl3 sl5 sl6 sl7 sl8 sl9 sl10 - stone_level
            ol1 ol2 ol4 - ore_atleastlevel
            ol0 ol3 ol5 ol6 ol7 ol8 ol9 ol10 - ore_level
            tl1 tl2 tl4 - timber_atleastlevel
            tl0 tl3 tl5 tl6 tl7 tl8 tl9 tl10 - timber_level
            cl1 cl2 cl4 - coal_atleastlevel
            cl0 cl3 cl5 cl6 cl7 cl8 cl9 cl10 - coal_level
            wl1 wl2 wl4 - wood_atleastlevel
            wl0 wl3 wl5 wl6 wl7 wl8 wl9 wl10 - wood_level
            il1 il2 il4 - iron_atleastlevel
            il0 il3 il5 il6 il7 il8 il9 il10 - iron_level)

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

        (space-in ?v - vehicle ?l - space_level)

        (available-stone ?s - store ?l - stone_level)
        (available-ore ?s - store ?l - ore_level)
        (available-timber ?s - store ?l - timber_level)
        (available-coal ?s - store ?l - coal_level)
        (available-wood ?s - store ?l - wood_level)
        (available-iron ?s - store ?l - iron_level)

        (housing ?p - place ?l - housing_level)

        (diff-stone ?l1 ?l2 ?l3 - stone_level)
        (diff-ore ?l1 ?l2 ?l3 - ore_level)
        (diff-timber ?l1 ?l2 ?l3 - timber_level)
        (diff-coal ?l1 ?l2 ?l3 - coal_level)
        (diff-wood ?l1 ?l2 ?l3 - wood_level)
        (diff-iron ?l1 ?l2 ?l3 - iron_level)
        (diff-space ?l1 ?l2 ?l3 - space_level)
        (diff-housing ?l1 ?l2 ?l3 - housing_level)

        (available-atleast-stone ?s - store ?l - stone_level)
        (available-atleast-ore ?s - store ?l - ore_level)
        (available-atleast-timber ?s - store ?l - timber_level)
        (available-atleast-coal ?s - store ?l - coal_level)
        (available-atleast-wood ?s - store ?l - wood_level)
        (available-atleast-iron ?s - store ?l - iron_level)

        (has-space ?v - vehicle)

        ; ?r_old < ?al <= ?rold + ?r_added
        (add-atleast-stone ?r_old - stone_level ?r_added - stone_atleastlevel ?al - stone_atleastlevel)
        (add-atleast-ore ?r_old - ore_level ?r_added - ore_atleastlevel ?al - ore_atleastlevel)
        (add-atleast-timber ?r_old - timber_level ?r_added - timber_atleastlevel ?al - timber_atleastlevel)
        (add-atleast-coal ?r_old - coal_level ?r_added - coal_atleastlevel ?al - coal_atleastlevel)
        (add-atleast-wood ?r_old - wood_level ?r_added - wood_atleastlevel ?al - wood_atleastlevel)
        (add-atleast-iron ?r_old - iron_level ?r_added - iron_atleastlevel ?al - iron_atleastlevel)
    
        ; ?r_old - r_consumed < ?al <= ?rold
        (del-atleast-stone ?r_old - stone_level ?r_consumed - stone_atleastlevel ?al - stone_atleastlevel)
        (del-atleast-ore ?r_old - ore_level ?r_consumed - ore_atleastlevel ?al - ore_atleastlevel)
        (del-atleast-timber ?r_old - timber_level ?r_consumed - timber_atleastlevel ?al - timber_atleastlevel)
        (del-atleast-coal ?r_old - coal_level ?r_consumed - coal_atleastlevel ?al - coal_atleastlevel)
        (del-atleast-wood ?r_old - wood_level ?r_consumed - wood_atleastlevel ?al - wood_atleastlevel)
        (del-atleast-iron ?r_old - iron_level ?r_consumed - iron_atleastlevel ?al - iron_atleastlevel)
    )

    (:functions (total-cost))

    ;; A.1: Loading and unloading.
    ; resource[place] -= 1, resource[vehicle] += 1, space[vehicle] -= 1\
    (:action load-stone
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-stone ?p sl1) (has-space ?v))
     :effect (and
            (forall (?rpold ?rpnew - stone_level) (when (and (available-stone ?p ?rpold) (diff-stone ?rpold sl1 ?rpnew)) (and (available-stone ?p ?rpnew) (not (available-stone ?p ?rpold)))))
            (forall (?rpold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?p ?rpold) (del-atleast-stone ?rpold sl1 ?al)) (not (available-atleast-stone ?p ?al))))

            (forall (?rvold ?rvnew - stone_level) (when (and (available-stone ?v ?rvold) (diff-stone ?rvnew sl1 ?rvold)) (and (not (available-stone ?v ?rvold)) (available-stone ?v ?rvnew))))
            (forall (?rvold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?v ?rvold) (add-atleast-stone ?rvold sl1 ?al)) (available-atleast-stone ?v ?al)))
          
            (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?sold spl1 ?snew)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
            (forall (?sold - space_level) (when (and (space-in ?v ?sold) (= ?sold spl1)) (not (has-space ?v))))
      
            (increase (labour) 1))
     )
     
    (:action load-ore
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-ore ?p ol1) (has-space ?v))
     :effect (and
          (forall (?rpold ?rpnew - ore_level) (when (and (available-ore ?p ?rpold) (diff-ore ?rpold ol1 ?rpnew)) (and (available-ore ?p ?rpnew) (not (available-ore ?p ?rpold)))))
          (forall (?rpold - ore_level ?al - ore_atleastlevel) (when (and (available-ore ?p ?rpold) (del-atleast-ore ?rpold ol1 ?al)) (not (available-atleast-ore ?p ?al))))

          (forall (?rvold ?rvnew - ore_level) (when (and (available-ore ?v ?rvold) (diff-ore ?rvnew ol1 ?rvold)) (and (not (available-ore ?v ?rvold)) (available-ore ?v ?rvnew))))
         (forall (?rvold - ore_level ?al - ore_atleastlevel) (when (and (available-ore ?v ?rvold) (add-atleast-ore ?rvold ol1 ?al)) (available-atleast-ore ?v ?al)))
          
         (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?sold spl1 ?snew)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
           (forall (?sold - space_level) (when (and (space-in ?v ?sold) (= ?sold spl1)) (not (has-space ?v))))
      
         (increase (labour) 1))
     )
     
    (:action load-timber
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-timber ?p tl1) (has-space ?v))
     :effect (and
          (forall (?rpold ?rpnew - timber_level) (when (and (available-timber ?p ?rpold) (diff-timber ?rpold tl1 ?rpnew)) (and (available-timber ?p ?rpnew) (not (available-timber ?p ?rpold)))))
          (forall (?rpold - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?p ?rpold) (del-atleast-timber ?rpold tl1 ?al)) (not (available-atleast-timber ?p ?al))))

          (forall (?rvold ?rvnew - timber_level) (when (and (available-timber ?v ?rvold) (diff-timber ?rvnew tl1 ?rvold)) (and (not (available-timber ?v ?rvold)) (available-timber ?v ?rvnew))))
         (forall (?rvold - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?v ?rvold) (add-atleast-timber ?rvold tl1 ?al)) (available-atleast-timber ?v ?al)))
          
         (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?sold spl1 ?snew)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
           (forall (?sold - space_level) (when (and (space-in ?v ?sold) (= ?sold spl1)) (not (has-space ?v))))
      
         (increase (labour) 1))
     )
     
    (:action load-wood
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-wood ?p wl1) (has-space ?v))
     :effect (and
          (forall (?rpold ?rpnew - wood_level) (when (and (available-wood ?p ?rpold) (diff-wood ?rpold wl1 ?rpnew)) (and (available-wood ?p ?rpnew) (not (available-wood ?p ?rpold)))))
          (forall (?rpold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?p ?rpold) (del-atleast-wood ?rpold wl1 ?al)) (not (available-atleast-wood ?p ?al))))

          (forall (?rvold ?rvnew - wood_level) (when (and (available-wood ?v ?rvold) (diff-wood ?rvnew wl1 ?rvold)) (and (not (available-wood ?v ?rvold)) (available-wood ?v ?rvnew))))
         (forall (?rvold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?v ?rvold) (add-atleast-wood ?rvold wl1 ?al)) (available-atleast-wood ?v ?al)))
          
         (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?sold spl1 ?snew)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
           (forall (?sold - space_level) (when (and (space-in ?v ?sold) (= ?sold spl1)) (not (has-space ?v))))
      
         (increase (labour) 1))
     )
     
    (:action load-coal
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-coal ?p cl1) (has-space ?v))
     :effect (and
          (forall (?rpold ?rpnew - coal_level) (when (and (available-coal ?p ?rpold) (diff-coal ?rpold cl1 ?rpnew)) (and (available-coal ?p ?rpnew) (not (available-coal ?p ?rpold)))))
          (forall (?rpold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?p ?rpold) (del-atleast-coal ?rpold cl1 ?al)) (not (available-atleast-coal ?p ?al))))

          (forall (?rvold ?rvnew - coal_level) (when (and (available-coal ?v ?rvold) (diff-coal ?rvnew cl1 ?rvold)) (and (not (available-coal ?v ?rvold)) (available-coal ?v ?rvnew))))
         (forall (?rvold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?v ?rvold) (add-atleast-coal ?rvold cl1 ?al)) (available-atleast-coal ?v ?al)))
          
         (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?sold spl1 ?snew)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
           (forall (?sold - space_level) (when (and (space-in ?v ?sold) (= ?sold spl1)) (not (has-space ?v))))
      
         (increase (labour) 1))
     )
     
    (:action load-iron
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-iron ?p il1) (has-space ?v))
     :effect (and
          (forall (?rpold ?rpnew - iron_level) (when (and (available-iron ?p ?rpold) (diff-iron ?rpold il1 ?rpnew)) (and (available-iron ?p ?rpnew) (not (available-iron ?p ?rpold)))))
          (forall (?rpold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?p ?rpold) (del-atleast-iron ?rpold il1 ?al)) (not (available-atleast-iron ?p ?al))))

          (forall (?rvold ?rvnew - iron_level) (when (and (available-iron ?v ?rvold) (diff-iron ?rvnew il1 ?rvold)) (and (not (available-iron ?v ?rvold)) (available-iron ?v ?rvnew))))
         (forall (?rvold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?v ?rvold) (add-atleast-iron ?rvold il1 ?al)) (available-atleast-iron ?v ?al)))
          
         (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?sold spl1 ?snew)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
           (forall (?sold - space_level) (when (and (space-in ?v ?sold) (= ?sold spl1)) (not (has-space ?v))))
      
         (increase (labour) 1))
     )
     
     ; resource[place] += 1, resource[vehicle] -= 1, space[vehicle] += 1
    (:action unload-stone
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-stone ?v sl1) )
     :effect (and
     (forall (?rpold ?rpnew - stone_level) (when (and (available-stone ?p ?rpold) (diff-stone ?rpnew sl1 ?rpold)) (and (not (available-stone ?p ?rpold)) (available-stone ?p ?rpnew))))
     (forall (?rpold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?p ?rpold) (add-atleast-stone ?rpold sl1 ?al)) (available-atleast-stone ?p ?al)))

     (forall (?rvold ?rvnew - stone_level) (when (and (available-stone ?v ?rvold) (diff-stone ?rvold sl1 ?rvnew) ) (and (not (available-stone ?v ?rvold)) (available-stone ?v ?rvnew))))
     (forall (?rvold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?v ?rvold) (del-atleast-stone ?rvold sl1 ?al)) (not (available-atleast-stone ?v ?al))))

     (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl1 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
     (has-space ?v)

     (increase (labour) 1))
    )

    (:action unload-ore
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-ore ?v ol1) )
     :effect (and
     (forall (?rpold ?rpnew - ore_level) (when (and (available-ore ?p ?rpold) (diff-ore ?rpnew ol1 ?rpold)) (and (not (available-ore ?p ?rpold)) (available-ore ?p ?rpnew))))
     (forall (?rpold - ore_level ?al - ore_atleastlevel) (when (and (available-ore ?p ?rpold) (add-atleast-ore ?rpold ol1 ?al)) (available-atleast-ore ?p ?al)))

     (forall (?rvold ?rvnew - ore_level) (when (and (available-ore ?v ?rvold) (diff-ore ?rvold ol1 ?rvnew) ) (and (not (available-ore ?v ?rvold)) (available-ore ?v ?rvnew))))
     (forall (?rvold - ore_level ?al - ore_atleastlevel) (when (and (available-ore ?v ?rvold) (del-atleast-ore ?rvold ol1 ?al)) (not (available-atleast-ore ?v ?al))))

     (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl1 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
     (has-space ?v)

     (increase (labour) 1))
    )

    (:action unload-timber
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-timber ?v tl1) )
     :effect (and
     (forall (?rpold ?rpnew - timber_level) (when (and (available-timber ?p ?rpold) (diff-timber ?rpnew tl1 ?rpold)) (and (not (available-timber ?p ?rpold)) (available-timber ?p ?rpnew))))
     (forall (?rpold - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?p ?rpold) (add-atleast-timber ?rpold tl1 ?al)) (available-atleast-timber ?p ?al)))

     (forall (?rvold ?rvnew - timber_level) (when (and (available-timber ?v ?rvold) (diff-timber ?rvold tl1 ?rvnew) ) (and (not (available-timber ?v ?rvold)) (available-timber ?v ?rvnew))))
     (forall (?rvold - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?v ?rvold) (del-atleast-timber ?rvold tl1 ?al)) (not (available-atleast-timber ?v ?al))))

     (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl1 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
     (has-space ?v)

     (increase (labour) 1))
    )

    (:action unload-wood
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-wood ?v wl1) )
     :effect (and
     (forall (?rpold ?rpnew - wood_level) (when (and (available-wood ?p ?rpold) (diff-wood ?rpnew wl1 ?rpold)) (and (not (available-wood ?p ?rpold)) (available-wood ?p ?rpnew))))
     (forall (?rpold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?p ?rpold) (add-atleast-wood ?rpold wl1 ?al)) (available-atleast-wood ?p ?al)))

     (forall (?rvold ?rvnew - wood_level) (when (and (available-wood ?v ?rvold) (diff-wood ?rvold wl1 ?rvnew) ) (and (not (available-wood ?v ?rvold)) (available-wood ?v ?rvnew))))
     (forall (?rvold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?v ?rvold) (del-atleast-wood ?rvold wl1 ?al)) (not (available-atleast-wood ?v ?al))))

     (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl1 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
     (has-space ?v)

     (increase (labour) 1))
    )

    (:action unload-coal
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-coal ?v cl1) )
     :effect (and
     (forall (?rpold ?rpnew - coal_level) (when (and (available-coal ?p ?rpold) (diff-coal ?rpnew cl1 ?rpold)) (and (not (available-coal ?p ?rpold)) (available-coal ?p ?rpnew))))
     (forall (?rpold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?p ?rpold) (add-atleast-coal ?rpold cl1 ?al)) (available-atleast-coal ?p ?al)))

     (forall (?rvold ?rvnew - coal_level) (when (and (available-coal ?v ?rvold) (diff-coal ?rvold cl1 ?rvnew) ) (and (not (available-coal ?v ?rvold)) (available-coal ?v ?rvnew))))
     (forall (?rvold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?v ?rvold) (del-atleast-coal ?rvold cl1 ?al)) (not (available-atleast-coal ?v ?al))))

     (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl1 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
     (has-space ?v)

     (increase (labour) 1))
    )

    (:action unload-iron
     :parameters (?v - vehicle ?p - place)
     :precondition (and (is-at ?v ?p) (available-atleast-iron ?v il1) )
     :effect (and
     (forall (?rpold ?rpnew - iron_level) (when (and (available-iron ?p ?rpold) (diff-iron ?rpnew il1 ?rpold)) (and (not (available-iron ?p ?rpold)) (available-iron ?p ?rpnew))))
     (forall (?rpold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?p ?rpold) (add-atleast-iron ?rpold il1 ?al)) (available-atleast-iron ?p ?al)))

     (forall (?rvold ?rvnew - iron_level) (when (and (available-iron ?v ?rvold) (diff-iron ?rvold il1 ?rvnew) ) (and (not (available-iron ?v ?rvold)) (available-iron ?v ?rvnew))))
     (forall (?rvold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?v ?rvold) (del-atleast-iron ?rvold il1 ?al)) (not (available-atleast-iron ?v ?al))))

     (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl1 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
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
     :precondition (and (is-train ?v) (connected-by-rail ?p1 ?p2) (is-at ?v ?p1) (available-atleast-coal ?v cl1) )
     :effect (and 
        (not (is-at ?v ?p1))
        (is-at ?v ?p2)
        (forall (?cold ?cnew - coal_level) (when (and (available-coal ?v ?cold) (diff-coal ?cold cl1 ?cnew)) (and (not (available-coal ?v ?cold)) (available-coal ?v ?cnew))))
        (forall (?cold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?v ?cold) (del-atleast-coal ?cold cl1 ?al)) (not (available-atleast-coal ?v ?al))))

        (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl1 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
        (has-space ?v)

        (increase (pollution) 1))
    )

    ; coal[vehicle] -= 2
    ; space[vehicle] += 2
    (:action move-ship
     :parameters (?v - vehicle ?p1 ?p2 - place)
     :precondition (and (is-ship ?v) (connected-by-sea ?p1 ?p2) (is-at ?v ?p1) (available-atleast-coal ?v cl2))
     :effect (and
        (not (is-at ?v ?p1))
        (is-at ?v ?p2) 
        (forall (?cold ?cnew - coal_level) (when (and (available-coal ?v ?cold) (diff-coal ?cold cl2 ?cnew)) (and (not (available-coal ?v ?cold)) (available-coal ?v ?cnew))))
        (forall (?cold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?v ?cold) (del-atleast-coal ?cold cl2 ?al)) (not (available-atleast-coal ?v ?al))))

        (forall (?sold ?snew - space_level) (when (and (space-in ?v ?sold) (diff-space ?snew spl2 ?sold)) (and (not (space-in ?v ?sold)) (space-in ?v ?snew))))
        (has-space ?v)

        (increase (pollution) 2))
    )
    
    ;; B.1: Building structures.
    ; timber [place] -= 1
    (:action build-coal-stack
     :parameters (?p - place)
     :precondition (and (not (has-coal-stack ?p)) (available-atleast-timber ?p tl1))
     :effect (and
          (has-coal-stack ?p)
     (forall (?told ?tnew - timber_level) (when (and (available-timber ?p ?told) (diff-timber ?told tl1 ?tnew)) (and (not (available-timber ?p ?told)) (available-timber ?p ?tnew))))
     (forall (?told - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?p ?told) (del-atleast-timber ?told tl1 ?al)) (not (available-atleast-timber ?p ?al))))

     (increase (labour) 2))
    )

    ; timber [place] -= 2
    (:action build-sawmill
     :parameters (?p - place)
     :precondition (and (not (has-sawmill ?p)) (available-atleast-timber ?p tl2))
     :effect (and
          (has-sawmill ?p)
          (forall (?told ?tnew - timber_level) (when (and (available-timber ?p ?told) (diff-timber ?told tl2 ?tnew)) (and (not (available-timber ?p ?told)) (available-timber ?p ?tnew))))
     (forall (?told - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?p ?told) (del-atleast-timber ?told tl2 ?al)) (not (available-atleast-timber ?p ?al))))

     (increase (labour) 2))
    )

    ; wood [place] -= 2
    ; stone [place] -= 2
    (:action build-ironworks
     :parameters (?p - place)
     :precondition (and (not (has-ironworks ?p)) (available-atleast-wood ?p wl2) (available-atleast-stone ?p sl2))
     :effect (and
          (has-ironworks ?p)    
          (forall (?sold ?snew - stone_level) (when (and (available-stone ?p ?sold) (diff-stone ?sold sl2 ?snew)) (and (not (available-stone ?p ?sold)) (available-stone ?p ?snew))))
     (forall (?sold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?p ?sold) (del-atleast-stone ?sold sl2 ?al)) (not (available-atleast-stone ?p ?al))))

       (forall (?wold ?wnew - wood_level) (when (and (available-wood ?p ?wold) (diff-wood ?wold wl2 ?wnew)) (and (not (available-wood ?p ?wold)) (available-wood ?p ?wnew))))
       (forall (?wold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?p ?wold) (del-atleast-wood ?wold wl2 ?al)) (not (available-atleast-wood ?p ?al))))
      
     (increase (labour) 3))
    )

    ; wood [place] -= 2
    ; stone [place] -= 2
    (:action build-docks
     :parameters (?p - place)
     :precondition (and (by-coast ?p) (not (has-docks ?p)) (available-atleast-wood ?p wl2) (available-atleast-stone ?p sl2)) 
     :effect (and
     (has-docks ?p)
     (forall (?wold ?wnew - wood_level) (when (and (available-wood ?p ?wold) (diff-wood ?wold wl2 ?wnew)) (and (not (available-wood ?p ?wold)) (available-wood ?p ?wnew))))
     (forall (?wold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?p ?wold) (del-atleast-wood ?wold wl2 ?al)) (not (available-atleast-wood ?p ?al))))

     (forall (?sold ?snew - stone_level) (when (and (available-stone ?p ?sold) (diff-stone ?sold sl2 ?snew)) (and (not (available-stone ?p ?sold)) (available-stone ?p ?snew))))
     (forall (?sold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?p ?sold) (del-atleast-stone ?sold sl2 ?al)) (not (available-atleast-stone ?p ?al))))

     (increase (labour) 2))
    )

    ; iron [place] -= 2
    ; stone [place] -= 2
    (:action build-wharf
     :parameters (?p - place)
     :precondition (and (has-docks ?p) (not (has-wharf ?p)) (available-atleast-iron ?p il2) (available-atleast-stone ?p sl2))
     :effect (and
     (has-wharf ?p) 
     (forall (?iold ?inew - iron_level) (when (and (available-iron ?p ?iold) (diff-iron ?iold il2 ?inew)) (and (not (available-iron ?p ?iold)) (available-iron ?p ?inew))))
     (forall (?iold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?p ?iold) (del-atleast-iron ?iold il2 ?al)) (not (available-atleast-iron ?p ?al))))

     (forall (?sold ?snew - stone_level) (when (and (available-stone ?p ?sold) (diff-stone ?sold sl2 ?snew)) (and (not (available-stone ?p ?sold)) (available-stone ?p ?snew))))
     (forall (?sold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?p ?sold) (del-atleast-stone ?sold sl2 ?al)) (not (available-atleast-stone ?p ?al))))

     (increase (labour) 2))
    )

    ; iron [place] -= 1
    ; wood [place] -= 1
    (:action build-rail
     :parameters (?p1 ?p2 - place)
     :precondition (and (connected-by-land ?p1 ?p2) (not (connected-by-rail ?p1 ?p2)) (available-atleast-iron ?p1 il1) (available-atleast-wood ?p1 wl1))
     :effect (and
          (connected-by-rail ?p1 ?p2)
          (forall (?wold ?wnew - wood_level) (when (and (available-wood ?p1 ?wold) (diff-wood ?wold wl1 ?wnew)) (and (not (available-wood ?p1 ?wold)) (available-wood ?p1 ?wnew))))
             (forall (?wold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?p1 ?wold) (del-atleast-wood ?wold wl1 ?al)) (not (available-atleast-wood ?p1 ?al))))

             (forall (?iold ?inew - iron_level) (when (and (available-iron ?p1 ?iold) (diff-iron ?iold il1 ?inew)) (and (not (available-iron ?p1 ?iold)) (available-iron ?p1 ?inew))))
             (forall (?iold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?p1 ?iold) (del-atleast-iron ?iold il1 ?al)) (not (available-atleast-iron ?p1 ?al))))

          (increase (labour) 2))
    )

    ; wood [place] -= 1
    ; stone [place] -= 1
    ; housing [place] += 1 (we do not need available-atleast-housing because it's not a precondition of any action)
    (:action build-house
     :parameters (?p - place)
     :precondition (and (available-atleast-wood ?p wl1) (available-atleast-stone ?p sl1))
     :effect (and
     (forall (?sold ?snew - stone_level) (when (and (available-stone ?p ?sold) (diff-stone ?sold sl1 ?snew)) (and (not (available-stone ?p ?sold)) (available-stone ?p ?snew))))
     (forall (?sold - stone_level ?al - stone_atleastlevel) (when (and (available-stone ?p ?sold) (del-atleast-stone ?sold sl1 ?al)) (not (available-atleast-stone ?p ?al))))

     (forall (?wold ?wnew - wood_level) (when (and (available-wood ?p ?wold) (diff-wood ?wold wl1 ?wnew)) (and (not (available-wood ?p ?wold)) (available-wood ?p ?wnew))))
     (forall (?wold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?p ?wold) (del-atleast-wood ?wold wl1 ?al)) (not (available-atleast-wood ?p ?al))))

     (forall (?hold ?hnew - housing_level) (when (and (housing ?p ?hold) (diff-housing ?hnew hl1 ?hold)) (and (not (housing ?p ?hold)) (housing ?p ?hnew))))
    ))


    ;; B.2: Building vehicles.
    ; timber [place] -= 1
    (:action build-cart
     :parameters (?p - place ?v - vehicle)
     :precondition (and (potential ?v) (available-atleast-timber ?p tl1))
     :effect (and
          (forall (?told ?tnew - timber_level) (when (and (available-timber ?p ?told) (diff-timber ?told tl1 ?tnew)) (and (not (available-timber ?p ?told)) (available-timber ?p ?tnew))))
         (forall (?told - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?p ?told) (del-atleast-timber ?told tl1 ?al)) (not (available-atleast-timber ?p ?al))))

          (is-at ?v ?p)
     (is-cart ?v)
     (not (potential ?v))
     (space-in ?v spl1)
          (has-space ?v)
     (increase (labour) 1))
    )


    ; iron [place] -= 2
    (:action build-train
     :parameters (?p - place ?v - vehicle)
     :precondition (and (potential ?v) (available-atleast-iron ?p il2))
     :effect (and
          (forall (?iold ?inew - iron_level) (when (and (available-iron ?p ?iold) (diff-iron ?iold il2 ?inew)) (and (not (available-iron ?p ?iold)) (available-iron ?p ?inew))))
     (forall (?iold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?p ?iold) (del-atleast-iron ?iold il2 ?al)) (not (available-atleast-iron ?p ?al))))

          (is-at ?v ?p)
     (is-train ?v)
     (not (potential ?v))
     (space-in ?v spl5)
         (has-space ?v)
     (increase (labour) 2))
    )

    ; iron [place] -= 4
    (:action build-ship
     :parameters (?p - place ?v - vehicle)
     :precondition (and (has-wharf ?p) (potential ?v) (available-atleast-iron ?p il4))
     :effect (and
          (forall (?iold ?inew - iron_level) (when (and (available-iron ?p ?iold) (diff-iron ?iold il4 ?inew)) (and (not (available-iron ?p ?iold)) (available-iron ?p ?inew))))
          (forall (?iold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?p ?iold) (del-atleast-iron ?iold il4 ?al)) (not (available-atleast-iron ?p ?al))))

          (is-at ?v ?p)
     (is-ship ?v)
     (not (potential ?v))
     (space-in ?v spl10)
     (has-space ?v)
     (increase (labour) 3))
    )

    ;; C.1: Refining resources.

    ; timber [place] -= 1
    ; coal [place] += 1
    (:action burn-coal
     :parameters (?p - place)
     :precondition (and (has-coal-stack ?p) (available-atleast-timber ?p tl1))
     :effect (and
     (forall (?told ?tnew - timber_level) (when (and (available-timber ?p ?told) (diff-timber ?told tl1 ?tnew)) (and (not (available-timber ?p ?told)) (available-timber ?p ?tnew))))
     (forall (?told - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?p ?told) (del-atleast-timber ?told tl1 ?al)) (not (available-atleast-timber ?p ?al))))

     (forall (?cold ?cnew - coal_level) (when (and (available-coal ?p ?cold) (diff-coal ?cnew cl1 ?cold)) (and (not (available-coal ?p ?cold)) (available-coal ?p ?cnew))))
     (forall (?cold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?p ?cold) (add-atleast-coal ?cold cl1 ?al)) (available-atleast-coal ?p ?al)))

     (increase (pollution) 1)) 
    )

    ; timber [place] -= 1
    ; wood [place] += 1
    (:action saw-wood
     :parameters (?p - place)
     :precondition (and (has-sawmill ?p) (available-atleast-timber ?p tl1))
     :effect (and
     (forall (?told ?tnew - timber_level) (when (and (available-timber ?p ?told) (diff-timber ?told tl1 ?tnew)) (and (not (available-timber ?p ?told)) (available-timber ?p ?tnew))))
     (forall (?told - timber_level ?al - timber_atleastlevel) (when (and (available-timber ?p ?told) (del-atleast-timber ?told tl1 ?al)) (not (available-atleast-timber ?p ?al))))

     (forall (?wold ?wnew - wood_level) (when (and (available-wood ?p ?wold) (diff-wood ?wnew wl1 ?wold)) (and (not (available-wood ?p ?wold)) (available-wood ?p ?wnew))))
     (forall (?wold - wood_level ?al - wood_atleastlevel) (when (and (available-wood ?p ?wold) (add-atleast-wood ?wold wl1 ?al)) (available-atleast-wood ?p ?al)))

    ))

    ; coal [place] -= 2
    ; ore [place] -= 1
    ; iron [place] += 1
    (:action make-iron
     :parameters (?p - place)
     :precondition (and (has-ironworks ?p) (available-atleast-coal ?p cl2) (available-atleast-ore ?p ol1))
     :effect (and
     (forall (?cold ?cnew - coal_level) (when (and (available-coal ?p ?cold) (diff-coal ?cold cl2 ?cnew)) (and (not (available-coal ?p ?cold)) (available-coal ?p ?cnew))))
     (forall (?cold - coal_level ?al - coal_atleastlevel) (when (and (available-coal ?p ?cold) (del-atleast-coal ?cold cl2 ?al)) (not (available-atleast-coal ?p ?al))))

     (forall (?oold ?onew - ore_level) (when (and (available-ore ?p ?oold) (diff-ore ?oold ol1 ?onew)) (and (not (available-ore ?p ?oold)) (available-ore ?p ?onew))))
     (forall (?oold - ore_level ?al - ore_atleastlevel) (when (and (available-ore ?p ?oold) (del-atleast-ore ?oold ol1 ?al)) (not (available-atleast-ore ?p ?al))))

     (forall (?iold ?inew - iron_level) (when (and (available-iron ?p ?iold) (diff-iron ?inew il1 ?iold)) (and (not (available-iron ?p ?iold)) (available-iron ?p ?inew))))
     (forall (?iold - iron_level ?al - iron_atleastlevel) (when (and (available-iron ?p ?iold) (add-atleast-iron ?iold il1 ?al)) (available-atleast-iron ?p ?al)))

     (increase (pollution) 2)))
)
