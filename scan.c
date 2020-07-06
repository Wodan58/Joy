/* FILE: scan.c */
/*
 *  module  : scan.c
 *  version : 1.18.1.3
 *  date    : 06/22/20
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

static struct {
    FILE* fp;
    char* name;
    int linenum;
} infile[INPSTACKMAX];
static int ilevel = 0;
static int linenumber = 0;
static char linbuf[INPLINEMAX + 1];
static int linelength = 0, currentcolumn = 0;
#if 0
static int errorcount = 0;
#endif
static int ch = ' ';

PUBLIC void inilinebuffer(char* str)
{
    infile[0].fp = srcfile;
    infile[0].name = str;
}

PUBLIC int getlinenum(void)
{
    int linenum;

    linenum = linenumber;
    linenumber = -1;
    return linenum;
}

PUBLIC void resetlinebuffer(int linenum)
{
    linenumber = linenum;
    linbuf[0] = 0;
    linelength = 0;
    currentcolumn = 0;
    ch = ' ';
}

PRIVATE void putline(void)
{
    int i;

    if (linenumber <= 0)
        return;
    if (echoflag > 2)
        printf("%4d", linenumber);
    if (echoflag > 1)
        putchar('\t');
    for (i = 0; linbuf[i] && linbuf[i] != '\n'; i++)
        putchar(linbuf[i]);
    putchar('\n');
}

PRIVATE void getch(void)
{
    if (currentcolumn == linelength) {
#ifdef USE_SHELL_ESCAPE
    Again:
#endif
        currentcolumn = linelength = 0;
        if (linenumber >= 0)
            linenumber++;
        if (fgets(linbuf, INPLINEMAX, srcfile))
            linelength = strlen(linbuf);
        else if (ilevel > 0) {
            fclose(infile[ilevel--].fp);
            srcfile = infile[ilevel].fp;
            linenumber = infile[ilevel].linenum;
        } else
            quit_();
        linbuf[linelength++] = ' '; /* to help getsym for numbers */
        linbuf[linelength++] = '\0';
        if (echoflag)
            putline();
#ifdef USE_SHELL_ESCAPE
        if (linbuf[0] == SHELLESCAPE) {
            system(&linbuf[1]);
            goto Again;
        }
#endif
    }
    ch = linbuf[currentcolumn++];
}

PRIVATE int endofbuffer(void) { return currentcolumn == linelength; }

PUBLIC void error(char* message)
{
    int i;

    putline();
    if (echoflag > 1)
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

PUBLIC int doinclude(char* filnam)
{
    FILE* fp;

    if (ilevel + 1 == INPSTACKMAX)
        execerror("fewer include files", "include");
    infile[ilevel].fp = srcfile;
    infile[ilevel].linenum = linenumber;
    linenumber = 0;
    if ((fp = fopen(filnam, "r")) != 0) {
        infile[++ilevel].fp = srcfile = fp;
        infile[ilevel].name = filnam;
        infile[ilevel].linenum = 0;
        return 1;
    }
    execerror("valid file name", "include");
    return -1; /* not reached */
}

#ifdef FGET_FROM_FILE
PUBLIC void redirect(FILE* fp)
{
    if (infile[ilevel].fp == fp)
	return;
    if (ilevel + 1 == INPSTACKMAX)
        execerror("fewer include files", "redirect");
    infile[++ilevel].fp = srcfile = fp;
    infile[ilevel].name = 0;
    infile[ilevel].linenum = 0;
}
#endif

char* my_strdup(char* str)
{
    char* ptr;
    size_t leng;

    leng = strlen(str);
    if ((ptr = malloc(leng + 1)) != 0)
        strcpy(ptr, str);
    return ptr;
}

PRIVATE int specialchar(void)
{
    getch();
    switch (ch) {
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case 'b':
        return '\b';
    case 'r':
        return '\r';
    case 'f':
        return '\f';
    case '\'':
        return '\'';
    case '\"':
        return '\"';
#ifdef REST_OF_UNIX_ESCAPES
    case 'v':
        return '\v';
    case '\\':
        return '\\';
#endif
    default:
        if (isdigit(ch)) {
            int i, num = ch - '0';
            for (i = 0; i < 2; i++) {
                getch();
                if (!isdigit(ch)) {
                    currentcolumn++; /* to get pointer OK */
                    error("digit expected");
                    currentcolumn--;
                }
                num = 10 * num + ch - '0';
            }
            return num;
        }
        return ch;
    }
}

PUBLIC void HashValue(char* s)
{
    for (hashvalue = 0; *s != '\0'; hashvalue += *s++)
        ;
    hashvalue %= HASHSIZE;
}

PRIVATE int peek(void) { return linbuf[currentcolumn]; }

PUBLIC void getsym(void)
{
    int i = 0;
    char string[INPLINEMAX];

Start:
    while (ch <= ' ')
        getch();
    switch (ch) {
    case '(':
        getch();
        if (ch == '*') {
            getch();
            do {
                while (ch != '*')
                    getch();
                getch();
            } while (ch != ')');
            getch();
            goto Start;
        }
        symb = LPAREN;
        return;
    case '#':
        currentcolumn = linelength;
        getch();
        goto Start;
    case ')':
        symb = RPAREN;
        getch();
        return;
    case '[':
        symb = LBRACK;
        getch();
        return;
    case ']':
        symb = RBRACK;
        getch();
        return;
    case '{':
        symb = LBRACE;
        getch();
        return;
    case '}':
        symb = RBRACE;
        getch();
        return;
    case '.':
        symb = PERIOD;
        getch();
        return;
    case ';':
        symb = SEMICOL;
        getch();
        return;
    case '\'':
        getch();
        if (ch == '\\')
            ch = specialchar();
        numb = ch;
        symb = CHAR_;
        getch();
        return;
    case '"':
        getch();
        while (ch != '"' && !endofbuffer()) {
            if (ch == '\\')
                ch = specialchar();
            string[i++] = ch;
            getch();
        }
        string[i] = '\0';
        getch();
        strg = my_strdup(string);
        symb = STRING_;
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
            int start = currentcolumn - 1, next;
            if (ch == '-')
                getch();
            else if (ch == '0') {
                if ((next = peek()) == 'x' || next == 'X') {
                    getch();
                    do
                        getch();
                    while (isxdigit(ch));
                    goto done;
                } else if (isdigit(next)) {
                    do
                        getch();
                    while (ch >= '0' && ch <= '7');
                    goto done;
                }
            }
            while (isdigit(ch))
                getch();
            if (ch == '.' && isdigit(peek())) {
                do
                    getch();
                while (isdigit(ch));
                if (ch == 'e' || ch == 'E') {
                    getch();
                    if (ch == '-' || ch == '+')
                        getch();
                    while (isdigit(ch))
                        getch();
                }
                dblf = strtod(&linbuf[start], NULL);
                symb = FLOAT_;
                return;
            }
        done:
#ifdef BIT_32
            numb = strtol(&linbuf[start], NULL, 0);
#else
            numb = strtoll(&linbuf[start], NULL, 0);
#endif
            symb = INTEGER_;
            return;
        }
    /* ELSE '-' is not unary minus, fall through */
    default:
        do {
            if (i < ALEN - 1)
                ident[i++] = ch;
            getch();
        } while (isalnum(ch) || strchr("-=_", ch));
        ident[i] = '\0';
        /* ensure same algorithm in inisymtab */
        HashValue(ident);
        if (isupper((int)ident[1])) {
            if (strcmp(ident, "LIBRA") == 0 || strcmp(ident, "DEFINE") == 0) {
                symb = LIBRA;
                return;
            }
            if (strcmp(ident, "HIDE") == 0) {
                symb = HIDE;
                return;
            }
            if (strcmp(ident, "IN") == 0) {
                symb = IN;
                return;
            }
            if (strcmp(ident, "END") == 0) {
                symb = END;
                return;
            }
            if (strcmp(ident, "MODULE") == 0) {
                symb = MODULE;
                return;
            }
            if (strcmp(ident, "PRIVATE") == 0) {
                symb = JPRIVATE;
                return;
            }
            if (strcmp(ident, "PUBLIC") == 0) {
                symb = JPUBLIC;
                return;
            }
            /* possibly other uppers here */
        }
        if (strcmp(ident, "==") == 0) {
            symb = EQDEF;
            return;
        }
        if (strcmp(ident, "true") == 0) {
            symb = BOOLEAN_;
            numb = 1;
            return;
        }
        if (strcmp(ident, "false") == 0) {
            symb = BOOLEAN_;
            numb = 0;
            return;
        }
        if (strcmp(ident, "maxint") == 0) {
            symb = INTEGER_;
            numb = MAXINT;
            return;
        }
        symb = ATOM;
        return;
    }
}
