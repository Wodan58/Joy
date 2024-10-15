/*
    module  : manual.h
    version : 1.9
    date    : 10/11/24
*/
#ifndef MANUAL_H
#define MANUAL_H

#define PLAIN (style == 0)
#define HTML (style == 1)
#define LATEX (style == 2)
#define HEADER(N, NAME, HEAD)						\
    if (strcmp(N, NAME) == 0) {						\
	printf("\n\n");							\
	if (HTML)							\
	    printf("<DT><BR><B>");					\
	if (LATEX)							\
	    printf("\\item[--- \\BX{");					\
	printf("%s", HEAD);						\
	if (LATEX)							\
	    printf("} ---] \\verb# #");					\
	if (HTML)							\
	    printf("</B><BR><BR>");					\
	printf("\n\n");							\
    }

void make_manual(int style)	/* 0=plain, 1=HTML, 2=Latex */
{
    char *n;
    int i, j;

    if (HTML)
	printf("<HTML>\n<DL>\n");
    for (i = BOOLEAN_, j = tablesize(); i < j; i++) {
	n = optable[i].name;
	HEADER(n, " truth value type", "literal") else
	HEADER(n, "false", "operand") else
	HEADER(n, "id", "operator") else
	HEADER(n, "null", "predicate") else
	HEADER(n, "i", "combinator") else
	HEADER(n, "help", "miscellaneous commands") else
	HEADER(n, "assign", "additional commands") else
	HEADER(n, "#genrec", "runtime commands")
#if 0
	if (n[0] != '_') {
#endif
	if (HTML)
	    printf("\n<DT>");
	else if (LATEX) {
	    if (n[0] == ' ') {
		n++;
		printf("\\item[\\BX{");
	    } else
		printf("\\item[\\JX{");
	}
	if (HTML && strcmp(n, "<=") == 0)
	    printf("&lt;=");
	else
	    printf("%s", n);
	if (LATEX)
	    printf("}]  \\verb#");
	if (HTML)
	    printf("<CODE>&nbsp;&nbsp;:&nbsp;&nbsp;</CODE>");
	/* the above line does produce the spaces around ":" */
	else
	    printf("  :  ");
	printf("%s", optable[i].messg1);
	if (HTML)
	    printf("\n<DD>");
	else if (LATEX)
	    printf("# \\\\ \n {\\small\\verb#");
	else
	    printf("\n");
	printf("%s", optable[i].messg2);
	if (LATEX)
	    printf("#}");
	printf("\n\n");
#if 0
	}
#endif
    }
    if (HTML)
	printf("\n</DL>\n</HTML>\n");
}
#endif
