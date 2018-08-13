#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE* fp=fopen("ami33.blocks","r");
    read_blocks_ami33(fp);
    return 0;
}
