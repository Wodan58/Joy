/* FILE : utils.c */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
# ifdef GC_BDW
#    include <gc.h>
# endif

# ifndef GC_BDW
static Node
    memory[MEMORYMAX],
    *memoryindex = memory,
    *mem_low = memory,
    *mem_mid;
#define MEM_HIGH (MEMORYMAX-1)
static int direction = +1;
static int nodesinspected, nodescopied;
static int start_gc_clock;
# endif

PUBLIC void inimem1(void)
{
    stk = conts = dump = dump1 = dump2 = dump3 = dump4 = dump5 = NULL;
# ifndef GC_BDW
    direction = +1;
    memoryindex = mem_low;
# endif
}
PUBLIC void inimem2(void)
{
# ifndef GC_BDW
    mem_low = memoryindex;
    mem_mid = mem_low + (MEM_HIGH)/2;
    if (tracegc > 1)
      { printf("memory = %ld : %ld\n",
		(long)memory,MEM2INT(memory));
	printf("memoryindex = %ld : %ld\n",
		(long)memoryindex,MEM2INT(memoryindex));
	printf("mem_low = %ld : %ld\n",
		(long)mem_low,MEM2INT(mem_low));
	printf("top of mem = %ld : %ld\n",
		(long)(&memory[MEM_HIGH]),MEM2INT((&memory[MEM_HIGH])));
	printf("mem_mid = %ld : %ld\n",
		(long)mem_mid,MEM2INT(mem_mid)); }
# endif
}

PUBLIC void printnode(Node *p)
{
# ifndef GC_BDW
    printf("%10ld:        %-10s %10ld %10ld\n",
	MEM2INT(p),
	symtab[(short) p->op].name,
	p->op == LIST_ ? (long)MEM2INT(p->u.lis) : p->u.num,
	MEM2INT(p->next));
# endif
}

# ifndef GC_BDW
PRIVATE Node *copy(n)
    Node *n;
{
    Node *temp;
    nodesinspected++;
    if (tracegc > 4) printf("copy ..\n");
    if (n == NULL) return NULL;
    if (n < mem_low) return n; /* later: combine with previous line */
    if (n->op == ILLEGAL_)
      { printf("copy: illegal node  "); printnode(n);  return(NULL); }
    if (n->op == COPIED_) return n->u.lis;
    temp = memoryindex; memoryindex += direction;
    temp->op = n->op;
/* Nick Forde recommmended replacing this line
    temp->u.num = n->op == LIST_ ? (long)copy(n->u.lis) : n->u.num;
  with the following case statement: */
    switch(n->op)
      { case INTEGER_:
	    temp->u.num = n->u.num; break;
	case SET_:
	    temp->u.set = n->u.set; break;
	case STRING_:
	    temp->u.str = n->u.str; break;
	case FLOAT_:
	    temp->u.dbl = n->u.dbl; break;
	case FILE_:
	    temp->u.fil = n->u.fil; break;
	case LIST_:
	    temp->u.num = (long)copy(n->u.lis); break;
	default:
	    temp->u.num = n->u.num; }
/* end of replacement */
    temp->next = copy(n->next);
    n->op = COPIED_;
    n->u.lis = temp;
    nodescopied++;
    if (tracegc > 3)
      { printf("%5d -    ",nodescopied); printnode(temp); }
    return temp;
}
# endif

# ifndef GC_BDW
PRIVATE void gc1(mess)
    char * mess;
{
    start_gc_clock = clock();
    if (tracegc > 1)
	printf("begin %s garbage collection\n",mess);
    direction = - direction;
    memoryindex = (direction == 1) ? mem_low : &memory[MEM_HIGH];
/*
    if (tracegc > 1)
      { printf("direction = %d\n",direction);
	printf("memoryindex = %d : %d\n",
		(long)memoryindex,MEM2INT(memoryindex)); }
*/
    nodesinspected = nodescopied = 0;

#define COP(X,NAME)						\
    if (X != NULL)						\
      { if (tracegc > 2)					\
	  { printf("old %s = ",NAME);				\
	    writeterm(X, stdout); printf("\n"); }		\
	X = copy(X);						\
	if (tracegc > 2)					\
	  { printf("new %s = ",NAME);				\
	    writeterm(X, stdout); printf("\n"); } }

    COP(stk,"stk"); COP(prog,"prog"); COP(conts,"conts");
    COP(dump,"dump"); COP(dump1,"dump1"); COP(dump2,"dump2");
    COP(dump3,"dump3"); COP(dump4,"dump4"); COP(dump5,"dump5");
}

PRIVATE void gc2(mess)
    char * mess;
{
    int this_gc_clock;
    this_gc_clock = clock() - start_gc_clock;
    if (this_gc_clock == 0) this_gc_clock = 1; /* correction */
    if (tracegc > 0)
	printf("gc - %d nodes inspected, %d nodes copied, clock: %d\n",
	       nodesinspected,nodescopied,this_gc_clock);
    if (tracegc > 1)
	printf("end %s garbage collection\n",mess);
    gc_clock += this_gc_clock;
}
# endif
PUBLIC void gc_(void)
{
# ifndef GC_BDW
    gc1("user requested");
    gc2("user requested");
# else
   GC_gcollect();
# endif
}

PUBLIC Node *newnode(Operator o, Types u, Node *r)
{
    Node *p;
# ifndef GC_BDW
    if (memoryindex == mem_mid)
      { gc1("automatic");
	if (o == LIST_) u.lis = copy(u.lis);
	r = copy(r);
	gc2("automatic"); }
    p = memoryindex;
    memoryindex += direction;
# else
    p = GC_malloc(sizeof(Node));
    if (!p) execerror("memory", "allocator");
# endif
    p->op = o;
    p->u = u;
    p->next = r;
D(  printnode(p); )
    return p;
}

PUBLIC void memoryindex_(void)
{
# ifndef GC_BDW
    stk = INTEGER_NEWNODE((long)MEM2INT(memoryindex),stk);
# else
    stk = INTEGER_NEWNODE(0L, stk);
# endif
}

#if 0
PRIVATE void readmodule_field(void)
{
    Entry *p;
D(  printf("Module %s at %d\n",location->name,(long) location); )
D(  p = location->u.module_fields; )
D(  while (p)  { printf("%s\n",p->name); p = p->next; } )
    p = location->u.module_fields;
    getsym();
    if (sym != PERIOD)
	error("period '.' expected after module name");
    else getsym();
    if (sym != ATOM)
      { error("atom expected as module field"); return; }
    lookup();
D(  printf("looking for field %s\n",id); )
    while (p && strcmp(id,p->name) != 0)
	p = p->next;
    if (p == NULL)
	error("no such field in module");
D(  printf("found field: %s\n",p->name); )
    stk = USR_NEWNODE(p,stk);
    return;
}
#endif

PUBLIC void readfactor(void)	/* read a JOY factor		*/
{
    switch (sym)
      { case ATOM:
	    lookup();
D(	    printf("readfactor: location = %ld\n", (long)location); )
/* replace the following two lines:
	    if (location->is_module)
	      { readmodule_field(); return; }
   with the following block */
	    while (location->is_module)
{
    Entry *mod_fields;
    mod_fields = location->u.module_fields;
    getsym();
    if (sym != PERIOD)
	error("period '.' expected after module name");
    else getsym();
    if (sym != ATOM)
      { error("atom expected as module field"); return; }
    lookup();
D(  printf("looking for field %s\n",id); )
    while (mod_fields && strcmp(id,mod_fields->name) != 0)
	mod_fields = mod_fields->next;
    if (mod_fields == NULL)
      { error("no such field in module"); abortexecution_(); }
D(  printf("found field: %s\n",mod_fields->name); )
    location = mod_fields;
}
/* end of replacement */
	    if (location < firstlibra)
		{ bucket.proc = location->u.proc;
		stk = newnode(LOC2INT(location), bucket, stk); }
		else stk =  USR_NEWNODE(location,stk);
	    return;
	case BOOLEAN_: case INTEGER_: case CHAR_: case STRING_:
	    bucket.num = num;
	    stk = newnode(sym,bucket,stk);
	    return;
	case FLOAT_:
	    stk = FLOAT_NEWNODE(dbl,stk);
	    return;
	case LBRACE:
	  { long set = 0; getsym();
	    while (sym != RBRACE)
	      { if (sym == CHAR_ || sym == INTEGER_)
		    set = set | (1 << num);
		  else error("numeric expected in set");
		getsym(); }
	    stk =  SET_NEWNODE(set,stk); }
	    return;
	case LBRACK:
	  { getsym();
	    readterm();
	    if (sym != RBRACK)
		error("']' expected");
	    return; }
	case LPAREN:
	    error("'(' not implemented");
	    getsym();
	    return;
	default:
	    error("a factor cannot begin with this symbol");
	    return; }
}

PUBLIC void readterm(void)
{
    stk = LIST_NEWNODE(0L,stk);
    if (sym <= ATOM)
      { readfactor();
	stk->next->u.lis = stk;
	stk = stk->next;
	stk->u.lis->next = NULL;
	dump = newnode(LIST_,stk->u,dump);
	getsym();
	while (sym <= ATOM)
	  { readfactor();
	    dump->u.lis->next = stk;
	    stk = stk->next;
	    dump->u.lis->next->next = NULL;
	    dump->u.lis = dump->u.lis->next;
	    getsym(); }
	dump = dump->next; }
}

PUBLIC void writefactor(Node *n, FILE *stm)
{
    if (n == NULL)
	execerror("non-empty stack","print");
    switch (n->op)
      { case BOOLEAN_:
	    fprintf(stm, "%s", n->u.num ? "true" : "false"); return;
	case INTEGER_:
	    fprintf(stm, "%ld",n->u.num); return;
	case FLOAT_:
	    fprintf(stm, "%g", n->u.dbl); return;
	case SET_:
	  { int i; long set = n->u.set;
	    fprintf(stm, "{");
	    for (i = 0; i < SETSIZE; i++)
		if (set & (1 << i))
		  { fprintf(stm, "%d",i);
		    set = set & ~(1 << i);
		    if (set != 0)
			fprintf(stm, " "); }
	    fprintf(stm, "}");
	    return; }
	case CHAR_:
	    fprintf(stm, "'%c", (char) n->u.num); return;
	case STRING_:
	    fprintf(stm, "\"%s\"",n->u.str); return;
	case LIST_:
	    fprintf(stm, "%s","[");
	    writeterm(n->u.lis, stm);
	    fprintf(stm, "%s","]");
	    return;
	case USR_:
	    fprintf(stm, "%s", n->u.ent->name ); return;
	case FILE_:
	    if (n->u.fil == NULL) fprintf(stm, "file:NULL");
	    else if (n->u.fil == stdin) fprintf(stm, "file:stdin");
	    else if (n->u.fil == stdout) fprintf(stm, "file:stdout");
	    else if (n->u.fil == stderr) fprintf(stm, "file:stderr");
	    else fprintf(stm, "file:%p", (void *)n->u.fil);
	    return;
	default:
	    fprintf(stm, "%s",symtab[(int) n->op].name); return; }
}

PUBLIC void writeterm(Node *n, FILE *stm)
{
    while  (n != NULL)
	{
	writefactor(n, stm);
	n = n->next;
	if (n != NULL)
	    fprintf(stm, " ");
	}
}
