#include"Header.h"
void Initial_Partition(FILE* fp1,Block* bk_list,Net* net_list,int B,int N,int T,float relax)
{
    int i,j;
    double avg_area,tot_area=Calculate_Total_Area(bk_list,B);
    printf("\n TOTAL AREA=%lf",tot_area);
    avg_area=tot_area/T;
    printf("\n AVG AREA=%lf",avg_area);
    fprintf(fp1,"\n***********************************************************************************************");
    fprintf(fp1,"\n");
    fprintf(fp1,"\n***********************************************************************************************");
    fprintf(fp1,"\n No of Tiers=%d\t Relaxation Percentage=%f",T,relax);
    fprintf(fp1,"\n Avg area for each tier=%lf",avg_area);
    fprintf(fp1,"\n Max area for each tier=%lf",avg_area*(1+relax));
    fprintf(fp1,"\n Min area for each tier=%lf",avg_area*(1-relax));
    Tier* tier_list=(Tier*)calloc(T,sizeof(Tier));
    //Initializes Tier Components
    for(i=0;i<T;i++)
    {
        tier_list[i].area_consumed=0;
        tier_list[i].max_area=avg_area*(1+relax);
        tier_list[i].min_area=avg_area*(1-relax);
        tier_list[i].tot_bk=0;
    }
    initialize_net_list(net_list,N,T);
    default_blocks_placement(bk_list,B);
    int cnt=0;
    int tier_cnt=0;
    int flag=0;
    int** Degree=(int**)calloc(B,sizeof(int*));
    for(i=0;i<B;i++)
    {
        Degree[i]=(int*)calloc(B,sizeof(int));
    }
    clock_t start_time=clock();
    /*for(i=0;i<N;i++)
    {
        Block_Component* tem=net_list[i].bk_ptr;
        while(tem!=NULL)
        {
            int bk_index=tem->bk_index;
            if(bk_list[bk_index].tier==-1)
            {
                place_block(tier_list,bk_list,bk_index,tier_cnt,bk_list[bk_index].tier); //place_blocks returns 1 if it successfully places the bk at tier_cn
                tier_cnt++;
                tier_cnt=(tier_cnt%T);
                cnt++;
                if(cnt>=B)
                {
                    flag=1;               //indicates that all the blocks are placed
                    break;
                }
            }
            tem=tem->right;
        }
        if(flag!=0)
        {
            break;
        }
    }*/
    printf("\n Implementing Modified Initial Partitioning");
    for(i=0;i<B;i++)
    {
        Net_Component* tem=bk_list[i].net_ptr;
        while(tem!=NULL)
        {
            Block_Component* adj=net_list[tem->net_index].bk_ptr;
            while(adj!=NULL)
            {
                j=adj->bk_index;
                if(i!=j)
                {
                    Degree[i][j]+=1;
                }
                adj=adj->right;
            }
            tem=tem->right;
        }
    }
    Block_Component* header=(Block_Component*)calloc(1,sizeof(Block_Component));
    for(i=0;i<B;i++)
    {
        initial_block_placement(Degree,tier_list,bk_list,header,B,T);
        //printf("\n Initial block placement done %dth time",i+1);
    }
    clock_t end_time=clock();
    printf("\n Initial Partitioning Done");
    custom_update_net_list(net_list,bk_list,N,B,T);
    /*FILE* fp=fopen("Net_list_after_initial_partition_ami49.txt","w+");
    for(i=0;i<N;i++)
    {
        fprintf(fp,"\nThe Net Degree=%d\t Number of Blocks=%d",net_list[i].degree,net_list[i].no_of_bk);
        fprintf(fp,"\nGND pin=%d\tPOW pin=%d",net_list[i].gnd,net_list[i].pwr);
        fprintf(fp,"\nTop_tier=%d\t Low_Tier=%d",net_list[i].top_tier.tier_index,net_list[i].low_tier.tier_index);
    }
    fclose(fp);*/
    //FILE* fp=fopen("Block_initial_placement_ami49.txt","w+");
    double time_taken=(double)(end_time-start_time)/CLOCKS_PER_SEC;
    fprintf(fp1,"\nTime Taken to execute Initial Partition:%0.6lf",time_taken);
    fprintf(fp1,"\nAfter Initial Partition Total Number of MIV=%d",claculate_MIV(bk_list,net_list,N,T));
    /*for(i=0;i<B;i++)
    {
        fprintf(fp,"\n Block_Name=%s\t Block_Index=%d\t Block_Tier=%d",bk_list[i].name,bk_list[i].index,bk_list[i].tier);
    }
    fclose(fp);*/
    printf("\n Calling Secondary Partition:");
    Secondary_Partition(fp1,Degree,net_list,bk_list,tier_list,N,B,T);
    free(tier_list);
    printf("\n Tier list freed");

}
