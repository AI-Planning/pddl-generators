(ns tidybot.core
  (:require [random-seed.core :refer :all])
  (:refer-clojure :exclude [rand rand-int rand-nth])
  (:use clojure.math.combinatorics)
  (:gen-class))


(defn symbol-cat [& args] (symbol (apply str args)))

(defn xc [x] (symbol-cat 'x x))
(defn yc [x] (symbol-cat 'y x))
(defn xrel [x] (symbol-cat 'xrel x))
(defn yrel [x] (symbol-cat 'yrel x))

(def sep "\n ")
(def sep2 "\n  ")




(defn clamp [x n]
  (max 0 (min (- n 1) x)))

(defn sample-table-side [[min max]]
  (+ min (rand-int (- (+ 1 max) min))))

(defn sample-box [world-size size-range type]
  (let [x-size (sample-table-side size-range)
        y-size (sample-table-side size-range)
        min-x (inc (rand-int (- world-size x-size 2)))
        min-y (inc (rand-int (- world-size y-size 2)))
        max-ind (- world-size 1)]
    [[min-x min-y]
     [(clamp (+ min-x x-size -1) world-size) (clamp (+ min-y y-size -1) world-size)]
     type]))

(defn too-close [[[xmin ymin] [xmax ymax]] [x y]]
  (and (<= (- xmin 1) x (+ xmax 1))
       (<= (- ymin 1) y (+ ymax 1))))

(defn collide-one-sided [[[xmin ymin] [xmax ymax]] t2]
  (some (partial too-close t2) (cartesian-product [xmin xmax] [ymin ymax])))

(defn collide [t1 t2]
  (or (collide-one-sided t1 t2) (collide-one-sided t2 t1)))

(def orientations '(u d l r))

(defn place-next-surface [surfaces world-size range type]
  (loop [s (sample-box world-size range type) tries 100]
    (if (some (partial collide s) surfaces)
      (if (> tries 0)
        (recur (sample-box world-size range type) (dec tries))
        surfaces)
      (cons s surfaces))))

(defn generate-surfaces
  ([world-size num-surfaces size-range cupboard-size-range cupboard-prob]
     (generate-surfaces world-size num-surfaces size-range cupboard-size-range cupboard-prob []))
  ([world-size num-surfaces size-range cupboard-size-range cupboard-prob init-surfaces]
     (loop [n num-surfaces
            surfaces init-surfaces]
       (if (= n 0)
         surfaces
         (recur (- n 1)
                (if (< (rand) cupboard-prob)
                  (place-next-surface surfaces world-size cupboard-size-range (rand-nth orientations))
                  (place-next-surface surfaces world-size size-range 'table))
                )))))

(defn get-cells [[min-x min-y] [max-x max-y]]
  (cartesian-product (range min-x (+ 1 max-x)) (range min-y (+ 1 max-y))))

(defn inside [[min-x min-y] [max-x max-y] [x y]]
  (and (< min-x x max-x) (< min-y y max-y)))

(defn possible-object-locations [[min max type]]
  (if (= type 'table)
    (filter #(not (inside min max %1)) (get-cells min max))
    (filter (partial inside min max) (get-cells min max))
    ))

(defn is-table [[min max type]]
  (= type 'table))

(defn sample-subset
  ([s n] (sample-subset s n []))
  ([s n curr] (if (> n 0)
                (let [x (rand-nth s)]
                  (recur (remove #(= %1 x) s) (- n 1) (cons x curr)))
                curr)))

(defn other-goals [l n]
  (take n (distinct (repeatedly #(rand-nth l)))))

(defn place-objects [surfaces]
  (let [locs (mapcat possible-object-locations surfaces)
        tables (filter is-table surfaces)
        cupboards (filter #(not (is-table %1)) surfaces)
        table-locs (mapcat possible-object-locations tables)
        cupboard-locs (mapcat possible-object-locations cupboards)
        init-locs (take (count cupboard-locs) (distinct (repeatedly #(rand-nth locs))))]
    (map (fn [g l] [l (cons g (other-goals table-locs (rand-int 2)))]) cupboard-locs init-locs)))
                         


(defn cupboard-obstacles [[[minx miny] [maxx maxy] type]]
  (for [x (range minx (inc maxx))
        y (range miny (inc maxy))
        :when
        (and (or (= x minx) (= x maxx) (= y miny) (= y maxy))
             (not (cond (= type 'u) (and (= y miny) (< minx x maxx))
                        (= type 'd) (and (= y maxy) (< minx x maxx))
                        (= type 'l) (and (= x minx) (< miny y maxy))
                        (= type 'r) (and (= x maxx) (< miny y maxy)))))]
    [x y]))

(defn make-tidybot-instance
  [name filename
   [width height] ; positions are 0 to width-1, etc.
   robots         ;[robot-name [init-x init-y]] pairs
   carts          ;[cart-name [init-x init-y]]
   tables       ;[[minx miny] [maxx maxy]] pairs
   cupboards      ;[[minx miny] [maxx maxy] dir] where dir is one of 'u, 'l, 'r, 'd
   objects        ;[object-name [init-x init-y] [[gx1 gy1] [gx2 gy2] ...]]
   gripper-radius]
;  (println width height robots carts surfaces cupboards (map cupboard-obstacles cupboards) objects )
  (->> 
        `(~'define ~sep
           (~'problem ~name) ~sep 
           (:domain ~'TIDYBOT) ~sep ~sep

           (:objects ~sep2
            ~@(for [[name] robots]  (str name " - robot " sep2))
            ~@(for [[name] carts] (str name " - cart " sep2))
            ~@(for [[name] objects] (str name " - object " sep2))
            ~@(for [x (range width)] (str (xc x) " - xc " sep2))             
            ~@(for [y (range height)] (str (yc y) " - yc " sep2))             
            ~@(for [x (range (- gripper-radius) (inc gripper-radius))]
                (str (xrel x) " - xrel " sep2))
            ~@(for [y (range (- gripper-radius) (inc gripper-radius))]
                (str (yrel y) " - yrel " sep2))            
            ) ~sep ~sep

           (:init ~sep2
            ;; Constants
            ~@(for [x (range (dec width))]
                (str (list  'leftof (xc x) (xc (inc x))) sep2))
            ~@(for [y (range (dec height))]
                (str (list 'above (yc y) (yc (inc y))) sep2))            
            ~@(for [x (range (- gripper-radius) gripper-radius)]
                (str (list 'leftof-rel (xrel x) (xrel (inc x))) sep2))
            ~@(for [y (range (- gripper-radius) gripper-radius)]
                (str (list 'above-rel (yrel y) (yrel (inc y))) sep2))            
            ~@(for [x (range width)
                    xr (range (- gripper-radius) (inc gripper-radius))
                    :let [sum (+ x xr)]
                    :when (<= 0 sum (dec width))]
                (str (list 'sum-x (xc x) (xrel xr) (xc sum)) sep2))
            ~@(for [y (range height)
                    yr (range (- gripper-radius) (inc gripper-radius))
                    :let [sum (+ y yr)]
                    :when (<= 0 sum (dec height))]
                (str (list 'sum-y (yc y) (yrel yr) (yc sum)) sep2))            
            ~(list 'zerox-rel (xrel 0)) ~sep2
            ~(list 'zeroy-rel (yrel 0)) ~sep2
            ~@(for [[name _ goals] objects
                    [gx gy]        goals]
                (str (list 'object-goal name (xc gx) (yc gy)) sep2))
            ~sep2
            
            ;; Robot base
            ~@(for [[name] robots] (str (list 'parked name) sep2))
            ~@(for [[name] robots] (str (list 'not-pushing name) sep2))
            ~@(for [[name [x y]] robots] (str (list 'base-pos name (xc x) (yc y)) sep2))
            ~@(for [[name [x y]] robots] (str (list 'base-obstacle (xc x) (yc y)) sep2))
            ~@(for [[[minx miny] [maxx maxy]] tables
                    x (range minx (inc maxx))
                    y (range miny (inc maxy))]
                (str (list 'base-obstacle (xc x) (yc y))
                     (list 'surface (xc x) (yc y)) sep2))
            ~@(for [c cupboards
                    [x y] (cupboard-obstacles c)]
                (str (list 'base-obstacle (xc x) (yc y))
                     (list 'gripper-obstacle (xc x) (yc y))
                     sep2))
            ~@(for [c cupboards
                    [x y] (possible-object-locations c)]
                (str (list 'surface (xc x) (yc y)) sep2))
            ~sep2

            ;; Cart
            ~@(for [[name [x y]] carts] (str (list 'cart-pos name (xc x) (yc y)) sep2))
            ~@(for [[name] carts] (str (list 'not-pushed name) sep2))
            ~@(for [[name [x y]] carts] (str (list 'base-obstacle (xc x) (yc y)) sep2))
            ~sep2

            ;; Objects
            ~@(for [[name [x y]] objects] (str (list 'object-pos name (xc x) (yc y)) sep2))
            ~sep2

            ;; Gripper
            ~@(for [[name] robots] (str (list 'gripper-empty name) sep2))
            ~@(for [[name] robots] (str (list 'gripper-rel name (xrel 0) (yrel 0)) sep2))
            ~@(for [[_ [x y]] objects] (str (list 'gripper-obstacle (xc x) (yc y)) sep2))
           ) ~sep ~sep

             (:goal ~sep2
                    ("and" ~sep2
                      ~@(for [[name _ goals] objects
                             :when (seq goals)]
                         (str `(~'object-done ~name) sep2)))
            ))
        print-str
        println))

(defn bottle-symbol [i]
  (symbol (format "bottle%d" i)))

(defn btl [i j]
  [(bottle-symbol j) [(+ 1 j) 1] [[(+ 1 j) (- i 2)] [(+ 2 j) (- i 2)]]])

(defn bottles [i k]
  (map (partial btl i) (range 0 k)))

;(def filename "/Users/jawolfe/projects/tidybot/test.pddl")
(def filename "/u/bhaskara/code/tidybot/test.pddl")

(defn make-multi-goal-instance [i k]
  "Make an ixi instance with k bottles"
  (tidybot.core/make-tidybot-instance
   'test "/home/bhaskara/ipc/tidybot/test.pddl"
   [i i] [['pr2 [0 0]]]
   [[[1 1] [(- i 2) (- i 2)]]]
   (bottles i k)
   1))

(defn right-table [i k] [[1 k] [(- i 1) k]])

(defn left-table [i k] [[0 k] [(- i 2) k]])
 
(defn left-tables [i] (map (partial left-table i) (range 1 (- i 1) 4)))

(defn right-tables [i] (map (partial right-table i) (range 3 (- i 1) 4)))

(defn goal [i] [(- i 2) (last (range 3 (- i 1) 4))])

(defn make-maze-instance [i]
  "Make a maze-like instance in an ixi map"
  (tidybot.core/make-tidybot-instance
   'test filename
   [i i] [['pr2 [0 0]]]
   (concat (left-tables i) (right-tables i))
   [['bottle [1 1] [(goal i)]]] 1))

(def x 42)


(defn make-test-instance []
  (make-tidybot-instance
   'test filename
    [7 7] [['prf [0 0]] ['prg [0 1]]]
   [['cart1 [0 3]]] 
   [[[2 2] [4 4]]]
   [['b1 [2 2] [[2 4]]]
    ['b2 [2 3] [[3 4]]]
    ['b3 [3 2] [[4 3]]]
    ['b4 [3 3] [[4 4]]]]
   1))

(defn make-small-test-instance []
  (make-tidybot-instance
   'test filename
    [3 5] [['prf [1 0]]]
   [['cart1 [1 1]]] 
   [[[2 0] [2 4]]]
   [['b1 [2 0] [[2 3]]]
    ['b2 [2 1] [[2 4]]]]
   1))

(defn make-tiny-test-instance []
  (make-tidybot-instance
   'test filename
    [3 4] [['prf [1 0]]]
   [['cart1 [1 1]]] 
   [[[2 0] [2 3]]]
   [['b1 [2 0] [[2 2]]]
    ['b2 [2 1] [[2 3]]]]
   1))


(defn number-objects [v]
  (for [i (range (count v))]
    (cons (str "object" i) (nth v i))))
  

;; (use 'tidybot 'edu.berkeley.ai.envs.strips 'edu.berkeley.ai.search 'edu.berkeley.ai.search.state-space 'edu.berkeley.ai.search.algorithms.textbook)
;; (make-small-test-instance)
;; (def e (read-strips-planning-instance (read-strips-planning-domain "/Users/jawolfe/projects/tidybot/tidybot.pddl") filename))
;; (def e (constant-predicate-simplify (read-strips-planning-instance (read-strips-planning-domain "/Users/jawolfe/projects/tidybot/tidybot.pddl") "/Users/jawolfe/projects/tidybot/1.pddl")))
;; (interactive-search (ss-node e))

(defn print-map [s tables cupboards]
  (let [obstacles (set (concat
                        (apply concat (map (fn [[mn mx]] (get-cells mn mx)) tables))
                        (apply concat (map cupboard-obstacles cupboards))))]
    (doseq [y (range s)]
      (doseq [x (range s)]
        (print (if (obstacles [x y]) 'X '.)))
      (println))))


(defn generate-world [filename world-size num-tables num-cupboards size-range cupboard-size random-seed]
  (set-random-seed! random-seed)
  (let [cupboards (generate-surfaces world-size num-cupboards size-range [cupboard-size cupboard-size] 1.0)
        surfaces (generate-surfaces world-size num-tables size-range [cupboard-size cupboard-size] 0.0 cupboards)
        objects (number-objects (place-objects surfaces))
        tables (filter is-table surfaces)]
;    (println surfaces)
;    (println objects)
#_    (print-map world-size tables cupboards)
    (make-tidybot-instance 'test filename
                           [world-size world-size] [['pr2 [0 0]]] [['cart [0 1]]]
                           (map #(take 2 %1) tables)
                           cupboards
                           objects
                           1)))

(defn -main [& args]
  ;; (println (count args))
  (if (not (<= 6 (count args) 7))
    (println "Usage is java -jar tidybot.jar world-size n-tables n-cupboards min-surface-size max-surface-size cupboard-size [random-seed]")
    (if (= (count args) 6)
      (let [[world-size n-tables n-cupboards min-surface-size max-surface-size cupboard-size] (map read-string args)]
        (generate-world :none world-size n-tables n-cupboards [min-surface-size max-surface-size] cupboard-size (rand-int Integer/MAX_VALUE)))
      (let [[world-size n-tables n-cupboards min-surface-size max-surface-size cupboard-size random-seed] (map read-string args)]
        (generate-world :none world-size n-tables n-cupboards [min-surface-size max-surface-size] cupboard-size random-seed))
    )
  )
)
