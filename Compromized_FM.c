#include"Header.h"
void Compromized_FM(FILE* fp1,int ** Degree,float** Cost,Gain* gain_list,Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T)
{
    int i,j,ret_index;
    int* heap_size=(int*)calloc(1,sizeof(int));
    heap_size[0]=T*B-1;
    build_gain_heap(gain_list,bk_list,heap_size[0]);
    ret_index=Extract_Heap(gain_list,bk_list,heap_size);
    Gain ele=gain_list[ret_index];
    int* bk_log=(int*)calloc(B,sizeof(int));
    double cost_log=-1;
    double div_log=-1;
    clock_t start_time=clock();
    int rep=((int)(log(B)/log(2)));
    printf("\nRepetation value=%d",rep);
    while(rep>0)
    {
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
            //printf("\n Element Gain=%lf",ele.gain_value);
        }
        //printf("\n Modified FM done");
        Area_coverage(bk_list,net_list,tier_list,B,N,T);
        //printf("\n Area Coverage done");
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
        if(cost_log==-1 || cost_log>TWL)
        {
            for(i=0;i<B;i++)
            {
                bk_log[i]=bk_list[i].tier;
            }
            cost_log=TWL;
        }
        j=(int)(log(B)/log(2));
        if(rep>1)
        {

            for(i=0;i<=j;i++)
            {
                Shuffle_blocks(Degree,bk_list,tier_list,net_list,B,T,N);
            }
            //printf("\n Shuffling Done");
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
            ret_index=Extract_Heap(gain_list,bk_list,heap_size);
            if(ret_index<0)
            {
                break;
            }
            ele=gain_list[ret_index];
        }
        rep--;
    }
    clock_t end_time=clock();
    //float entropy=total_entropy(net_list,bk_list,N,B,T);
    //printf("\n\n\n Total entropy=%f\t Toatal net list=%d\n\n\n",entropy,N);
    printf("\n Placements Done");
    double time_taken=(double)(end_time-start_time)/CLOCKS_PER_SEC;
    fprintf(fp1,"\nTime Taken to execute Compromized FM:%0.6lf",time_taken);
    //fprintf(fp1,"\nAt End Total Number of MIV after Area coverage=%d",claculate_MIV(bk_list,net_list,N,T));
    double tot_area=0;
    for(i=0;i<B;i++)
    {
        int pre_tier=bk_list[i].tier;
        place_block(tier_list,bk_list,i,bk_log[i],pre_tier);
        Net_Component* tem=bk_list[i].net_ptr;
        while(tem!=NULL)
        {
            update_net_list(net_list,bk_list,tem->net_index,i,bk_log[i],pre_tier);
            tem=tem->right;
        }
    }
    fprintf(fp1,"\nAt End Total Number of MIV after Compromised FM=%d",claculate_MIV(bk_list,net_list,N,T));
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
    fprintf(fp1,"\n\n TWL Calculated After Compromised FM by COST_LOG=%lf\n",cost_log );
    double hwl=0;
    double TWL=0;
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
    fprintf(fp1,"\n\n TWL Calculated After Compromised FM =%lf\n",TWL );
}

void Shuffle_blocks(int** Degree,Block* bk_list,Tier* tier_list,Net* net_list,int B,int T,int N)
{
    int i,j,k,l;
    double avg_area=(tier_list[0].max_area+tier_list[0].min_area)/2;
    double* divergence=(double*)calloc(T,sizeof(double));
    for(i=0;i<T;i++)
    {
        divergence[i]=(tier_list[i].area_consumed-avg_area)/avg_area;
    }
    k=0;
    j=0;
    for(i=1;i<T;i++)
    {
        if(divergence[i]>divergence[k])
        {
            k=i;
        }
        if(divergence[i]<=divergence[j])
        {
            j=i;
        }
        /*else if(j==-1 || divergence[i]>divergence[j])
        {
            j=i;
        }*/

    }
    int f_ind=-1,s_ind=-1,f_flag=0,s_flag=0;
    double f_val=-1,s_val=-1;
    for(i=0;i<B;i++)
    {
        if(bk_list[i].tier==k)
        {
            if(f_ind==-1)
            {
                f_val=calculate_block_entropy(bk_list,net_list,i,k,T);
                f_ind=i;
            }
            else{
                float tem=calculate_block_entropy(bk_list,net_list,i,k,T);
                if(tem>f_val)
                {
                    f_val=tem;
                    f_ind=i;
                }
            }
            f_flag=1;
        }
        if(bk_list[i].tier==j)
        {
            if(s_ind==-1)
            {
                s_val=calculate_block_entropy(bk_list,net_list,i,j,T);
                s_ind=i;
            }
            else{
                float tem=calculate_block_entropy(bk_list,net_list,i,j,T);
                if(tem>s_val)
                {
                    s_val=tem;
                    s_ind=i;
                }
            }
            s_flag=1;
        }
    }
    if(f_flag==1 && s_flag==1)
    {
        place_block(tier_list,bk_list,f_ind,j,k);
        Net_Component* tem=bk_list[f_ind].net_ptr;
        while(tem!=NULL)
        {
            update_net_list(net_list,bk_list,tem->net_index,f_ind,j,k);
            tem=tem->right;
        }
        place_block(tier_list,bk_list,s_ind,k,j);
        tem=bk_list[s_ind].net_ptr;
        while(tem!=NULL)
        {
            update_net_list(net_list,bk_list,tem->net_index,s_ind,j,k);
            tem=tem->right;
        }
    }
    //printf("\n before freeing divergence");
    free(divergence);
}


/*void Shuffle_blocks(int** Degree,Block* bk_list,Tier* tier_list,Net* net_list,int B,int T,int N)
{
    //printf("\n Inside Shuffle");
    int i,j,k,l;
    double avg_area=(tier_list[0].max_area+tier_list[0].min_area)/2;
    double* divergence=(double*)calloc(T,sizeof(double));
    for(i=0;i<T;i++)
    {
        divergence[i]=(tier_list[i].area_consumed-avg_area)/avg_area;
    }
    k=0;
    j=0;
    for(i=1;i<T;i++)
    {
        if(divergence[i]>divergence[k])
        {
            k=i;
        }
        if(divergence[i]<divergence[j])
        {
            j=i;
        }
        /*else if(j==-1 || divergence[i]>divergence[j])
        {
            j=i;
        }

    }
    //printf("\n k=%d\tj=%d",k,j);
    double f_hst=-1,s_hst=-1,fst=0;
    int f_ind,s_ind;
    int f_flag=0,s_flag=0;
    for(i=0;i<B;i++)
    {
        fst=0;
        //printf("\ti=%d",i);
        if(bk_list[i].tier==k)
        {
            double tot_degree=0;
            for(l=0;l<B;l++)
            {
                if(bk_list[l].tier==k)
                {
                    tot_degree+=Degree[i][l];
                }
            }
            for(l=0;l<B;l++)
            {
                if(bk_list[l].tier==k && l!=i)
                {
                    double fact=((bk_list[l].length+bk_list[l].width)+(Degree[i][l]/tot_degree)*(bk_list[i].length+bk_list[i].width));
                    fst+=Degree[i][l]*fact*fact;
                }
            }
            if(f_hst==-1 || fst>f_hst)
            {
                f_hst=fst;
                f_ind=i;
                f_flag=1;
                //printf("\n f_ind=%d",f_ind);
            }
        }
        fst=0;
        if(bk_list[i].tier==j)
        {

            double tot_degree=0;
            for(l=0;l<B;l++)
            {
                if(bk_list[l].tier==j)
                {
                    tot_degree+=Degree[i][l];
                }
            }
            for(l=0;l<B;l++)
            {
                if(bk_list[l].tier==j && l!=i)
                {
                    double fact=((bk_list[l].length+bk_list[l].width)+(Degree[i][l]/tot_degree)*(bk_list[i].length+bk_list[i].width));
                    fst+=Degree[i][l]*fact*fact;
                }
            }
            if(s_hst==-1 || fst>s_hst)
            {
                s_hst=fst;
                s_ind=i;
                s_flag=1;
                //printf("\n s_ind=%d",s_ind);
            }
        }
    }
    //printf("\n f_ind=%d\ts_ind=%d",f_ind,s_ind);
    if(f_flag==1 && s_flag==1)
    {
        place_block(tier_list,bk_list,f_ind,j,k);
        Net_Component* tem=bk_list[f_ind].net_ptr;
        while(tem!=NULL)
        {
            update_net_list(net_list,bk_list,tem->net_index,f_ind,j,k);
            tem=tem->right;
        }
        place_block(tier_list,bk_list,s_ind,k,j);
        tem=bk_list[s_ind].net_ptr;
        while(tem!=NULL)
        {
            update_net_list(net_list,bk_list,tem->net_index,s_ind,j,k);
            tem=tem->right;
        }
    }
    //printf("\n before freeing divergence");
    free(divergence);
    //printf("\n divergence freed");
}*/

float total_entropy(Net* net_list,Block* bk_list,int N,int B,int T)
{
    int i,j;
    float entropy=0;
    double twl=0;
    calculate_net_distribution(net_list,bk_list,B,N,T);
    for(i=0;i<N;i++)
    {
        twl=0;
        for(j=0;j<T;j++)
        {
            twl+=net_list[i].net_tier_com[j].total_wire_length;
        }
        for(j=0;j<T;j++)
        {
            double contr=net_list[i].net_tier_com[j].total_wire_length/twl;
            if(contr!=0)
            {
                entropy+=(-1)*contr*(log(contr)/log(T));
            }
            else{
                entropy+=0;
            }
        }
    }
    return entropy;
}

int count_terminal(Block* bk_list,Net* net_list,int B,int N)
{
    int i;
    int ter=0;
    for(i=0;i<N;i++)
    {
        Block_Component* tem=net_list[i].bk_ptr;
        while(tem!=NULL)
        {
            ter+=1;
            tem=tem->right;
        }
    }
    return ter;
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
    for(i=0;i<T;i++)
    {
        if(tier_list[i].area_consumed>tier_list[i].max_area)
        {
            Max_Area_Coverage(bk_list,net_list,tier_list,B,N,T,i);
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
    free(gain_list);
    free(Cost);
}


void Max_Area_Coverage(Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T,int tier_no)
{
    //printf("\n Inside Max Area Coverage");
    //printf("\n BLOCK AREA=%lf\tMAX AREA ALLOWED=%lf",tier_list[tier_no].area_consumed,tier_list[tier_no].max_area);
    int i,j;
    int* heap_size=(int*)calloc(1,sizeof(int));
    float** Cost=(float**)calloc(B,sizeof(float*));
    for(i=0;i<B;i++)
    {
        Cost[i]=(float*)calloc(T,sizeof(float));
    }
    Gain* gain_list=(Gain*)calloc(B*T,sizeof(Gain));
    calculate_net_distribution(net_list,bk_list,B,N,T);
    for(i=0;i<B;i++)
    {
        bk_list[i].Current_Entropy=calculate_block_entropy(bk_list,net_list,i,bk_list[i].tier,T);
    }
    //k=0;
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
    int ret_index=Extract_Heap(gain_list,bk_list,heap_size);
    Gain ele=gain_list[ret_index];
    //printf("\n Gain ele got");
    while(tier_list[tier_no].area_consumed>tier_list[tier_no].max_area && ret_index>=0)
    {
        //printf("\n Element Gain=%lf",ele.gain_value);
        int pre_tier=bk_list[ele.bk_index].tier;
        int next_tier=ele.tier_index;
        if(pre_tier==tier_no && next_tier!=tier_no && tier_list[next_tier].area_consumed+bk_list[ele.bk_index].area<=tier_list[next_tier].max_area)
        {
            //printf("\n Element Gain=%lf",ele.gain_value);
            place_block(tier_list,bk_list,ele.bk_index,next_tier,tier_no);
            Net_Component* tem=bk_list[ele.bk_index].net_ptr;
            while(tem!=NULL)
            {
                update_net_list(net_list,bk_list,tem->net_index,ele.bk_index,tier_no,pre_tier);
                tem=tem->right;
            }
            for(i=0;i<B;i++)
            {
                bk_list[i].Current_Entropy=calculate_block_entropy(bk_list,net_list,i,bk_list[i].tier,T);
            }
            //k=0;
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
        /*if(next_tier==tier_no)
        {
            printf("\n GAIN VALUE=%f",ele.gain_value);
        }
        else if(!(tier_list[next_tier].area_consumed+bk_list[ele.bk_index].area<=tier_list[next_tier].max_area))
        {
            printf("\n NEXT TIER AREA=%lf\t NEXT PROPOSED AREA=%lf\t MAX AREA=%lf",tier_list[next_tier].area_consumed,(tier_list[next_tier].area_consumed+bk_list[ele.bk_index].area),tier_list[next_tier].max_area);
            printf("\n BLOCK AREA=%lf\t BLOCK INDEX=%d",bk_list[ele.bk_index].area,ele.bk_index);
        }*/
        ret_index=Extract_Heap(gain_list,bk_list,heap_size);
        //printf("\n Heap Size=%d",heap_size[0]);
        if(ret_index<0)
        {
            //printf("\n return index less than 0");
            break;
        }
        ele=gain_list[ret_index];
    }
    /*if(tier_list[tier_no].area_consumed>tier_list[tier_no].max_area)
    {
        EXCHANGE_BLOCKS_MAX(bk_list,tier_list,B,T,tier_no);
    }*/
    free(gain_list);
    for(i=0;i<B;i++)
    {
        free(Cost[i]);
    }
    free(Cost);
   // printf("\n AFTER MAX COVERAGE");
    //printf("\n BLOCK AREA=%lf\tMAX AREA ALLOWED=%lf",tier_list[tier_no].area_consumed,tier_list[tier_no].max_area);
}

void EXCHANGE_BLOCKS_MAX(Block* bk_list,Tier* tier_list,int B,int T,int tier_no)
{
    Block* tem_bk_list=(Block*)calloc(B,sizeof(Block));
    int i,j,cnt=-1;
    for(i=0;i<B;i++)
    {
        tem_bk_list[i]=bk_list[i];
    }
    quick_sort(tem_bk_list,0,B-1);
    for(i=0;i<B;i++)
    {
        printf("\n AREA=%lf \t Block-index=%d",tem_bk_list[i].area,tem_bk_list[i].index);
    }
    while(tier_list[tier_no].area_consumed>tier_list[tier_no].max_area)
    {
        for(i=cnt+1;i<B;i++)
        {
            if(tem_bk_list[i].tier==tier_no)
            {
                cnt=i;
                break;
            }
        }
        for(j=cnt+1;j<B;j++)
        {
            if(tem_bk_list[j].tier!=tier_no && tier_list[tem_bk_list[j].tier].area_consumed+bk_list[tem_bk_list[j].index].area-bk_list[tem_bk_list[cnt].index].area<=tier_list[tem_bk_list[j].tier].max_area)
            {
                printf("\n BK_AREA=%lf\tNEXT BK_AREA=%lf",tem_bk_list[cnt].area,tem_bk_list[j].area);
                printf("\n BK_TIER=%d\t NEXT BK_TIER=%d",tem_bk_list[cnt].tier,tem_bk_list[j].tier);
                int next_tier=tem_bk_list[j].tier;
                bk_list[tem_bk_list[j].index].tier=tem_bk_list[j].tier=tier_no;
                bk_list[tem_bk_list[cnt].index].tier=tem_bk_list[cnt].tier=next_tier;
                tier_list[next_tier].area_consumed+=(bk_list[tem_bk_list[cnt].index].area-bk_list[tem_bk_list[j].index].area);
                tier_list[tier_no].area_consumed+=(bk_list[tem_bk_list[j].index].area-bk_list[tem_bk_list[cnt].index].area);
                printf("\n Block exchanged");
                printf("\n BK_TIER=%d\t NEXT BK_TIER=%d",tem_bk_list[cnt].tier,tem_bk_list[j].tier);
                break;
            }
        }
    }
    free(tem_bk_list);
}

void quick_sort(Block* bk_list,int p,int q)
{
    if(p>=q)
    {
        return;
    }
    int r=QS_partition(bk_list,p,q);
    quick_sort(bk_list,p,r-1);
    quick_sort(bk_list,r+1,q);
}

int QS_partition(Block* bk_list,int p,int q)
{
    int i,j=p-1;
    for(i=p;i<q;i++)
    {
        if(bk_list[i].area>bk_list[q].area)
        {
            j++;
            Block tem=bk_list[i];
            bk_list[i]=bk_list[j];
            bk_list[j]=tem;
        }
    }
    j++;
    Block tem=bk_list[q];
    bk_list[q]=bk_list[j];
    bk_list[j]=tem;
    return j;
}
