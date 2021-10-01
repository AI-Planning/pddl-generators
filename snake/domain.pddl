; Snake is not a game where everything is known a priori. 
; Therefore we make two modifications: 
; 1. The player knows where points will spawn. 
; 2. Points can spawn inside the snake, but they can only be collected by the head of the snake.
; 3. There is a constant number of points at a time and no two points can spawn in the same location
; Based on the submission

(define (domain snake)
	(:requirements :strips :negative-preconditions)

	(:constants dummyPoint)

	(:predicates 
		(isAdjacent ?x ?y) ;up down left right of a field
		(tailSnake ?x) ;the last field of the snake
		(headSnake ?x) ;the first field of the snake
                (nextSnake ?x ?y) ;pieces of the snake that are connected. from front to back
		(blocked ?x) ;a field that is occupied by the snake or by an obstacle
		(spawn ?x) ;next point that will spawn
		(nextSpawn ?x ?y) ;point y will spawn after point x
		(isPoint ?x) ;a field that has a point that can be collected by the snake
	)

	(:action move
		:parameters (?head ?newHead ?tail ?newTail)
		:precondition (and
			(headSnake ?head)
			(isAdjacent ?head ?newHead)
         		(tailSnake ?tail)
			(nextSnake ?newTail ?tail)
			(not (blocked ?newHead))
			(not (isPoint ?newHead))
		)
        	:effect (and
			(blocked ?newHead)
			(headSnake ?newHead)
			(nextSnake ?newHead ?head)
			(not (headSnake ?head))
			(not (blocked ?tail))
			(not (tailSnake ?tail))
			(not (nextSnake ?newTail ?tail))
			(tailSnake ?newTail)
			)
	)

	(:action move-and-eat-spawn
		:parameters  (?head ?newHead ?spawnPoint ?nextSpawnPoint)
		:precondition (and
			      (headSnake ?head)
			      (isAdjacent ?head ?newHead)
			      (not (blocked ?newHead))
			      (isPoint ?newHead)
			      (spawn ?spawnPoint)
			      (nextSpawn ?spawnPoint ?nextSpawnPoint)
			      (not (= ?spawnPoint dummyPoint))
		)
        	:effect (and
			(blocked ?newHead)
			(headSnake ?newHead)
			(nextSnake ?newHead ?head)
			(not (headSnake ?head))
			(not (isPoint ?newHead))
			(isPoint ?spawnPoint)
			(not (spawn ?spawnPoint))
			(spawn ?nextSpawnPoint)
			)
	)

	(:action move-and-eat-no-spawn
		:parameters  (?head ?newHead)
		:precondition (and
			(headSnake ?head)
			(isAdjacent ?head ?newHead)
			(not (blocked ?newHead))
			(isPoint ?newHead)
			(spawn dummyPoint)
		)
        	:effect (and
			(blocked ?newHead)
			(headSnake ?newHead)
			(nextSnake ?newHead ?head)
			(not (headSnake ?head))
			(not (isPoint ?newHead))
			)
	)
)