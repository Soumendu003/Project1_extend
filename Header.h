#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<limits.h>
#include<time.h>
#include<math.h>
typedef struct net_com Net_Component;
struct net_com{
    int net_index;
    Net_Component* right;
};
typedef struct gain Gain;
struct gain{
    int bk_index;
    int tier_index;
    float gain_value;
    int current_index;
};
typedef struct bk_com Block_Component;
struct bk_com{
    int bk_index;
    Block_Component* right;
};
typedef struct block Block;
struct block{
    char name[10];
    int index;
    double area;
    double length;
    double width;
    int tier;
    bool lock;
    int no_of_adj_bk;
    int bk_degree;
    float Current_Entropy;
    Block_Component* adj_bk_ptr;
    Net_Component* net_ptr;
};
typedef struct net_tier_com Net_Tier_Component;
struct net_tier_com{
    int tier_index;
    int bk_count;
    double total_wire_length;
    Block_Component* bk_ptr;
};
typedef struct net Net;
struct net{
    int index;
    int degree;
    Net_Tier_Component top_tier;
    Net_Tier_Component low_tier;
    int no_of_bk;
    bool gnd;
    bool pwr;
    bool pad;
    bool V;
    Block_Component* bk_ptr;
    Net_Tier_Component* net_tier_com;
};
typedef  struct tier Tier;
struct tier{
    double area_consumed;
    double max_area;
    double min_area;
    int tot_bk;
    Block_Component* bk_com;
};
typedef struct wait Wait_List;
struct wait{
    int wait_bk_cnt;
    Gain* gain_component;
};
void read_blocks_ami33(FILE* fp);
void Read_Hard_Blocks(FILE* fp1);
void read_nets_ami33(FILE* fp,Block* bk_list,int B);
void Read_Hard_Nets(FILE* fp1,Block* bk_list,int B);
void free_net_component(Net_Component* ele);
void free_net_components(Block ele);
void free_block_component(Block_Component* ele);
void Initial_Partition(FILE* fp1,Block* bk_list,Net* net_list,int B,int N,int T,float relax);
void initialize_net_list(Net* net_list,int N,int T);
void default_blocks_placement(Block* bk_list,int B);
int place_block(Tier* tier_list,Block* bk_list,int bk_index,int tier_cnt,int prev_tier);
void custom_update_net_list(Net* net_list,Block* bk_list,int N,int B,int T);
void update_net_list(Net* net_list,Block* bk_list,int net_index,int bk_index,int tier_cnt,int pre_tier);
int claculate_MIV(Block* bk_list,Net* net_list,int N,int T);
void Secondary_Partition(FILE* fp1,Net* net_list,Block* bk_list,Tier* tr_list,int N,int B,int T);
double Calculate_Total_Area(Block* bk_list,int B);
void calculate_net_distribution(Net* net_list,Block* bk_list,int B,int N,int T);
float calculate_block_entropy(Block* bk_list,Net* net_list,int bk_index,int tier_no,int T);
void calculate_gain_list(float** Cost,Gain* gain_list,Block* bk_list,int B,int T);
void Compromized_FM(FILE* fp1,float** Cost,Gain* gain_list,Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T);
void build_gain_heap(Gain* gain_list,Block* bk_list,int last_index);
void Max_Heapify_Gain(Gain* gain_list,Block* bk_list,int ele_index,int last_index);
void insert_net_component(Block* bk_list,int bk_index,int net_index);
void free_block_components(Net ele);
int Extract_Heap(Gain* gain_list,Block* bk_list,int* heap_size);
void insert_bk_component(Net* net_list,int index,int bk_index);
int search_block(Block* bk_list,int lwr,int uppr,char* name);
void Area_coverage(Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T);
void Min_Area_Coverage(Block* bk_list,Net* net_list,Tier* tier_list,int B,int N,int T,int tier_no);
void calculate_tier_gain_list(float* Cost,Block* bk_list,Gain* gain_list,int B);
