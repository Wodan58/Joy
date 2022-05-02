/* FILE: scan.c */
/*
 *  module  : scan.c
 *  version : 1.34
 *  date    : 05/02/22
 */
#include "globals.h"

PUBLIC void quit_(pEnv env);

static struct {
    FILE *fp;
#if 0
    char name[ALEN];
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

/*
    inilinebuffer - initialise the stack of input files. The filename parameter
                    is currently not used. It could be used in error messages.
*/
PUBLIC void inilinebuffer(pEnv env, char *str)
{
    infile[0].fp = env->srcfile;
#if 0
    if (!str)
        strcpy(infile[0].name, "stdin");
    else {
        strncpy(infile[0].name, str, ALEN);
        infile[0].name[ALEN - 1] = 0;
    }
#endif
}

#ifdef READ_PRIVATE_AHEAD
/*
    getlinenum - return the linenumber before reading a section the first time.
                 echoing the lines read is suppressed, because they will be
                 read again, in the normal case.
*/
PUBLIC int getlinenum(void)
{
    int linenum;

    linenum = linenumber;
    linenumber = -1;
    return linenum;
}

/*
    resetlinebuffer - revert to an earlier line, when rereading the source file.
*/
PUBLIC void resetlinebuffer(int linenum)
{
    linenumber = linenum;
    linbuf[0] = 0;
    linelength = 0;
    currentcolumn = 0;
    ch = ' ';
}
#endif

/*
    putline - echo an input line.
*/
PRIVATE void putline(pEnv env)
{
    int i;

    if (linenumber <= 0)
        return;
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

    putline(env);
    if (env->echoflag > 1)
        putchar('\t');
    for (i = 0; i < currentcolumn - 2; i++)
        if (linbuf[i] <= ' ')
            putchar(linbuf[i]);
        else
            putchar(' ');
    printf("^\n\t%s\n", message);
#if 0
    errorcount++;
#endif
}

PRIVATE void my_include(pEnv env, char *filnam, FILE *fp)
{
    infile[++ilevel].fp = env->srcfile = fp;
#if 0
    strncpy(infile[ilevel].name, filnam, ALEN);
    infile[ilevel].name[ALEN - 1] = 0;
#endif
    infile[ilevel].linenum = 0;
}

/*
    doinclude - insert the contents of a file in the input.
                Files are read in the current directory or if that fails
                from the same directory as argv[0]. This argv[0] contains
                a filename parameter, or the JOY executable. If that path
                also fails an error is generated unless error is set to 0.
                Reading the directory from argv[0] was added in order to
                supports tests during out-of-source builds.
*/
PUBLIC void doinclude(pEnv env, char *filnam, int error)
{
    FILE *fp;
#ifdef SEARCH_ARGV0_DIRECTORY
    int leng;
    char *path, *str;
#endif

    if (ilevel + 1 == INPSTACKMAX)
        execerror("fewer include files", "include");
    infile[ilevel].fp = env->srcfile;
    infile[ilevel].linenum = linenumber;
    linenumber = 0;
    if ((fp = fopen(filnam, "r")) != 0) {
        my_include(env, filnam, fp);
        return;
    }
#ifdef SEARCH_ARGV0_DIRECTORY
    if ((path = strrchr(env->g_argv[0], '/')) != 0) {
        leng = path - env->g_argv[0];
        str = GC_malloc_atomic(leng + strlen(filnam) + 2);
        sprintf(str, "%.*s/%s", leng, env->g_argv[0], filnam);
        if ((fp = fopen(str, "r")) != 0) {
            my_include(env, filnam, fp);
            return;
        }
    }
#endif
    if (error)
        execerror("valid file name", "include");
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
    getsym - lexical analyzer, filling env->yylval and returning the token type
             in env->symb.
*/
PUBLIC void getsym(pEnv env)
{
    int i = 0, begin, next;
    char string[INPLINEMAX + 1];

#ifdef READ_PRIVATE_AHEAD
    if (unget_symb) {
        env->symb = unget_symb;
        unget_symb = 0;
        return;
    }
#endif
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
            env->yylval.num = strtol(&linbuf[begin], 0, 0); /* BIT_32 */
            env->symb = INTEGER_;
            return;
        }
        goto not_unary_minus;
not_unary_minus:
    /* ELSE '-' is not unary minus, fall through */
    default:
        do {
            if (i < ALEN - 1)
                env->ident[i++] = ch;
            getch(env);
        } while (isalnum(ch) || strchr("-=_", ch));
        if (ch == '.') {
            next = peek();
            if (isalnum(next) || strchr("-=_", next)) {
                do {
                    if (i < ALEN - 1)
                        env->ident[i++] = ch;
                    getch(env);
                } while (isalnum(ch) || strchr("-=_", ch));
            }
        }
        env->ident[i] = 0;
        if (isupper((int)env->ident[1])) {
            if (!strcmp(env->ident, "LIBRA") || !strcmp(env->ident, "DEFINE")) {
                env->symb = LIBRA;
                return;
            }
            if (!strcmp(env->ident, "HIDE")) {
                env->symb = HIDE;
                return;
            }
            if (!strcmp(env->ident, "IN")) {
                env->symb = IN;
                return;
            }
            if (!strcmp(env->ident, "END")) {
                env->symb = END;
                return;
            }
            if (!strcmp(env->ident, "MODULE")) {
                env->symb = MODULE;
                return;
            }
            if (!strcmp(env->ident, "PRIVATE")) {
                env->symb = JPRIVATE;
                return;
            }
            if (!strcmp(env->ident, "PUBLIC")) {
                env->symb = JPUBLIC;
                return;
            }
            /* possibly other uppers here */
        }
        if (!strcmp(env->ident, "==")) {
            env->symb = EQDEF;
            return;
        }
        if (!strcmp(env->ident, "true")) {
            env->symb = BOOLEAN_;
            env->yylval.num = 1;
            return;
        }
        if (!strcmp(env->ident, "false")) {
            env->symb = BOOLEAN_;
            env->yylval.num = 0;
            return;
        }
        if (!strcmp(env->ident, "maxint")) {
            env->symb = INTEGER_;
            env->yylval.num = MAXINT;
            return;
        }
        env->symb = ATOM;
        return;
    }
}
