/* FILE: utils.c */
/*
 *  module  : utils.c
 *  version : 1.19.1.6
 *  date    : 06/25/20
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "globals.h"

static Node memory[MEMORYMAX], *memoryindex = memory, *mem_low = memory,
                               *mem_mid;
#define MEM_HIGH (MEMORYMAX - 1)
static int direction = 1;
static int nodesinspected, nodescopied;
static int start_gc_clock;

PUBLIC void inimem1(void)
{
    stck = conts = dump = dump1 = dump2 = dump3 = dump4 = dump5 = NULL;
    direction = 1;
    memoryindex = mem_low;
}

#ifdef STATS
static double avail;

static void report_avail(void)
{
    fprintf(stderr, "%.0f user nodes available\n", avail);
}

static void count_avail(void)
{
    double new_avail;

    if (!avail)
        atexit(report_avail);
    new_avail = &memory[MEMORYMAX] - mem_low;
    if (avail > new_avail || !avail)
        avail = new_avail;
}
#endif

PUBLIC void inimem2(void)
{
    mem_low = memoryindex;
#ifdef STATS
    count_avail();
#endif
    mem_mid = mem_low + (&memory[MEM_HIGH] - mem_low) / 2;
#ifdef ENABLE_TRACEGC
    if (tracegc > 1) {
        printf("memory = %p : %p\n", (void*)memory, (void*)MEM2INT(memory));
        printf("memoryindex = %p : %p\n", (void*)memoryindex,
            (void*)MEM2INT(memoryindex));
        printf("mem_low = %p : %p\n", (void*)mem_low, (void*)MEM2INT(mem_low));
        printf("top of mem = %p : %p\n", (void*)&memory[MEM_HIGH],
            (void*)MEM2INT((&memory[MEM_HIGH])));
        printf("mem_mid = %p : %p\n", (void*)mem_mid, (void*)MEM2INT(mem_mid));
    }
#endif
}

PUBLIC void printnode(Node* p)
{
    printf("%10p:        %-10s %10p %10p\n", (void*)MEM2INT(p),
        symtab[(int)p->op].name,
        p->op == LIST_ ? (void*)MEM2INT(p->u.lis) : (void*)(size_t)p->u.num,
        (void*)MEM2INT(p->next));
}

PRIVATE Node* copy(Node* n)
{
    Node* temp;

    nodesinspected++;
#ifdef ENABLE_TRACEGC
    if (tracegc > 4)
        printf("copy ..\n");
#endif
    if (n == NULL)
        return NULL;
    if (n < mem_low)
        return n; /* later: combine with previous line */
    if (n->op == ILLEGAL_) {
        printf("copy: illegal node  ");
        printnode(n);
        return NULL;
    }
    if (n->op == COPIED_)
        return n->u.lis;
    temp = memoryindex;
    memoryindex += direction;
    temp->op = n->op;
    /* Nick Forde recommmended replacing this line
    temp->u.num = n->op == LIST_ ? (long)copy(n->u.lis) : n->u.num;
   with the following case statement: */
    switch (n->op) {
    case COPIED_:
    default:
        temp->u.lis = n->u.lis;
        break;
    case USR_:
        temp->u.ent = n->u.ent;
        break;
    case ANON_FUNCT_:
        temp->u.proc = n->u.proc;
        break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        temp->u.num = n->u.num;
        break;
    case SET_:
        temp->u.set = n->u.set;
        break;
    case STRING_:
        temp->u.str = n->u.str;
        break;
    case LIST_:
        temp->u.lis = copy(n->u.lis);
        break;
    case FLOAT_:
        temp->u.dbl = n->u.dbl;
        break;
    case FILE_:
        temp->u.fil = n->u.fil;
        break;
    }
    /* end of replacement */
    temp->next = copy(n->next);
    n->op = COPIED_;
    n->u.lis = temp;
    nodescopied++;
#ifdef ENABLE_TRACEGC
    if (tracegc > 3) {
        printf("%5d -    ", nodescopied);
        printnode(temp);
    }
#endif
    return temp;
}

#ifdef STATS
static double collect;

static void report_collect(void)
{
    fprintf(stderr, "%.0f garbage collections\n", collect);
}

static void count_collect(void)
{
    if (++collect == 1)
        atexit(report_collect);
}
#endif

#ifdef ENABLE_TRACEGC
PRIVATE void gc1(char* mess)
#else
PRIVATE void gc1(void)
#endif
{
#ifdef STATS
    count_collect();
#endif
    start_gc_clock = clock();
#ifdef ENABLE_TRACEGC
    if (tracegc > 1)
        printf("begin %s garbage collection\n", mess);
#endif
    direction = -direction;
    memoryindex = (direction == 1) ? mem_low : &memory[MEM_HIGH];
/*
    if (tracegc > 1) {
        printf("direction = %d\n", direction);
        printf("memoryindex = %d : %d\n",
                (long)memoryindex, MEM2INT(memoryindex));
    }
*/
    nodesinspected = nodescopied = 0;

#ifdef ENABLE_TRACEGC
#define COP(X, NAME)                                                           \
    if (X != NULL) {                                                           \
        if (tracegc > 2) {                                                     \
            printf("old %s = ", NAME);                                         \
            writeterm(X, stdout);                                              \
            printf("\n");                                                      \
        }                                                                      \
        X = copy(X);                                                           \
        if (tracegc > 2) {                                                     \
            printf("new %s = ", NAME);                                         \
            writeterm(X, stdout);                                              \
            printf("\n");                                                      \
        }                                                                      \
    }
#else
#define COP(X, NAME) if (X) X = copy(X)
#endif

    COP(stck, "stck");
    COP(prog, "prog");
    COP(conts, "conts");
    COP(dump, "dump");
    COP(dump1, "dump1");
    COP(dump2, "dump2");
    COP(dump3, "dump3");
    COP(dump4, "dump4");
    COP(dump5, "dump5");
}

#ifdef ENABLE_TRACEGC
PRIVATE void gc2(char* mess)
#else
PRIVATE void gc2(void)
#endif
{
    int this_gc_clock;

    this_gc_clock = clock() - start_gc_clock;
    if (this_gc_clock == 0)
        this_gc_clock = 1; /* correction */
#ifdef ENABLE_TRACEGC
    if (tracegc > 0)
        printf("gc - %d nodes inspected, %d nodes copied, clock: %d\n",
            nodesinspected, nodescopied, this_gc_clock);
    if (tracegc > 1)
        printf("end %s garbage collection\n", mess);
#endif
    gc_clock += this_gc_clock;
}

PUBLIC void gc_(void)
{
#ifdef ENABLE_TRACEGC
    gc1("user requested");
    gc2("user requested");
#else
    gc1();
    gc2();
#endif
}

#ifdef STATS
static double nodes;

static void report_nodes(void)
{
    fprintf(stderr, "%.0f nodes used\n", nodes);
}

static void count_nodes(void)
{
    if (++nodes == 1)
        atexit(report_nodes);
}
#endif

PUBLIC Node* newnode(Operator o, Types u, Node* r)
{
    Node* p;

    if (memoryindex == mem_mid) {
#ifdef ENABLE_TRACEGC
        gc1("automatic");
#else
        gc1();
#endif
        if (o == LIST_)
            u.lis = copy(u.lis);
        r = copy(r);
#ifdef ENABLE_TRACEGC
        gc2("automatic");
#else
        gc2();
#endif
        if ((direction == 1 && memoryindex >= mem_mid)
            || (direction == -1 && memoryindex <= mem_mid))
            execerror("memory", "copying");
    }
    p = memoryindex;
    memoryindex += direction;
    p->op = o;
    p->u = u;
    p->next = r;
    D(printnode(p);)
#ifdef STATS
    count_nodes();
#endif
    return p;
}

PUBLIC void memoryindex_(void)
{
    stck = INTEGER_NEWNODE(MEM2INT(memoryindex), stck);
}

PUBLIC void readfactor(int priv) /* read a JOY factor        	*/
{
    long_t set = 0;

    switch (symb) {
    case ATOM:
        lookup(priv);
        D(printf("readfactor: location = %p\n", (void*)location);)
        while (location->is_module) {
            Entry* mod_fields = location->u.module_fields;
            getsym();
            if (symb != PERIOD)
                error("period '.' expected after module name");
            else
                getsym();
            if (symb != ATOM) {
                error("atom expected as module field");
                return;
            }
            D(printf("looking for field %s\n", ident);)
            while (mod_fields && strcmp(ident, mod_fields->name) != 0)
                mod_fields = mod_fields->next;
            if (mod_fields == NULL) {
                error("no such field in module");
                abortexecution_();
            }
            D(printf("found field: %s\n", mod_fields->name);)
            location = mod_fields;
        }
        if (!priv) {
            if (location < firstlibra) {
                bucket.proc = location->u.proc;
                stck = newnode(LOC2INT(location), bucket, stck);
            } else
                stck = USR_NEWNODE(location, stck);
        }
        return;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        if (!priv) {
            bucket.num = numb;
            stck = newnode(symb, bucket, stck);
        }
        return;
    case STRING_:
        if (!priv) {
            bucket.str = strg;
            stck = newnode(symb, bucket, stck);
        }
        return;
    case FLOAT_:
        if (!priv)
            stck = FLOAT_NEWNODE(dblf, stck);
        return;
    case LBRACE:
        while (getsym(), symb != RBRACE)
            if (symb == CHAR_ || symb == INTEGER_)
                set |= ((long_t)1 << numb);
            else
                error("numeric expected in set");
        if (!priv)
            stck = SET_NEWNODE(set, stck);
        return;
    case LBRACK:
        getsym();
        readterm(priv);
        if (symb != RBRACK)
            error("']' expected");
        return;
    case LPAREN:
        error("'(' not implemented");
        getsym();
        return;
    default:
        error("a factor cannot begin with this symbol");
    }
}

PUBLIC void readterm(int priv)
{
    Node** my_dump;

    if (!priv) {
        stck = LIST_NEWNODE(0L, stck);
        my_dump = &stck->u.lis;
    }
    while (symb <= ATOM) {
        readfactor(priv);
        if (!priv) {
            *my_dump = stck;
            my_dump = &stck->next;
            stck = *my_dump;
            *my_dump = 0;
        }
        getsym();
    }
}

PUBLIC void writefactor(Node* n, FILE* stm)
{
    int i;
    char* p;
    long_t set;

    if (n == NULL)
        execerror("non-empty stack", "print");
    switch (n->op) {
    case BOOLEAN_:
        fprintf(stm, "%s", n->u.num ? "true" : "false");
        return;
    case INTEGER_:
#ifdef BIT_32
        fprintf(stm, "%ld", (long)n->u.num);
#else
        fprintf(stm, "%lld", n->u.num);
#endif
        return;
    case FLOAT_:
        fprintf(stm, "%g", n->u.dbl);
        return;
    case SET_:
        set = n->u.set;
        fprintf(stm, "{");
        for (i = 0; i < SETSIZE; i++)
            if (set & ((long_t)1 << i)) {
                fprintf(stm, "%d", i);
                set &= ~((long_t)1 << i);
                if (set != 0)
                    fprintf(stm, " ");
            }
        fprintf(stm, "}");
        return;
    case CHAR_:
        fprintf(stm, "'%c", (char)n->u.num);
        return;
    case STRING_:
        fputc('"', stm);
        for (p = n->u.str; p && *p; p++) {
            if (*p == '"' || *p == '\\' || *p == '\n')
                fputc('\\', stm);
            fputc(*p == '\n' ? 'n' : *p, stm);
        }
        fputc('"', stm);
        return;
    case LIST_:
        fprintf(stm, "%s", "[");
        writeterm(n->u.lis, stm);
        fprintf(stm, "%s", "]");
        return;
    case USR_:
        fprintf(stm, "%s", n->u.ent->name);
        return;
    case FILE_:
        if (n->u.fil == NULL)
            fprintf(stm, "file:NULL");
        else if (n->u.fil == stdin)
            fprintf(stm, "file:stdin");
        else if (n->u.fil == stdout)
            fprintf(stm, "file:stdout");
        else if (n->u.fil == stderr)
            fprintf(stm, "file:stderr");
        else
            fprintf(stm, "file:%p", (void*)n->u.fil);
        return;
    default:
        fprintf(stm, "%s", symtab[n->op].name);
        return;
    }
}

PUBLIC void writeterm(Node* n, FILE* stm)
{
    while (n != NULL) {
        writefactor(n, stm);
        n = n->next;
        if (n != NULL)
            fprintf(stm, " ");
    }
}
