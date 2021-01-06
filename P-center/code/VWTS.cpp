#include<cstdio>
#include<iostream>
#include<algorithm>
#include<cstring>
#include<ctime>
#include<assert.h>
#include"MY_SET"
// #define debug_flag
#define show_delta
#define show_time
#define inf 1000000007
#define BIT_WIDTH 30
#define ll long long
#define calc_id(int_offset,bit_offset) int_offset*BIT_WIDTH+bit_offset
#define debug(x) cerr<<#x<<"="<<x<<endl
using namespace std;

const int maxn=3e3+50;
MY_SET U(maxn),X(maxn);
MY_SET *C[maxn],*V[maxn];
bool is_center[maxn];
MY_SET *covered[maxn];
ll delta[maxn],backup[maxn][2];
ll weight[maxn];
ll f,best_f;
int t0;
// for input
int n,p;
int time_limit,seed=time(0);

#ifdef show_delta
    void print_delta()
    {
        for(int i=1;i<=n;i++)
        {
            if(is_center[i])
                cerr<<"c "<<i<<": "<<delta[i]<<endl;
            else
                cerr<<"  "<<i<<": "<<delta[i]<<endl;
        }
        return ;
    }
#endif

void read(int argc,char** argv)
{
    if(argc==1)
    {
        printf("file path: ");
        char file_path[81];
        scanf("%s",file_path);
        freopen(file_path,"r",stdin);
    }
    else
    {
        freopen(argv[1],"r",stdin);
        freopen(argv[2],"w",stdout);
        sscanf(argv[3],"%d",&time_limit);
        sscanf(argv[4],"%d",&seed);
    }

    scanf("%d%d",&n,&p);

    for(int i=1;i<=n;i++)
    {
        U.insert(i);
        V[i]=new MY_SET(n);
        C[i]=new MY_SET(n);
        covered[i]=new MY_SET(n);
        weight[i]=1;
    }

    for(int i=1,x;i<=n;i++)
    {
        int num;
        scanf("%d",&num);

        while(num--)
        {
            scanf("%d",&x);
            x++;
            V[i]->insert(x);
            C[x]->insert(i);
        }
    }

    return ;
}

ll calc_f()
{
    ll rt=0;
    int id=0;
    for(int int_offset=0;int_offset<U.volume/BIT_WIDTH+1;int_offset++)
    {
        if(!(U.bit[int_offset]))
        {
            continue;
        }

        for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
        {
            if((1<<bit_offset)>U.bit[int_offset]) break;

            if((1<<bit_offset)&(U.bit[int_offset]))
            {
                id=calc_id(int_offset,bit_offset);
                rt+=weight[id]*(!(covered[id]->size));
            }
        }
    }
    return rt;
}

void greedy_init()
{
    int size[maxn];
    for(int i=1;i<=n;i++)
        size[i]=V[i]->size;

    int max_size=0;
    int max_cnt=0;
    int max_id=0;
    srand(seed);

    #ifdef show_time
        int t1=time(0);
    #endif

    for(int pcnt=1;pcnt<=p;pcnt++)
    {

        #ifdef debug_flag
            for(int i=1;i<=n;i++)
            // printf("%d: %d\n",i,size[i]);
            cerr<<i<<": "<<size[i];
        #endif

        max_size=0;
        max_cnt=0;
        for(int i=1;i<=n;i++)
        {
            if(size[i]>max_size)
            {
                max_size=size[i];
                max_cnt=1;
                max_id=i;
            }
            else if(size[i]==max_size)
            {
                max_cnt++;

                #ifdef debug_flag
                    debug(max_id);
                    debug(i);
                #endif

                if(rand()%max_cnt==0)
                {

                    #ifdef debug_flag
                        // printf("change %d -> %d\n",max_id,i);
                        cerr<<"change "<<max_id<<" -> "<<i<<endl;
                    #endif

                    max_id=i;
                }
            }
        }

        is_center[max_id]=true;
        int original_size=V[max_id]->size;

        #ifdef debug_flag
            debug(max_id);
        #endif

        for(int int_offset=0;int_offset<(V[max_id]->volume)/BIT_WIDTH+1;int_offset++)
        {
            if(!(V[max_id]->bit[int_offset]))
            {
                continue;
            }

            for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
            {
                if((1<<bit_offset)>(V[max_id]->bit[int_offset])) break;

                if((1<<bit_offset)&(V[max_id]->bit[int_offset]))
                {
                    original_size--;
                    int id=calc_id(int_offset,bit_offset);
                    covered[id]->insert(max_id);
                    if(!(X.have_elem(id)))
                    {

                        #ifdef debug_flag
                            ebug(id);
                        #endif

                        X.insert(id);
                        U.erase(id);

                        for(int inner_int_offset=0;inner_int_offset<(C[id]->volume)/BIT_WIDTH+1;inner_int_offset++)
                        {
                            if(!(C[id]->bit[inner_int_offset]))
                            {
                                continue;
                            }

                            for(int inner_bit_offset=0;inner_bit_offset<BIT_WIDTH;inner_bit_offset++)
                            {
                                if((1<<inner_bit_offset)>(C[id]->bit[inner_int_offset])) break;

                                if((1<<inner_bit_offset)&(C[id]->bit[inner_int_offset]))
                                {

                                    #ifdef debug_flag
                                        debug(calc_id(inner_int_offset,inner_bit_offset));
                                    #endif

                                    size[calc_id(inner_int_offset,inner_bit_offset)]--;
                                }
                            }
                        }
                    }
                }
            }
        }

        #ifdef debug_flag
            for(int i=1;i<=n;i++)
                // printf("%d: %d\n",i,size[i]);
                cerr<<i<<": "<<size[i]<<endl;
        #endif

    }

    #ifdef show_time
        cerr<<"time for greedy_init loop: "<<time(0)-t1<<"s"<<endl;
    #endif

}

void init_delta()
{
    for(int i=1;i<=n;i++)
        if(covered[i]->size==1)
        {
            int the_one=covered[i]->get_the_kth(1);
            delta[the_one]+=weight[i];
        }
        else if(!is_center[i])
        {
            for(int int_offset=0;int_offset<(V[i]->volume)/BIT_WIDTH+1;int_offset++)
            {
                if(!(V[i]->bit[int_offset]))
                {
                    continue;
                }

                for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
                {
                    if((1<<bit_offset)>V[i]->bit[int_offset]) break;

                    if((1<<bit_offset)&(V[i]->bit[int_offset]))
                    {
                        int id=calc_id(int_offset,bit_offset);
                        if(covered[id]->size==0)
                        {
                            delta[i]+=weight[id];
                        }
                    }
                }
            }
        }

    #ifdef show_delta
        print_delta();
    #endif
}

bool can_stop_VWTS()
{
    if(U.size==0) return true;
    if(time(0)-t0>=9*60+50) return true;
    return false;
}

ll try_to_open(int new_center)
{
    int backup_num=0;
    ll benefit=0;

    for(int int_offset=0;int_offset<(V[new_center]->volume)/BIT_WIDTH+1;int_offset++)
    {
        if(!(V[new_center]->bit[int_offset]))
        {
            continue;
        }

        for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
        {
            if((1<<bit_offset)>V[new_center]->bit[int_offset]) break;

            if((1<<bit_offset)&(V[new_center]->bit[int_offset]))
            {
                int id=calc_id(int_offset,bit_offset);
                if(covered[id]->size==1)
                {
                    int the_one=covered[id]->get_the_kth(1);
                    backup_num++;
                    backup[backup_num][0]=the_one;
                    backup[backup_num][1]=delta[the_one];
                    // cancel_penalty
                    delta[the_one]-=weight[id];
                }
                else if(covered[id]->size==0)
                {
                    benefit+=weight[id];
                }
            }
        }
    }
    backup[0][0]=backup_num;
    return benefit;
}

// void backup()
// {
//     for(int i=1;i<=n;i++)
//         backup_delta[i]=delta[i];
//     return ;
// }

void find_best_pair(int& u, int& v)
{
    int left=U.size();
    int picked=U.get_the_kth(rand()%left+1);
    // backup();
    ll best_swap_val=inf;
    ll benefit=0;

    for(int int_offset=0;int_offset<(C[picked]->volume)/BIT_WIDTH+1;int_offset++)
    {
        if(!(C[picked]->bit[int_offset]))
        {
            continue;
        }

        for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
        {
            if((1<<bit_offset)>C[picked]->bit[int_offset]) break;

            if((1<<bit_offset)&(C[picked]->bit[int_offset]))
            {
                int new_center=calc_id(int_offset,bit_offset);

                if(covered[new_center]->size)
                {
                    cerr<<"Error! New center has been covered!"<<endl;
                    return ;
                }

                benefit=try_to_open(new_center);
                
            }
        }
    }
}

void VWTS()
{
    f=calc_f();
    int left=U.size;
    int u,v;
    while(!can_stop_VWTS())
    {
        #ifdef debug_flag
          debug("hello");
        #endif

        find_best_pair(u,v);
    }
}

void print_result()
{
    for(int i=1;i<=n;i++)
    {
        if(is_center[i]) printf("%d ",i);
    }
    putchar('\n');

    #ifdef show_time
        cerr<<"time: "<<time(0)-t0<<"s"<<endl;
    #endif

    return ;
}

void release_memory()
{
    for(int i=1;i<=n;i++)
    {
        delete C[i];
        delete V[i];
        delete covered[i];
    }

    return ;
}

int main(int argc,char**argv)
{
    read(argc,argv);
    t0=time(0);
    greedy_init();
    init_delta();
    VWTS();
    print_result();
    release_memory();
    return 0;
}

/*
../data/1simple.txt
*/


/* template for looping a MY_SET
/*

for(int int_offset=0;int_offset<(C[picked]->volume)/BIT_WIDTH+1;int_offset++)
{
    if(!(C[picked]->bit[int_offset]))
    {
        continue;
    }

    for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
    {
        if((1<<bit_offset)>C[picked]->bit[int_offset]) break;

        if((1<<bit_offset)&(C[picked]->bit[int_offset]))
        {
            
        }
    }
}

*/

/* for debug */
// MY_SET* new_set=new MY_SET(10);
// debug(new_set->volume);
// new_set->insert(12);
// debug(new_set->size);
// new_set->insert(0);
// new_set->insert(5);
// new_set->insert(0);
// debug(new_set->size);
// debug(new_set->get_the_kth(1));
// debug(new_set->get_the_kth(2));
// debug(new_set->get_the_kth(3));
// debug(new_set->get_the_kth(4));
// greedy_init();