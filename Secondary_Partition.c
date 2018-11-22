#include"Header.h"
void Secondary_Partition(FILE* fp1,int** Degree,Net* net_list,Block* bk_list,Tier* tr_list,int N,int B,int T)
{
    int i,j;
    float** Cost=(float**)calloc(B,sizeof(float*));
    printf("\n Cost Declared");
    calculate_net_distribution(net_list,bk_list,B,N,T);
    printf("\n Net Distribution Done");
    for(i=0;i<B;i++)
    {
        bk_list[i].Current_Entropy=calculate_block_entropy(bk_list,net_list,i,bk_list[i].tier,T);
    }
    printf("\n Current Entropy calculated");
    for(j=0;j<B;j++)
    {
        Cost[j]=(float*)calloc(T,sizeof(float));
    }
    printf("\n All Cost rows Declared");
    for(i=0;i<B;i++)
    {
        for(j=0;j<T;j++)
        {
            Cost[i][j]=calculate_block_entropy(bk_list,net_list,i,j,T);
        }
    }
    printf("\n Entropy Cost calculated");
    Gain* gain_list=(Gain*)calloc(T*B,sizeof(Gain));
    //create_and_link_gain_list(gain_list,bk_list,T,B);
    calculate_gain_list(Cost,gain_list,bk_list,B,T);
    /*FILE* fp=fopen("test_Gain_list_ami49.txt","w");
    k=0;
    for(i=0;i<B;i++)
    {
        fprintf(fp,"\nFor Block %d :",i);
        for(j=0;j<T;j++)
        {
            fprintf(fp,"\n\t\tTier=%d\tGain=%d",j,gain_list[k].gain_value);
            k++;
        }
    }
    fclose(fp);*/
    printf("\n Going to call CMFM");
    Compromized_FM(fp1,Degree,Cost,gain_list,bk_list,net_list,tr_list,B,N,T);
    printf("\n Gain_list going to be freed");
    free(gain_list);
    for(i=0;i<B;i++)
    {
        free(Cost[i]);
    }
    free(Cost);
}
