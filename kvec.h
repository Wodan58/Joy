/* The MIT License
   Copyright (c) 2008, by Attractive Chaos <attractor@live.co.uk>
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/*
  An example:

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if 1
#include <gc.h>
#else
#define GC_malloc(x)	calloc(1, (x))
#define GC_realloc	realloc
#define GC_free		free
#endif
#include "kvec.h"

int main()
{
    int i, j;
    vector(int) *array;		// declaration

    vec_init(array);		// initialization
    vec_push(array, 1);		// append
    vec_add(array, 2, 20);	// dynamic
    vec_at(array, 15) = 3;	// static
    for (i = 0, j = vec_size(array); i < j; i++)		// size
	printf("array[%d] = %d\n", i, vec_at(array, i));	// access
    vec_destroy(array);		// destructor
    return 0;
}
*/

/*
    module  : kvec.h
    version : 1.14
    date    : 09/20/24

 1. Change type of n, m from size_t to unsigned. Reason: takes less memory.
 2. Remove (type*) casts. Reason: not needed for C.
 3. Replace sizeof(type) with sizeof(*a). Reason: no need to specify type.
 4. Remove kv_a. Reason: compiler complains about lvalue required.
 5. Rename kv_A to kv_at. Reason: looks like a better name.
 6. Remove roundup32. Reason: not used anymore.
 7. Change initial value of m to 1 instead of 2. Reason: takes less memory.
 8. Change vector type to pointer. Reason: more convenient to pass around.
 9. Remove protection against multiple inclusion. Reason: it must be possible
    to have vectors of different types within the same source file. Undone.
10. Remove stdlib.h. Reason: it should be possible to have different memory
    allocaters for different vectors.
11. Protect kv_size against NULL parameter. Reason: more convenient interface.
12. Change type from kvec_t to vector. Reason: _t is in the compiler namespace.
13. Change memory allocator to mem_malloc and mem_realloc. Reason: this allows
    those vectors to be garbage collected.
14. Add macros stk_init and stk_push. Reason: they allow checks on NULL pointer
    and statistics.
15. Change prefix kv_ into vec_. Reason: easier when changing existing code.
16. Add macro vec_setsize. Reason: vec_size cannot be used for this purpose.
17. Add vec_back macro. Reason: sometimes a value needs to be read, not popped.
18. Macro vec_pushp is not used. Maybe it should be removed? Done.
19. Add automatic initialization in vec_push. Reason: more convenient interface.
20. Add macro vec_equal. Reason: part of the requirements of the vector type.
21. Add macro vec_reverse. Reason: more convenient than as function.
22. Add automatic initialization in vec_copy. Reason: more convenient interface.
23. Rename vec_resize to vec_grow. Reason: vector can only grow.
24. Add vec_shrink macro. Reason: reduce memory footprint of large vectors.
25. Add vec_end macro. Reason: can be used as stack pointer. Removed.
26. Use GC_malloc and GC_realloc. Reason: simpler interface.
27. Remove stk_ macros. Reason: Two different memory allocators is confusing.
28. MAX_BLOCK added. Reason: 29 bits is the maximum size in gc.c. Removed.
29. vec_push requires vec_init; vec_add added. Reason: a little bit faster.
30. vec_copy_cons and vec_copy_rest added, allowing insert/delete at the front.
31. vec_arity added: useful for quotations that need save/restore of the stack.
32. vec_concat added: useful when building a vector while reading its factors.
33. vec_copy_reverse added: useful in the stack/unstack operators.
34. vec_shallow_copy and vec_shallow_copy_take_ownership added: faster.
35. reverse-bit recycled as recurse-bit. Some comments have been added.
36. vec_copy_all added. This is nicer than always using vec_copy_count.

  2008-09-22 (0.1.0):
	* The initial version.
*/
#ifndef AC_KVEC_H
#define AC_KVEC_H

typedef enum arity_t {
    ARITY_UNKNOWN,	/* 0 */
    ARITY_NOT_OK,	/* 1 */
    ARITY_OK		/* 2 */
} arity_t;

typedef enum owner_t {
    NOT_OWNER,		/* 0 */
    OWNER,		/* 1 */
} owner_t;

/*
 * m = capacity; n = current fill.
 *
 * a = 0 -> ARITY_UNKNOWN
 * a = 1 -> ARITY_KNOWN
 * e = 0 -> ARITY_NOT_OK
 * e = 1 -> ARITY_OK
 *
 * A vector can be kept in reverse order: the element at index n-1 is then
 * considered the first element and the element at index 0 is then the last
 * element. A vector need not be in reverse order: the developer can decide.
 *
 * A NULL pointer is a valid vector when given to vec_size or vec_max.
 * GC_malloc and GC_realloc also clear the (extra) allocated area.
 */
#define vector(type)		struct { unsigned a : 1, r : 1, m : 30; \
					 unsigned e : 1, o : 1, n : 30; \
				type *c; }
#define vec_init(v)		do { (v) = GC_malloc(sizeof(*(v)));	\
				(v)->a = (v)->e = (v)->r = (v)->m =	\
				(v)->n = 0; (v)->o = NOT_OWNER;		\
				(v)->c = 0; } while (0)
#define vec_destroy(v)		do { GC_free((v)->c); GC_free(v); } while (0)
#define vec_at(v, i)		((v)->c[i])
#define vec_pop(v)		((v)->c[--(v)->n])
#define vec_back(v)		((v)->c[(v)->n - 1])
#define vec_max(v)		((v) ? (v)->m : 0)
#define vec_size(v)		((v) ? (v)->n : 0)
#define vec_setsize(v, s)	((v)->n = (s))
#define vec_getarity(v)		((v)->a + (v)->e)
#define vec_setarity(v, s)      do { (v)->a = 1; (v)->e = (s) - 1; } while (0)
#define vec_getrecur(v)		((v)->r)
#define vec_setrecur(v, s)      ((v)->r = (s))
#define vec_grow(v, s)		do { (v)->m = (s); (v)->c = GC_realloc((v)->c,\
				sizeof(*(v)->c) * (s)); } while (0)
#define vec_shrink(v)		do { if ((v)->n) { (v)->m = (v)->n; (v)->c =  \
				GC_realloc((v)->c, sizeof(*(v)->c) * (v)->m); \
				} } while (0)
#define vec_equal(v, w)		((v)->n == (w)->n && !memcmp((v)->c, (w)->c,  \
				sizeof(*(v)) * (v)->n))

/* vec_push assumes that v has been initialized before it being called */
#define vec_push(v, x) 							\
	do {								\
	    if ((v)->n == (v)->m) { (v)->m = (v)->m ? round((v)->m *	\
	    1.618) : 2; (v)->c = GC_realloc((v)->c, sizeof(*(v)->c) *	\
	    (v)->m); } (v)->c[(v)->n++] = (x);				\
	} while (0)

/* vec_add adds an element at index, even when the index did not exist */
#define vec_add(v, x, i) 						\
	do {								\
	    if ((v)->m <= (unsigned)(i)) { (v)->m = (v)->n = (i) + 1;	\
	    (v)->c = GC_realloc((v)->c, sizeof(*(v)->c) * (v)->m); }	\
	    else if ((v)->n <= (unsigned)(i)) (v)->n = (i) + 1;		\
	    (v)->c[i] = (x);						\
	} while (0)

/* vec_reverse assumes that an extra element has been added as scratch */
#define vec_reverse(v)							\
	do {								\
	    int i, j, k = vec_size(v) - 1;				\
	    for (i = 0, j = k - 1; i < j; i++, j--) {			\
		vec_at((v), k) = vec_at((v), i);			\
		vec_at((v), i) = vec_at((v), j);			\
		vec_at((v), j) = vec_at((v), k);			\
	    } vec_setsize((v), k);					\
	} while (0)

/* vec_copy_reverse copies w to v while reversing the order of members */
#define vec_copy_reverse(v, w)						\
	do { int i;							\
	    for (i = vec_size(w) - 1; i >= 0; i--)			\
		vec_push((v), vec_at((w), i));				\
	} while (0)
#endif

/*
 * The functions that follow all initialize the target first, before doing the
 * copying. The shallow_copy function and the ownership bit make it possible to
 * add a new first element at the end without copying the whole vector.
 * Adding an element at index 0 always requires such copying.
 */

/* vec_copy_count creates v, then copies first x elements from w to v. */
#define vec_copy_count(v, w, x)						\
	do {								\
	    vec_init(v); (v)->m = (v)->n = (x); if ((v)->m) { (v)->c =	\
	    GC_malloc(sizeof(*(v)->c) * (v)->m); memcpy((v)->c, (w)->c,	\
	    sizeof(*(v)->c) * (v)->m); }				\
	} while (0)

/* vec_copy_all creates v, then copies all elements from w to vector v */
#define vec_copy_all(v, w)						\
	vec_copy_count((v), (w), (w)->n)				\

/* vec_copy_cons copies w to v, preceded by an extra initial element x */
#define vec_copy_cons(v, w, x) 						\
	do {								\
	    vec_init(v); (v)->m = (v)->n = (w)->n + 1; (v)->c =		\
	    GC_malloc(sizeof(*(v)->c) * (v)->m); (v)->c[0] = (x);	\
	    if ((w)->n) memcpy(&(v)->c[1], (w)->c, sizeof(*(v)->c) *	\
	    (w)->n);							\
	} while (0)

/* vec_copy_rest copies w to v, leaving out the first x elements of w. */
#define vec_copy_rest(v, w, x) 						\
	do {								\
	    vec_init(v); (v)->m = (v)->n = (w)->n - (x); if ((v)->m) {	\
	    (v)->c = GC_malloc(sizeof(*(v)->c) * (v)->m); memcpy((v)->c,\
	    &(w)->c[(x)], sizeof(*(v)->c) * (v)->m); }			\
	} while (0)

/* vec_concat copies the contents of vectors v and w to a new vector u */
#define vec_concat(u, v, w)						\
	do {								\
	    vec_init(u); (u)->m = (u)->n = (v)->n + (w)->n;		\
	    (u)->c = GC_malloc(sizeof(*(u)->c) * (u)->m); if ((v)->n)	\
	    memcpy((u)->c, (v)->c, sizeof(*(u)->c) * (v)->n);		\
	    if ((w)->n) memcpy((u)->c + (v)->n, (w)->c, sizeof(*(u)->c) \
	    * (w)->n);							\
	} while (0)

/* vec_shallow_copy makes a copy without taking ownership of the array */
#define vec_shallow_copy(v, w)						\
	do {								\
	    vec_init(v); (v)->a = (w)->a; (v)->e = (w)->e;		\
	    (v)->r = (w)->r; (v)->m = (w)->m; (v)->n = (w)->n;		\
	    (v)->c = (w)->c;						\
	} while (0)

/* vec_shallow_copy_take_ownership makes a copy while taking ownership */
#define vec_shallow_copy_take_ownership(v, w)				\
	do {								\
	    if ((w)->o == OWNER) { vec_shallow_copy(v, w);		\
		(w)->o = NOT_OWNER; (v)->o = OWNER; }			\
	    else vec_copy_all(v, w);					\
	} while (0)
