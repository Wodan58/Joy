/*
 *  module  : gc.c
 *  version : 1.71
 *  date    : 02/23/26
 */
#include <stdio.h>	/* for printf-debugging */
#include <setjmp.h>
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*
 *  pointer_t is an unsigned type, large enough to hold a pointer.
 */
#ifdef _MINIX
typedef unsigned pointer_t;
#define TO_POINTER(x)	((void *)(x))
#define TO_INTEGER(x)	((pointer_t)(x))
#else
#include <stdint.h>
typedef uint64_t pointer_t;
#define TO_POINTER(x)	((void *)(intptr_t)(x))
#define TO_INTEGER(x)	((pointer_t)(intptr_t)(x))
#endif

#ifdef _MINIX
#define kh_inline
#define POSIX
#endif

#ifdef __linux__
#include <unistd.h>
#define POSIX
#endif

#ifdef __APPLE__
#include <mach-o/getsect.h>
#include <mach-o/dyld.h>
#endif

#include "khash.h"
/*
 *  Note: including the exported functions in the implementation file, ensures
 *  that the function signatures are synchronized. The -I flag is required.
 */
#include <gc.h>

#define GC_COLL		0
#define GC_LEAF		1
#define GC_MARK		2

#define GROW_FACTOR	2
#ifdef _MINIX
#define BSS_ALIGN	2
#else
#define BSS_ALIGN	4
#endif
#define MIN_ITEMS	170	/* initial number of items */

/*
 *  When pointers are aligned at 16 bytes, the lower 4 bits are always zero.
 */
#ifdef _MINIX
#define HASH_FUNCTION(key)	(key)

typedef struct mem_info {
    unsigned flags: 2;
    unsigned size: 14;
} mem_info;
#else
#define HASH_FUNCTION(key)	(khint_t)((key) >> 4)

typedef struct mem_info {
    unsigned flags: 2;
    unsigned size: 30;
} mem_info;
#endif

/*
 *  The map contains a pointer as key and mem_info as value. The pointer_t is
 *  an unsigned integer large enough in size to hold a pointer.
 */
KHASH_INIT(Backup, pointer_t, mem_info, 1, HASH_FUNCTION, kh_int64_hash_equal)

static khash_t(Backup) *MEM;	/* backup of pointers */
static khint_t max_items;	/* max. items before gc */
static pointer_t lower, upper;	/* heap bounds */
#ifdef COUNT_COLLECTIONS
static size_t GC_gc_no;		/* number of garbage collections */
static size_t memory_use;	/* amount of memory currently used */
static size_t free_bytes;	/* amount of memory on the freelist */
#endif

/*
 *  Pointers to memory segments.
 */
#ifdef SCAN_BSS_MEMORY
static char *start_of_bss, *end_of_bss;

#ifdef _MSC_VER
static char *start_of_data, *end_of_data;

extern IMAGE_DOS_HEADER __ImageBase;

typedef struct {
    const char *name;
    void *vaddr;
    DWORD vsize;
    DWORD raw_offset;
    DWORD raw_size;
    DWORD characteristics;
} section_info;

typedef struct {
    section_info sections[32];	/* 32 should be enough */
    int count;
} section_table;

section_table get_all_sections(void)
{
    int i;
    BYTE *base = (BYTE *)&__ImageBase;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + __ImageBase.e_lfanew);
    IMAGE_SECTION_HEADER *sec = (IMAGE_SECTION_HEADER *)
	((BYTE *)&nt->OptionalHeader + nt->FileHeader.SizeOfOptionalHeader);

    section_table out = {0};
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        section_info *s = &out.sections[out.count++];
        s->name = (const char *)sec[i].Name;
        s->vaddr = base + sec[i].VirtualAddress;
        s->vsize = sec[i].Misc.VirtualSize;
        s->raw_offset = sec[i].PointerToRawData;
        s->raw_size = sec[i].SizeOfRawData;
        s->characteristics = sec[i].Characteristics;
    }
    return out;
}
#endif

#ifdef __APPLE__
typedef struct
{
    void *bss_start;
    size_t bss_size;
} section_info;

section_info get_macho_sections(void)
{
    section_info out = {0};
    unsigned long size = 0;

    out.bss_start = getsectiondata(NULL, "__DATA", "__bss", &size);
    out.bss_size = size;
    return out;
}
#endif

/*
 *  Determine sections of memory. This is highly system dependent and not
 *  tested on __APPLE__.
 */
static void init_heap(void)
{
#ifdef POSIX
    extern char edata, end;

    start_of_bss = &edata;
    end_of_bss = &end;
#endif

#ifdef __x86_64__
    extern char __bss_start__, __bss_end__;

    start_of_bss = &__bss_start__;
    end_of_bss = &__bss_end__;
#else
#ifdef __MINGW32__
    extern char _bss_start__, _bss_end__;

    start_of_bss = &_bss_start__;
    end_of_bss = &_bss_end__;
#endif
#endif

#ifdef _MSC_VER
    int i;
    section_info *s;
    section_table t = get_all_sections();

    for (i = 0; i < t.count; i++) {
	s = &t.sections[i];
	if (!strncmp(s->name, ".data", 5)) {
            start_of_data = s->vaddr;
	    end_of_data = (char *)s->vaddr + s->vsize;
	}
        if (!strncmp(s->name, ".bss", 4)) {
            start_of_bss = s->vaddr;
	    end_of_bss = (char *)s->vaddr + s->vsize;
	}
    }
#endif
 
#ifdef __APPLE__
    section_info s = get_macho_sections();

    start_of_bss = s.bss_start;
    end_of_bss = s.bss_start + s.bss_size;
#endif
}
#endif	/* SCAN_BSS_MEMORY */

/*
 *  Report of the amount of memory allocated is delegated to valgrind.
 */
#ifdef FREE_ON_EXIT
static void mem_exit(void)
{
    khint_t key;

    for (key = 0; key != kh_end(MEM); key++)
	if (kh_exist(MEM, key))
	    free((void *)kh_key(MEM, key));
    kh_destroy(Backup, MEM);
}
#endif

/*
 *  Initialise gc memory.
 */
void GC_INIT(void)
{
#ifdef SCAN_BSS_MEMORY
    init_heap();
#endif
#ifdef FREE_ON_EXIT
    free(malloc(1));
    atexit(mem_exit);
#endif
    MEM = kh_init(Backup);
    max_items = MIN_ITEMS;
}

/*
 *  Walk registered blocks and free those that have not been marked.
 *  Note: kh_del from khash.h can be used in a loop. The one from khashl.h
 *  cannot be used.
 */
static void scan(void)
{
    khint_t key;

    for (key = 0; key != kh_end(MEM); key++) {
	if (kh_exist(MEM, key)) {
	    if (kh_val(MEM, key).flags & GC_MARK)
		kh_val(MEM, key).flags &= ~GC_MARK;
	    else {
#ifdef COUNT_COLLECTIONS
		free_bytes += kh_val(MEM, key).size;
#endif		
		free(TO_POINTER(kh_key(MEM, key)));
		kh_del(Backup, MEM, key);
	    }
	}
    }
}

/*
 *  Register an allocated memory block and garbage collect if there are too
 *  many blocks already.
 */
static void remind(char *ptr, size_t size, int flags)
{
    int rv;
    khint_t key;
    pointer_t value;

    value = TO_INTEGER(ptr);
    if (lower > value || !lower)
	lower = value;
    if (upper < value + size)
	upper = value + size;
    key = kh_put(Backup, MEM, value, &rv);
    kh_val(MEM, key).flags = flags;
    kh_val(MEM, key).size = size;
#ifdef COUNT_COLLECTIONS
    memory_use += size;
#endif
/*
 *  See if there are already too many items allocated. If yes, trigger the
 *  garbage collector. As the number of items that need to be remembered is
 *  unknown, it is set to twice the number of items that are currently being
 *  used. This allows a 100% growth in the number of items allocated.
 */
    if (max_items < kh_size(MEM)) {
	GC_gcollect();
	max_items = kh_size(MEM) * GROW_FACTOR;
    }
}

/*
 *  Register an allocated memory block. The block is cleared with zeroes.
 */
static void *mem_block(size_t size, int leaf)
{
    void *ptr = 0;

    ptr = malloc(size);
#ifdef TEST_MALLOC_RETURN
    if (!ptr)
	fatal("memory exhausted");	/* LCOV_EXCL_LINE */
#endif
    memset(ptr, 0, size);
    remind(ptr, size, leaf);
    return ptr;
}

/*
 *  Register a memory block that contains no other blocks.
 */
#ifdef USE_GC_MALLOC_ATOMIC
void *GC_malloc_atomic(size_t size)
{
    return mem_block(size, GC_LEAF);
}
#endif

/*
 *  Register a memory block that can be collected.
 */
#ifdef USE_GC_MALLOC
void *GC_malloc(size_t size)
{
    return mem_block(size, GC_COLL);
}
#endif

#if defined(USE_GC_REALLOC) || defined(USE_GC_FREE)
/*
 *  Forget about a memory block and return its flags.
 */
#ifdef COUNT_COLLECTIONS
static unsigned char forget(void *ptr, unsigned *size)
#else
static unsigned char forget(void *ptr)
#endif
{
    khint_t key;
    unsigned char flags = 0;

    if ((key = kh_get(Backup, MEM, TO_INTEGER(ptr))) != kh_end(MEM)) {
	flags = kh_val(MEM, key).flags;
#ifdef COUNT_COLLECTIONS	
	*size = kh_val(MEM, key).size;
#endif
	kh_del(Backup, MEM, key);
    }
    return flags;
}
#endif

/*
 *  Remove a pointer from the hash table.
 */
#ifdef USE_GC_FREE
void GC_free(void *ptr)
{
#ifdef COUNT_COLLECTIONS
    unsigned old_size = 0;
#endif

    if (!ptr)
	return;
#ifdef COUNT_COLLECTIONS
    forget(ptr, &old_size);
    memory_use -= old_size;
#else
    flags = forget(ptr);
#endif
}
#endif

/*
 *  Enlarge an already allocated memory block.
 */
#ifdef USE_GC_REALLOC
void *GC_realloc(void *ptr, size_t size)
{
    unsigned char flags;
#ifdef COUNT_COLLECTIONS
    unsigned old_size = 0;
#endif

    if (!ptr)
	return GC_malloc(size);		/* LCOV_EXCL_LINE */
#ifdef COUNT_COLLECTIONS
    flags = forget(ptr, &old_size);
    memory_use -= old_size;
    memory_use += size;
#else
    flags = forget(ptr);
#endif
    ptr = realloc(ptr, size);
#ifdef TEST_MALLOC_RETURN
    if (!ptr)
	fatal("memory exhausted");	/* LCOV_EXCL_LINE */
#endif
    remind(ptr, size, flags);
    return ptr;
}
#endif

/*
 *  Duplicate a string. A string does not contain internal pointers.
 */
#ifdef USE_GC_STRDUP
char *GC_strdup(const char *str)
{
    char *ptr;
    size_t leng;

    leng = strlen(str) + 1;
    if ((ptr = GC_malloc_atomic(leng)) != 0)
	strcpy(ptr, str);
    return ptr;
}
#endif

#ifdef COUNT_COLLECTIONS
/*
 *  Return the number of garbage collections.
 *  This is reported in the main function.
 */
long GC_get_gc_no(void)
{
    return GC_gc_no;
}

/*
 *  Return the amount of memory currently in use.
 */
/* LCOV_EXCL_START */
size_t GC_get_memory_use(void)
{
    return memory_use;
}

/*
 *  Return the amount of memory on the freelist.
 */
size_t GC_get_free_bytes(void)
{
    return free_bytes;
}
/* LCOV_EXCL_STOP */
#endif

/*
 *  Mark a block as in use. Also remember interior pointers, to be processed
 *  later on.
 */
static void mark_ptr(char *ptr)
{
    khint_t key;
    size_t i, size;
    pointer_t value;

    value = TO_INTEGER(ptr);
    if (value < lower || value >= upper)	/* check whether in heap */
	return;
    if ((key = kh_get(Backup, MEM, value)) != kh_end(MEM)) {
	if (kh_val(MEM, key).flags & GC_MARK)	/* check whether already done */
	    return;
	kh_val(MEM, key).flags |= GC_MARK;
	if (kh_val(MEM, key).flags & GC_LEAF)	/* leaf nodes w/o interior */
	    return;
	size = kh_val(MEM, key).size / sizeof(char *);
	for (i = 0; i < size; i++)
	    mark_ptr(((char **)TO_POINTER(value))[i]);
    }
}

/*
 *  Mark blocks that can be found on the stack.
 */
static void mark_stk(void)
{
    pointer_t ptr = TO_INTEGER(&ptr);

#ifdef STACK_GROWS_UPWARD
    if (ptr > bottom)
	for (; ptr > TO_INTEGER(bottom_of_stack); ptr -= sizeof(char *))
	    mark_ptr(*(char **)TO_POINTER(ptr));
    else
#endif
	for (; ptr < TO_INTEGER(bottom_of_stack); ptr += sizeof(char *))
	    mark_ptr(*(char **)TO_POINTER(ptr));
}

/*
 *  Mark blocks that are pointed to from static (un)initialized memory.
 */
#ifdef SCAN_BSS_MEMORY
static void mark_bss(void)
{
    pointer_t ptr = TO_INTEGER(start_of_bss);

    for (; ptr < TO_INTEGER(end_of_bss); ptr += BSS_ALIGN)
	mark_ptr(*(char **)TO_POINTER(ptr));
#ifdef _MSC_VER
    ptr = TO_INTEGER(start_of_data);
    for (; ptr < TO_INTEGER(end_of_data); ptr += BSS_ALIGN)
	mark_ptr(*(char **)TO_POINTER(ptr));
#endif
}
#endif

/*
 *  Collect garbage.
 *
 *  Pointers that are reachable from registers or stack are marked
 *  as well as all pointers that are reachable from those pointers.
 *  In other words: roots for garbage collection are searched in
 *  registers, on the stack, and in the blocks themselves.
 */
void GC_gcollect(void)
{
    jmp_buf env;
    void (* volatile m)(void) = mark_stk;

    memset(&env, 0, sizeof(jmp_buf));
    setjmp(env);
    (*m)();
#ifdef SCAN_BSS_MEMORY
    mark_bss();
#endif
    scan();
#ifdef COUNT_COLLECTIONS
    GC_gc_no++;
#endif
}
