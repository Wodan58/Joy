(* FILE:  lsptst.lsp *)

(* Lisp tests,
   to be used with the Lisp interpreter in the library lsplib.joy *)

						(* DEFINITIONS		*)
	[ DEF  NIL  [QUOTE []] ]
	NIL
	[ ATOM NIL ]
	[ NULL NIL ]

	[ DEF  PI  3.14 ]
	[ * PI 100.1 ]

	[ DEF  NAMELIST
	  [QUOTE [Peter Paul Mary]] ]
	[ CDR NAMELIST ]

	[ DEF  PAIR
	  [ LAMBDA [x y] CONS x [CONS y NIL]] ]
	[ PAIR 11 22]

	[ DEF  SECOND
	  [ LAMBDA [lis] [CAR [CDR lis]] ] ]
	[ SECOND  [QUOTE [11 22 33 44]] ]

	[ DEF  PRIME
	  [ LAMBDA [n] prime n ] ]
	[ PRIME 13 ]
	[ PRIME 14 ]

						(* LIBRARY COMBINATORS	*)
(*
	FOLDR
	FOLDL
*)
	[ FOLDR [QUOTE [a b c]] [QUOTE [d e]]
		[LAMBDA [x y] CONS x y] ]
	[ FOLDL [QUOTE [a b c]] [QUOTE [d e]]
		[LAMBDA [x y] CONS x y] ]

						(* DEFS USING FOLDR	*)
	[ DEFUN   CONCAT  [lis1 lis2]
	    FOLDR lis1 lis2 [LAMBDA [l r] CONS l r] ]
	[ CONCAT  [QUOTE [1 2]]  [QUOTE [3 4 5]] ]

	[ DEFUN   FLATTEN  [lis]
	    FOLDR lis NIL CONCAT ]
	[ FLATTEN  [ QUOTE [[1 2] [] [3]] ] ]

	[ DEFUN   MEMBER  [el lis]
	    FOLDR lis false [LAMBDA [x y] IF [EQ el x] true y] ]
	[ MEMBER  Bob  [QUOTE [Peter Paul Mary]] ]
	[ MEMBER  Bob  [QUOTE [Peter Bob Mary]] ]

	[ DEFUN   SUM  [lis]
	    FOLDR lis 0 [LAMBDA [x y] [+ x y]] ]
	[ SUM  [QUOTE [1 2 3 4]] ]

	[ DEFUN  LIST-TO-FLOAT  [lis]
	    FOLDR lis 0.0 [LAMBDA [x y] + x [/ y 10.0]] ]
	[ LIST-TO-FLOAT [QUOTE [3 1 4 1 5]] ]

	[ DEFUN   MAP  [lis fun]
	    FOLDR lis NIL [LAMBDA [l r] CONS [fun l] r] ]
	[ MAP  [QUOTE [1 2 3 4]]  [LAMBDA [n] * n n] ]
	[ MAP  [QUOTE [[John Smith] [Mary Robinson]]]  [LAMBDA [l] CAR l] ]
	[ MAP  [QUOTE [10 11 12 13 14 15 16]]  PRIME ]

	[ DEFUN   FILTER  [lis pred]
	    FOLDR lis NIL
		  [LAMBDA [l r] IF [pred l] [CONS l r] r] ]
	[ FILTER  [QUOTE [1 3 1 4 2]]  [LAMBDA [n] < n 3] ]
	[ FILTER  [QUOTE [1 [2 3] 4 [] [5] 6]] [LAMBDA [x] ATOM x] ]
	[ FILTER  [QUOTE [10 11 12 13 14 15 16]]  PRIME ]

						(* DEFS USING FOLDL	*)
	[ DEFUN   SHUNT  [l1 l2]
	    FOLDL l1 l2 [LAMBDA [l r] CONS l r] ]
	[ SHUNT  [QUOTE [1 2 3]]  [QUOTE [4 5 6 7]] ]

	[ DEFUN   REVERSE  [lis]  SHUNT lis NIL ]
	[ REVERSE  [QUOTE [Smith Jones Robinson]] ]

	[ DEFUN  LIST-TO-INT [lis]
	    FOLDL lis 0 [LAMBDA [x y] + x [* 10 y]] ]
	[ LIST-TO-INT [QUOTE [3 1 4 1 5]] ]

						(* DEFS USING FOLDR2	*)
	[ DEFUN  ZIP  [l1 l2]
	    FOLDR2 l1 l2 NIL [LAMBDA [x y z] CONS [PAIR x y] z] ]
	[ ZIP  NAMELIST  [QUOTE [Smith Jones Robinson]] ]

	[ DEFUN  WEAVE  [l1 l2]
	    FOLDR2 l1 l2 NIL [LAMBDA [x y z] CONS x [CONS y z]] ]
	[ WEAVE  NAMELIST  [QUOTE [Smith Jones Robinson]] ]

	[ DEFUN  SUM-OF-PRODUCTS [l1 l2]
	    FOLDR2 l1 l2 0 [LAMBDA [x y z] + [* x y] z] ]
	[ SUM-OF-PRODUCTS [QUOTE [1 2 3]] [QUOTE [1 10 100]] ]

	[ DEFUN  MAP2  [l1 l2 f]
	    FOLDR2 l1 l2 NIL [LAMBDA [x y z] CONS [f x y] z]  ]
	[ MAP2 [QUOTE [1 2 3]] [QUOTE [1 10 100]]
	       [LAMBDA [x y] * x y] ]

						(* DEFS USING RECFOLDR	*)
	[ DEFUN  RECFLATTEN [lis]
	    RECFOLDR lis NIL [LAMBDA [x l] CONS x l] ]
	[ RECFLATTEN [QUOTE [1  []  [[2 3] [] 4 [5]]  [[[[6]]]] ]] ]

	[ DEFUN  RECSUM  [lis]
	    RECFOLDR lis 0 [LAMBDA [x y] + x y] ]
	[ RECSUM [QUOTE [1  []  [[2 3] [] 4 [5]]  [[[[6]]]] ]] ]

			(* simulating recursion by Self-Application	*)

	[ DEFUN   FACT0  [f n]
	    IF [NULL n] 1 [* n [f f [pred n]]] ]
	[ DEFUN   FACT  [n] FACT0 FACT0 n ]
	[ FACT  5 ]

	[ DEFUN   LAST0  [f lis]
	    IF [NULL [CDR lis]]  [CAR lis]  [f f [CDR lis]] ]
	[ DEFUN   LAST  [lis] LAST0 LAST0 lis ]
	[ LAST  [QUOTE [11 22 33 44 55]] ]

(* END:  lsptst.lsp *)
