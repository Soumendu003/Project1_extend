#include"Header.h"
void Compromized_FM(FILE* fp1,float** Cost,Gain* gain_list,Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T)
{
    int i,j,ret_index;
    int* heap_size=(int*)calloc(1,sizeof(int));
    heap_size[0]=T*B-1;
    build_gain_heap(gain_list,bk_list,heap_size[0]);
    ret_index=Extract_Heap(gain_list,bk_list,heap_size);
    Gain ele=gain_list[ret_index];
    clock_t start_time=clock();
    printf("\n Element Gain=%lf",ele.gain_value);
    while(ret_index>=0 && ele.gain_value>0)
    {
        int pre_tier=bk_list[ele.tier_index].tier;
        if(place_block(tier_list,bk_list,ele.bk_index,ele.tier_index,pre_tier))
        {
            Net_Component* tem=bk_list[ele.bk_index].net_ptr;
            while(tem!=NULL)
            {
                update_net_list(net_list,bk_list,tem->net_index,ele.bk_index,ele.tier_index,pre_tier);
                tem=tem->right;
            }
            calculate_net_distribution(net_list,bk_list,B,N,T);
            for(i=0;i<B;i++)
            {
                bk_list[i].Current_Entropy=calculate_block_entropy(bk_list,net_list,i,bk_list[i].tier,T);
            }
            for(i=0;i<B;i++)
            {
                for(j=0;j<T;j++)
                {
                    Cost[i][j]=calculate_block_entropy(bk_list,net_list,i,j,T);
                }
            }
            calculate_gain_list(Cost,gain_list,bk_list,B,T);
            heap_size[0]=T*B-1;
            build_gain_heap(gain_list,bk_list,heap_size[0]);
        }
        ret_index=Extract_Heap(gain_list,bk_list,heap_size);
        if(ret_index<0)
        {
            break;
        }
        ele=gain_list[ret_index];
        printf("\n Element Gain=%lf",ele.gain_value);
    }
    clock_t end_time=clock();
    printf("\n Placements Done");
    fprintf(fp1,"\nAt End Total Number of MIV after Compromised FM=%d",claculate_MIV(bk_list,net_list,N,T));
    double time_taken=(double)(end_time-start_time)/CLOCKS_PER_SEC;
    fprintf(fp1,"\nTime Taken to execute Compromized FM:%0.6lf",time_taken);
    start_time=clock();
    Area_coverage(bk_list,net_list,tier_list,B,N,T);
    end_time=clock();
    fprintf(fp1,"\nAt End Total Number of MIV after Min area coverage=%d",claculate_MIV(bk_list,net_list,N,T));
    time_taken=(double)(end_time-start_time)/CLOCKS_PER_SEC;
    fprintf(fp1,"\nTime Taken to execute Area Coverage:%0.6lf",time_taken);
   /* FILE* fp=fopen("Final_Block_Placement.txt","w");
    for(i=0;i<B;i++)
    {
        fprintf(fp,"\n Block_Name=%s\t Block_Index=%d\t Block_Tier=%d",bk_list[i].name,bk_list[i].index,bk_list[i].tier);
    }
    fclose(fp);*/
    double tot_area=0;
    for(i=0;i<B;i++)
    {
        tot_area+=bk_list[i].area;
    }
    fprintf(fp1,"\n\n Total Area of all the blocks =%lf\n",tot_area);
    tot_area=0;
    double divergence=0,avg_area=(tier_list[0].max_area+tier_list[0].min_area)/2;
    for(i=0;i<T;i++)
    {
        tot_area+=tier_list[i].area_consumed;
        divergence=(tier_list[i].area_consumed-avg_area)/avg_area;
        fprintf(fp1,"\n Divergence of tier area from avg area=%lf",divergence);
    }
    fprintf(fp1,"\n\nTotal area of tiers=%lf\n",tot_area);
    double TWL=0;
    double hwl=0;
    for(i=0;i<T;i++)
    {
        for(j=0;j<N;j++)
        {
            hwl=0;
            Block_Component* tem=net_list[j].bk_ptr;
            while(tem!=NULL)
            {
                if(bk_list[tem->bk_index].tier==i)
                {
                    //printf("\n Block No=%d\t Net_No=%d\tTier_No=%d",tem->bk_index,j,i);
                    hwl+=(bk_list[tem->bk_index].length+bk_list[tem->bk_index].width);

                }
                tem=tem->right;
            }
            //printf("\n HWL=%lf",hwl);
            TWL+=(hwl)*(hwl);
        }
    }
    fprintf(fp1,"\n\n TWL Calculated =%lf\n",TWL );
}

void build_gain_heap(Gain* gain_list,Block* bk_list,int last_index)
{
    int i;
    for(i=(last_index-1)/2;i>=0;i--)
    {
        Max_Heapify_Gain(gain_list,bk_list,i,last_index);
    }
    return;
}

void Max_Heapify_Gain(Gain* gain_list,Block* bk_list,int ele_index,int last_index)
{
    int j;
    j=ele_index*2+1;
    while(j<=last_index)
    {
        if(j+1<=last_index)
        {
            if(gain_list[j+1].gain_value>gain_list[j].gain_value)
            {
                j=j+1;
            }
            else if(gain_list[j+1].gain_value==gain_list[j].gain_value)
            {
                if(bk_list[gain_list[j+1].bk_index].bk_degree>bk_list[gain_list[j].bk_index].bk_degree)
                {
                    j=j+1;
                }
            }
        }
        if(gain_list[ele_index].gain_value<gain_list[j].gain_value)
        {
            Gain tem=gain_list[ele_index];
            gain_list[ele_index]=gain_list[j];
            gain_list[j]=tem;
            gain_list[ele_index].current_index=j;
            gain_list[j].current_index=ele_index;
            ele_index=j;
            j=ele_index*2+1;
        }
        else if(gain_list[ele_index].gain_value==gain_list[j].gain_value)
        {
            if(bk_list[gain_list[ele_index].bk_index].bk_degree<bk_list[gain_list[j].bk_index].bk_degree)
            {
                Gain tem=gain_list[ele_index];
                gain_list[ele_index]=gain_list[j];
                gain_list[j]=tem;
                gain_list[ele_index].current_index=j;
                gain_list[j].current_index=ele_index;
            }
            return;
        }
        else{
            return;
        }
    }
}

int Extract_Heap(Gain* gain_list,Block* bk_list,int* heap_size)
{
    if(heap_size[0]<0)
    {
        return -1;
    }
    Gain ret_val=gain_list[0];
    gain_list[0]=gain_list[heap_size[0]];
    gain_list[0].current_index=0;
    gain_list[heap_size[0]]=ret_val;
    gain_list[heap_size[0]].current_index=heap_size[0];
    heap_size[0]-=1;
    Max_Heapify_Gain(gain_list,bk_list,0,heap_size[0]);
    return (heap_size[0]+1);
}

void Area_coverage(Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T)
{
    int i;
    for(i=0;i<T;i++)
    {
        if(tier_list[i].area_consumed<tier_list[i].min_area)
        {
            Min_Area_Coverage(bk_list,net_list,tier_list,B,N,T,i);
        }
    }
}
/*        while(tier_list[i].area_consumed>tier_list[i].max_area || tier_list[i].area_consumed<tier_list[i].min_area)
        {
            calculate_net_distribution(net_list,bk_list,B,N,T);
            for(j=0;j<B;j++)
            {
                bk_list[j].Current_Entropy=calculate_block_entropy(bk_list,net_list,j,bk_list[i].tier,T);
            }
            for(j=0;j<B;j++)
            {
                Cost[j]=calculate_block_entropy(bk_list,net_list,j,i,T);
            }
            calculate_tier_gain_list(Cost,bk_list,gain_list,B);
            if(tier_list[i].area_consumed>tier_list[i].max_area)
            {

            }
        }
    }
}
*/

void Min_Area_Coverage(Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T,int tier_no)
{
    int j;
    int* heap_size=(int*)calloc(1,sizeof(int));
    float* Cost=(float*)calloc(B,sizeof(float));
    Gain* gain_list=(Gain*)calloc(B,sizeof(Gain));
    calculate_net_distribution(net_list,bk_list,B,N,T);
    for(j=0;j<B;j++)
    {
        bk_list[j].Current_Entropy=calculate_block_entropy(bk_list,net_list,j,bk_list[j].tier,T);
    }
    for(j=0;j<B;j++)
    {
        Cost[j]=calculate_block_entropy(bk_list,net_list,j,tier_no,T);
    }
    calculate_tier_gain_list(Cost,bk_list,gain_list,B);
    heap_size[0]=B-1;
    build_gain_heap(gain_list,bk_list,heap_size[0]);
    int ret_index=Extract_Heap(gain_list,bk_list,heap_size);
    Gain ele=gain_list[ret_index];
    while(tier_list[tier_no].area_consumed<tier_list[tier_no].min_area && ret_index>=0)
    {
        int pre_tier=bk_list[ele.bk_index].tier;
        if(pre_tier!=tier_no && tier_list[pre_tier].area_consumed-bk_list[ele.bk_index].area>=tier_list[pre_tier].min_area)
        {
            place_block(tier_list,bk_list,ele.bk_index,tier_no,pre_tier);
            Net_Component* tem=bk_list[ele.bk_index].net_ptr;
            while(tem!=NULL)
            {
                update_net_list(net_list,bk_list,tem->net_index,ele.bk_index,tier_no,pre_tier);
                tem=tem->right;
            }
            calculate_net_distribution(net_list,bk_list,B,N,T);
            for(j=0;j<B;j++)
            {
                bk_list[j].Current_Entropy=calculate_block_entropy(bk_list,net_list,j,bk_list[j].tier,T);
            }
            for(j=0;j<B;j++)
            {
                Cost[j]=calculate_block_entropy(bk_list,net_list,j,tier_no,T);
            }
            calculate_tier_gain_list(Cost,bk_list,gain_list,B);
            heap_size[0]=B-1;
            build_gain_heap(gain_list,bk_list,heap_size[0]);
        }
        ret_index=Extract_Heap(gain_list,bk_list,heap_size);
        if(ret_index<0)
        {
            break;
        }
        ele=gain_list[ret_index];
    }
}


