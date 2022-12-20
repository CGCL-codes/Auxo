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
	float ecnt;//���������
	int costCounter;//ѹ��ͼ����
	set<int> nodeSet;
	unordered_map<int, int> nodesID;                         //���ҽڵ��Ӧ�ڽӱ�λ��,node-λ��
	vector<set<int>> adjNodes;                               //�ڽӱ�
	vector<int> hashValue;
	unordered_map<int, int> nodesHashList;                   //node-hashvalue
	//unordered_map<int, map<int,char>> groupSupernodes;     //minHash-supnodes
	

	unordered_map<int, map<int,char>> supList;               //supnode-[node]
	unordered_map<int, int> nodeToSup;                       //node-supnode
	unordered_map<int,vector<int>> adjSup;                   //���������нڵ���ڽӵ㲢��,supnode-[node]
	unordered_map<int, map<int,char>> supGraph;              //ѹ��ͼ
	unordered_map<int, map<int,char>> interAddC;             //�����C+,node-[nodes]������
	unordered_map<int, map<int,char>> intraAddC;             //������C+������Ҳ����
	unordered_map<int, map<int,char>> interDelC;             //�����C-
	unordered_map<int, map<int,char>> intraDelC;             //������C-
	set<int> supLoop;                                        //�����Ի�


	map<int, map<int, int>> realNumEdges;                    //�����ڡ�����ʵ������
	int add_u;//�ӱ�(u,v)
	int add_v;
	map<int, vector<int>> moveNode_realEdges;                //�ƶ��Ľڵ�Ķ�Ӧ��ͬ�������ر�,supnode-[nodes]


	//base
	unordered_map<int, set<int>> reCalcSimilar_base;          //�ӱ��Ժ���Ҫ���¼���Ľڵ�ͳ����������,node-[supernodes]


	//�Ա�MoSSo
	int c;                                                   //�ڽӵ���ȡc��
	float e;                                                 //escape probability
	int allnodes;                                            //��ͼ�ܹ��ڵ�����
	unordered_map<int, int> node_inGroup;                    //�ڵ�-minHash
	unordered_map<int, map<int, char>> groupNodes;           //minHash-nodes-'a'


	//optimistic_heristic_random
	unordered_map<int, set<int>> reCalcSimilar3;         //u-nodes��v-nodes; ��N(v)��u��2hop�ڵ�Ľ�����N(u)��v��2hop�ڵ�Ľ���

public:
	Graph();
	int printEndResult();

	void EndResult();

	int checkEdge(int, int);
	void addNodeToSupG(int);
	void addEdgeToSupG(int, int);//����Ҫ��ϼ��������Եĺ���
	void addEdge_update(int node1, int node2, int vv1, int mm1);
	float nodeSimilarSup1(int, int);//�ڵ�ͳ����������,node-supNode


	//�Ա�MoSSo
	void HashAllNodes();//���ݼ���ȫ���ڵ���й�ϣ
	void addNodeToSupG_MoSSo(int);
	void addEdgeToSupG_MoSSo(int, int);
	set<int> getRandomNeighbor_MoSSo(int);
	void MoSSo(int node);


	//base
	void addEdgeToSupG1(int node1, int node2);//��Ӧ����ڵ�ͳ������������test
	void changedSimilar1();//alg1
	void traverseReCaleSimi1();	//alg2
	void update1(int node, int s1, int s2, bool moveOrNew);
	void changedSimilar2();//alg3����¼u-[supernodes_include2hopUp] ��������traverseReCaleSimi1

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


	void changedSimilar3();//alg3��¼u-[nodes_2hopUp] ��������traverseReCaleSimi3
	int selectDstSup(map<int, float>& allpro, float allPro);//���������ѡȡ����
	void traverseReCaleSimi3();//alg5
	void traverseReCaleSimi4();//alg6
	void traverseReCaleSimi5();//alg4


	//test
	bool check_Correctness();
	float output();
};
#endif //_GRAPH_