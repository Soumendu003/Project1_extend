#include"Header.h"
int main()
{
    FILE* fp=fopen("n300.blocks","r");
    Read_Hard_Blocks(fp);
    return 0;
}
