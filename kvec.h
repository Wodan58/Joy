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

#include <stdlib.h>
#define GC_malloc	malloc
#define GC_realloc	realloc
#define GC_free		free
#include "kvec.h"

int main()
{
    int i, j;
    vector(int) *array;      // declaration

    vec_init(array);         // initialization
    vec_push(array, 10);     // append
    vec_add(array, 20) = 5;  // dynamic
    vec_at(array, 15) = 4;   // static
    for (i = 0, j = vec_size(array); i < j; i++) // size
	printf("array[%d] = %d\n", i, vec_at(array, i)); // access
    vec_destroy(array);      // destructor
    return 0;
}
*/

/*
    module  : kvec.h
    version : 1.10
    date    : 07/21/23

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
25. Add vec_end macro. Reason: can be used as stack pointer.
26. Use GC_malloc and GC_realloc. Reason: simpler interface.
27. Remove stk_ macros. Reason: Two different memory allocators is confusing.
28. MAX_BLOCK added. Reason: 29 bits is the maximum size in gc.c. Removed.
29. vec_push requires vec_init; vec_add added. Reason: a little bit faster.

  2008-09-22 (0.1.0):
	* The initial version.
*/
#ifndef AC_KVEC_H
#define AC_KVEC_H

#define vector(type)		struct { unsigned n, m; type *a; }
#define vec_init(v)		do { (v) = GC_malloc(sizeof(*(v))); \
				(v)->n = (v)->m = 0; (v)->a = 0; } while (0)
#define vec_destroy(v)		do { GC_free((v)->a); GC_free(v); } while (0)
#define vec_at(v, i)		((v)->a[i])
#define vec_pop(v)		((v)->a[--(v)->n])
#define vec_back(v)		((v)->a[(v)->n - 1])
#define vec_end(v)		((v)->a + (v)->n)
#define vec_size(v)		((v) ? (v)->n : 0)
#define vec_setsize(v, s)	((v)->n = (s))
#define vec_max(v)		((v)->m)
#define vec_grow(v, s)		((v)->m = (s), (v)->a = GC_realloc((v)->a, \
				sizeof(*(v)->a) * (s)))
#define vec_shrink(v)		do { if ((v)->n) { ((v)->m = (v)->n; (v)->a = \
				GC_realloc((v)->a, sizeof(*(v)->a) * (v)->m));\
				} } while (0)
#define vec_equal(v, w)		((v)->n == (w)->n && !memcmp((v)->a, (w)->a, \
				sizeof(*(v)) * (v)->n))

/* vec_copy assumes that v1 does not exist yet and needs to be created */
#define vec_copy(v1, v0) 						\
	do {								\
	    vec_init(v1); (v1)->n = (v1)->m = (v0)->n;			\
	    (v1)->a = GC_malloc(sizeof(*(v1)->a) * (v1)->m);		\
	    memcpy((v1)->a, (v0)->a, sizeof(*(v1)->a) * (v1)->m);	\
	} while (0)

/* vec_push assumes that v has been initialized before it being called */
#define vec_push(v, x) 							\
	do {								\
	    if ((v)->n == (v)->m) { (v)->m = (v)->m ? (v)->m << 1 : 2;  \
		(v)->a = GC_realloc((v)->a, sizeof(*(v)->a) * (v)->m);	\
	    } (v)->a[(v)->n++] = (x);					\
	} while (0)

/* vec_add adds an element at index, even when the index did not exist */
#define vec_add(v, x, i) 						\
	do {								\
	    if ((v)->m <= (unsigned)(i)) { (v)->m = (v)->n = (i) + 1;	\
	        (v)->a = GC_realloc((v)->a, sizeof(*(v)->a) * (v)->m);	\
	    } else if ((v)->n <= (unsigned)(i)) (v)->n = (i) + 1;	\
	    (v)->a[(i)] = (x);						\
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
#endif
