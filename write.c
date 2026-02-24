/*
 *  module  : write.c
 *  version : 1.5
 *  date    : 02/23/26
 */
#include "globals.h"

/*
 * writechar - print a character to a string.
 */
void writechar(int ch, char *str)
{
    if (ch >= 8 && ch <= 13)
	sprintf(str, "'\\%c", "btnvfr"[ch - 8]);
    else if (iscntrl(ch) || ch == 32)
	sprintf(str, "'\\%03d", ch);
    else
	sprintf(str, "'%c", ch);
}

/*
 * writestring - print a character in a string to a string.
 */
void writestring(int ch, char *str)
{
    if (ch == '"')
	strcpy(str, "\\\"");
    else if (ch >= 8 && ch <= 13)
	sprintf(str, "\\%c", "btnvfr"[ch - 8]);
    else if (iscntrl(ch))
	sprintf(str, "\\%03d", ch);
    else
	sprintf(str, "%c", ch);
}

/*
 * writefactor - print a factor in readable format to stdout.
 */
void writefactor(pEnv env, Index n, FILE *fp)
{
    int i;
    uint64_t set, j;
    char *ptr, buf[MAXNUM], tmp[MAXNUM];

#if 0
/*
 * This cannot happen. writefactor has a small number of customers: writeterm,
 * main, put, fput. They all check that the stack is not empty, so this code
 * only serves as a reminder for future customers.
 */
    if (!n)
	execerror(env, "non-empty stack", "writefactor");
#endif
    switch (nodetype(n)) {
    case USR_:
	fprintf(fp, "%s", vec_at(env->symtab, nodevalue(n).ent).name);
	break;

    case ANON_FUNCT_:
	fprintf(fp, "%s", opername(operindex(env, nodevalue(n).proc)));
	break;

    case BOOLEAN_:
	fprintf(fp, "%s", nodevalue(n).num ? "true" : "false");
	break;

    case CHAR_:
	writechar(nodevalue(n).num, buf);
	fprintf(fp, "%s", buf);
	break;

    case INTEGER_:
	fprintf(fp, "%" PRId64, nodevalue(n).num);
	break;

    case SET_:
	putc('{', fp);
	for (i = 0, j = 1, set = nodevalue(n).set; i < SETSIZE; i++, j <<= 1)
	    if (set & j) {
		fprintf(fp, "%d", i);
		set &= ~j;
		if (set)
		    putc(' ', fp);
	    }
	putc('}', fp);
	break;

    case STRING_:
	putc('"', fp);
#ifdef NOBDW
	for (ptr = (char *)&nodevalue(n); *ptr; ptr++) {
#else
	for (ptr = nodevalue(n).str; *ptr; ptr++) {
#endif
	    writestring(*ptr, buf);
	    fprintf(fp, "%s", buf);
	}
	putc('"', fp);
	break;

    case LIST_:
	putc('[', fp);
	writeterm(env, nodevalue(n).lis, fp);
	putc(']', fp);
	break;

    case FLOAT_:
	snprintf(buf, MAXNUM, "%g", nodevalue(n).dbl); /* exponent char e */
	if (strchr(buf, '.') == 0) {		/* locate decimal point */
	    if ((ptr = strchr(buf, 'e')) == 0) {/* locate start of exponent */
		i = buf[strlen(buf) - 1];
		if (isdigit(i))			/* check digit present */
		    strcat(buf, ".0");		/* add decimal point and 0 */
	    } else {
		strcpy(tmp, ptr);		/* save exponent */
		strcpy(ptr, ".0");		/* add decimal point and 0 */
		strcat(buf, tmp);		/* restore exponent */
	    }
	}
	fprintf(fp, "%s", buf);
	break;

    case FILE_:
	if (!nodevalue(n).fil)
	    fprintf(fp, "NULL");
	else if (nodevalue(n).fil == stdin)
	    fprintf(fp, "stdin");
	else if (nodevalue(n).fil == stdout)
	    fprintf(fp, "stdout");
	else if (nodevalue(n).fil == stderr)
	    fprintf(fp, "stderr");
	else
	    fprintf(fp, "%p", (void *)nodevalue(n).fil);
	break;

    case BIGNUM_:
#ifdef NOBDW
	fprintf(fp, "%s", (char *)&nodevalue(n));
#else
	fprintf(fp, "%s", nodevalue(n).str);
#endif
	break;

    default:
	error("a factor cannot begin with this symbol");
	break;
    }
}

/*
 * writeterm - print the contents of a list in readable format to fp.
 */
void writeterm(pEnv env, Index n, FILE *fp)
{
    while (n) {
	writefactor(env, n, fp);
	POP(n);
	if (n)
	    putc(' ', fp);
    }
}
