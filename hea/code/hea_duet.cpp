#define _CRT_SECURE_NO_WARNINGS
#include<cstdio>
#include<iostream>
#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<stdlib.h>
#include<cstring>
#include<assert.h>
#define ll long long
#define debug(x) cerr<<#x<<"="<<x<<endl
#define MAX_ITERATION 8000
#define MAX_HYBRID_ITERATION 1000000
#define inf 1000000007
#define POPULATION_NUM 2
#define ELITE_NUM 2
#define INTRODUCE_CYCLE 10
#define FATHER_NUM 2
#define MUTATION_RATE 10000
using namespace std;

const int maxn=1010;
const int max_color_num=100;
struct edge{
    int to,next;
}e[maxn*maxn];
struct movement{
    int u,vi,vj;
    ll delt;
    int tot;
};
struct people{
    int sol[maxn];
    ll f;
}population[POPULATION_NUM+5],elite[ELITE_NUM+5];
int f_counter[maxn];
int set_size[POPULATION_NUM][max_color_num+5];
int belong[POPULATION_NUM][maxn];
int sol[maxn],best_sol[maxn];
int final_best_sol[maxn];
int adjacent_color_table[maxn][max_color_num+5];
ll tabu_tenure[maxn][max_color_num+5];
int head[maxn];
int map[POPULATION_NUM][max_color_num+5][maxn];
ll best_f,f,final_best_f=inf;
int iter,hybrid_iter;
int color_num=48;
int seed=time(0)%8051001;
int n,m,cnt;
int cycle;
int last_f,convergence_cnt;
int start_time;

void insert(int x,int y)
{
    e[++cnt].to=y;
    e[cnt].next=head[x];
    head[x]=cnt;
}

void read(int argc,char** argv)
{
    if(argc==1)
    {
        printf("file path: ");
        char file_path[81];  // ="../data/DSJC125.1.col";
        scanf("%s",file_path);
        printf("color num: ");
        scanf("%d",&color_num);
        freopen(file_path,"r",stdin);
    }
    else
    {
        freopen(argv[1],"r",stdin);
        freopen(argv[2],"w",stdout);
        sscanf(argv[3],"%d",&color_num);
        sscanf(argv[4],"%d",&seed);
    }

    char line[100];
    fgets(line,90,stdin);

    while(line[0]!='p')
    {
        fgets(line,90,stdin);
    }

    char temp[10];
    sscanf(line+1,"%s%d%d",temp,&n,&m);
    m>>=1;
    for(int i=1,x,y;i<=m;i++)
    {
        scanf("%s%d%d",temp,&x,&y);
        insert(x,y);
        insert(y,x);
    }

    return ;
}

void init_tabu(people& now)
{
    memset(adjacent_color_table,0,sizeof(adjacent_color_table));
    memset(tabu_tenure,0,sizeof(tabu_tenure));

    iter=0;
    f=now.f;
    best_f=f;

    for(int i=1;i<=n;i++)
    {
        sol[i]=now.sol[i];
        best_sol[i]=sol[i];
    }

    int temp_f=0;
    for(int i=1;i<=n;i++)
    {
        for(int j=head[i];j;j=e[j].next)
        {
            adjacent_color_table[i][now.sol[e[j].to]]++;
            if(sol[i]==sol[e[j].to]) temp_f++;
        }
    }

    temp_f>>=1;
    // assert(f==temp_f);
    return ;
}

bool can_stop()
{
    if(!best_f) return true;
    if(iter>MAX_ITERATION) return true;
    return false;
}

bool can_stop_hybrid()
{

    if(!final_best_f) return true;
    // if(hybrid_iter>MAX_HYBRID_ITERATION) return true;
    if(time(0)-start_time>=60*5+40) return true;
    return false;
}

int calc_delt(int u,int vi,int vj)
{
    return adjacent_color_table[u][vj]-adjacent_color_table[u][vi];
}

void update(movement &best_move,int u,int vi,int vj,int delt)
{
    if(delt!=best_move.delt)
    {
        best_move.u=u;
        best_move.vi=vi;
        best_move.vj=vj;
        best_move.delt=delt;
        best_move.tot=1;
    }
    else
    {
        best_move.tot++;
        // srand(int(clock()));
        // srand(seed);
        // seed=(seed*int(clock())+10231242)<<7;
        if(rand()%best_move.tot==0)
        {
            best_move.u=u;
            best_move.vi=vi;
            best_move.vj=vj;
            best_move.delt=delt;
        }
    }

    return ;
}

void find_move(movement &new_move)
{
    movement best_tabu,best_non_tabu;
    best_tabu.delt=best_non_tabu.delt=inf;
    best_tabu.tot=best_non_tabu.tot=0;

    ll rt=0;
    int normal_tabu_cnt=0,unnormal_tabu_cnt=0;
    int normal_nontabu_cnt=0,unnormal_nontabu_cnt=0;
    for(int i=1;i<=n;i++)
        if(adjacent_color_table[i][sol[i]]>0)
        {
            for(int k=1;k<=color_num;k++)
                if(k!=sol[i])
                {
                    rt=adjacent_color_table[i][k]-adjacent_color_table[i][sol[i]];  // calc_delt(i,sol[i],k);
                    if(iter<=tabu_tenure[i][k])
                    {
                        if(rt<=best_tabu.delt)
                        {
                            update(best_tabu,i,sol[i],k,rt);
                            // cerr<<"normal update best tabu\n";
                            normal_tabu_cnt++;
                        }
                        // else if(rand()%MUTATION_RATE==0)
                        // {
                        //     update(best_tabu,i,sol[i],k,rt);
                        //     // cerr<<"unnormal update best tabu\n";
                        //     unnormal_tabu_cnt++;
                        // }
                    }
                    else
                    {
                        if(rt<=best_non_tabu.delt)
                        {
                            update(best_non_tabu,i,sol[i],k,rt);
                            // cerr<<"normal update best non tabu\n";
                            normal_nontabu_cnt++;
                        }
                        // else if(rand()%MUTATION_RATE==0)
                        // {
                        //     update(best_non_tabu,i,sol[i],k,rt);
                        //     // cerr<<"unnormal update best non tabu\n";
                        //     normal_nontabu_cnt++;
                        // }
                    }
                }
        }

    // cerr<<"tabu mutation rate: "<<double(unnormal_tabu_cnt)/(unnormal_tabu_cnt+normal_tabu_cnt)<<endl;
    // cerr<<"non tabu mutation rate: "<<double(unnormal_nontabu_cnt)/(unnormal_nontabu_cnt+normal_nontabu_cnt)<<endl;
    if(((best_tabu.delt+f<best_f)&&(best_tabu.delt<best_non_tabu.delt))||(best_non_tabu.delt==inf))
        new_move=best_tabu;
    else //if((best_non_tabu.delt!=inf))
        new_move=best_non_tabu;
    // if(new_move.delt>0) system("pause");
    // else new_move.u=0;
    // assert(new_move.delt!=inf);
    return ;
}

void make_move(movement &new_move)
{
    // if(!(new_move.u)) return ;
    sol[new_move.u]=new_move.vj;
    // best_sol[new_move.u]=new_move.vj;
    for(int i=head[new_move.u];i;i=e[i].next)
    {
        adjacent_color_table[e[i].to][new_move.vi]--;
        adjacent_color_table[e[i].to][new_move.vj]++;
    }

    // assert(f);
    f+=new_move.delt;
    tabu_tenure[new_move.u][new_move.vi]=iter+f+rand()%10+1;
    // if(f) tabu_tenure[new_move.u][new_move.vi]=iter+110/f+f+rand()%10;
    // tabu_tenure[new_move.u][new_move.vi]=iter+(double)f*0.6+rand()%10;


    if(f<best_f)
    {
        best_f=f;
        for(int i=1;i<=n;i++)
            best_sol[i]=sol[i];
    }

    return ;
}

void tabu_search(people &start)
{
    // seed=seed*time(0)%8051001;
    // srand(seed);
    movement new_move;
    new_move.u=0;
    new_move.vi=new_move.vj=0;
    new_move.tot=0;
    new_move.delt=inf;

    while(!can_stop())
    {
        find_move(new_move);
        make_move(new_move);
        iter++;
    }

    start.f=best_f;
    for(int i=1;i<=n;i++)
        start.sol[i]=best_sol[i];
    return ;
}

void calc_f(people& now)
{
    now.f=0;
    for(int i=1;i<=n;i++)
    {
        for(int j=head[i];j;j=e[j].next)
            if(now.sol[i]==now.sol[e[j].to])
                now.f++;
    }
    now.f>>=1;

    return ;
}

void generate_people(people &now)
{
    // seed=(seed*int(clock())+10231242)&0xff23ade;
    // srand(seed);

    for(int i=1;i<=n;i++)
    {
        now.sol[i]=rand()%color_num+1;
    }

    calc_f(now);

    return ;
}

void generate_population()
{
    for(int i=1;i<=POPULATION_NUM;i++)
    {
        generate_people(population[i]);
    }

    for(int i=1;i<=ELITE_NUM;i++)
    {
        generate_people(elite[i]);
    }

    return ;
}

void crossover(int id1,int id2,people& son)
{
    memset(set_size,0,sizeof(set_size));
    // memset(belong,0,sizeof(belong));
    // memset(map,0,sizeof(map));

    int color1,color2;
    for(int i=1;i<=n;i++)
    {
        color1=population[id1].sol[i];
        belong[0][i]=color1;
        set_size[0][color1]++;
        map[0][color1][set_size[0][color1]]=i;
        map[0][color1][0]=set_size[0][color1];

        color2=population[id2].sol[i];
        belong[1][i]=color2;
        set_size[1][color2]++;
        map[1][color2][set_size[1][color2]]=i;
        map[1][color2][0]=set_size[1][color2];
    }

    int tot=n;
    int op=0;
    int max_size=0,max_id=0,max_cnt=0;
    int new_id=0;

    // seed=seed*time(0)%8051001;
    // srand(seed);

    while(tot)
    {
        
        // if(rand()%100<MUTATION_RATE)
        // {
        //     op=op^1;
        //     continue;
        // }

        max_size=0;
        max_id=0;
        max_cnt=0;

        for(int i=1;i<=color_num;i++)
            if(set_size[op][i]>max_size)
            {
                max_cnt=1;
                max_size=set_size[op][i];
                max_id=i;
            }
            else if(set_size[op][i]==max_size)
            {
                max_cnt++;
                if(rand()%max_cnt==0)
                {
                    max_id=i;
                }
            }

        new_id++;
        tot-=max_size;

        //for(int i=1;i<=n;i++)
        for(int k=1;k<=map[op][max_id][0];k++)
        {
            int i=map[op][max_id][k];
            if(belong[op][i]==max_id)
            {
                son.sol[i]=new_id;
                set_size[op][belong[op][i]]--;
                set_size[op^1][belong[op^1][i]]--;
                // set_size[(op+2)%FATHER_NUM][belong[(op+2)%FATHER_NUM][i]]--;
                belong[op][i]=0;
                belong[op^1][i]=0;
                // belong[(op+2)%FATHER_NUM][i]=0;
            }
        }

        op^=1;
        if(new_id==color_num) break;
    }

    // seed=seed*time(0)%8051001;
    // srand(seed);

    if(tot)
    {
        for(int i=1;i<=n;i++)
            if(belong[op][i])
            {
                belong[op][i]=0;
                son.sol[i]=rand()%new_id+1;
                tot--;
            }
    }

    calc_f(son);

    return ;
}

bool check_ans()
{
    for(int i=1;i<=n;i++)
        for(int j=head[i];j;j=e[j].next)
            if(final_best_sol[i]==final_best_sol[e[j].to])
                return false;

    return true;
}

void update_elite(people& elite,people& p)
{
    if(p.f<elite.f)
    {
        for(int i=1;i<=n;i++)
            elite.sol[i]=p.sol[i];
        elite.f=p.f;
    }
    if(p.f==elite.f)
    {
        if(p.f>=maxn) return ;
        f_counter[p.f]++;
        if(rand()%f_counter[p.f]==0)
        {
            for(int i=1;i<=n;i++)
                elite.sol[i]=p.sol[i];
        }
    }
    return ;
}

void update_final_best(people &now)
{
    if(now.f<final_best_f)
    {
        final_best_f=now.f;
        for(int i=1;i<=n;i++)
            final_best_sol[i]=now.sol[i];
        convergence_cnt=hybrid_iter;
    }

    return ;
}

void hybrid_evolution()
{
    people new_p1,new_p2;
    int p1_id=1,p2_id=2;

    while(!can_stop_hybrid())
    {
        crossover(p1_id,p2_id,new_p1);
        crossover(p2_id,p1_id,new_p2);

        init_tabu(new_p1);
        tabu_search(new_p1);
        // debug(best_f);
        population[p1_id]=new_p1;

        init_tabu(new_p2);
        tabu_search(new_p2);
        // debug(best_f);
        population[p2_id]=new_p2;
    
        update_elite(elite[1],new_p1);
        update_elite(elite[1],new_p2);

        update_final_best(elite[1]);

        // debug(final_best_f);

        hybrid_iter++;
        if(hybrid_iter%INTRODUCE_CYCLE==0)
        {
            population[p1_id]=elite[2];
            elite[2]=elite[1];
            generate_people(elite[1]);
            // cerr<<"introduce new population\n";

            // cycle++;
            // if(cycle%50==0)
            // {
            //     if(last_f==final_best_f) 
            //     {
            //         generate_people(population[p2_id]);
            //         // system("pause");
            //     }
            //     else last_f=final_best_f;
            // }
        }
    }

    return ;
}



int main(int argc,char** argv)
{
    start_time=time(0);

    read(argc,argv);
    srand(time(0));
    seed=(rand()+seed)*time(0);  // ;%8051001;  // seed 4704294 has best effort;
    srand(seed);
    generate_population();
    hybrid_evolution();

    for(int i=1;i<=n;i++)
    {
        printf("%d %d\n",i,final_best_sol[i]);
    }

    // if(check_ans()) cerr<<"right\n";
    // else cerr<<"wrong\n";
    // debug(time(0)-start_time);
    return 0;
}

/*  
../data/DSJC125.1.col
../data/DSJC250.9.col
../data/DSJC500.5.col
*/