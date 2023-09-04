/*
    module  : manual.h
    version : 1.3
    date    : 09/04/23
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

PRIVATE void make_manual(int style /* 0=plain, 1=HTML, 2=Latex */)
{
    int i;
    char *n;

    if (HTML)
	printf("<HTML>\n<DL>\n");
    for (i = BOOLEAN_; (n = opername(i)) != 0; i++) {
	/* clang-format off */
	HEADER(n, " truth value type", "literal") else
	HEADER(n, "false", "operand") else
	HEADER(n, "id", "operator") else
	HEADER(n, "null", "predicate") else
	HEADER(n, "i", "combinator") else
	HEADER(n, "help", "miscellaneous commands") else
	HEADER(n, "casting", "additional commands")
	if (n[0] != '_') {
	    /* clang-format on */
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
		printf(" <CODE>      :  </CODE> ");
	    /* the above line does not produce the spaces around ":" */
	    else
		printf("\t:  ");
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
	}
    }
    if (HTML)
	printf("\n</DL>\n</HTML>\n");
}
#endif
