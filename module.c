/*
    module  : module.c
    version : 1.2
    date    : 04/12/22
*/
#include "globals.h"

/*
 * hide stack of private sections. The sections are numbered, while keeping the
 * string presentation instead of the number itself.
 */
static int hide_index = -1;
static unsigned char inside_hide;

/*
 * stack of module names. Only one module needs to be considered active for the
 * purpose of prefixing the module to the symbol name.
 */
static int module_index = -1;

/*
 * initmod registers ident as a module name. Modules within modules are
 * supported. Up to a certain extent, that is.
 */
void initmod(pEnv env, char *name)
{
    if (++module_index >= DISPLAYMAX) {
        module_index = DISPLAYMAX - 1;
        execerror(env, "index", "display");
    }
    env->module_stack[module_index] = GC_strdup(name);
}

/*
 * initpriv starts a new local section. Local sections within local sections
 * are supported. Private sections are numbered consecutively. If a module is
 * active then the hide_index is registered in the module. The variable
 * inside_hide tells whether defined names should be registered as private.
 * This is done by the function classify and used in the function enterdef.
 * Only register a new private section during the first read. During the
 * second read, the number that was installed should be picked up again.
 */
void initpriv(pEnv env, int priv)
{
    static int hide_count;
    char str[ALEN];

    if (++hide_index >= DISPLAYMAX) {
        hide_index = DISPLAYMAX - 1;
        execerror(env, "index", "display");
    }
    if (priv) {
        sprintf(str, "%d", ++hide_count);
        env->hide_stack[hide_index] = GC_strdup(str);
    }
    inside_hide = 1;
}

/*
 * stoppriv registers the transition from private to public definitions. Names
 * should no longer be prefixed with the name of the private section.
 */
void stoppriv(void) { inside_hide = 0; }

/*
 * exitpriv lowers the hide_index after reading the public section.
 */
void exitpriv(void)
{
    if (hide_index >= 0)
        hide_index--;
}

/*
 * exitmod deregisters a module. It also ends an outstanding private section.
 */
void exitmod(void)
{
    if (module_index >= 0)
        module_index--;
}

/*
 * classify prepends ident with private section or module name, whichever
 * comes first. Names are stored in the symbol table together with this prefix,
 * allowing the same names to be used in different private sections and modules.
 * The symbol table is flat, in the sense that it has no hierarchy. For that
 * reason names must be made unique with this prefix. For public names it is
 * sufficient that they are preceded by the module name; for private names it
 * is ok to have the private number as a prefix.
 *
 * classify is used when entering the name that has a definition.
 */
char *classify(pEnv env, char *name)
{
    size_t leng;
    char *buf = 0, *str;

    /*
     * inside a private section, names that are to be entered in the symbol
     * table should get the hide number as a prefix.
     */
    if (inside_hide)
        buf = env->hide_stack[hide_index];
    /*
     * inside a module, public names that are to be entered in the symbol table
     * should get the module name as a prefix. That is also the name used when
     * accessing the symbol.
     */
    else if (module_index >= 0)
        buf = env->module_stack[module_index];
    /*
     * buf, when filled, contains either a module identifier, or a number
     * string.
     */
    if (buf) {
        leng = strlen(buf) + strlen(name) + 2;
        str = GC_malloc_atomic(leng);
        sprintf(str, "%s.%s", buf, name);
    } else
        str = GC_strdup(name);
    /*
     * str will contain either the unadorned name, or a classified name.
     */
    return str;
}

/*
 * qualify does the reverse of classify. A name can be given in full, by
 * prefixing the name with a module, but this only needs to be done outside a
 * module, and only one module name needs to be given. For private sections it
 * is different. If there is a private section active, then the name needs to
 * be searched there and if a module is active then the name needs to be
 * searched there as well. The return value is the result of lookup, or the
 * full name in case the lookup was unsuccesful.
 *
 * qualify is used when reading a name, as part of a definition.
 */
pEntry qualify(pEnv env, char *name)
{
    int index;
    size_t leng;
    khiter_t key;
    char *buf, *str;

    /*
     * if ident has a prefix, it is already a fully qualified name and can be
     * searched in the symbol table right away. The prefix can only be a module
     * name. If the name is not found, there is an error and a 0 is returned.
     */
    if (strchr(name, '.'))
        if ((key = kh_get(Symtab, env->hash, name)) != kh_end(env->hash))
            return kh_value(env->hash, key);
        else
            return 0;
    /*
     * the hide stack is searched, trying each of the hidden sections. The
     * return value from lookup is returned.
     */
    if (hide_index >= 0)
        for (index = hide_index; index >= 0; index--) {
            buf = env->hide_stack[index];
            leng = strlen(buf) + strlen(name) + 2;
            str = GC_malloc_atomic(leng);
            sprintf(str, "%s.%s", buf, name);
            if ((key = kh_get(Symtab, env->hash, str)) != kh_end(env->hash))
                return kh_value(env->hash, key);
        }
    /*
     * if the name can not be found in the local tables, it should be searched
     * in the currently active module, if there is one.
     */
    if (module_index >= 0) {
        buf = env->module_stack[module_index];
        leng = strlen(buf) + strlen(name) + 2;
        str = GC_malloc_atomic(leng);
        sprintf(str, "%s.%s", buf, name);
        if ((key = kh_get(Symtab, env->hash, str)) != kh_end(env->hash))
            return kh_value(env->hash, key);
    }
    /*
     * if the name is not fully classified, cannot be found in the local tables
     * and also not in the module, it needs to be searched as is. If not found,
     * it is not an error, but simply an undefined name.
     */
    if ((key = kh_get(Symtab, env->hash, name)) != kh_end(env->hash))
        return kh_value(env->hash, key);
    return 0;
}
