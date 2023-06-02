/* FILE: scan.c */
/*
 *  module  : scan.c
 *  version : 1.39
 *  date    : 06/02/23
 */
#include "globals.h"

PUBLIC void quit_(pEnv env);

static struct {
    FILE *fp;
#if 0
    char *name;
#endif
    int linenum;
} infile[INPSTACKMAX];
static int ilevel;
static int linenumber;
static char linbuf[INPLINEMAX + 1];
static int linelength, currentcolumn;
#if 0
static int errorcount;
#endif
static int ch = ' ';
#ifdef READ_PRIVATE_AHEAD
static Symbol unget_symb;
#endif

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
#if 0
    infile[0].name = str ? str : "stdin";
#endif
}

/*
    putline - echo an input line.
*/
PRIVATE void putline(pEnv env)
{
    int i;

    if (env->echoflag > 2)
        printf("%4d", linenumber);
    if (env->echoflag > 1)
        putchar('\t');
    for (i = 0; linbuf[i] && linbuf[i] != '\n'; i++)
        putchar(linbuf[i]);
    putchar('\n');
}

/*
    getch - return one character in ch.
*/
PRIVATE void getch(pEnv env)
{
    int i;

    if (currentcolumn == linelength) {
#ifdef USE_SHELL_ESCAPE
again:
#endif
        currentcolumn = linelength = 0;
        if (linenumber >= 0)
            linenumber++;
        if (fgets(linbuf, INPLINEMAX, env->srcfile)) {
            for (i = 0; linbuf[i]; i++)
                ;
            linelength = i;
        } else if (ilevel > 0) {
            fclose(infile[ilevel--].fp);
            env->srcfile = infile[ilevel].fp;
            linenumber = infile[ilevel].linenum;
        } else
            quit_(env);
        linbuf[linelength++] = ' '; /* to help getsym for numbers */
        linbuf[linelength++] = 0;
        if (env->echoflag)
            putline(env);
#ifdef USE_SHELL_ESCAPE
        if (linbuf[0] == SHELLESCAPE) {
            system(&linbuf[1]);
            goto again;
        }
#endif
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
    error - error processing during source file reads.
*/
PUBLIC void error(pEnv env, char *message)
{
    int i;

    if (!env->echoflag)
        putline(env);
    if (env->echoflag > 1)
        putchar('\t');
    for (i = 0; i < currentcolumn - 2; i++)
        if (linbuf[i] <= ' ')
            putchar(linbuf[i]);
        else
            putchar(' ');
    printf("^\n\t%s\n", message);
    env->stck = 0;
#if 0
    errorcount++;
#endif
}

PRIVATE void my_include(pEnv env, char *filnam, FILE *fp)
{
    infile[++ilevel].fp = env->srcfile = fp;
#if 0
    infile[ilevel].name = filnam;
#endif
    infile[ilevel].linenum = linenumber = 0;
}

/*
    doinclude - insert the contents of a file in the input.
                Files are read in the current directory or if that fails
                from the same directory as where the executable is stored.
                If that path also fails an error is generated unless error
                is set to 0.
*/
PUBLIC void doinclude(pEnv env, char *filnam, int error)
{
    FILE *fp;
    char *ptr, *str;

    if (ilevel + 1 == INPSTACKMAX)
        execerror(env, "fewer include files", "include");
    infile[ilevel].fp = env->srcfile;
    infile[ilevel].linenum = linenumber;
    if ((ptr = strrchr(str = filnam, '/')) != 0) {
	str = GC_strdup(filnam);
        env->pathname = filnam; /* switch to new pathname */
        *ptr = 0;
    }
    if ((fp = fopen(str, "r")) != 0) {
        my_include(env, str, fp);
        return;
    }
/*
    Search the new include file in the same directory where the previous
    include file is located.
*/
#ifdef SEARCH_EXEC_DIRECTORY
    if (strcmp(env->pathname, ".")) {
        str = GC_malloc_atomic(strlen(env->pathname) + strlen(filnam) + 2);
        sprintf(str, "%s/%s", env->pathname, filnam);
        if ((fp = fopen(str, "r")) != 0) {
            my_include(env, str, fp);
            return;
        }
    }
#endif
    if (error)
        execerror(env, "valid file name", "include");
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
    char ident[ALEN], string[INPLINEMAX + 1];

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
                } else if (isdigit(next)) {
                    do
                        getch(env);
                    while (ch >= '0' && ch <= '7');
                    goto done;
                }
            }
            while (isdigit(ch))
                getch(env);
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
            env->yylval.num = strtol(&linbuf[begin], 0, 0);
            env->symb = INTEGER_;
            return;
        }
        goto not_unary_minus;
not_unary_minus:
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
            for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
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
PUBLIC void dumptok(Token tok, int num)
{
    fprintf(stdout, "%d) ", num);
    switch (tok.symb) {
    case CHAR_    : fprintf(stdout, "%ld", tok.yylval.num);
                    break;
    case STRING_  : fprintf(stdout, "\"%s\"", tok.yylval.str);
                    break;
    case FLOAT_   : fprintf(stdout, "%g", tok.yylval.dbl);
                    break;
    case INTEGER_ : fprintf(stdout, "%ld", tok.yylval.num);
                    break;
    case ATOM     : fprintf(stdout, "%s", tok.yylval.str);
                    break;
    case LBRACK   : fprintf(stdout, "LBRACK");
                    break;
    case LBRACE   : fprintf(stdout, "LBRACE");
                    break;
    case LPAREN   : fprintf(stdout, "LPAREN");
                    break;
    case RBRACK   : fprintf(stdout, "RBRACK");
                    break;
    case RPAREN   : fprintf(stdout, "RPAREN");
                    break;
    case RBRACE   : fprintf(stdout, "RBRACE");
                    break;
    case PERIOD   : fprintf(stdout, "PERIOD");
                    break;
    case SEMICOL  : fprintf(stdout, "SEMICOL");
                    break;
    case LIBRA    : fprintf(stdout, "LIBRA");
                    break;
    case EQDEF    : fprintf(stdout, "EQDEF");
                    break;
    case HIDE     : fprintf(stdout, "HIDE");
                    break;
    case IN       : fprintf(stdout, "IN");
                    break;
    case END      : fprintf(stdout, "END");
                    break;
    case MODULE   : fprintf(stdout, "MODULE");
                    break;
    case JPRIVATE : fprintf(stdout, "PRIVATE");
                    break;
    case JPUBLIC  : fprintf(stdout, "PUBLIC");
                    break;
    }
    fprintf(stdout, "\n");
}
#endif

/*
    ungetsym - insert a symbol in the input stream. The symbol has already been
               read, but needs to be read again, because another symbol must be
               processed first.
*/
#ifdef READ_PRIVATE_AHEAD
PUBLIC void ungetsym(Symbol symb)
{
    unget_symb = symb;
}
#endif

/*
    getsym - wrapper around my_getsym, storing tokens read, reading from the
             store or just calling my_getsym itself.
*/
PUBLIC void getsym(pEnv env)
{
    Token tok;

#ifdef READ_PRIVATE_AHEAD
    if (unget_symb) {
        env->symb = unget_symb;
        unget_symb = 0;
        return;
    }
#endif
    if (env->token_list) {
        my_getsym(env);
        tok.yylval = env->yylval;
        tok.symb = env->symb;
#ifdef DEBUG_TOKENS
        dumptok(tok, 1);
#endif
        vec_push(env->tokens, tok);
    } else if (env->token_index < vec_size(env->tokens)) {
        tok = vec_at(env->tokens, env->token_index);
        env->yylval = tok.yylval;
        env->symb = tok.symb;
#ifdef DEBUG_TOKENS
        dumptok(tok, 2);
#endif
        env->token_index++;
    } else {
        env->tokens = 0; /* reset token vector and index */
        env->token_index = 0;
        my_getsym(env);
#ifdef DEBUG_TOKENS
        tok.yylval = env->yylval;
        tok.symb = env->symb;
        dumptok(tok, 3);
#endif
    }
}
