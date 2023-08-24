/* FILE: scan.c */
/*
 *  module  : scan.c
 *  version : 1.53
 *  date    : 08/24/23
 */
#include "globals.h"

void quit_(pEnv env);

static struct {
    FILE *fp;
    char *name;
    int linenum;
} infile[INPSTACKMAX];
static int ilevel;
static int linenumber;
static char linbuf[INPLINEMAX + 2];
static int linelength, currentcolumn;
#if 0
static int errorcount;
#endif
static int ch = ' ';

static struct keys {
    char *name;
    Symbol symb;
} keywords[] = {
    { "LIBRA",   LIBRA    },
    { "DEFINE",  LIBRA    },
    { "HIDE",    HIDE     },
    { "IN",      IN       },
    { "END",     END      },
    { "MODULE",  MODULE   },
    { "PRIVATE", JPRIVATE },
    { "PUBLIC",  JPUBLIC  },
    /* possibly other uppers here */
    { "==",      EQDEF    }
};

/*
    inilinebuffer - initialise the stack of input files. The filename parameter
		    could be used in error messages.
*/
PUBLIC void inilinebuffer(pEnv env, char *str)
{
    infile[0].fp = env->srcfile;
    infile[0].name = str ? str : "stdin";
}

/*
    putline - echo an input line.
*/
PRIVATE void putline(pEnv env, FILE *fp, int echo)
{
    int i;

    if (echo) {
	if (env->echoflag > 2)
	    fprintf(fp, "%4d", linenumber);
	if (env->echoflag > 1)
	    fputc('\t', fp);
    }
    for (i = 0; linbuf[i] && linbuf[i] != '\n'; i++)
	fputc(linbuf[i], fp);
    fputc('\n', fp);
}

/*
    getch - return one character in ch.
*/
PRIVATE void getch(pEnv env)
{
    int i;

    if (currentcolumn == linelength) {
again:
	currentcolumn = linelength = 0;
	if (linenumber >= 0)
	    linenumber++;
	if (fgets(linbuf, INPLINEMAX, env->srcfile)) {
	    for (i = 0; linbuf[i]; i++)
		;
	    linelength = i;
	} else if (ilevel > 0) {
	    fclose(infile[ilevel].fp);
	    env->srcfile = infile[--ilevel].fp;
	    linenumber = infile[ilevel].linenum;
	} else
	    quit_(env);
	linbuf[linelength++] = ' '; /* to help getsym for numbers */
	linbuf[linelength++] = 0;
	if (env->echoflag)
	    putline(env, stdout, 1); /* echo line to stdout */
	if (linbuf[0] == SHELLESCAPE) {
	    system(&linbuf[1]);
	    goto again;
	}
    }
    ch = linbuf[currentcolumn++];
}

/*
    endofbuffer - test whether the entire buffer has been processed.
*/
PRIVATE int endofbuffer(void)
{
    return currentcolumn == linelength;
}

/*
    error - error processing during source file reads to stderr.
*/
PUBLIC void error(pEnv env, char *message)
{
    int leng;

    fflush(stdout);
    leng = fprintf(stderr, "%s:%d:", infile[ilevel].name, linenumber);
    putline(env, stderr, 0); /* line w/o line number to stderr */
    leng += currentcolumn - 2; /* no tab expansion */
    fprintf(stderr, "%*s^\n%*s%s\n", leng, "", leng, "", message);
#if 0
    errorcount++;
#endif
}

PRIVATE void redirect(pEnv env, char *filnam, FILE *fp)
{
    infile[ilevel].linenum = linenumber;
    if (ilevel + 1 == INPSTACKMAX)
	execerror("fewer include files", "include");
    infile[++ilevel].fp = env->srcfile = fp;
    infile[ilevel].name = filnam;
    infile[ilevel].linenum = linenumber = 0;
}

/*
    include - insert the contents of a file in the input.

	      Files are read in the current directory or if that fails
	      from the same directory as where the executable is stored.
	      If that path also fails an error is generated unless error
	      is set to 0.
*/
PUBLIC int include(pEnv env, char *filnam, int error)
{
    FILE *fp;
    char *ptr, *str;

/*
    First try to open filnam in the current working directory.
*/
    if ((fp = fopen(filnam, "r")) != 0) {
/*
    Replace the pathname of argv[0] with the pathname of filnam.
*/
	if (strchr(filnam, '/')) {
	    env->pathname = GC_strdup(filnam);
	    ptr = strrchr(env->pathname, '/');
	    *ptr = 0;
	}
    }
/*
    Prepend pathname to the filename and try again.
*/
    else if (strcmp(env->pathname, ".")) {
	str = GC_malloc_atomic(strlen(env->pathname) + strlen(filnam) + 2);
	sprintf(str, "%s/%s", env->pathname, filnam);
	if ((fp = fopen(str, "r")) != 0) {
/*
    If this succeeds, establish a new pathname.
*/
	    env->pathname = GC_strdup(str);
	    ptr = strrchr(env->pathname, '/');
	    *ptr = 0;
	}
    }
    if (fp) {
	redirect(env, filnam, fp);
	return 0; /* ok */
    }
    if (error)
	execerror("valid file name", "include");
    return 1; /* nok */
}

/*
    specialchar - handle character escape sequences.
*/
PRIVATE int specialchar(pEnv env)
{
    getch(env);
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
	    int i, num = ch - '0';
	    for (i = 0; i < 2; i++) {
		getch(env);
		if (!isdigit(ch)) {
		    currentcolumn++; /* to get pointer OK */
		    error(env, "digit expected");
		    currentcolumn--;
		}
		num = 10 * num + ch - '0';
	    }
	    return num;
	}
	return ch;
    }
}

/*
    peek - look at the next character.
*/
PRIVATE int peek(void)
{
    return linbuf[currentcolumn];
}

/*
    getsym - lexical analyzer, filling env->yylval and returning the token type
	     in env->symb.
*/
PRIVATE void my_getsym(pEnv env)
{
    int i = 0, begin, next;
    char ident[ALEN], string[INPLINEMAX + 2];

start:
    while (ch <= ' ')
	getch(env);
    switch (ch) {
    case '(':
	getch(env);
	if (ch == '*') {
	    getch(env);
	    do {
		while (ch != '*')
		    getch(env);
		getch(env);
	    } while (ch != ')');
	    getch(env);
	    goto start;
	}
	env->symb = LPAREN;
	return;
    case '#':
	currentcolumn = linelength;
	getch(env);
	goto start;
    case ')':
	env->symb = RPAREN;
	getch(env);
	return;
    case '[':
	env->symb = LBRACK;
	getch(env);
	return;
    case ']':
	env->symb = RBRACK;
	getch(env);
	return;
    case '{':
	env->symb = LBRACE;
	getch(env);
	return;
    case '}':
	env->symb = RBRACE;
	getch(env);
	return;
    case '.':
	env->symb = PERIOD;
	getch(env);
	return;
    case ';':
	env->symb = SEMICOL;
	getch(env);
	return;
    case '\'':
	getch(env);
	if (ch == '\\')
	    ch = specialchar(env);
	env->yylval.num = ch;
	env->symb = CHAR_;
	getch(env);
	return;
    case '"':
	getch(env);
	while (ch != '"' && !endofbuffer()) {
	    if (ch == '\\')
		ch = specialchar(env);
	    string[i++] = ch;
	    getch(env);
	}
	string[i] = 0;
	getch(env);
	env->yylval.str = GC_strdup(string);
	env->symb = STRING_;
	return;
    case '-': /* PERHAPS unary minus */
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
	if (isdigit(ch) || isdigit(peek())) {
	    begin = currentcolumn - 1;
	    if (ch == '-')
		getch(env);
	    else if (ch == '0') {
		if ((next = peek()) == 'x' || next == 'X') {
		    getch(env);
		    do
			getch(env);
		    while (isxdigit(ch));
		    goto done;
		} else if (next >= '1' && next <= '7') {
		    do
			getch(env);
		    while (ch >= '0' && ch <= '7');
		    goto done;
		}
		getch(env);
		goto first;
	    }
	    while (isdigit(ch))
		getch(env);
first:
	    if (ch == '.' && isdigit(peek())) {
		do
		    getch(env);
		while (isdigit(ch));
		if (ch == 'e' || ch == 'E') {
		    getch(env);
		    if (ch == '-' || ch == '+')
			getch(env);
		    while (isdigit(ch))
			getch(env);
		}
		env->yylval.dbl = strtod(&linbuf[begin], 0);
		env->symb = FLOAT_;
		return;
	    }
done:
	    env->yylval.num = strtoll(&linbuf[begin] +
				      (linbuf[begin] == '-' ? 1 : 0), 0, 0);
	    if (env->yylval.num == MAXINT) {
		env->yylval.dbl = strtod(&linbuf[begin], 0);
		env->symb = FLOAT_;
	    } else {
		if (linbuf[begin] == '-')
		    env->yylval.num = -env->yylval.num;
		env->symb = INTEGER_;
	    }
	    return;
	}
	goto next;
next:
    /* ELSE '-' is not unary minus, fall through */
    default:
	do {
	    if (i < ALEN - 1)
		ident[i++] = ch;
	    getch(env);
	} while (isalnum(ch) || strchr("-=_", ch));
	if (ch == '.') {
	    next = peek();
	    if (isalnum(next) || strchr("-=_", next)) {
		do {
		    if (i < ALEN - 1)
			ident[i++] = ch;
		    getch(env);
		} while (isalnum(ch) || strchr("-=_", ch));
	    }
	}
	ident[i] = 0;
	if (isupper((int)ident[1]) || ident[0] == '=')
	    for (i = 0; i < (int)(sizeof(keywords) / sizeof(keywords[0])); i++)
		if (!strcmp(ident, keywords[i].name)) {
		    env->symb = keywords[i].symb;
		    return;
		}
	env->yylval.str = GC_strdup(ident);
	env->symb = ATOM;
	return;
    }
}

#ifdef DUMP_TOKENS
PRIVATE void dumptok(Token tok, int num)
{
    printf("%d) ", num);
    switch (tok.symb) {
    case CHAR_    : printf("%d", (int)tok.yylval.num);
		    break;
    case STRING_  : printf("\"%s\"", tok.yylval.str);
		    break;
    case FLOAT_   : printf("%g", tok.yylval.dbl);
		    break;
    case INTEGER_ : printf("%" PRId64, tok.yylval.num);
		    break;
    case ATOM     : printf("%s", tok.yylval.str);
		    break;
    case LBRACK   : printf("LBRACK");
		    break;
    case LBRACE   : printf("LBRACE");
		    break;
    case LPAREN   : printf("LPAREN");
		    break;
    case RBRACK   : printf("RBRACK");
		    break;
    case RPAREN   : printf("RPAREN");
		    break;
    case RBRACE   : printf("RBRACE");
		    break;
    case PERIOD   : printf("PERIOD");
		    break;
    case SEMICOL  : printf("SEMICOL");
		    break;
    case LIBRA    : printf("LIBRA");
		    break;
    case EQDEF    : printf("EQDEF");
		    break;
    case HIDE     : printf("HIDE");
		    break;
    case IN       : printf("IN");
		    break;
    case END      : printf("END");
		    break;
    case MODULE   : printf("MODULE");
		    break;
    case JPRIVATE : printf("PRIVATE");
		    break;
    case JPUBLIC  : printf("PUBLIC");
		    break;
    }
    printf("\n");
}
#endif

/*
    Push a symbol into the tokenlist.
*/
static void push_symb(pEnv env)
{
    Token tok;

    tok.symb = env->symb;
    tok.yylval = env->yylval;
    vec_push(env->tokens, tok);
}

/*
    getsym - wrapper around my_getsym, storing tokens read, reading from the
	     store or just calling my_getsym itself. This allows tokens to be
	     read twice.

    After reading MODULE or JPRIVATE, read all tokens upto END, and include
    them in the tokenlist. All symbols preceding "==" are declared.
*/
PUBLIC void getsym(pEnv env)
{
    Token tok;
    int module = 0, private = 0, hide = 0, modl = 0, hcnt = 0;

/*
    If there is a tokenlist, extract tokens from there.
*/
    if (vec_size(env->tokens)) {
	tok = vec_pop(env->tokens);
#ifdef DUMP_TOKENS
	dumptok(tok, 1); /* tokens from the first pop */
#endif
	env->symb = tok.symb;
	env->yylval = tok.yylval;
	return;
    }
/*
    There is no tokenlist, use the normal procedure to get one.
*/
    my_getsym(env);
/*
    There is a token available, do some extra processing, in case the token is
    MODULE or HIDE: MODULE .. END or HIDE .. END.
*/
    if (env->symb == MODULE || env->symb == HIDE) {
/*
    Copy the global variables of modl.c into local variables.
*/
	savemod(&hide, &modl, &hcnt);
	do {
	    switch (env->symb) {
	    case MODULE	  : push_symb(env);
			    my_getsym(env);
			    if (env->symb == ATOM) {
				initmod(env, env->yylval.str);
				module++;
			    } else
				error(env, "atom expected as name of module");
			    break;
	    case HIDE	  :
	    case JPRIVATE : initpriv(env);
			    if (!module)
				private++;
			    break;
	    case IN	  :
	    case JPUBLIC  : stoppriv();
			    break;
	    case EQDEF	  : if (strchr(env->yylval.str, '.') == 0)
				env->yylval.str = classify(env,env->yylval.str);
			    enteratom(env);
			    break;
	    case PERIOD	  :
	    case END	  : if (module) {
				exitmod();
				module--;
			    } else if (private) {
				exitpriv();
				private--;
			    }
			    if (!module && !private)
				goto done;
			    break;
	    }
	    push_symb(env);
	    my_getsym(env);
	} while (1);
/*
    Restore the global variables in module.c from the local copies.
*/
done:	undomod(hide, modl, hcnt);
	push_symb(env); /* store the last symbol that was read */
	push_symb(env); /* extra sym for the benefit of reverse */
	vec_reverse(env->tokens);
    }
/*
    If there is a tokenlist, extract tokens from there.
*/
    if (vec_size(env->tokens)) {
	tok = vec_pop(env->tokens);
#ifdef DUMP_TOKENS
	dumptok(tok, 2); /* tokens from the second pop */
#endif
	env->symb = tok.symb;
	env->yylval = tok.yylval;
    } else {
#ifdef DUMP_TOKENS
	tok.symb = env->symb;
	tok.yylval = env->yylval;
	dumptok(tok, 3); /* there was no value popped */
#endif
    }
}
