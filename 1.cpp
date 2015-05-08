#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <map>
using namespace std;

#define MAX_VERTEX_NUM 3000             // 最大顶点个数
#define MAX_KCLASS_NUM 3000


/*-----------------------------------图的邻接表存储表示-----------------*/
typedef struct ArcNode{
    int adjvex;                         //该弧所指向的顶点的位置
    struct ArcNode *nextarc;            //指向下一条弧的指针
}ArcNode;

typedef struct VNode{
    ArcNode *firstarc=NULL;             //指向第一条依附该顶点的弧
    ArcNode *lastarc=NULL;              //指向最后一条依附该顶点的弧
    ArcNode *firstspe=NULL;             //指向第一条特殊边
    int color=0;                        //还未染色
}VNode,AdjList[MAX_VERTEX_NUM];

//---------------------顶点对----------
typedef pair<int ,int> NodePair;
//--------------------存储kclass
typedef struct ENode{
    int EdgeNum;
    struct ENode * nextenode=NULL;
}ENode;
typedef struct EFNode{
    ENode * firste=NULL;
}EFNode;
//----------------------------------图的一般表示---------------------
typedef struct{
    AdjList vertices;
    int vexnum,arcnum;                  //图的当前顶点数和弧数
    int oldarcnum;                      //图的原本弧数
    int *deg,maxdeg=0;                           //顶点度数
    map<NodePair,int> NodeToEdge;//存储边
    NodePair *PairToNode;
    int *sup,maxsup=0;
    int kmax;

    EFNode kclass[MAX_KCLASS_NUM];
}ALGraph;


NodePair Transform(int a,int b)
{
    NodePair temp;
    if(a<b)
    {
        temp.first=a;
        temp.second=b;
    }
    else
    {
        temp.first=b;
        temp.second=a;
    }
    return temp;
}
bool CreateALGraph(ALGraph &g)
{
    int i,v1,v2;
    ArcNode *p1,*p2;

    ifstream file("e:/my.txt");          //输入数据

    file>>g.vexnum;
    file>>g.arcnum;
    g.oldarcnum=g.arcnum;
    g.deg=new int[g.vexnum+1];
    for(i=1;i<=g.vexnum;i++)
        g.deg[i]=0;
    for(i=1;i<=g.arcnum;i++)//输入边的信息
    {
        file>>v1>>v2;

        g.deg[v1]++;
        if(g.deg[v1]>g.maxdeg)  g.maxdeg=g.deg[v1];
        g.deg[v2]++;
        if(g.deg[v2]>g.maxdeg)  g.maxdeg=g.deg[v2];

        //将此边输入到NodeTopair
        g.NodeToEdge.insert(pair<NodePair,int>(Transform(v1,v2),i));

        p1=(ArcNode*)malloc(sizeof(ArcNode));//建立结点
        if(!p1) return false;
        p2=(ArcNode*)malloc(sizeof(ArcNode));//建立结点
        if(!p2) return false;

        p1->adjvex=v2;
        p1->nextarc=g.vertices[v1].firstarc;//顶点v1的链表
        g.vertices[v1].firstarc=p1;//添加到最左边
        if(!g.vertices[v1].lastarc)     g.vertices[v1].lastarc=p1;

        p2->adjvex=v1;
        p2->nextarc=g.vertices[v2].firstarc;//顶点v2的链表
        g.vertices[v2].firstarc=p2;//添加到最左边
        if(!g.vertices[v2].lastarc)     g.vertices[v2].lastarc=p2;

    }
    cout<<"Graph Created!"<<endl;
    return true;
}

bool IsBiparGraph(ALGraph &g)//判断是否为二分图，若是则分成二分图的两个顶点集合
{
    queue<int> Q;
    int precolor,v,u;
    ArcNode *w;
    for(v=1;v<=g.vexnum;v++)
    {
        if(g.vertices[v].color==0)
        {
            g.vertices[v].color=1;
            Q.push(v);
            while(!Q.empty())
            {
                u=Q.front();
                Q.pop();
                w=g.vertices[u].firstarc;
                precolor=g.vertices[u].color;
                while(w!=NULL)
                {
                    if(g.vertices[w->adjvex].color==0)
                    {
                        g.vertices[w->adjvex].color=-precolor;
                        Q.push(w->adjvex);
                    }
                    else if(g.vertices[w->adjvex].color!=-precolor)
                    {
                        return false;
                    }
                    w=w->nextarc;
                }
            }
        }
    }
    return true;
}

bool AddSpecialEdge(ALGraph& g)
{
    int i,j,k;
    ArcNode* p,*p1,*p2;
    int neighbor[MAX_VERTEX_NUM],num;
    NodePair temp;

    cout<<"Adding SpecialEdge..."<<endl;
    for(i=1;i<=g.vexnum;i++)
    {
        p=g.vertices[i].firstarc;
        num=0;
        while(p)
        {
            num++;
            neighbor[num]=p->adjvex;
            p=p->nextarc;
        }
        if(num>=2)
        {
            for(j=1;j<num;j++)
                for(k=j+1;k<=num;k++)
                {
                    temp=Transform(neighbor[j],neighbor[k]);

                    if(g.NodeToEdge[temp]==0)
                    {
                        g.NodeToEdge[temp]=++g.arcnum;
                        //将特殊边(neighbor[j],neighbor[k])添加到图g中，在以后的算法中将不分别特殊边与普通边，都视为普通边
                        p1=(ArcNode*)malloc(sizeof(ArcNode));//建立结点
                        if(!p1) return false;
                        p2=(ArcNode*)malloc(sizeof(ArcNode));//建立结点
                        if(!p2) return false;

                        p1->adjvex=neighbor[j];
                        p1->nextarc=g.vertices[neighbor[k]].firstspe;//顶点neighbor[k]的链表
                        g.vertices[neighbor[k]].firstspe=p1;//添加到最左边

                        p2->adjvex=neighbor[k];
                        p2->nextarc=g.vertices[neighbor[j]].firstspe;//顶点neighbor[j]的链表
                        g.vertices[neighbor[j]].firstspe=p2;//添加到最左边

                        g.deg[neighbor[j]]++;
                        g.deg[neighbor[k]]++;
                        if(g.deg[neighbor[j]]>g.maxdeg)     g.maxdeg=g.deg[neighbor[j]];
                        if(g.deg[neighbor[k]]>g.maxdeg)     g.maxdeg=g.deg[neighbor[k]];
                    }
                }
        }
    }
    for(i=1;i<=g.vexnum;i++)
    {
        if(g.vertices[i].firstarc)
            g.vertices[i].lastarc->nextarc=g.vertices[i].firstspe;
    }
    cout<<"Add SpecialEdge complete!"<<endl;
    return true;
}

bool CountSup(ALGraph &g)//计算每条边的support
{
    int i,j;
    g.sup=new int[g.arcnum+1];
    int *vert=new int[g.vexnum+1];
    int *pos=new int[g.vexnum+1];

    for(i=1;i<=g.arcnum;i++)
        g.sup[i]=0;
    //插入排序，按照度递减的顺序存储在vert中
    for(i=1;i<=g.vexnum;i++)
        vert[i]=i;
    for(i=2;i<=g.vexnum;i++)
    {
        if(g.deg[vert[i]]>g.deg[vert[i-1]])
        {
            vert[0]=vert[i];
            vert[i]=vert[i-1];
            for(j=i-2;g.deg[vert[0]]>g.deg[vert[j]];--j)
                vert[j+1]=vert[j];
            vert[j+1]=vert[0];
        }
    }
    for(i=1;i<=g.vexnum;i++)
        pos[vert[i]]=i;
    //---------------------------------------
    //---------------forward list all the triangles in a graph
    AdjList A;
    int v,u,w,x;
    ArcNode *p,*p1,*p2;
    ArcNode *pr,*pn,*pe;
    cout<<"Counting up The Support..."<<endl;

    for(i=1;i<=g.vexnum;i++)
    {
        cout<<"\r";
        cout<<(int)((float)i/g.vexnum*100)<<"%";
        v=vert[i];
        p=g.vertices[v].firstarc;
        while(p)
        {
            u=p->adjvex;//对v的每个邻居
            if(pos[u]<pos[v])
            {
                p=p->nextarc;
                continue;
            }
            //foreach w in A[u] 交 A[v] ,w,u,v是一个三角形
            p1=A[u].firstarc;
            p2=A[v].firstarc;
            while(p1 && p2)
            {

                if(p1->adjvex==p2->adjvex)
                {
                    x=g.NodeToEdge[Transform(u,v)];
                    g.sup[x]++;
                    if(g.sup[x]>g.maxsup) g.maxsup=g.sup[x];

                    x=g.NodeToEdge[Transform(u,p1->adjvex)];
                    g.sup[x]++;
                    if(g.sup[x]>g.maxsup) g.maxsup=g.sup[x];

                    x=g.NodeToEdge[Transform(v,p1->adjvex)];
                    g.sup[x]++;
                    if(g.sup[x]>g.maxsup) g.maxsup=g.sup[x];

                    p2=p2->nextarc;
                    p1=p1->nextarc;
                }
                else if(p1->adjvex<p2->adjvex)
                    p1=p1->nextarc;
                else if(p2->adjvex<p1->adjvex)
                    p2=p2->nextarc;
            }

             //将v添加到A[u]中
            pe=(ArcNode*)malloc(sizeof(ArcNode));
            pe->adjvex=v;
            pe->nextarc=NULL;
            if(A[u].firstarc==NULL)
                A[u].firstarc=pe;
            else if(A[u].firstarc->adjvex>v)
            {
                pe->nextarc=A[u].firstarc;
                A[u].firstarc=pe;
            }
            else
            {
                pr=A[u].firstarc;
                pn=pr->nextarc;
                while(pn!= NULL && pn->adjvex < v)
                {
                    pr=pn;
                    pn=pn->nextarc;
                }

                pe->nextarc=pr->nextarc;
                pr->nextarc=pe;

            }

            p=p->nextarc;
        }
    }
    //--------------------------------------------------


    delete []vert;
    delete []pos;
    cout<<"Counting complete!"<<endl;

}

bool KClass(ALGraph &g)
{
    int i,e,start,s,num;

    int *edge=new int[g.arcnum+1];//正确
    int *pos=new int[g.arcnum+1];//正确
    int *bin=new int[g.maxsup+1];//正确       sup正确   deg正确   邻接表正确
    //初始化PairToNode,边的编号对顶点对
    g.PairToNode=new NodePair[g.arcnum+1];//正确
    for(map<NodePair,int>::iterator ite=g.NodeToEdge.begin();ite!=g.NodeToEdge.end();ite++)
        g.PairToNode[ite->second]=ite->first;

    //桶排序
    for(i=0;i<=g.maxsup;i++)
        bin[i]=0;
    for(e=1;e<=g.arcnum;e++)
        bin[g.sup[e]]++;
    start=1;
    for(s=0;s<=g.maxsup;s++)
    {
        num=bin[s];
        bin[s]=start;
        start=start+num;
    }
    for(e=1;e<=g.arcnum;e++)
    {
        pos[e]=bin[g.sup[e]];
        edge[pos[e]]=e;
        bin[g.sup[e]]++;
    }
    //----------------------------------------------
    //----------恢复bin
    for(s=g.maxsup;s>=1;s--)
        bin[s]=bin[s-1];
    bin[0]=1;
    //---------------------------------
    //-------------------计算kclass----------------------------------------
    int k,u,v,w;
    int e1,e2,se1,pe1,se2,pe2,x,px;
    ArcNode *p;
    ENode *pe,*pr,*pn;
    k=2;i=1;
    cout<<"Calculate the k-class..."<<endl;

    while(i<=g.arcnum)
    {

        e=edge[i];
        if(g.sup[e]<=k-2)
        {
            cout<<"\r";
            cout<<(int)((float)i/g.arcnum*100)<<"%";
            u=g.PairToNode[e].first;
            v=g.PairToNode[e].second;

            g.NodeToEdge[Transform(u,v)]=0;
            bin[g.sup[e]]++;
            bin[g.sup[e]-1]=bin[g.sup[e]];
            //-----------------------
            pe=(ENode*)malloc(sizeof(ENode));
            pe->EdgeNum=e;
            pe->nextenode=NULL;
            if(g.kclass[k].firste==NULL)
                g.kclass[k].firste=pe;
            else if(g.kclass[k].firste->EdgeNum>e)
            {
                pe->nextenode=g.kclass[k].firste;
                g.kclass[k].firste=pe;
            }
            else
            {
                pr=g.kclass[k].firste;
                pn=pr->nextenode;
                while(pn!= NULL && pn->EdgeNum < e)
                {
                    pr=pn;
                    pn=pn->nextenode;
                }

                pe->nextenode=pr->nextenode;
                pr->nextenode=pe;

            }
            //----------------------------
            if(g.deg[v]<g.deg[u])
            {
                u=v-u;
                v=v-u;
                u=v+u;
            }
            p=g.vertices[u].firstarc;
            while(p)
            {
                w=p->adjvex;
                if(g.NodeToEdge[Transform(u,w)]!=0 && g.NodeToEdge[Transform(v,w)]!=0)
                {
                    //将边(v,w),(u,w)support减一并重新排序
                    e1=g.NodeToEdge[Transform(u,w)];
                    e2=g.NodeToEdge[Transform(v,w)];


                    se1=g.sup[e1];    pe1=pos[e1];
                    px=bin[se1];    x=edge[px];
                    if(x!=e1)
                    {
                        pos[e1]=px;     edge[pe1]=x;
                        pos[x]=pe1;     edge[px]=e1;
                    }
                    bin[se1]++;
                    g.sup[e1]--;

                    se2=g.sup[e2];    pe2=pos[e2];
                    px=bin[se2];    x=edge[px];
                    if(x!=e2)
                    {
                        pos[e2]=px;     edge[pe2]=x;
                        pos[x]=pe2;     edge[px]=e2;
                    }
                    bin[se2]++;
                    g.sup[e2]--;
                }
                p=p->nextarc;
            }
            i++;
            g.deg[u]--;
            g.deg[v]--;
        }
        else
        {
            k++;
        }
    }
    g.kmax=k;
    delete []pos;
    delete []edge;
    delete []bin;
    cout<<"Calculate kclsss complete"<<endl;
    cout<<"kmax:"<<k<<endl;
    return true;

}

bool KTruss(ALGraph g)
{
    int k=g.kmax;
    int kn;
    ENode *p,*p1,*p2,*p3;
    p3=(ENode*)malloc(sizeof(ENode));

    cout<<"Calculate the K-truss..."<<endl;
    ofstream file("e:/myout.txt");
    p=g.kclass[k].firste;
    file<<k<<"truss:";
    while(p)
    {
        if(p->EdgeNum<=g.oldarcnum)
            file<<g.PairToNode[p->EdgeNum].first<<"  "<<g.PairToNode[p->EdgeNum].second<<"  ";
        p=p->nextenode;
    }
    kn=k-1;
    while(g.kclass[kn].firste!=NULL || kn>=2)
    {
        if(g.kclass[kn].firste==NULL)
        {
            kn--;
            continue;
        }
        p3=(ENode*)malloc(sizeof(ENode));

        cout<<"\r";
        file<<kn<<"truss:";

        p1=g.kclass[kn].firste;
        p2=g.kclass[k].firste;
        if(p1->EdgeNum<p2->EdgeNum)
             g.kclass[kn].firste=p1;
        else
             g.kclass[kn].firste=p2;
        while(p1 && p2)
        {
            if(p1->EdgeNum>p2->EdgeNum)
            {
                p3->nextenode=p2;
                p3=p3->nextenode;
                p2=p2->nextenode;
            }
            else
            {
                p3->nextenode=p1;
                p3=p3->nextenode;
                p1=p1->nextenode;
            }
        }
        if(p1)
            p3->nextenode=p1;
        if(p2)
            p3->nextenode=p2;

        p3=NULL;
        p=g.kclass[kn].firste;
        k=kn;
        kn--;
        while(p)
        {
            if(p->EdgeNum<=g.oldarcnum)
                file<<g.PairToNode[p->EdgeNum].first<<"  "<<g.PairToNode[p->EdgeNum].second<<"  ";
            p=p->nextenode;
        }


    }
    cout<<"Calculate ktruss compelete";
}
int main(void)
{
    ALGraph g;
    CreateALGraph(g);
    if(!IsBiparGraph(g))//如果不是二分如，程序结束
        return 1;
    AddSpecialEdge(g);

    CountSup(g);
    KClass(g);

    KTruss(g);

    /*ENode *p;
    for(int i=0;i<=kmax;i++)
    {
        p=kclass[i].firste;
        cout<<i<<":";
        while(p)
        {
            cout<<p->EdgeNum<<"  ";
            p=p->nextenode;

        }
        cout<<endl;

    }*/
}
