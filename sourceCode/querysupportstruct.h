#ifndef Q_H_INCLUDED
#define Q_H_INCLUDED
#include<iostream>
#include<queue>
#include<cmath>
#include<stdlib.h>
#include<memory.h>
#include <sys/time.h>
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
typedef unsigned long ul;
typedef unsigned char uc;

class Blizzard_MPQ{
private:
    ul *MagicTable;
public:
    Blizzard_MPQ();
    ul MPQHash(const char* str, ul HashType);

};

int calHash(int tmp_g,int tmp_s,int k,int w);

struct Gbasket{
	uint64 *srcFig,*dstFig;
	uint64 *idx;
	weightSize* weight;
	Gbasket(uint64* _srcFig,uint64*_dstFig,uint64* _idx,weightSize* _weight){
	    srcFig=_srcFig;dstFig=_dstFig;
	    idx=_idx;
	    weight=_weight;
	}
	~Gbasket(){
	    delete [] dstFig;
        delete [] srcFig;
        delete [] idx;
        delete [] weight;
	}
};

struct basketNode{
    Gbasket *value;
    basketNode *next[2];
    double bound;
    int level;

    basketNode(){
        for(int i=0;i<2;i++){
            next[i]=NULL;
        }
        value=NULL;
        bound=0;
        level=0;
    }
    ~basketNode(){
        delete value;
    }
};

class Auxo{
private:
	int w;
	int r;
	int p;
	int s;
	int f;
	int rfnum;
	int halfR;
	int rper;
	bool useT;
	int tablesize;
	unsigned int* fvalue;//fvalue[i] means (1<<f)-1;
	int *fper;//fper[i] means the number of fingerprint an uint64 can contain
	Blizzard_MPQ *mpq;


	public:
	    basketNode *mainTree;
	    int valueCou;
	    uint64 memoryAllocated;
	    double insertNum;
	    int matrixSize;
		Auxo(int width, int range, int p_num, int f_num);
		~Auxo()
		{
			freeValueTree(mainTree);
			delete []fvalue;
			delete []fper;

		 }
		 Gbasket* initialGbasket(int fsize1,int fsize2);
		 void MatrixAppend(basketNode* basketCurr,int level);
		 bool insert(string s1, string s2,int weight,double& hashTime,double& insertMatrix);
		 void cleanupBuffer();
		 int edgeQuery(string s1, string s2,double& hashTime,double& queryMatrix);
		 bool query(string s1, string s2);
		 int nodeValueQuery(string s1, int type, double& hashTime,double& queryMatrix);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(string s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(string s1, vector<string> &IDs);
		 void nodePrecursorQuery(string s2, vector<string> &IDs);
		 unsigned int getNodeValue(basketNode* basketCurr,int level,unsigned int *poses, unsigned int g1,bool type);
		 void freeValueTree(basketNode* root);
};

class GSS_Chain{
private:
	int w;
	int r;
	int p;
	int s;
	int f;
	int rfnum;
	int halfR;
	int rper;
	bool useT;
	int tablesize;
	unsigned int* fvalue;//fvalue[i] means (1<<f)-1;
	int *fper;//fper[i] means the number of fingerprint an uint64 can contain
	Blizzard_MPQ *mpq;


	public:
	    basketNode *valueTree;
	    int valueCou;
	    uint64 memoryAllocated;
	    double insertNum;
	    int matrixSize;
		GSS_Chain(int width, int range, int p_num, int f_num);
		~GSS_Chain()
		{
			freeValueTree(valueTree);
			delete []fvalue;
			delete []fper;

        }
		 Gbasket* initialGbasket(int fsize1,int fsize2);
		 void MatrixAppend(basketNode* basketCurr);
		 void insert(string s1, string s2,int weight,double& hashTime,double& insertMatrix);
		 void cleanupBuffer();
		 int edgeQuery(string s1, string s2,double& hashTime,double& queryMatrix);
		 bool query(string s1, string s2);
		 int nodeValueQuery(string s1, int type, double& hashTime,double& queryMatrix);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(string s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(string s1, vector<string> &IDs);
		 void nodePrecursorQuery(string s2, vector<string> &IDs);
		 void freeValueTree(basketNode* root);
};

class ComAuxo{
private:
	int w;
	int r;
	int p;
	int s;
	int f;
	int rfnum;
	int halfR;
	int rper;
	bool useT;
	int tablesize;
	int num1,num2;
	unsigned int* fvalue;//fvalue[i] means (1<<f)-1;
	int *fper;//fper[i] means the number of fingerprint an uint64 can contain
	Blizzard_MPQ *mpq;


	public:
	    basketNode *mainTree,*deputyTree;
	    int valueCou;
	    uint64 memoryAllocated;
	    double insertNum;
	    int matrixSize;
		ComAuxo(int width, int range, int p_num, int f_num);
		~ComAuxo()
		{
			freeValueTree(mainTree);
			freeValueTree(deputyTree);
			delete []fvalue;
			delete []fper;

		 }
		 basketNode* getDeputyBasketNode(unsigned int fp1,unsigned int fp2,int& level);
		 Gbasket* initialGbasket(int fsize1,int fsize2);
		 void MatrixAppend(basketNode* basketCurr,int& level);
		 bool insert(string s1, string s2,int weight,double& hashTime,double& insertMatrix);
		 void cleanupBuffer();
		 int edgeQuery(string s1, string s2,double& hashTime,double& queryMatrix);
		 bool query(string s1, string s2);
		 int nodeValueQuery(string s1, int type, double& hashTime,double& queryMatrix);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(string s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(string s1, vector<string> &IDs);
		 void nodePrecursorQuery(string s2, vector<string> &IDs);
		 int getMainNodeValue(basketNode* basketCurr,int level,unsigned int *poses, unsigned int g1,bool type);
		 void freeValueTree(basketNode* root);
		 bool reachQuery(string s1,string s2);
		 void sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, unsigned int g1,vector<unsigned int> neis,unsigned int prefix,int preLen);
};
#endif // Q_H_INCLUDED