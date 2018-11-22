#include"Header.h"
int main()
{
    FILE* fp=fopen("ami33.blocks","r");
    //Read_Hard_Blocks(fp);
    read_blocks_ami33(fp);
    return 0;
}
