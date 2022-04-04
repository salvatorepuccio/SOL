#include <stdlib.h>
#include <stdio.h>
#include "stringsmanager.h"

int main(){

    char *string = NULL; int ret;

    ret = mystrcat(&string,"uno//",0,1);
    printf("1: '%s' dim: %d\n",string, ret);


    ret = mystrcat(&string,"due//",0,1);
    printf("2: '%s' dim: %d\n",string, ret);

    ret = mystrcat(&string,"tre3//",3,1);
    printf("3: '%s' dim: %d\n",string, ret);

    return 0;

}