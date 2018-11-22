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
             //printf("\n Block Name=%s",bk_list[cnt].name);
             int area;
             fscanf(fp,"%s%d",str,&area);
             //printf("\t Area=%d",area);
             bk_list[cnt].area=area;
             bk_list[cnt].length=bk_list[cnt].width=sqrt(area);
            // printf("\n Block Length=%lf\t Block Width=%lf",bk_list[cnt].length,bk_list[cnt].width);
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

void Read_Hard_Blocks(FILE* fp1)
{
    int i,j,k,B,T,cnt=0;
    char* str=(char*)calloc(50,sizeof(char));
    fscanf(fp1,"%d",&B);
    printf("\n No of Blocks=%d",B);
    Block* bk_list=(Block*)calloc(B,sizeof(Block));
    fscanf(fp1,"%d",&T);
    while(cnt<B)
    {
         fscanf(fp1,"%s",str);
         if(/*str[0]=='s' && str[1]=='b')//(*/str[0]=='M')
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
             fscanf(fp1,"%s%d",str,&area);
             //printf("\t Area=%d",area);
             fscanf(fp1,"%[^\n]",str);
             printf("\n String=%s",str);
             j=0;
             k=0;
             while(str[j]!='\0')
             {
                 if(str[j]=='(')
                 {
                     if(k==2)
                     {
                         j++;
                         int val=0;
                         while(str[j]!=',')
                         {
                             val=val*10+(str[j]-48);
                             j++;
                         }
                         bk_list[cnt].length=val;
                         j+=2;
                         val=0;
                         while(str[j]!=')')
                         {
                             val=val*10+(str[j]-48);
                             j++;
                         }
                         bk_list[cnt].width=val;
                         break;
                     }
                     else{
                        k++;
                     }
                 }
                 j++;
             }
             //printf("\n Block Length=%lf\t Block Width=%lf",bk_list[cnt].length,bk_list[cnt].width);
             bk_list[cnt].area=(bk_list[cnt].length)*(bk_list[cnt].width);
             printf("\t Block Area=%lf",bk_list[cnt].area);
             bk_list[cnt].index=cnt;
             cnt++;
         }
    }
    //free(str);
    fclose(fp1);
    for(i=0;i<B;i++)
    {
        bk_list[i].index=i;
    }
    fp1=fopen("ami49.nets","r");
    Read_Hard_Nets(fp1,bk_list,B);
    fclose(fp1);
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
        twl=0;
        if(bk_list[bk_index].tier==tier_no)
        {
            for(i=0;i<T;i++)
            {
                twl+=net_list[tem->net_index].net_tier_com[i].total_wire_length;
            }
            if(twl==0)
            {
                printf("\n TWL=%lf",twl);
            }
            for(i=0;i<T;i++)
            {
                double contr=(net_list[tem->net_index].net_tier_com[i].total_wire_length)/twl;
                //printf("\n Concentration Value=%lf",contr);
                if(contr!=0)
                {
                    entropy+=(-1)*contr*(log(contr)/log(T));
                }
                else{
                    entropy+=0;
                }
               // printf("\n Entropy=%f",entropy);
            }
        }
        else
        {
            for(i=0;i<T;i++)
            {
                twl+=net_list[tem->net_index].net_tier_com[i].total_wire_length;
            }
            if(twl==0)
            {
                printf("\n TWL=%lf",twl);
            }
            for(i=0;i<T;i++)
            {
                double contr;
                if(i==bk_list[bk_index].tier && i!=tier_no)
                {
                    contr=(net_list[tem->net_index].net_tier_com[i].total_wire_length-bk_list[bk_index].length-bk_list[bk_index].width)/twl;
                }
                else if(i==tier_no && i!=bk_list[bk_index].tier)
                {
                    contr=(net_list[tem->net_index].net_tier_com[i].total_wire_length+bk_list[bk_index].length+bk_list[bk_index].width)/twl;
                }
                else{
                    contr=(net_list[tem->net_index].net_tier_com[i].total_wire_length)/twl;
                }
                if(contr!=0)
                {
                    entropy+=(-1)*contr*(log(contr)/log(T));
                }
                else{
                    entropy+=0;
                }
                //printf("\n Entropy=%f",entropy);
            }
        }
        tem=tem->right;
    }
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

void calculate_tier_gain_list(float* Cost,Block* bk_list,Gain* gain_list,int B)
{
    int i;
    for(i=0;i<B;i++)
    {
        gain_list[i].bk_index=i;
        gain_list[i].gain_value=Cost[i]-bk_list[i].Current_Entropy;
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

/*void initial_block_placement(int** Degree,Tier* tier_list,Block* bk_list,Block_Component* header,int B,int T)
{
    int size,i,j,k;
    double* count_list=(double*)calloc(B,sizeof(double));
    if(header->right!=NULL)
    {
        Block_Component* tem=header->right;
        while(tem!=NULL)
        {
            count_list[tem->bk_index]=-1;
            tem=tem->right;
        }
        for(i=0;i<B;i++)
        {
            if(count_list[i]!=-1)
            {
                Block_Component* ele=header->right;
                while(ele!=NULL)
                {
                    count_list[i]+=Degree[i][ele->bk_index]*(bk_list[ele->bk_index].length+bk_list[ele->bk_index].width);
                    ele=ele->right;
                }
            }
        }
    }
    else{
        for(i=0;i<B;i++)
        {
            for(j=0;j<B;j++)
            {
                if(i!=j)
                {
                    count_list[i]+=Degree[i][j]*(bk_list[j].length+bk_list[j].width);
                }
            }
        }
    }
    int least_index=-1;
    for(i=0;i<B;i++)
    {
        if(count_list[i]!=-1)
        {
            if(least_index!=-1)
            {
                if(count_list[least_index]>count_list[i])
                {
                    least_index=i;
                }
            }
            else{
                least_index=i;
            }
        }
    }
    if(least_index!=-1)
    {
        double* tier_count=(double*)calloc(T,sizeof(double));
        Block_Component* tem=header->right;
        if(tem==NULL)
        {
            place_block(tier_list,bk_list,least_index,0,-1);
        }
        else{
            while(tem!=NULL)
            {
                tier_count[bk_list[tem->bk_index].tier]+=Degree[least_index][tem->bk_index]*(bk_list[tem->bk_index].length+bk_list[tem->bk_index].width);
                tem=tem->right;
            }
            for(i=0;i<T;i++)
            {
                tier_count[i]=tier_count[i]/count_list[least_index];
            }
            int least_tier=0;
            for(i=0;i<T;i++)
            {
                if(tier_count[i]<tier_count[least_tier])
                {
                    least_tier=i;
                }
            }
            place_block(tier_list,bk_list,least_index,least_tier,-1);
        }
        free(tier_count);
    }
    Block_Component* New=(Block_Component*)calloc(1,sizeof(Block_Component));
    New->bk_index=least_index;
    if(header->right==NULL)
    {
        header->right=New;
    }
    else{
        Block_Component* tem=header->right;
        while(tem->right!=NULL)
        {
            tem=tem->right;
        }
        tem->right=New;
    }
    free(count_list);
    return;
}*/


/*void initial_block_placement(int** Degree,Tier* tier_list,Block* bk_list,Block_Component* header,int B,int T)
{
    int size,i,j,k;
    double** count_list=(double**)calloc(B,sizeof(double*));
    for(i=0;i<B;i++)
    {
        count_list[i]=(double*)calloc(T,sizeof(double*));
    }
    if(header->right!=NULL)
    {
        Block_Component* tem=header->right;
        while(tem!=NULL)
        {
            count_list[tem->bk_index][0]=-1;
            tem=tem->right;
        }
        for(i=0;i<B;i++)
        {
            if(count_list[i][0]!=-1)
            {
                Block_Component* ele=header->right;
                while(ele!=NULL)
                {
                    count_list[i][bk_list[ele->bk_index].tier]+=Degree[i][ele->bk_index]*(bk_list[ele->bk_index].length+bk_list[ele->bk_index].width+bk_list[i].length+bk_list[i].width);
                    ele=ele->right;
                }
            }
        }
    }
    else{
        for(i=0;i<B;i++)
        {
            for(j=0;j<B;j++)
            {
                if(i!=j)
                {
                    count_list[i][0]+=Degree[i][j]*(bk_list[j].length+bk_list[j].width+bk_list[i].length+bk_list[i].width);
                }
            }
        }
    }
    int least_index=-1;
    int final_least_tier=-1;
    for(i=0;i<B;i++)
    {
        if(count_list[i][0]!=-1)
        {
            int least_tier=0;
            for(j=0;j<T;j++)
            {
                if(count_list[i][j]<count_list[i][least_tier])
                {
                    least_tier=j;
                }
            }
            if(least_index!=-1)
            {
                if(count_list[least_index][final_least_tier]>count_list[i][least_tier])
                {
                    least_index=i;
                    final_least_tier=least_tier;
                }
            }
            else{
                least_index=i;
                final_least_tier=least_tier;
            }
        }
    }
    if(least_index!=-1)
    {
        Block_Component* tem=header->right;
        if(tem==NULL)
        {
            place_block(tier_list,bk_list,least_index,0,-1);
        }
        else{
            place_block(tier_list,bk_list,least_index,final_least_tier,-1);
        }
    }
    Block_Component* New=(Block_Component*)calloc(1,sizeof(Block_Component));
    New->bk_index=least_index;
    if(header->right==NULL)
    {
        header->right=New;
    }
    else{
        Block_Component* tem=header->right;
        while(tem->right!=NULL)
        {
            tem=tem->right;
        }
        tem->right=New;
    }
    for(i=0;i<B;i++)
    {
        free(count_list[i]);
    }
    free(count_list);
    return;
}*/


void initial_block_placement(int** Degree,Tier* tier_list,Block* bk_list,Block_Component* header,int B,int T)
{
    int size,i,j,k;
    double* count_list=(double*)calloc(B,sizeof(double));
    if(header->right!=NULL)
    {
        Block_Component* tem=header->right;
        while(tem!=NULL)
        {
            count_list[tem->bk_index]=-1;
            tem=tem->right;
        }
        for(i=0;i<B;i++)
        {
            if(count_list[i]!=-1)
            {
                Block_Component* ele=header->right;
                double total=0;
                while(ele!=NULL)
                {
                    total+=Degree[i][ele->bk_index];
                    ele=ele->right;
                }
                ele=header->right;
                while(ele!=NULL)
                {
                    double fact=((bk_list[ele->bk_index].length+bk_list[ele->bk_index].width)+(Degree[i][ele->bk_index]/total)*(bk_list[i].length+bk_list[i].width));
                    count_list[i]+=Degree[i][ele->bk_index]*fact*fact;
                    ele=ele->right;
                }
            }
        }
    }
    else{
        for(i=0;i<B;i++)
        {
            double total=0;
            for(j=0;j<B;j++)
            {
                total+=Degree[i][j];
            }
            for(j=0;j<B;j++)
            {
                if(i!=j)
                {
                    double fact=((bk_list[i].length+bk_list[i].width)+Degree[i][j]*(bk_list[j].length+bk_list[j].width));
                    count_list[i]+=(Degree[i][j]/total)*fact*fact;
                }
            }
        }
    }
    int least_index=-1;
    for(i=0;i<B;i++)
    {
        if(count_list[i]!=-1)
        {
            if(least_index!=-1)
            {
                if(count_list[least_index]<count_list[i])
                {
                    least_index=i;
                }
            }
            else{
                least_index=i;
            }
        }
    }
    if(least_index!=-1)
    {
        double* tier_count=(double*)calloc(T,sizeof(double));
        Block_Component* tem=header->right;
        if(tem==NULL)
        {
            place_block(tier_list,bk_list,least_index,0,-1);
        }
        else{
            while(tem!=NULL)
            {
                double fact=(bk_list[tem->bk_index].length+bk_list[tem->bk_index].width+bk_list[least_index].length+bk_list[least_index].width);
                tier_count[bk_list[tem->bk_index].tier]+=Degree[least_index][tem->bk_index]*fact*fact;
                tem=tem->right;
            }
            for(i=0;i<T;i++)
            {
                tier_count[i]=tier_count[i]/count_list[least_index];
            }
            int least_tier=0;
            for(i=0;i<T;i++)
            {
                if(tier_count[i]<tier_count[least_tier])
                {
                    least_tier=i;
                }
            }
            place_block(tier_list,bk_list,least_index,least_tier,-1);
        }
        free(tier_count);
    }
    Block_Component* New=(Block_Component*)calloc(1,sizeof(Block_Component));
    New->bk_index=least_index;
    if(header->right==NULL)
    {
        header->right=New;
    }
    else{
        Block_Component* tem=header->right;
        while(tem->right!=NULL)
        {
            tem=tem->right;
        }
        tem->right=New;
    }
    free(count_list);
    return;
}
