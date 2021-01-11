/*
    module  : rand.c
    version : 1.1
    date    : 01/08/21
*/
unsigned _seed;

int my_rand(void)
{
    _seed = _seed * 1103515245 + 12345;
    return (_seed >> 16) & 0x7FFF;
}
