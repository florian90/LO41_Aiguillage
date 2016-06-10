
#include "structures.h"

int getPriorite(Type t)
{
    if(t == TGV)
        return 1;
    else if (t == GL)
        return 2;
    else
        return 3;
}
