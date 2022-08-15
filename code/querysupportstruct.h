#ifndef Q_H_INCLUDED
#define Q_H_INCLUDED
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<set>
#include<map>
#include<cmath>
#include<stdlib.h>
#include<bitset>
#include<memory.h>
#include <sys/time.h>
#include "omp.h"
#include<time.h>
#include <fstream>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <sstream>
using namespace std;

#define UNMAX 0xffffffff

#define prime 739
#define bigger_p 1048576
#define timer 5
#define M 80000
#define Type 0
typedef unsigned long long uint64;
typedef unsigned short weightSize;
typedef unsigned int ui;
typedef unsigned long long ul;
typedef unsigned char uc;

class edgeval{
public:
    unsigned int val;
    string from,to;
    edgeval(string _from,string _to,unsigned int _val);
    bool operator < (const edgeval& b)const;
};


/*
Below is the priority_queue we design for the top-k heavy edges query
*/
class heap{
public:
    edgeval **num;
    int s;
    void heapSubTree(int k,int n);
    heap(){}
    void initialHeap(int l);
    int find(string s1,string s2);
    ~heap();
    void pop();
    void push(string s1,string s2,unsigned int weight);
    bool increase(string s1,string s2,unsigned int weight);
    unsigned int top();
    void topres(string &s1,string &s2);
    bool empty();
    int size();

};

class Blizzard_MPQ{
private:
    ul MagicTable[0x500];
public:
    Blizzard_MPQ();
    ul MPQHash(const char* str, ul HashType);

};


class Gbasket
{
public:
	uint64 *srcFig,*dstFig;
	uint64 *idx;
	weightSize* weight;
	Gbasket(uint64* _srcFig,uint64*_dstFig,uint64* _idx,weightSize* _weight);
	~Gbasket();
};
class basketNode{
public:
    Gbasket *value;
    basketNode *next[2];
    double bound;
    int level;

    basketNode();
    ~basketNode();
};


struct  hashvalue
{
	unsigned int key;
	int IDnum;
};
bool mycomp(const hashvalue &hv1, const hashvalue &hv2);
bool operator==(const hashvalue &hv1, const hashvalue &hv2);
int countjoin( vector<hashvalue> &V1, vector<hashvalue> &V2);
struct GSnode
{
	unsigned int h;
	GSnode* next;
};
struct degree_node
{
	unsigned int h;
	int degree;
};
bool comp_degree(const degree_node &dn1, const degree_node &dn2);

class GSketch
{
	private:
    set<unsigned long long> nodeSet;
	map<unsigned int, int> degree;
	map<unsigned int, int> index;
	map<unsigned int, int> idnums;
	vector<degree_node> sorted_nodes;
	int nodenum;
	int triangle_num;
	int edge_num;
	vector<GSnode*> sketch;
	public:
	GSketch(){
	    nodenum=0;
        edge_num=0;
        triangle_num=0;
	};
	~GSketch(){
	    degree.clear();
        for(int i=0;i<nodenum;i++)
        {
            GSnode* np=sketch[i];
            GSnode* tmp;
            while(np!=NULL)
            {
                tmp=np->next;
                delete np;
                np=tmp;
            }
        }
        sketch.clear();
        index.clear();
        idnums.clear();
        sorted_nodes.clear();
	};
	void get_neighbors(unsigned int h, vector<hashvalue>& hv);
	void insert_edge(unsigned int h1, unsigned int h2);
	void insert_node(unsigned int h1, int IDnum);
	void clean();
	int countTriangle();
};

class Auxo
{
private:
	int w;
	int r;
	int p;
	int f;
	int rfnum;
	int halfR;
	int rper;
	unsigned int* fvalue;//fvalue[i] means (1<<f)-1;
	int *fper;//fper[i] means the number of fingerprint an uint64 can contain
	Blizzard_MPQ *mpq;

	vector<basketNode*> checkMatrices;
    vector<int> posfind;
    vector<int> weiRes;
    vector<int> matrixLevel;


	public:
	    double fullCount;
	    double loadFactor;
	    basketNode *mainTree;
	    ul valueCou;
	    uint64 memoryAllocated1,memoryAllocated2,memoryAllocated3;
	    double insertNum;
	    ul matrixSize;
		Auxo(int width, int range, int p_num, int f_num);
		~Auxo()
		{
			freeValueTree(mainTree);
			delete []fvalue;
			delete []fper;

		 }
		 Gbasket* initialGbasket(int fsize1,int fsize2);
		 void MatrixAppend(basketNode* basketCurr,int level);
		 bool insert(string s1, string s2,int weight,double& insertMatrix);
		 void cleanupBuffer();
		 int edgeQuery(string s1, string s2,double& queryMatrix);
		 bool query(string s1, string s2);
		 int nodeValueQuery(string s1, int type, double& queryMatrix);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(string s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(string s1, vector<string> &IDs);
		 void nodePrecursorQuery(string s2, vector<string> &IDs);
		 unsigned int getNodeValue(basketNode* basketCurr,int level,unsigned int *poses, unsigned int g1,bool type);
		 void freeValueTree(basketNode* root);
		 void sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, ul g1,vector<ul>& neis,ul prefix,int preLen,bool type);
		 bool reachQuery(string s1,string s2,double &queryTime);
};

class GSS_Chain
{
private:
	int w;
	int r;
	int p;
	int f;
	int rfnum;
	int halfR;
	int rper;
	unsigned int* fvalue;//fvalue[i] means (1<<f)-1;
	int *fper;//fper[i] means the number of fingerprint an uint64 can contain
	Blizzard_MPQ *mpq;
	vector<basketNode*> checkMatrices;
    vector<int> posfind;
    vector<int> weiRes;
    vector<int> matrixLevel;


	public:
	    double fullCount;
	    double loadFactor;
	    basketNode *valueTree;
	    ul valueCou;
	    uint64 memoryAllocated1,memoryAllocated2,memoryAllocated3;
	    double insertNum;
	    ul matrixSize;
		GSS_Chain(int width, int range, int p_num, int f_num);
		~GSS_Chain()
		{
			freeValueTree(valueTree);
			delete []fvalue;
			delete []fper;

        }
		 Gbasket* initialGbasket(int fsize1,int fsize2);
		 void MatrixAppend(basketNode* basketCurr);
		 void insert(string s1, string s2,int weight,double& insertMatrix);
		 void cleanupBuffer();
		 int edgeQuery(string s1, string s2,double& queryMatrix);
		 bool query(string s1, string s2);
		 int nodeValueQuery(string s1, int type, double& queryMatrix);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(string s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(string s1, vector<string> &IDs);
		 void nodePrecursorQuery(string s2, vector<string> &IDs);
		 void freeValueTree(basketNode* root);
		 void sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, ul g1,vector<ul>& neis,ul prefix,int preLen,bool type);
		 bool reachQuery(string s1,string s2,double &queryTime);

};

class AuxoPro
{
private:
	int w;
	int r;
	int p;
	int f;
	int rfnum;
	int halfR;
	int rper;
	int num1,num2;
	ul* fvalue;//fvalue[i] means (1<<f)-1;
	int *fper;//fper[i] means the number of fingerprint an uint64 can contain
	Blizzard_MPQ *mpq;
	vector<basketNode*> checkMatrices;
    vector<int> posfind;
    vector<int> weiRes;
    vector<int> matrixLevel;


	public:
	    double fullCount;
	    double loadFactor;
	    basketNode *mainTree,*deputyTree;
	    ul valueCou;
	    uint64 memoryAllocated1,memoryAllocated2,memoryAllocated3;
	    double insertNum;
	    ul matrixSize;
	    int cnt;
		AuxoPro(int width, int range, int p_num, int f_num);
		~AuxoPro()
		{
			freeValueTree(mainTree);
			freeValueTree(deputyTree);
			delete []fvalue;
			delete []fper;

		 }
		 basketNode* getDeputyBasketNode(unsigned int fp1,unsigned int fp2,int& level);
		 Gbasket* initialGbasket(int fsize1,int fsize2);
		 void MatrixAppend(basketNode* basketCurr,int& level);
		 int insert(string s1, string s2,int weight,double& insertMatrix);
		 void cleanupBuffer();
		 int edgeQuery(string s1, string s2,double& queryMatrix);
		 bool query(string s1, string s2);
		 int nodeValueQuery(string s1, int type, double& queryMatrix);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(string s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(string s1, vector<ul> &neis);
		 void nodePrecursorQuery(string s2, vector<string> &IDs);
		 void getMainNodeValue(basketNode* basketCurr,int level,ul g1,
                            bool type,int& cnt);
		 void freeValueTree(basketNode* root);
		 bool reachQuery(string s1,string s2,double &queryTime);
		 void sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, ul g1,vector<ul>& neis,ul prefix,int preLen,bool type);
		 void findAllEdgeForTriangle(GSketch* gs,ul preLen1,ul preLen2,ul prefix1, ul prefix2, int level,basketNode* basketCurr);
		 int triangleCounting();
};

ul calHash(ul tmp_g,ul tmp_s,int k,int w);

#endif // Q_H_INCLUDED
