/* FILE : scan.c */
/*
 *  module  : scan.c
 *  version : 1.86
 *  date    : 10/11/24
 */
#include "globals.h"

static struct keys {
    char *name;
    Operator sym;
} keywords[] = {
    { "LIBRA",   LIBRA   },
    { "DEFINE",  LIBRA   },
    { "HIDE",    HIDE    },
    { "IN",      IN__    },
    { "END",     '.'     },
    { "MODULE",  MODULE_ },
    { "PRIVATE", PRIVATE },
    { "PUBLIC",  PUBLIC  },
    { "CONST",   CONST_  },
    { "INLINE",  CONST_  },
    /* possibly other uppers here */
    { "==",      EQDEF   }
};

static FILE *srcfile;			/* FILE pointer to input file */
static char *filenam;			/* name of input file in messages */
static int linenum, linepos;		/* line number for errors */
static char linebuf[INPLINEMAX + 1];	/* buffered input line */
static struct {
    FILE *fp;
    int line;
    char name[FILENAMEMAX + 1];		/* filename in error messages */
} infile[INPSTACKMAX];
static int ilevel = -1;			/* index in infile-structure */
static int startnum, startpos, endpos;	/* y, x, and len of tokens */

/*
 * getch reads the next character from srcfile.
 */
int getch(pEnv env)
{
    int ch;

again:
    if (vec_size(env->pushback))
	return vec_pop(env->pushback);
    if ((ch = fgetc(srcfile)) == EOF) {
	if (!ilevel)
	    abortexecution_(ABORT_QUIT);
	fclose(srcfile);
	if (env->finclude_busy)
	    longjmp(env->finclude, 1);	/* back to finclude */
	srcfile = infile[--ilevel].fp;
	linenum = infile[ilevel].line;
	filenam = infile[ilevel].name;
	goto again;
    }
    if (!linepos && ch == SHELLESCAPE) {
	vec_setsize(env->string, 0);
	while ((ch = fgetc(srcfile)) != '\n' && ch != EOF)
	    vec_push(env->string, ch);
	vec_push(env->string, 0);
#ifndef WINDOWS_S
	if (!env->ignore)
	    (void)system(&vec_at(env->string, 0));
#endif
	vec_setsize(env->string, 0);
	goto again;
    }
    if (ch == '\n') {
	if (env->echoflag > 2)
	    printf("%4d", linenum);
	if (env->echoflag > 1)
	    putchar('\t');
	if (env->echoflag)
	    printf("%.*s\n", linepos, linebuf);	/* echo line */
	linenum++;
	linepos = 0;
    } else if (linepos < INPLINEMAX)
	linebuf[linepos++] = ch;
    linebuf[linepos] = 0;
    return ch;
}

/*
 * ungetch unreads a character.
 */
void ungetch(int ch)
{
    if (ch == '\n')
	linenum--;		/* about to unread newline */
    ungetc(ch, srcfile);
    linepos--;			/* read too far, push back */
}

/*
 * error prints a message in case of an error.
 */
void error(char *str)
{
    int leng;

    fflush(stdout);
    leng = fprintf(stderr, "%s:%d:", filenam, linenum);
    leng += fprintf(stderr, "%.*s", linepos, linebuf);	/* echo line */
    fprintf(stderr, "\n%*s^", --leng, "");		/* caret corrected */
    fprintf(stderr, "\n%*s%s\n", leng, "", str);	/* message */
}

/*
 * redirect - register another file descriptor to read from.
 */
static void redirect(pEnv env, char *str, int j, FILE *fp)
{
    int i;
    char *new_path, *old_path;

    /*
     * If a filename is given with a pathname, then that pathname will be used
     * for additional attempts.
     */
    if (strrchr(str, '/')) {
	new_path = GC_strdup(str);
	str = strrchr(new_path, '/');
	*str++ = 0;
	for (i = 0; i < j; i++) {
	    old_path = vec_at(env->pathnames, i);
	    if (!strcmp(new_path, old_path))
		break;
	}
	if (i == j)
	    vec_push(env->pathnames, new_path);
    }
    if (ilevel >= 0)
	infile[ilevel].line = linenum;		/* save last line number */
    if (ilevel + 1 == INPSTACKMAX)		/* increase include level */
	execerror(env, "fewer include files", "include");
    infile[++ilevel].fp = srcfile = fp;		/* use new file pointer */
    infile[ilevel].line = linenum = 1;		/* start with line 1 */
    strncpy(filenam = infile[ilevel].name, str, FILENAMEMAX);
}

/*
 * inilinebuffer - initialise the stack of input files. The filename parameter
 *		   is used in error messages.
 */
void inilinebuffer(pEnv env)
{
    redirect(env, "stdin", 0, stdin);
}

/*
 * include - insert the contents of a file in the input.
 *
 * Files are read in the current directory or if that fails from a previous
 * location. Generating an error is left to the call site.
 *
 * Return code is 1 if the file could not be opened for reading.
 */
int include(pEnv env, char *name)
{
    int i, j;
    FILE *fp;
    size_t leng;
    char *path, *str = name;			/* str = path/name */

    /*
     * The home directory is added to the list of directories to be searched.
     */
    if (!env->homedir) {			/* should be present */
	env->homedir = getenv("HOME");		/* unix/cygwin */
#ifdef WINDOWS
	if (!env->homedir)
	    env->homedir = getenv("HOMEPATH");	/* windows */
#endif
	if (env->homedir)
	    vec_push(env->pathnames, env->homedir);
    }
    /*
     * The current directory is tried first.
     * Then all saved directories are tried until there is one that succeeds.
     */
    for (j = i = vec_size(env->pathnames); i >= 0; i--) {
	if (i != vec_size(env->pathnames)) {
	    path = vec_at(env->pathnames, i);
	    leng = strlen(path) + strlen(name) + 2;
	    str = GC_malloc_atomic(leng);
	    snprintf(str, leng, "%s/%s", path, name);
	}
	if ((fp = fopen(str, "r")) != 0) {	/* try to read */
	    redirect(env, str, j, fp);		/* stop trying */
	    return 0;
	}
    }
    return 1;		/* file cannot be opened for reading */
}

/*
 * special reads a character escape sequence.
 */
static int special(pEnv env)
{
    int ch, i, my_num;

    ch = getch(env);
    switch (ch) {
    case 'b':
	return '\b';
    case 't':
	return '\t';
    case 'n':
	return '\n';
    case 'v':
	return '\v';
    case 'f':
	return '\f';
    case 'r':
	return '\r';
    case '\"':
	return '\"';
    case '\'':
	return '\'';
    case '\\':
	return '\\';
    default:
	if (isdigit(ch)) {
	    my_num = ch - '0';
	    for (i = 0; i < 2; i++) {
		ch = getch(env);
		if (!isdigit(ch)) {
		    error("digit expected");
		    ungetch(ch);		/* not a digit, push back */
		    break;
		}
		my_num = 10 * my_num + ch - '0';
	    }
	    return my_num;
	}
	return ch;
    }
}

/*
 * getsym reads the next symbol from code or from srcfile. The return value is
 * the character after the symbol that was read.
 */
static int my_getsym(pEnv env, int ch)
{
    static char *exclude = "\"#'().;[]{}", *include = "-=_";
    char *ptr;
    int i, sign, type;

    vec_setsize(env->string, 0);
start:
    while (ch <= ' ')
	ch = getch(env);
    startnum = linenum;		/* line of a token */
    startpos = linepos;		/* start position of a token */
    switch (ch) {
    case '(':
	ch = getch(env);
	if (ch == '*') {
	    ch = getch(env);
	    do {
		while (ch != '*')
		    ch = getch(env);
		ch = getch(env);
	    } while (ch != ')');
	    ch = getch(env);
	    goto start;
	}
	env->sym = '(';
	return ch;

    case '#':
	do
	    ch = getch(env);
	while (ch != '\n');
	goto start;

    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '.':
    case ';':
	env->sym = ch;
	endpos = linepos;
	return getch(env);	/* read past ch */

    case '\'':
	ch = getch(env);
	if (ch == '\\')
	    ch = special(env);
	env->num = ch;
	env->sym = CHAR_;
	endpos = linepos;
	return getch(env);	/* read past ch */

    case '"':
	ch = getch(env);
	while (ch != '"') {
	    if (ch == '\\')
		ch = special(env);
	    vec_push(env->string, ch);
	    ch = getch(env);
	}
	vec_push(env->string, 0);
	env->str = GC_strdup(&vec_at(env->string, 0));
	env->sym = STRING_;
	endpos = linepos;
	return getch(env);	/* read past " */

    default:
	vec_push(env->string, ch);
	sign = ch;		/* possible sign */
	ch = getch(env);
	type = isdigit(sign) || (sign == '-' && isdigit(ch));	/* numeric */
	while (ch > ' ' && !strchr(exclude, ch)) {
	    vec_push(env->string, ch);
	    ch = getch(env);
	}
	if (ch == '.') {
	    ch = getch(env);
	    if (type) {
		if (!isdigit(ch)) {	/* test float */
		    ungetch(ch);	/* read too far, push back */
		    ch = '.';		/* restore full stop */
		    goto einde;
		}
		type = 2;		/* floating point */
	    } else if (!isalnum(ch) && !strchr(include, ch)) {	/* member */
		ungetch(ch);		/* read too far, push back */
		ch = '.';		/* restore full stop */
		goto einde;
	    }
	    vec_push(env->string, '.');
	    while (ch > ' ' && !strchr(exclude, ch)) {
		vec_push(env->string, ch);
		ch = getch(env);
	    }
	}
einde:	vec_push(env->string, 0);
	ptr = &vec_at(env->string, 0);
	endpos = startpos + strlen(ptr) - 1;
	if (type) {
	    if (type == 2) {
		env->dbl = strtod(&vec_at(env->string, 0), &ptr);
		env->sym = FLOAT_;
	    } else {
		if (sign == '-')
		    env->num = strtoll(&vec_at(env->string, 1), &ptr, 0);
		else
		    env->num = strtoll(&vec_at(env->string, 0), &ptr, 0);
		if (env->num == MAXINT_) {
		    env->dbl = strtod(&vec_at(env->string, 0), &ptr);
		    env->sym = FLOAT_;
		} else {
		    if (sign == '-')
			env->num = -env->num;
		    env->sym = INTEGER_;
		}
	    }
	    if (*ptr) {
		endpos -= strlen(ptr);
		vec_push(env->pushback, ch);
		for (i = strlen(ptr) - 1; i >= 0; i--)
		    vec_push(env->pushback, ptr[i]);
		return getch(env);
	    }
	    return ch;
	}
	type = ptr[1];
	if (isupper(type) || type == '=') {
	    type = sizeof(keywords) / sizeof(keywords[0]);
	    for (sign = 0; sign < type; sign++)
		if (!strcmp(ptr, keywords[sign].name)) {
		    env->sym = keywords[sign].sym;
		    return ch;
		}
	}
	env->str = GC_strdup(ptr);
	env->sym = USR_;
    }
    return ch;
}

static void dumptok(pEnv env, int y, int x, int pos)
{
    printf("(%d,%d:%d) ", y, x, pos);
    switch (env->sym) {
    case USR_     : printf("%s", env->str);
		    break;
    case CHAR_    : printf("%d", (int)env->num);
		    break;
    case INTEGER_ : printf("%" PRId64, env->num);
		    break;
    case STRING_  : printf("\"%s\"", env->str);
		    break;
    case FLOAT_   : printf("%g", env->dbl);
		    break;
    case '['      : printf("LBRACK");
		    break;
    case ']'      : printf("RBRACK");
		    break;
    case '{'      : printf("LBRACE");
		    break;
    case '}'      : printf("RBRACE");
		    break;
    case '('      : printf("LPAREN");
		    break;
    case ')'      : printf("RPAREN");
		    break;
    case '.'      : printf("PERIOD");
		    break;
    case ';'      : printf("SEMICOL");
		    break;
    case LIBRA    : printf("LIBRA");
		    break;
    case EQDEF    : printf("EQDEF");
		    break;
    case HIDE     : printf("HIDE");
		    break;
    case IN__     : printf("IN");
		    break;
    case MODULE_  : printf("MODULE");
		    break;
    case PRIVATE  : printf("PRIVATE");
		    break;
    case PUBLIC   : printf("PUBLIC");
		    break;
    case CONST_   : printf("CONST");
		    break;
    }
    printf("\n");
}

/*
 * push_sym - push a symbol into the tokenlist.
 */
static void push_sym(pEnv env)
{
    Token node;

    switch (node.op = env->sym) {
    case CHAR_:
    case INTEGER_:
	node.u.num = env->num;
	break;
    case FLOAT_:
	node.u.dbl = env->dbl;
	break;
    case USR_:
    case STRING_:
	node.u.str = env->str;
	break;
    }
    node.y = startnum;
    node.x = startpos;
    node.pos = endpos;
    vec_push(env->tokens, node);
}

/*
 * getsym - wrapper around my_getsym, storing tokens read, reading from the
 *	    store or just calling my_getsym itself. This allows tokens to be
 *	    read twice.
 *
 * After reading MODULE or PRIVATE, read all tokens upto END, and include them
 * in the tokenlist. All symbols preceding "==" are declared.
 */
int getsym(pEnv env, int ch)
{
    Token node;
    int module = 0, private = 0, hide = 0, modl = 0, hcnt = 0;

/*
 * If there is a tokenlist, extract tokens from there.
 */
    if (vec_size(env->tokens)) {
begin:	node = vec_pop(env->tokens);
	switch (env->sym = node.op) {
	case CHAR_:
	case INTEGER_:
	    env->num = node.u.num;
	    break;
	case FLOAT_:
	    env->dbl = node.u.dbl;
	    break;
	case USR_:
	case STRING_:
	    env->str = node.u.str;
	    break;
	}
	if (env->printing)
	    dumptok(env, node.y, node.x, node.pos); /* tokens from tokenlist */
	return ch;
    }
/*
 * There is no tokenlist, use the normal procedure to get one.
 */
    ch = my_getsym(env, ch);
/*
 * There is a token available, do some extra processing, in case the token is
 * MODULE or HIDE: MODULE .. END or HIDE .. END.
 */
    if (env->sym == MODULE_ || env->sym == HIDE) {
/*
 * Copy the global variables of modl.c into local variables.
 */
	node.op = env->sym;
	savemod(&hide, &modl, &hcnt);
	do {
	    switch (env->sym) {
	    case MODULE_  : push_sym(env);
			    ch = my_getsym(env, ch);
			    if (env->sym == USR_) {
				initmod(env, env->str);
				module++;
			    } else
				error("atom expected as name of module");
			    break;
	    case HIDE	  :
	    case PRIVATE  : initpriv(env);
			    if (!module)
				private++;
			    break;
	    case IN__	  :
	    case PUBLIC   : stoppriv();
			    break;
	    case EQDEF	  : if (node.op == USR_) {
				if (!strchr(env->str, '.'))
				    env->str = classify(env, env->str);
				enteratom(env, env->str);
			    }
			    break;
	    case '.'	  : if (module) {
				exitmod();
				module--;
			    } else if (private) {
				exitpriv();
				private--;
			    }
			    if (!module && !private)
				goto einde;
			    break;
	    }
	    node.op = env->sym;	/* previous symbol */
	    push_sym(env);
	    ch = my_getsym(env, ch);
	} while (1);
/*
 * Restore the global variables in module.c from the local copies.
 */
einde:	undomod(hide, modl, hcnt);
	push_sym(env);		/* store the last symbol that was read */
	push_sym(env);		/* extra sym for the benefit of reverse */
	vec_reverse(env->tokens);
    }
/*
 * If there is a tokenlist, extract tokens from there.
 */
    if (vec_size(env->tokens))
	goto begin;
    if (env->printing)
	dumptok(env, startnum, startpos, endpos); /* tokens read directly */
    return ch;
}
