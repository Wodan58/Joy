(* FILE:  lsplib.lsp *)

(* Lisp library, with only definitions,
   to be used with the Lisp interpreter in the library lsplib.joy .
   The file lsptst.lsp contains the same definitions,
   but with some tests following each definition.			*)

	[ DEF  NIL  [QUOTE []] ]
	[ DEF  PI  3.14 ]
	[ DEF  NAMELIST
	  [QUOTE [Peter Paul Mary]] ]
	[ DEF  PAIR
	  [ LAMBDA [x y] CONS x [CONS y NIL]] ]
	[ DEF  SECOND
	  [ LAMBDA [lis] [CAR [CDR lis]] ] ]
	[ DEF  PRIME
	  [ LAMBDA [n] prime n ] ]
						(* DEFS USING FOLDR	*)
	[ DEFUN   CONCAT  [lis1 lis2]
	    FOLDR lis1 lis2 [LAMBDA [l r] CONS l r] ]
	[ DEFUN   FLATTEN  [lis]
	    FOLDR lis NIL CONCAT ]
	[ DEFUN   MEMBER  [el lis]
	    FOLDR lis false [LAMBDA [x y] IF [EQ el x] true y] ]
	[ DEFUN   SUM  [lis]
	    FOLDR lis 0 [LAMBDA [x y] [+ x y]] ]
	[ DEFUN  LIST-TO-FLOAT  [lis]
	    FOLDR lis 0.0 [LAMBDA [x y] + x [/ y 10.0]] ]
	[ DEFUN   MAP  [lis fun]
	    FOLDR lis NIL [LAMBDA [l r] CONS [fun l] r] ]
	[ DEFUN   FILTER  [lis pred]
	    FOLDR lis NIL
		  [LAMBDA [l r] IF [pred l] [CONS l r] r] ]

						(* DEFS USING FOLDL	*)
	[ DEFUN   SHUNT  [l1 l2]
	    FOLDL l1 l2 [LAMBDA [l r] CONS l r] ]
	[ DEFUN   REVERSE  [lis]  SHUNT lis NIL ]
	[ DEFUN  LIST-TO-INT [lis]
	    FOLDL lis 0 [LAMBDA [x y] + x [* 10 y]] ]

						(* DEFS USING FOLDR2	*)
	[ DEFUN  ZIP  [l1 l2]
	    FOLDR2 l1 l2 NIL [LAMBDA [x y z] CONS [PAIR x y] z] ]
	[ DEFUN  WEAVE  [l1 l2]
	    FOLDR2 l1 l2 NIL [LAMBDA [x y z] CONS x [CONS y z]] ]
	[ DEFUN  SUM-OF-PRODUCTS [l1 l2]
	    FOLDR2 l1 l2 0 [LAMBDA [x y z] + [* x y] z] ]
	[ DEFUN  MAP2  [l1 l2 f]
	    FOLDR2 l1 l2 NIL [LAMBDA [x y z] CONS [f x y] z]  ]
	[ DEFUN  RECFLATTEN [lis]
	    RECFOLDR lis NIL [LAMBDA [x l] CONS x l] ]
	[ DEFUN  RECSUM  [lis]
	    RECFOLDR lis 0 [LAMBDA [x y] + x y] ]

			(* simulating recursion by Self-Application	*)

	[ DEFUN   FACT0  [f n]
	    IF [NULL n] 1 [* n [f f [pred n]]] ]
	[ DEFUN   FACT  [n] FACT0 FACT0 n ]
	[ DEFUN   LAST0  [f lis]
	    IF [NULL [CDR lis]]  [CAR lis]  [f f [CDR lis]] ]
	[ DEFUN   LAST  [lis] LAST0 LAST0 lis ]

(* END:  lsplib.lsp *)
