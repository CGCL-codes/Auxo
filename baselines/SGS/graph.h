#ifndef _GRAPH_
#define _GRAPH_
#include <iostream>
#include <unordered_map>
#include <vector>
#include<set>
#include<map>
#include <stdlib.h>
#include <iostream>
#include <semaphore.h>
#include <vector>
#include <map>
#include <bitset>
#include <cstring>
#include <ctime>
#include <omp.h>
#include <inttypes.h>

using namespace std;


class Graph
{
private:
	int numNodes;
	int num_supNode;
	int adjKeyNodes1;
	int adjKeyNodes2;
	int rnd;
	float ecnt;//处理边总数
	int costCounter;//压缩图边数
	set<int> nodeSet;
	unordered_map<int, int> nodesID;                         //查找节点对应邻接表位置,node-位置
	vector<set<int>> adjNodes;                               //邻接表
	vector<int> hashValue;
	unordered_map<int, int> nodesHashList;                   //node-hashvalue
	//unordered_map<int, map<int,char>> groupSupernodes;     //minHash-supnodes
	

	unordered_map<int, map<int,char>> supList;               //supnode-[node]
	unordered_map<int, int> nodeToSup;                       //node-supnode
	unordered_map<int,vector<int>> adjSup;                   //超点内所有节点的邻接点并集,supnode-[node]
	unordered_map<int, map<int,char>> supGraph;              //压缩图
	unordered_map<int, map<int,char>> interAddC;             //超点间C+,node-[nodes]，反存
	unordered_map<int, map<int,char>> intraAddC;             //超点内C+，点内也反存
	unordered_map<int, map<int,char>> interDelC;             //超点间C-
	unordered_map<int, map<int,char>> intraDelC;             //超点内C-
	set<int> supLoop;                                        //超点自环


	map<int, map<int, int>> realNumEdges;                    //超点内、间真实边数量
	int add_u;//加边(u,v)
	int add_v;
	map<int, vector<int>> moveNode_realEdges;                //移动的节点的对应不同超点的相关边,supnode-[nodes]


	//base
	unordered_map<int, set<int>> reCalcSimilar_base;          //加边以后需要重新计算的节点和超点的相似性,node-[supernodes]


	//对比MoSSo
	int c;                                                   //邻接点中取c个
	float e;                                                 //escape probability
	int allnodes;                                            //流图总共节点数量
	unordered_map<int, int> node_inGroup;                    //节点-minHash
	unordered_map<int, map<int, char>> groupNodes;           //minHash-nodes-'a'


	//optimistic_heristic_random
	unordered_map<int, set<int>> reCalcSimilar3;         //u-nodes、v-nodes; 存N(v)和u的2hop节点的交集，N(u)和v的2hop节点的交集

public:
	Graph();
	int printEndResult();

	void EndResult();

	int checkEdge(int, int);
	void addNodeToSupG(int);
	void addEdgeToSupG(int, int);//不需要组合计算相似性的函数
	void addEdge_update(int node1, int node2, int vv1, int mm1);
	float nodeSimilarSup1(int, int);//节点和超点的相似性,node-supNode


	//对比MoSSo
	void HashAllNodes();//数据集中全部节点进行哈希
	void addNodeToSupG_MoSSo(int);
	void addEdgeToSupG_MoSSo(int, int);
	set<int> getRandomNeighbor_MoSSo(int);
	void MoSSo(int node);


	//base
	void addEdgeToSupG1(int node1, int node2);//对应计算节点和超点相似性组合test
	void changedSimilar1();//alg1
	void traverseReCaleSimi1();	//alg2
	void update1(int node, int s1, int s2, bool moveOrNew);
	void changedSimilar2();//alg3，记录u-[supernodes_include2hopUp] ——搭配traverseReCaleSimi1

	//random
	int getIntraPi(int sup);
	int getInterPi(int s1, int s2);
	void movedNode_realEdges(int);
	vector<int> getDelta_moveNode(int s1, int s2, bool move, bool New);


	void update(int node, int s1, int s2, bool moveOrNew);
	void update_realNumEdges(int s1, int s2, bool moveOrNew);
	void update_adjSup(int node, int s1, int s2, bool moveOrNew);
	void delete_node_suploop(int node);
	void update_suploop(int node, int supnode, bool srcOrdst, bool moveOrNew);
	void delete_node_supEdge(int node);
	void update_supEdge(int node, int s1, int s2, bool srcOrdst);
	void deleteSupedge(int n1, int sup, int otherSup);
	void remain_CAdd(int n1, int S2, bool srcOrdst);
	void addSupedge(int n1, int sup, int otherSup);
	void remain_CDel(int n1, int S2, int srcOrdst);


	void changedSimilar3();//alg3记录u-[nodes_2hopUp] ——搭配traverseReCaleSimi3
	int selectDstSup(map<int, float>& allpro, float allPro);//根据随机数选取超点
	void traverseReCaleSimi3();//alg5
	void traverseReCaleSimi4();//alg6
	void traverseReCaleSimi5();//alg4


	//test
	bool check_Correctness();
	float output();
};
#endif //_GRAPH_