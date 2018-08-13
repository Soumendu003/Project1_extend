#include"Header.h"
void read_blocks_ami33(FILE* fp)
{
    int i,B,T,cnt=0;
    char* str=(char*)calloc(50,sizeof(char));
    fscanf(fp,"%d",&B);
    printf("\n No of Blocks=%d",B);
    Block* bk_list=(Block*)calloc(B,sizeof(Block));
    fscanf(fp,"%d",&T);
    while(cnt<B)
    {
         fscanf(fp,"%s",str);
         if(str[0]=='b' && str[1]=='k')
         {
             i=0;
             while(str[i+2]!='\0')
             {
                 bk_list[cnt].name[i]=str[i+2];
                 i++;
             }
             bk_list[cnt].name[i]='\0';
             printf("\n Block Name=%s",bk_list[cnt].name);
             int area;
             fscanf(fp,"%s%d",str,&area);
             printf("\t Area=%d",area);
             bk_list[cnt].area=area;
             bk_list[cnt].length=bk_list[cnt].width=sqrt(area);
             printf("\n Block Length=%lf\t Block Width=%lf",bk_list[cnt].length,bk_list[cnt].width);
             bk_list[cnt].index=cnt;
             cnt++;
         }
    }
    fclose(fp);
    for(i=0;i<B;i++)
    {
        bk_list[i].index=i;
    }
    fp=fopen("ami33.nets","r");
    read_nets_ami33(fp,bk_list,B);
    fclose(fp);
    return;
}

void free_block_component(Block_Component* ele)
{
    if(ele->right!=NULL)
    {
        free_block_component(ele->right);
    }
    free(ele);
    return;
}

void free_block_components(Net ele)
{
    Block_Component* tem=ele.bk_ptr;
    if(tem!=NULL)
    {
        free_block_component(tem);
    }

}

void default_blocks_placement(Block* bk_list,int B)
{
    int i;
    for(i=0;i<B;i++)
    {
        bk_list[i].tier=-1;
        bk_list[i].Current_Entropy=0;
    }
}

int place_block(Tier* tier_list,Block* bk_list,int bk_index,int tier_cnt,int prev_tier)
{
    if(prev_tier>=0 )
    {
        tier_list[tier_cnt].area_consumed+=bk_list[bk_index].area;
        tier_list[prev_tier].area_consumed-=bk_list[bk_index].area;
        bk_list[bk_index].tier=tier_cnt;
        //update_net_list(net_list,net_index,tier_cnt);
        return 1;
    }
    else
    {
        tier_list[tier_cnt].area_consumed+=bk_list[bk_index].area;
        bk_list[bk_index].tier=tier_cnt;
        return 1;
    }
}

float calculate_block_entropy(Block* bk_list,Net* net_list,int bk_index,int tier_no,int T)
{
    Net_Component* tem=bk_list[bk_index].net_ptr;
    float entropy=0;
    int i;
    double twl=0;
    while(tem!=NULL)
    {
        if(bk_list[bk_index].tier==tier_no)
        {
            for(i=0;i<T;i++)
            {
                twl+=net_list[tem->net_index].net_tier_com[i].total_wire_length;
            }
            for(i=0;i<T;i++)
            {
                double contr=net_list[tem->net_index].net_tier_com[i].total_wire_length/twl;
                entropy+=(-1)*contr*(log(contr)/log(T));
            }
        }
        else
        {
            for(i=0;i<T;i++)
            {
                twl+=net_list[tem->net_index].net_tier_com[i].total_wire_length;
            }
            for(i=0;i<T;i++)
            {
                double contr;
                if(i==bk_list[bk_index].tier)
                {
                    contr=(net_list[tem->net_index].net_tier_com[i].total_wire_length-bk_list[bk_index].length-bk_list[bk_index].width)/twl;
                }
                else if(i==tier_no)
                {
                    contr=(net_list[tem->net_index].net_tier_com[i].total_wire_length+bk_list[bk_index].length+bk_list[bk_index].width)/twl;
                }
                else{
                    contr=(net_list[tem->net_index].net_tier_com[i].total_wire_length)/twl;
                }
                entropy+=(-1)*contr*(log(contr)/log(T));
            }
        }
        tem=tem->right;
    }
    printf("\n Entropy value=%lf",entropy);
    return entropy;
}

int search_block(Block* bk_list,int lwr,int uppr,char* name)
{
    int val=(lwr+uppr)/2;
    if(strcmp(bk_list[val].name,name)==0)
    {
        return val;
    }
    else if(strcmp(bk_list[val].name,name)>0)
    {
        return search_block(bk_list,lwr,val-1,name);
    }
    else{
        return search_block(bk_list,val+1,uppr,name);
    }
}


double Calculate_Total_Area(Block* bk_list,int B)
{
    int i;
    double area=0;
    for(i=0;i<B;i++)
    {
        area+=bk_list[i].area;
    }
    return area;
}

void calculate_gain_list(float** Cost,Gain* gain_list,Block* bk_list,int B,int T)
{
    int i,j,k=0;
    for(i=0;i<B;i++)
    {
        for(j=0;j<T;j++)
        {
            gain_list[k].bk_index=i;
            gain_list[k].tier_index=j;
            gain_list[k].gain_value=Cost[i][j]-bk_list[i].Current_Entropy;
            k++;
        }
    }
}

void insert_bk_component(Net* net_list,int index,int bk_index)
{
    Block_Component* tem=net_list[index].bk_ptr;
    while(tem!=NULL)
    {
        if(tem->bk_index==bk_index)
        {
            return;
        }
        tem=tem->right;
    }
    tem=(Block_Component*)calloc(1,sizeof(Block_Component));
    tem->bk_index=bk_index;
    tem->right=net_list[index].bk_ptr;
    net_list[index].bk_ptr=tem;
    return;
}
