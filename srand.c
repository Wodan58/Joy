/*
    module  : srand.c
    version : 1.1
    date    : 01/08/21
*/
extern unsigned _seed;

void my_srand(unsigned num)
{
    _seed = num;
}
