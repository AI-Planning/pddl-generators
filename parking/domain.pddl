(define (domain parking)
 (:requirements :strips :typing :action-costs)
 (:types car curb)
 (:predicates
    (at-curb ?car - car)
    (at-curb-num ?car - car ?curb - curb)
    (behind-car ?car ?front-car - car)
    (car-clear ?car - car)
    (curb-clear ?curb - curb)
 )

(:functions (total-cost) - number)

	(:action move-curb-to-curb
		:parameters (?car - car ?curbsrc ?curbdest - curb)
		:precondition (and
			(car-clear ?car)
			(curb-clear ?curbdest)
			(at-curb-num ?car ?curbsrc)
		)
		:effect (and
			(not (curb-clear ?curbdest))
			(curb-clear ?curbsrc)
			(at-curb-num ?car ?curbdest)
			(not (at-curb-num ?car ?curbsrc))
                        (increase (total-cost) 1)
		)
	)

	(:action move-curb-to-car
		:parameters (?car - car ?curbsrc - curb ?cardest - car)
		:precondition (and
			(car-clear ?car)
			(car-clear ?cardest)
			(at-curb-num ?car ?curbsrc)
			(at-curb ?cardest)
		)
		:effect (and
			(not (car-clear ?cardest))
			(curb-clear ?curbsrc)
			(behind-car ?car ?cardest)
			(not (at-curb-num ?car ?curbsrc))
			(not (at-curb ?car))
                        (increase (total-cost) 1)
		)
	)

	(:action move-car-to-curb
		:parameters (?car - car ?carsrc - car ?curbdest - curb)
		:precondition (and
			(car-clear ?car)
			(curb-clear ?curbdest)
			(behind-car ?car ?carsrc)
		)
		:effect (and
			(not (curb-clear ?curbdest))
			(car-clear ?carsrc)
			(at-curb-num ?car ?curbdest)
			(not (behind-car ?car ?carsrc))
			(at-curb ?car)
                        (increase (total-cost) 1)
		)
	)

	(:action move-car-to-car
		:parameters (?car - car ?carsrc - car ?cardest - car)
		:precondition (and
			(car-clear ?car)
			(car-clear ?cardest)
			(behind-car ?car ?carsrc)
			(at-curb ?cardest)
		)
		:effect (and
			(not (car-clear ?cardest))
			(car-clear ?carsrc)
			(behind-car ?car ?cardest)
			(not (behind-car ?car ?carsrc))
                        (increase (total-cost) 1)
		)
	)
)
