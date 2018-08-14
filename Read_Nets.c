#include"Header.h"
void read_nets_ami33(FILE* fp1,Block* bk_list,int B)
{
    int i,j,N,tot_pin,deg,cnt=0;
    fscanf(fp1,"%d",&N);
    fscanf(fp1,"%d",&tot_pin);
    //printf("\n No of Net=%d\tNo of Pin=%d",N,tot_pin);
    Net* net_list=(Net*)calloc(N,sizeof(Net));
    char* str=(char*)calloc(50,sizeof(char));
    while(cnt<N)
    {
        if(feof(fp1))
        {
            break;
        }
        fscanf(fp1,"%s",str);
        if(str[0]=='N' && str[1]=='e')
        {
            fscanf(fp1,"%s%d",str,&deg);
            net_list[cnt].no_of_bk=0;
            net_list[cnt].degree=deg;
            i=0;
            while(i<deg)
            {
                fscanf(fp1,"%s",str);
                if(str[0]=='b' && str[1]=='k')
                {
                    //printf("\n Pin Detected=%s",str);
                    i++;
                    net_list[cnt].no_of_bk++;
                    j=0;
                    char name[20];
                    int val=0;
                    while(str[j+2]!='\0')
                    {
                        name[j]=str[j+2];
                        j++;
                    }
                    name[j]='\0';
                    j=search_block(bk_list,0,B-1,name);
                   // printf("\n Value of j=%d",j);
                    insert_bk_component(net_list,cnt,j);
                }
                else if(str[0]=='G')
                {
                    i++;
                    net_list[cnt].gnd=true;
                }
                else if(str[0]=='P')
                {
                    i++;

                    net_list[cnt].pwr=true;
                }
                else if(str[0]=='V')
                {
                    i++;
                    net_list[cnt].V=true;
                }
                else if(str[0]=='N')
                {
                    i++;
                    //printf("\n Pad Detected");
                    net_list[cnt].pad=true;
                }
            }
            cnt++;
        }
    }
    printf("\n Reading Done");
    for(i=0;i<N;i++)
    {
        Block_Component* tem=net_list[i].bk_ptr;
        while(tem!=NULL)
        {
            insert_net_component(bk_list,tem->bk_index,i);
            tem=tem->right;
        }
    }
    printf("\n Net components inserted");
    float relaxation=0.05;
    FILE* fp=fopen("ami33_output_entropy.txt","w");
    for(j=2;j<=5;j++)
    {
        for(i=1;i<=5;i++)
        {
            Initial_Partition(fp,bk_list,net_list,B,N,j,i*relaxation);
        }
    }
    for(i=0;i<N;i++)
    {
        free_block_components(net_list[i]);

    }
    for(i=0;i<B;i++)
    {
        free_net_components(bk_list[i]);
    }
    free(net_list);
    free(bk_list);
    return;

}

void free_net_components(Block ele)
{
    Net_Component* tem=ele.net_ptr;
    if(tem!=NULL)
    {
        free_net_component(tem);
    }
}
void free_net_component(Net_Component* ele)
{
    if(ele->right!=NULL)
    {
        free_net_component(ele->right);
    }
    free(ele);
    return;
}

void initialize_net_list(Net* net_list,int N,int T)
{
    int i;
    for(i=0;i<N;i++)
    {
        if(net_list[i].gnd || net_list[i].pwr || net_list[i].V)
        {
            net_list[i].low_tier.tier_index=0;
            net_list[i].low_tier.bk_count=0;
        }
        else{
            net_list[i].low_tier.tier_index=INT_MAX;
            net_list[i].low_tier.bk_count=0;
        }
        net_list[i].top_tier.tier_index=-1;
        net_list[i].top_tier.bk_count=0;
        net_list[i].net_tier_com=(Net_Tier_Component*)calloc(T,sizeof(Net_Tier_Component));
    }
}

void custom_update_net_list(Net* net_list,Block* bk_list,int N,int B,int T)
{
    int i;
    for(i=0;i<N;i++)
    {
        if(net_list[i].top_tier.tier_index<0 || net_list[i].low_tier.tier_index>=T)
        {
            Block_Component* tem=net_list[i].bk_ptr;
            while(tem!=NULL)
            {
                update_net_list(net_list,bk_list,i,tem->bk_index,bk_list[tem->bk_index].tier,-1);  //Before Custom Update all the previous tier of the blocks was by default -1.
                tem=tem->right;
            }
        }
    }
}

void update_net_list(Net* net_list,Block* bk_list,int net_index,int bk_index,int tier_cnt,int pre_tier)
{
    if(net_list[net_index].low_tier.tier_index>tier_cnt)
    {
        net_list[net_index].low_tier.tier_index=tier_cnt;
        net_list[net_index].low_tier.bk_count=1;
    }
    if(net_list[net_index].top_tier.tier_index<tier_cnt)
    {
        net_list[net_index].top_tier.tier_index=tier_cnt;
        net_list[net_index].top_tier.bk_count=1;
    }
    if(net_list[net_index].low_tier.tier_index==tier_cnt)
    {
        net_list[net_index].low_tier.bk_count++;
    }
    if(net_list[net_index].top_tier.tier_index==tier_cnt)
    {
        net_list[net_index].top_tier.bk_count++;
    }
    if(net_list[net_index].low_tier.tier_index<tier_cnt)
    {
        if(!(net_list[net_index].gnd || net_list[net_index].pad || net_list[net_index].pwr || net_list[net_index].V))
        {
            if(net_list[net_index].low_tier.bk_count==1 && net_list[net_index].low_tier.tier_index==pre_tier)
            {
                Block_Component* tem=net_list[net_index].bk_ptr;
                int least_tier;
                int least_cnt=0;
                if(tem!=NULL)
                {
                    least_tier=bk_list[tem->bk_index].tier;
                    while(tem!=NULL)
                    {
                        if(bk_list[tem->bk_index].tier<least_tier)
                        {
                            least_tier=bk_list[tem->bk_index].tier;
                            least_cnt=1;
                        }
                        if(bk_list[tem->bk_index].tier==least_tier)
                        {
                            least_cnt++;
                        }
                        tem=tem->right;
                    }
                }
                net_list[net_index].low_tier.tier_index=least_tier;
                net_list[net_index].low_tier.bk_count=least_cnt;
            }
        }
    }
    if(net_list[net_index].top_tier.tier_index>tier_cnt)
    {
        if(net_list[net_index].top_tier.bk_count==1 && net_list[net_index].top_tier.tier_index==pre_tier)
        {
            Block_Component* tem=net_list[net_index].bk_ptr;
            int highest_tier;
            int highest_cnt=0;
            if(tem!=NULL)
            {
                highest_tier=bk_list[tem->bk_index].tier;
                while(tem!=NULL)
                {
                    if(bk_list[tem->bk_index].tier>highest_tier)
                    {
                        highest_tier=bk_list[tem->bk_index].tier;
                        highest_cnt=1;
                    }
                    if(bk_list[tem->bk_index].tier==highest_tier)
                    {
                        highest_cnt++;
                    }
                    tem=tem->right;
                }
            }
            net_list[net_index].top_tier.tier_index=highest_tier;
            net_list[net_index].top_tier.bk_count=highest_cnt;
        }
    }

}

int claculate_MIV(Net* net_list,int N,int T)
{
    int i,MIV=0;
    for(i=0;i<N;i++)
    {
        MIV+=net_list[i].top_tier.tier_index-net_list[i].low_tier.tier_index;
    }
    return MIV;
}

void calculate_net_distribution(Net* net_list,Block* bk_list,int B,int N,int T)
{
    int i,j;
    double* arr=(double*)calloc(T,sizeof(double));
    for(i=0;i<N;i++)
    {
        Block_Component* tem=net_list[i].bk_ptr;
        while(tem!=NULL)
        {
            arr[bk_list[tem->bk_index].tier]+=bk_list[tem->bk_index].length+bk_list[tem->bk_index].width;
            tem=tem->right;
        }
        for(j=0;j<T;j++)
        {
           net_list[i].net_tier_com[j].total_wire_length=arr[j];
           arr[j]=0;
        }
    }
    printf("\n Calculations Done");
    free(arr);
    return;
}

void insert_net_component(Block* bk_list,int bk_index,int net_index)
{
    Net_Component* tem=(Net_Component*)calloc(1,sizeof(Net_Component));
    tem->net_index=net_index;
    tem->right=bk_list[bk_index].net_ptr;
    bk_list[bk_index].net_ptr=tem;
    bk_list[bk_index].bk_degree++;
    return;
}
