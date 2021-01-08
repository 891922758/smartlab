#include<cstdio>
#include<iostream>
#include<algorithm>
#include<cstring>
#include<ctime>
#include<assert.h>
#include"MY_SET"
// #define debug_flag
// #define show_delta
#define test_mode
// #define watch_mode
// #define show_time
#define inf 1000000007
#define BIT_WIDTH 30
#define ll long long
#define calc_id(int_offset,bit_offset) int_offset*BIT_WIDTH+bit_offset
#define debug(x) cerr<<#x<<"="<<x<<endl
using namespace std;

const int maxn=3e3+50;
MY_SET U(maxn),X(maxn);
MY_SET X_best(maxn);
int C[maxn][maxn],V[maxn][maxn];
bool is_center[maxn];
MY_SET *covered[maxn];
ll delta[maxn],backup[maxn][2];
ll weight[maxn];
ll f,best_f;
int last_new,last_old;
int t0;
int least_uncovered;
// for input
int n,p;
int time_limit=60*0+30,seed=time(0);
#ifdef test_mode
    char test_result[]="result.txt";
    FILE* result_fp=fopen(test_result,"a");
#endif

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
    void print_weight()
    {
        cerr<<"----------weight table----------"<<endl;
        for(int i=1;i<=n;i++)
        {
            cerr<<"\t"<<i<<": "<<weight[i]<<endl;
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
        #ifndef test_mode
            freopen(argv[1],"r",stdin);
            freopen(argv[2],"w",stdout);
            sscanf(argv[3],"%d",&time_limit);
            sscanf(argv[4],"%d",&seed);
        #endif
        #ifdef test_mode
            freopen(argv[1],"r",stdin);
            freopen(argv[2],"w",stdout);
            sscanf(argv[3],"%d",&time_limit);
            sscanf(argv[4],"%d",&seed);
        #endif
    }

    #ifdef test_mode
        fprintf(result_fp,"%s\t\t",argv[1]);
    #endif
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
        // cerr<<"time for greedy_init loop: "<<time(0)-t1<<"s"<<endl;
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
        print_weight();
    #endif
}

bool can_stop_VWTS()
{
    if(U.size==0) return true;
    if(time(0)-t0>=time_limit) return true;
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
            // debug(V[new_center][0]);
            // debug(benefit);
            // debug(delta[new_center]);
            cerr<<"Error! Benefit is not equal to delta!\n";
            #ifdef show_delta
                print_delta();
            #endif
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

void find_best_pair(int& u, int& v)
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
        if(new_center==last_new||new_center==last_old)
            continue;

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

                    if(id==last_new||id==last_old) continue;
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
    #ifdef watch_mode
        // debug(X.size);
        if(u==v)
            cerr<<"Warning!new center is equal to old center!\n";
    #endif
    merge_influence(u,v);
    return ;
}

void increase_weight()
{
    #ifdef debug_flag
        // cerr<<"increasing weight!\n";
        int delta_[maxn];
        for(int i=1;i<=n;i++)
            delta_[i]=delta[i];
    #endif

    #ifdef watch_mode
        // cerr<<"increasing!\n";
        int cnt=0;
    #endif

    for(int i=1;i<=n;i++)
        if(covered[i]->size==0)
        {
            weight[i]++;
            #ifdef watch_mode
                cnt++;
            #endif
            for(int j=1;j<=C[i][0];j++)
            {
                int id=C[i][j];
                #ifdef watch_mode
                    // cerr<<id<<"  ";
                    if(weight[i]<0)
                    {
                        cerr<<"Error! Weight overflows!\n";
                        return ;
                    }
                    if(is_center[id])
                    {
                        cerr<<"Error! Some center doesn't cover the point it should do!\n";
                        return ;
                    }
                #endif
                delta[id]++;
            }
        }

    #ifdef watch_mode
        // debug(cnt);
    #endif

    #ifdef debug_flag
        memset(delta,0,sizeof(delta));
        init_delta();
        for(int i=1;i<=n;i++)
            if(delta_[i]!=delta[i])
            {
                cerr<<"----------diff increment update & whole update----------"<<endl;
                cerr<<"\t  "<<i<<": "<<delta_[i]<<"  "<<delta[i]<<endl;
            }

        if(cnt!=U.size)
            cerr<<"Error! cnt is not equal to U.size!"<<endl;
    #endif

    return ;
}

void VWTS()
{
    #ifdef watch_mode
        cerr<<"using VWTS\n";
    #endif
    f=calc_f();
    int u,v;
    X_best=X;
    int last_U_size=U.size;
    least_uncovered=U.size;
    while(!can_stop_VWTS())
    {
        #ifdef debug_flag
          debug("hello");
        #endif

        find_best_pair(u,v);
        do_swap(u,v);
        if(U.size<least_uncovered)
        {
            X_best=X;
            least_uncovered=U.size;
        }
        else if(U.size>=last_U_size) increase_weight();
        last_U_size=U.size;

        #ifdef watch_mode
            // cerr<<"in VWTS"<<endl;
            // debug(U.size);
            // cerr<<"\n";
            // debug(U.the_one);
            // debug(weight[U.the_one]);
            // if(U.the_one!=-1)
            // for(int i=1;i<=n;i++)
            //     if(weight[i]>weight[U.the_one])
            //         cerr<<i<<"has larger weight "<<weight[i]<<" than "<<weight[U.the_one]<<endl;
            debug(least_uncovered);
        #endif
    }
    return ;
}

bool check()
{
    MY_SET checker(n);
    for(int int_offset=0;int_offset<(X_best.volume)/BIT_WIDTH+1;int_offset++)
    {
        if(!(X_best.bit[int_offset]))
        {
            continue;
        }
        for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
        {
            if((1<<bit_offset)>X_best.bit[int_offset]) break;
            if((1<<bit_offset)&(X_best.bit[int_offset]))
            {
                int id=calc_id(int_offset,bit_offset);
                for(int j=1;j<=V[id][0];j++)
                {
                    checker.insert(V[id][j]);
                }
            }
        }
    }
    #ifdef watch_mode
        cerr<<"Uncovered: "<<n-checker.size<<endl;
    #endif
    return checker.size==n;
}

void print_result()
{
    #ifdef test_mode
        for(int int_offset=0;int_offset<(X_best.volume)/BIT_WIDTH+1;int_offset++)
        {
            if(!(X_best.bit[int_offset]))
            {
                continue;
            }
            for(int bit_offset=0;bit_offset<BIT_WIDTH;bit_offset++)
            {
                if((1<<bit_offset)>X_best.bit[int_offset]) break;
                if((1<<bit_offset)&(X_best.bit[int_offset]))
                {
                    printf("%d ",calc_id(int_offset,bit_offset));
                }
            }
        }
        putchar('\n');
    #endif

    #ifdef test_mode
        if(check()) fprintf(result_fp,"right\t\t%lds\n",time(0)-t0);
        else fprintf(result_fp,"wrong\t\t%lds\t\t%d\n",time(0)-t0,least_uncovered);
        fclose(result_fp);
    #endif

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
    #ifdef watch_mode
        if(check()) cerr<<"right!\n";
        else cerr<<"wrong!\n";
        cerr<<time(0)-t0<<"s"<<endl;
    #endif
    return 0;
}

/*
../data/1simple.txt
../data/pcb3038p100r206.31.txt
../data/pcb3038p100r206.6.txt
../data/pcb3038p100r206.63.txt
../data/pcb3038p10r728.54.txt
../data/pcb3038p150r164.40.txt
../data/pcb3038p150r164.55.txt
../data/pcb3038p150r164.77.txt
../data/pcb3038p200r140.06.txt
../data/pcb3038p200r140.09.txt
../data/pcb3038p200r140.90.txt
../data/pcb3038p20r493.04.txt
../data/pcb3038p250r122.25.txt
../data/pcb3038p300r115.00.txt
../data/pcb3038p30r393.50.txt
../data/pcb3038p350r104.68.txt
../data/pcb3038p400r96.88.txt
../data/pcb3038p40r336.42.txt
../data/pcb3038p450r88.55.txt
../data/pcb3038p500r84.58.txt
../data/pcb3038p50r297.83.txt
../data/pcb3038p50r298.04.txt
../data/pcb3038p50r298.10.txt
../data/pmed1.n100p5.txt
../data/pmed10.n200p67.txt
../data/pmed11.n300p5.txt
../data/pmed12.n300p10.txt
../data/pmed13.n300p30.txt
../data/pmed14.n300p60.txt
../data/pmed15.n300p100.txt
../data/pmed16.n400p5.txt
../data/pmed17.n400p10.txt
../data/pmed18.n400p40.txt
../data/pmed19.n400p80.txt
../data/pmed2.n100p10.txt
../data/pmed20.n400p133.txt
../data/pmed21.n500p5.txt
../data/pmed22.n500p10.txt
../data/pmed23.n500p50.txt
../data/pmed24.n500p100.txt
../data/pmed25.n500p167.txt
../data/pmed26.n600p5.txt
../data/pmed27.n600p10.txt
../data/pmed28.n600p60.txt
../data/pmed29.n600p120.txt
../data/pmed3.n100p10.txt
../data/pmed30.n600p200.txt
../data/pmed31.n700p5.txt
../data/pmed32.n700p10.txt
../data/pmed33.n700p70.txt
../data/pmed34.n700p140.txt
../data/pmed35.n800p5.txt
../data/pmed36.n800p10.txt
../data/pmed37.n800p80.txt
../data/pmed38.n900p5.txt
../data/pmed39.n900p10.txt
../data/pmed4.n100p20.txt
../data/pmed40.n900p90.txt
../data/pmed5.n100p33.txt
../data/pmed6.n200p5.txt
../data/pmed7.n200p10.txt
../data/pmed8.n200p20.txt
../data/pmed9.n200p40.txt
../data/rl1323p100r789.70.txt
../data/rl1323p10r3077.30.txt
../data/rl1323p20r2016.40.txt
../data/rl1323p30r1631.50.txt
../data/rl1323p40r1352.36.txt
../data/rl1323p50r1187.27.txt
../data/rl1323p60r1063.01.txt
../data/rl1323p70r971.93.txt
../data/rl1323p80r895.06.txt
../data/rl1323p90r832.00.txt
../data/u1060p100r570.01.txt
../data/u1060p10r2273.08.txt
../data/u1060p110r538.84.txt
../data/u1060p120r510.27.txt
../data/u1060p130r499.65.txt
../data/u1060p140r452.46.txt
../data/u1060p150r447.01.txt
../data/u1060p20r1580.80.txt
../data/u1060p30r1207.77.txt
../data/u1060p40r1020.56.txt
../data/u1060p50r904.92.txt
../data/u1060p60r781.17.txt
../data/u1060p70r710.75.txt
../data/u1060p80r652.16.txt
../data/u1060p90r607.87.txt
../data/u1817p100r126.99.txt    
../data/u1817p10r457.91.txt     
../data/u1817p110r109.25.txt    1
../data/u1817p120r107.76.txt    0
../data/u1817p130r104.73.txt    11
../data/u1817p140r101.60.txt    4
../data/u1817p150r91.60.txt     2
../data/u1817p20r309.01.txt
../data/u1817p30r240.99.txt     
../data/u1817p40r209.45.txt     3
../data/u1817p50r184.91.txt     1
../data/u1817p60r162.64.txt     2
../data/u1817p70r148.11.txt     0
../data/u1817p80r136.77.txt     10
../data/u1817p90r129.51.txt     1
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

// is_center[1]=is_center[4]=1;
// X.insert(1);
// X.insert(4);
// covered[1]->insert(1);
// covered[4]->insert(1);
// covered[1]->insert(4);
// covered[4]->insert(4);
// U.erase(1);
// U.erase(4);