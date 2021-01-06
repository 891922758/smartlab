#include<cstdio>
#include<iostream>
#include<algorithm>
#include<cstring>
#include<ctime>
#include<assert.h>
#include"MY_SET"
// #define debug_flag
// #define show_delta
#define watch_mode
#define show_time
#define inf 1000000007
#define BIT_WIDTH 30
#define ll long long
#define calc_id(int_offset,bit_offset) int_offset*BIT_WIDTH+bit_offset
#define debug(x) cerr<<#x<<"="<<x<<endl
using namespace std;

const int maxn=3e3+50;
MY_SET U(maxn),X(maxn);
int C[maxn][maxn],V[maxn][maxn];
bool is_center[maxn];
MY_SET *covered[maxn];
ll delta[maxn],backup[maxn][2];
ll weight[maxn];
ll f,best_f;
int last_new,last_old;
int t0;
// for input
int n,p;
int time_limit,seed=time(0);

#ifdef show_delta
    void print_delta()
    {
        cerr<<"----------delta table----------\n";
        for(int i=1;i<=n;i++)
        {
            cerr<<"\t";
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
            V[i][++V[i][0]]=x;
            C[x][++C[x][0]]=i;
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
        size[i]=V[i][0];

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
        X.insert(max_id);

        #ifdef debug_flag
            debug(max_id);
        #endif

        for(int i=1;i<=V[max_id][0];i++)
        {
            int id=V[max_id][i];
            covered[id]->insert(max_id);
            if(covered[id]->size==1)
            {
                U.erase(id);
                for(int j=1;j<=C[id][0];j++)
                {
                    size[C[id][j]]--;
                }
            }
        }

        #ifdef debug_flag
            for(int i=1;i<=n;i++)
                // printf("%d: %d\n",i,size[i]);
                cerr<<i<<": "<<size[i]<<endl;
            debug(X.size);
        #endif

    }

    #ifdef show_time
        cerr<<"time for greedy_init loop: "<<time(0)-t1<<"s"<<endl;
    #endif

}

void init_delta()
{
    for(int i=1;i<=n;i++)
    {
        if(covered[i]->size==1)
        {
            int the_one=covered[i]->get_the_kth(1);
            #ifdef watch_mode
                if(!is_center[the_one])
                {
                    cerr<<"Error! Not covered by a center!\n";
                    return ;
                }
            #endif
            delta[the_one]+=weight[i];
        }

        if(!is_center[i])
        {
            for(int j=1;j<=V[i][0];j++)
            {
                int id=V[i][j];
                if(covered[id]->size==0)
                {
                    delta[i]+=weight[id];
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

    for(int i=1;i<=V[new_center][0];i++)
    {
        int id=V[new_center][i];
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

    #ifdef watch_mode
        if(benefit!=delta[new_center])
        {
            debug(V[new_center][0]);
            debug(benefit);
            debug(delta[new_center]);
            cerr<<"Error! Benefit is not equal to delta!\n";
        }
    #endif

    backup[0][0]=backup_num;
    return benefit;
}

void restore_backup()
{
    int backup_num=backup[0][0];
    for(int i=backup_num;i;i--)
    {
        delta[backup[i][0]]=backup[i][1];
    }
    return ;
}

void find_best_pair(int& u, int& v,ll& swap_val)
{
    int left=U.size;
    int picked=U.get_the_kth(rand()%left+1);
    // backup();
    ll best_swap_val=inf;
    int best_swap_cnt=0;
    ll benefit=0;

    for(int i=1;i<=C[picked][0];i++)
    {
        int new_center=C[picked][i];

        #ifdef watch_mode
            if(is_center[new_center])
            {
                cerr<<"Error! New center is already a center!"<<endl;
                return ;
            }
        #endif

        benefit=try_to_open(new_center);

        #ifdef debug_flag
            debug(new_center);
            debug(benefit);
        #endif

        for(int int_offset=0;int_offset<(X.volume)/BIT_WIDTH+1;int_offset++)
        {
            if(!(X.bit[int_offset]))
            {
                continue;
            }
            for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
            {
                if((1<<bit_offset)>X.bit[int_offset]) break;
                if((1<<bit_offset)&(X.bit[int_offset]))
                {
                    int id=calc_id(int_offset,bit_offset);

                    #ifdef debug_flag
                        debug(int_offset);
                        debug(bit_offset);
                        debug(id);
                    #endif

                    if(new_center==last_old&&id==last_new) continue;
                    if(delta[id]-benefit<best_swap_val)
                    {
                        best_swap_val=delta[id]-benefit;
                        best_swap_cnt=1;
                        u=new_center;
                        v=id;
                    }
                    else if(delta[id]-benefit==best_swap_val)
                    {
                        best_swap_cnt++;
                        if(rand()%best_swap_cnt==0)
                        {
                            u=new_center;
                            v=id;
                        }
                    }
                }
            }
        }
        restore_backup();
    }
    last_new=u;
    last_old=v;
    swap_val=best_swap_val;
    return ;
}

void merge_influence(int new_center,int old_center)
{
    for(int i=1;i<=V[new_center][0];i++)
    {
        int id=V[new_center][i];

        if(covered[id]->size==0)
        {
            for(int j=1;j<=C[id][0];j++)
            {
                int the_one=C[id][j];
                if(the_one==new_center) continue;

                #ifdef watch_mode
                    if(is_center[the_one])
                    {
                        cerr<<"Error! Non-Center covered other1!\n";
                        return ;
                    }
                #endif

                delta[the_one]-=weight[id];  // cancel reward
            }
            U.erase(id);
        }
        else if(covered[id]->size==1)
        {
            int the_one=covered[id]->get_the_kth(1);
            delta[the_one]-=weight[id];  // cancel penalty
        }

        covered[id]->insert(new_center);
    }

    for(int i=1;i<=V[old_center][0];i++)
    {
        int id=V[old_center][i];

        covered[id]->erase(old_center);

        if(covered[id]->size==0)
        {
            for(int j=1;j<=C[id][0];j++)
            {
                int the_one=C[id][j];
                if(the_one==old_center) continue;

                #ifdef watch_mode
                    if(is_center[the_one])
                    {
                        cerr<<"Error! Non-Center covered other2!\n";
                        return ;
                    }
                #endif

                delta[the_one]+=weight[id];  // add reward
            }
            U.insert(id);
        }
        else if(covered[id]->size==1)
        {
            int the_one=covered[id]->get_the_kth(1);
            delta[the_one]+=weight[id];  // add penalty
        }
    }

    return ;
}

void do_swap(int u,int v)
{
    is_center[v]=false;
    X.erase(v);
    is_center[u]=true;
    X.insert(u);
    merge_influence(u,v);
    return ;
}

void increase_weight()
{
    for(int i=1;i<=n;i++)
        if(covered[i]->size==0)
        {
            weight[i]++;
            for(int j=1;j<=C[i][0];j++)
            {
                int id=C[i][j];
                #ifdef watch_mode
                    if(is_center[id])
                        cerr<<"Error! Some center doesn't cover the point it should do!\n";
                        return ;
                #endif
                delta[id]++;
                memset(delta,0,sizeof(delta));
                init_delta();
            }
        }
    return ;
}

void VWTS()
{
    f=calc_f();
    int u,v;
    ll decrement;
    while(!can_stop_VWTS())
    {
        #ifdef debug_flag
          debug("hello");
        #endif

        find_best_pair(u,v,decrement);
        do_swap(u,v);
        if(decrement>=0) increase_weight();

        #ifdef watch_mode
            debug(U.size);
        #endif
    }
    return ;
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
../data/u1817p150r91.60.txt
../data/u1817p140r101.60.txt
../data/u1817p130r104.73.txt
../data/u1817p10r457.91.txt
../data/u1060p150r447.01.txt
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