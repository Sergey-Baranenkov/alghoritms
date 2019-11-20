#include "misc.h"
#include <string.h>

int isValue(char * str)
{
    for(int i = 0; i < (int)strlen(str); i++)
    {
        if(!isdigit(str[i])) return 0;
    }
    return 1;
}

