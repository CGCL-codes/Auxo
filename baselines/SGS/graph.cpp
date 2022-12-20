#include "graph.h"
#include<algorithm> 
#include <random>
#include <time.h>
#include <map>
#include <stack>
//#include <sys/time.h>

using namespace std;

Graph::Graph(void)
{
	numNodes = 0;
	num_supNode = 0;
	adjKeyNodes1 = 0;
	adjKeyNodes2 = 0;
	rnd = 0;
	add_u = 0;
	add_v = 0;
	ecnt = 0.0;//�ӱ�����
	costCounter = 0;

	//�Ա�MoSSo
	c = 120;
	e = 0.3;
	allnodes = 896305;
}

void Graph::HashAllNodes()
{
	vector<int> tmp;                             //�洢ʣ�����������ϣȡֵ��ȡ��
	for (int i = 1; i <= allnodes; ++i)
	{
		tmp.push_back(i);
	}

	for (int i = 0; i < allnodes; ++i)
	{
		do 
		{
			rnd = 1 + rand() % (allnodes);
		} while (tmp.at(rnd - 1) == -1); 
		hashValue.push_back(rnd);         // [1,allnodes]��Χ����allnodes�����ظ�������
		tmp.at(rnd - 1) = -1;      
	}
	
	////�̶���ϣֵ����
	//hashValue.clear();
	//for (auto it = nodesID.begin(); it != nodesID.end(); ++it) {
	//	nodesHashList[it->first] = it->second;
	//}
}

int Graph::printEndResult() 
{
	int allEdges = 0;
	
	/*for (auto ith = supList.begin(); ith != supList.end(); ++ith)
	{
		cout << ith->first << " END-------SupList = ";
		for (auto vvv = ith->second.begin(); vvv != ith->second.end(); ++vvv)
		{
			cout << ", " << vvv->first;
		}
		cout << endl;
	}*/

	for (auto ith2 = supGraph.begin(); ith2 != supGraph.end(); ++ith2)
	{
		allEdges = allEdges + ith2->second.size();
		/*cout << ith2->first << " END-------supGraph = ";
		for (auto vvv2 = ith2->second.begin(); vvv2 != ith2->second.end(); ++vvv2)
		{
			allEdges += 1;
			cout << ", " << vvv2->first;
		}
		cout << endl;*/
	}

	for (auto ith3 = interAddC.begin(); ith3 != interAddC.end(); ++ith3)
	{
		allEdges = allEdges + ith3->second.size();
		/*cout << ith3->first << " END-------interAddC = ";
		for (auto vvv3 = ith3->second.begin(); vvv3 != ith3->second.end(); ++vvv3)
		{
			allEdges += 1;
			cout << ", " << vvv3->first;
		}
		cout << endl;*/
	}

	for (auto ith4 = intraAddC.begin(); ith4 != intraAddC.end(); ++ith4)
	{
		allEdges = allEdges + ith4->second.size();
		/*cout << ith4->first << " END-------intraAddC = ";
		for (auto vvv4 = ith4->second.begin(); vvv4 != ith4->second.end(); ++vvv4)
		{
			allEdges += 1;
			cout << ", " << vvv4->first;
		}
		cout << endl;*/
	}

	for (auto ith5 = interDelC.begin(); ith5 != interDelC.end(); ++ith5)
	{
		allEdges = allEdges + ith5->second.size();
		/*cout << ith5->first << " END-------interDelC = ";
		for (auto vvv5 = ith5->second.begin(); vvv5 != ith5->second.end(); ++vvv5)
		{
			allEdges += 1;
			cout << ", " << vvv5->first;
		}
		cout << endl;*/
	}

	for (auto ith6 = intraDelC.begin(); ith6 != intraDelC.end(); ++ith6)
	{
		allEdges = allEdges + ith6->second.size();
		/*cout << ith6->first << " END-------intraDelC = ";
		for (auto vvv6 = ith6->second.begin(); vvv6 != ith6->second.end(); ++vvv6)
		{
			allEdges += 1;
			cout << ", " << vvv6->first;
		}
		cout << endl;*/
	}

	/*for (auto ith7 = supLoop.begin(); ith7 != supLoop.end(); ++ith7) {
		cout << "END-------supLoop =" << *ith7 << ";";
	}
	cout << endl;*/

	allEdges = allEdges / 2;
	//cout << "END-------allEdges :" << allEdges << endl;

	return allEdges;
}

void Graph::EndResult()
{
	int allEdges = 0;

	for (auto ith = supList.begin(); ith != supList.end(); ++ith)
	{
		if (ith->first == 853)
		{
			cout << ith->first << " END-------SupList = " << ith->second.size() << endl;
			/*for (auto vvv = ith->second.begin(); vvv != ith->second.end(); ++vvv)
			{
				cout << ", " << vvv->first;
			}
			cout << endl;*/
		}
		if (ith->first == 2444)
		{
			cout << ith->first << " END-------SupList = " << ith->second.size() << endl;
			/*for (auto vvv = ith->second.begin(); vvv != ith->second.end(); ++vvv)
			{
				cout << ", " << vvv->first;
			}
			cout << endl;*/
		}
	}

	int after2 = 0;
	int after4 = 0;
	int numSupE1 = 0;
	int numSupE2 = 0;
	for (auto ith2 = supGraph.begin(); ith2 != supGraph.end(); ++ith2)
	{
		allEdges = allEdges + ith2->second.size();
		if (ith2->first ==853)
		{
			for (auto vvv2 = ith2->second.begin(); vvv2 != ith2->second.end(); ++vvv2)
			{
				if (vvv2->first !=  2444)
				{
					numSupE1 = numSupE1 + 1;
				}
			}
		}
		if (ith2->first == 2444)
		{
			numSupE2 = numSupE2 + ith2->second.size();
			for (auto vvv2 = ith2->second.begin(); vvv2 != ith2->second.end(); ++vvv2)
			{
				if (vvv2->first == 853)
				{
					after4 = after4 + 1;
				}
				else
				{
					after2 = after2 + 1;
				}
			}
		}
	}
	cout << "853 END-------supGraph = " << numSupE1 << endl;
	cout << "2444 END-------supGraph = " << numSupE2 << endl;

	int num_interAddC1 = 0;
	int num_interAddC2 = 0;
	for (auto ith3 = interAddC.begin(); ith3 != interAddC.end(); ++ith3)
	{
		allEdges = allEdges + ith3->second.size();
		if (supList.at(853).find(ith3->first) != supList.at(853).end())
		{
			for (auto vvv3 = ith3->second.begin(); vvv3 != ith3->second.end(); ++vvv3)
			{
				if (supList[2444].find(vvv3->first) == supList[2444].end())
				{
					num_interAddC1 = num_interAddC1 + 1;
				}
			}
		}
		if (supList.find(2444) != supList.end())
		{
			if (supList.at(2444).find(ith3->first) != supList.at(2444).end())
			{
				num_interAddC2 = num_interAddC2 + ith3->second.size();

				for (auto vvv3 = ith3->second.begin(); vvv3 != ith3->second.end(); ++vvv3)
				{
					if (supList[853].find(vvv3->first) == supList[853].end())
					{
						after2 = after2 + 1;
					}
					else
					{
						after4 = after4 + 1;
					}
				}
			}
		}
	}
	cout <<"853 END-------interAddC = " << num_interAddC1 << endl;
	cout <<"2444 END-------interAddC = " << num_interAddC2 << endl;

	int num_intraAddC1 = 0;
	int num_intraAddC2 = 0;
	for (auto ith4 = intraAddC.begin(); ith4 != intraAddC.end(); ++ith4)
	{
		allEdges = allEdges + ith4->second.size();
		if (supList.at(853).find(ith4->first) != supList.at(853).end())
		{
			num_intraAddC1 = num_intraAddC1 + ith4->second.size();
			/*for (auto vvv4 = ith4->second.begin(); vvv4 != ith4->second.end(); ++vvv4)
			{
				allEdges += 1;
				cout << ", " << vvv4->first;
			}
			cout << endl;*/
		}
		if (supList.find(2444) != supList.end()) {
			if (supList.at(2444).find(ith4->first) != supList.at(2444).end())
			{
				num_intraAddC2 = num_intraAddC2 + ith4->second.size();
				/*for (auto vvv4 = ith4->second.begin(); vvv4 != ith4->second.end(); ++vvv4)
				{
				}*/
			}
		}
	}
	after2 = after2 + num_intraAddC2 / 2;
	cout << " 853 END-------intraAddC = " << num_intraAddC1/2 << endl;
	cout << " 2444 END-------intraAddC = " << num_intraAddC2/2 << endl;

	int num_interDelC1 = 0;
	int num_interDelC2 = 0;
	for (auto ith5 = interDelC.begin(); ith5 != interDelC.end(); ++ith5)
	{
		allEdges = allEdges + ith5->second.size();
		if (supList.at(853).find(ith5->first) != supList.at(853).end())
		{
			for (auto vvv3 = ith5->second.begin(); vvv3 != ith5->second.end(); ++vvv3)
			{
				if (supList[2444].find(vvv3->first) == supList[2444].end())
				{
					num_interDelC1 = num_interDelC1 + 1;
				}

			}
		}
		if (supList.find(2444) != supList.end()) {
			if (supList.at(2444).find(ith5->first) != supList.at(2444).end())
			{
				num_interDelC2 = num_interDelC2 + ith5->second.size();

				for (auto vvv3 = ith5->second.begin(); vvv3 != ith5->second.end(); ++vvv3)
				{
					if (supList[853].find(vvv3->first) == supList[853].end())
					{
						after2 = after2 + 1;
					}
					else
					{
						after4 = after4 + 1;
					}
				}
			}
		}
		
	}
	cout << "853 END-------interDelC = " << num_interDelC1 << endl;
	cout << "2444 END-------interDelC = " << num_interDelC2 << endl;

	int num_intraDelC1 = 0;
	int num_intraDelC2 = 0;
	for (auto ith6 = intraDelC.begin(); ith6 != intraDelC.end(); ++ith6)
	{
		allEdges = allEdges + ith6->second.size();
		if (supList.at(853).find(ith6->first) != supList.at(853).end())
		{
			num_intraDelC1 = num_intraDelC1 + ith6->second.size();
			/*for (auto vvv4 = ith4->second.begin(); vvv4 != ith4->second.end(); ++vvv4)
			{
				allEdges += 1;
				cout << ", " << vvv4->first;
			}
			cout << endl;*/
		}
		if (supList.find(2444) != supList.end()) {
			if (supList.at(2444).find(ith6->first) != supList.at(2444).end())
			{
				num_intraDelC2 = num_intraDelC2 + ith6->second.size();
				/*for (auto vvv4 = ith4->second.begin(); vvv4 != ith4->second.end(); ++vvv4)
				{
					allEdges += 1;
					cout << ", " << vvv4->first;
				}
				cout << endl;*/
			}
		}
	}
	after2 = after2 + num_intraDelC2 / 2;
	cout << " 853 END-------intraDelC = " << num_intraDelC1 / 2 << endl;
	cout << " 2444 END-------intraDelC = " << num_intraDelC2 / 2 << endl;

	
	/*for (auto ith7 = supLoop.begin(); ith7 != supLoop.end(); ++ith7) {
		if (add_u == 1847 && add_v == 542)
		{
			cout << "END-------supLoop =";
			cout << ";" << *ith7 << ";";
		}
	}
	cout << endl;*/
	cout << "after2:" << after2 << endl;
	cout << "after4:" << after4 << endl;
	allEdges = allEdges / 2;
	cout << "END-------allEdges :" << allEdges << endl;
}

//��ͼѹ��ͼ���ӱ�
int Graph::checkEdge(int n1, int n2)
{
	if (nodeSet.find(n1) != nodeSet.end())
	{
		int ch1 = nodesID.at(n1);
		int ch3 = adjNodes[ch1].count(n2);
		return ch3;
	}
	else
	{
		return 0;
	}
}

void Graph::addNodeToSupG(int node)
{
	if (nodeSet.find(node) == nodeSet.end()) {          //���node�Ƿ����½ڵ�
		nodeSet.insert(node);
		nodesID[node] = numNodes;                       //������ҽڵ��Ӧ�ڽӱ�λ��,��0��ʼ

		supList[num_supNode][node] = 'a';                 //��ʼ������
		vector<int> allAdj;
		adjSup.insert(make_pair(num_supNode, allAdj));      //��ʼ�����������нڵ���ڽӵ�
		nodeToSup.insert(make_pair(node, num_supNode));     //��ʼ���ڵ��Ӧ�ĳ���


		map<int, map<int, char>> map1;
		realNumEdges[num_supNode][num_supNode] = 0;           //��ʼ��node�������ڱ�����

		set<int> v;
		adjNodes.push_back(v);

		numNodes = numNodes + 1;
		num_supNode = num_supNode + 1;
	}
}
 
void Graph::addEdgeToSupG(int node1, int node2)//�ӱߺ󣬸��µ�ǰ��ͼ
{
	//cout << "addEdgeToSupG:" << "(" << node1 << "," << node2 << ")" << endl;
	ecnt = ecnt + 1;
	add_u = node1;
	add_v = node2;
	addNodeToSupG(node1);
	addNodeToSupG(node2);
	adjKeyNodes1 = nodesID.at(node1);
	adjKeyNodes2 = nodesID.at(node2);
	adjNodes[adjKeyNodes1].insert(node2); //node1�ڽӵ������node2
	adjNodes[adjKeyNodes2].insert(node1); //node2�ڽӵ������node1

	int vv1 = nodeToSup.at(node1);
	int mm1 = nodeToSup.at(node2);

	/*vector<int> vv2;  //����node1���ڳ�����ڽӵ�
	vv2.push_back(node2);
	vector<int> vv3;
	set_intersection(vv2.begin(), vv2.end(), adjSup[vv1].begin(), adjSup[vv1].end(), back_inserter(vv3));
	if (vv3.size() == 0)
	{
		vector<int> vv4;
		set_union(vv2.begin(), vv2.end(), adjSup[vv1].begin(), adjSup[vv1].end(), back_inserter(vv4));
		adjSup[vv1] = vv4;
	}
	vector<int> mm2; //����node2���ڳ�����ڽӵ�
	mm2.push_back(node1);
	vector<int> mm3;
	set_intersection(mm2.begin(), mm2.end(), adjSup[mm1].begin(), adjSup[mm1].end(), back_inserter(mm3));
	if (mm3.size() == 0)
	{
		vector<int> mm4;
		set_union(mm2.begin(), mm2.end(), adjSup[mm1].begin(), adjSup[mm1].end(), back_inserter(mm4));
		adjSup[mm1] = mm4;
	}*/

	addEdge_update(node1, node2, vv1, mm1);//���£��ӱ����ڳ���ı�
	/*if (node1 == 2429 && node2 == 906)
	{
		cout << "-----------add-edge-before---update------" << endl;
		EndResult();
		cout << "-------------------------------------------" << endl;
	}*/
}

void Graph::addEdge_update(int node1, int node2, int s1, int s2) 
{
	if (s1 == s2)	//�ӵı��ڳ�����,���³��ߣ�C+C-
	{
		realNumEdges[s1][s1] += 1;
		if (realNumEdges[s1][s1] <= getIntraPi(s1) - realNumEdges[s1][s1])//�Ի���C+ <= C-
		{
			intraAddC[node1][node2] = 'a';
			intraAddC[node2][node1] = 'a';

			costCounter = costCounter + 2;
		}
		else//C-, P
		{
			if (supLoop.find(s1) != supLoop.end())   //��ΪC-��C-��ɾ(node1,node2)
			{
				intraDelC[node1].erase(node2);
				if (intraDelC[node1].size() == 0)
				{
					unordered_map<int, map<int, char>>::iterator flag = intraDelC.find(node1);
					intraDelC.erase(flag);
				}

				intraDelC[node2].erase(node1);
				if (intraDelC[node2].size() == 0)
				{
					unordered_map<int, map<int, char>>::iterator flag = intraDelC.find(node2);
					intraDelC.erase(flag);
				}

				costCounter = costCounter - 2;
			}
			else//C+���Ի���C-����ɾC+�����C-
			{
				supLoop.insert(s1);//��ӳ����Ի�
				int num = 0;
				for (auto kx6 = supList[s1].begin(); kx6 != supList[s1].end(); ++kx6)//����ɾ������vv1�ڽڵ�������intraAddC�ı�
				{
					if (intraAddC.find(kx6->first) != intraAddC.end())
					{
						num = num + intraAddC.at(kx6->first).size();
						intraAddC.erase(kx6->first);
					}
	
					int adj = nodesID.at(kx6->first);
					for (auto j = supList[s1].begin(); j != supList[s1].end(); ++j)
					{
						if (kx6->first == j->first || kx6->first > j->first)
						{
							continue;
						}

						if (adjNodes[adj].find(j->first) == adjNodes[adj].end())
						{
							intraDelC[kx6->first][j->first] = 'a';//���C-
							intraDelC[j->first][kx6->first] = 'a';

							costCounter = costCounter + 2;
						}
					}
				}
				costCounter = costCounter - num;
			}
		}
	}
	else  //�����ӱ�
	{
		if (realNumEdges[s1].find(s2) != realNumEdges[s1].end()) //�����������(s1, mm1)��������1
		{
			realNumEdges[s1][s2] = realNumEdges[s1][s2] + 1;
			realNumEdges[s2][s1] = realNumEdges[s2][s1] + 1;
		}
		else
		{
			realNumEdges[s1][s2] = 1;
			realNumEdges[s2][s1] = 1;
		}

		//�ӱߺ󣬸���n1,n2���ڳ�������֮���P,C+C-
		if (realNumEdges[s1][s2] <= getInterPi(s1, s2) - realNumEdges[s1][s2] + 1) //C+ <= P + C-
		{
			interAddC[node1][node2] = 'a';
			interAddC[node2][node1] = 'a';

			costCounter = costCounter + 2;
		}
		else  //C-, P
		{
			if (supGraph.find(s1) != supGraph.end())
			{
				if (supGraph[s1].find(s2) != supGraph[s1].end())//����P��C-
				{
					interDelC[node1].erase(node2);//��ɾ
					if (interDelC[node1].size() == 0)
					{
						unordered_map<int, map<int, char>>::iterator flag = interDelC.find(node1);
						interDelC.erase(flag);
					}

					interDelC[node2].erase(node1);//��ɾ
					if (interDelC[node2].size() == 0)
					{
						unordered_map<int, map<int, char>>::iterator flag = interDelC.find(node2);
						interDelC.erase(flag);
					}

					costCounter = costCounter - 2;
				}
				else //C+��ΪP��C-��ɾ��C+,���P��C-
				{
					supGraph[s1][s2] = 'a';    //�ӳ���
					supGraph[s2][s1] = 'a';

					interAddC[node1][node2] = 'a';
					interAddC[node2][node1] = 'a';

					costCounter = costCounter + 4;

					for (auto kx6 = supList[s1].begin(); kx6 != supList[s1].end(); ++kx6)
					{
						for (auto j = supList[s2].begin(); j != supList[s2].end(); ++j)
						{
							if (interAddC.find(kx6->first) != interAddC.end()) {
								if (interAddC[kx6->first].find(j->first) == interAddC[kx6->first].end())//C+�в�����(kx6->first,j->first)
								{
									interDelC[kx6->first][j->first] = 'a';//���C-
									interDelC[j->first][kx6->first] = 'a';

									costCounter = costCounter + 2;
								}
								else
								{
									interAddC[kx6->first].erase(j->first); //ɾ��C+
									if (interAddC[kx6->first].size() == 0)
									{
										unordered_map<int, map<int, char>>::iterator flag = interAddC.find(kx6->first);
										interAddC.erase(flag);
									}

									interAddC[j->first].erase(kx6->first);//��ɾ
									if (interAddC[j->first].size() == 0)
									{
										unordered_map<int, map<int, char>>::iterator flag = interAddC.find(j->first);
										interAddC.erase(flag);
									}

									costCounter = costCounter - 2;
								}
							}
							else
							{
								interDelC[kx6->first][j->first] = 'a';//���C-
								interDelC[j->first][kx6->first] = 'a';

								costCounter = costCounter + 2;
							}
						}
					}
				}
			}
			else  //C+��ΪP��C-
			{
				supGraph[s1][s2] = 'a'; //�ӳ���
				supGraph[s2][s1] = 'a';

				interAddC[node1][node2] = 'a';
				interAddC[node2][node1] = 'a';

				costCounter = costCounter + 4;

				for (auto kx6 = supList[s1].begin(); kx6 != supList[s1].end(); ++kx6)
				{
					for (auto j = supList[s2].begin(); j != supList[s2].end(); ++j)
					{
						if (interAddC.find(kx6->first) != interAddC.end()) {
							if (interAddC[kx6->first].find(j->first) == interAddC[kx6->first].end())//C+�в�����(kx6->first,j->first)
							{
								interDelC[kx6->first][j->first] = 'a';//���C-
								interDelC[j->first][kx6->first] = 'a';

								costCounter = costCounter + 2;
							}
							else
							{
								interAddC[kx6->first].erase(j->first); //ɾ��C+
								if (interAddC[kx6->first].size() == 0)
								{
									unordered_map<int, map<int, char>>::iterator flag = interAddC.find(kx6->first);
									interAddC.erase(flag);
								}

								interAddC[j->first].erase(kx6->first);//��ɾ
								if (interAddC[j->first].size() == 0)
								{
									unordered_map<int, map<int, char>>::iterator flag = interAddC.find(j->first);
									interAddC.erase(flag);
								}

								costCounter = costCounter - 2;
							}
						}
						else
						{
							interDelC[kx6->first][j->first] = 'a';//���C-
							interDelC[j->first][kx6->first] = 'a';

							costCounter = costCounter + 2;
						}
					}
				}
			}
		}
	}
}

//float Graph::nodeSimilarSup1(int node1, int sup1)
//{
//	float demo = 0;              //Jccard��ĸ
//	float nume = 0;              //����
//	int ns1 = nodesID.at(node1);
//	vector<int> diff1;        //����node1���ڽӵ㣬��������sup1����max{w(A,v),w(B,v)}
//	set_difference(adjNodes[ns1].begin(), adjNodes[ns1].end(), adjSup[sup1].begin(), adjSup[sup1].end(), back_inserter(diff1));
//	demo += diff1.size();
//
//	vector<int> diff2;        //����sup1���ڽӵ㣬��������node1,��max
//	set_difference(adjSup[sup1].begin(), adjSup[sup1].end(), adjNodes[ns1].begin(), adjNodes[ns1].end(), back_inserter(diff2));
//	int diff2Len = diff2.size();
//	set<int> suplist_sup1;
//	for (auto bl1 = supList[sup1].begin(); bl1 != supList[sup1].end(); ++bl1)
//	{
//		suplist_sup1.insert(bl1->first);
//	}
//	for (int i = 0; i < diff2Len; ++i)
//	{
//		vector<int> diff2_wB;
//		int gg = nodesID.at(diff2[i]);
//
//		set_intersection(adjNodes[gg].begin(), adjNodes[gg].end(), suplist_sup1.begin(), suplist_sup1.end(), back_inserter(diff2_wB));   //ͬ����w(B,v)
//		int wB2 = diff2_wB.size();
//		demo += wB2;
//	}
//
//	vector<int> same;                      //node1��sup1�Ľ�������min,max
//	set_intersection(adjNodes[ns1].begin(), adjNodes[ns1].end(), adjSup[sup1].begin(), adjSup[sup1].end(), back_inserter(same));
//	int sameLen = same.size();
//	for (int i = 0; i < sameLen; i++)
//	{
//		vector<int> same_wB;
//		int hh = nodesID.at(same[i]);
//		set_intersection(adjNodes[hh].begin(), adjNodes[hh].end(), suplist_sup1.begin(), suplist_sup1.end(), back_inserter(same_wB));      //ͬ����w(B,v)
//		int wA3 = 1;
//		int wB3 = same_wB.size();
//		nume += min(wA3, wB3);
//		demo += max(wA3, wB3);
//	}
//	float result = nume / demo;
//	//cout << "simialrity result=" << result << endl;
//	return result;
//}

float Graph::nodeSimilarSup1(int node1, int sup1)
{
	int n1_nbr = nodesID.at(node1);
	int n1_locaSup = nodeToSup.at(node1);
	int fz = 0;
	int fm = 0;
	float result;
	vector<int> n1_s1_nbr;
	set<int> sup1_set;
	if (n1_locaSup != sup1)
	{
		if (supList.find(sup1) != supList.end())
		{
			set_union(adjNodes[n1_nbr].begin(), adjNodes[n1_nbr].end(), adjSup[sup1].begin(), adjSup[sup1].end(), back_inserter(n1_s1_nbr));
			for (auto i = supList.at(sup1).begin(); i != supList.at(sup1).end(); ++i)
			{
				sup1_set.insert(i->first);
			}
		}
		else //������һ���ƶ�ʱ�򣬳�����ֻ����һ���ڵ㣬�ϲ������������У��ó��㱻ɾ����
		{
			int sup1_newID;
			if (add_u == node1) //��ǰ�ڼ���add_u����س����
			{
				sup1_newID = nodeToSup.at(add_v);//��ʾ֮ǰ�ƶ����ڵ�add_v
			}
			else //��ǰ�ڼ���add_v����س���
			{
				sup1_newID = nodeToSup.at(add_u);
			}
			set_union(adjNodes[n1_nbr].begin(), adjNodes[n1_nbr].end(), adjSup[sup1_newID].begin(), adjSup[sup1_newID].end(), back_inserter(n1_s1_nbr));
			for (auto i = supList.at(sup1_newID).begin(); i != supList.at(sup1_newID).end(); ++i)
			{
				sup1_set.insert(i->first);
			}
		}
		for (auto i = n1_s1_nbr.begin(); i != n1_s1_nbr.end(); ++i)
		{
			int wAv = 0;
			if (adjNodes[n1_nbr].find(*i) != adjNodes[n1_nbr].end())
			{
				wAv = 1;
			}
			int i1 = nodesID.at(*i);
			vector<int> wBv_nodes;
			set_intersection(adjNodes[i1].begin(), adjNodes[i1].end(), sup1_set.begin(), sup1_set.end(), back_inserter(wBv_nodes));
			int wBv = wBv_nodes.size();
			fz = fz + min(wAv, wBv);
			fm = fm + max(wAv, wBv);
		}
		float fz_float = fz;
		result = fz_float / fm;
	}
	else //����node1��������������ԣ��൱�ڼ����node1����ڵ㼯��������
	{
		if (supList.at(n1_locaSup).size() == 1)
		{
			result = 0;
		}
		else
		{
			set<int> sup1_set;
			for (auto i = supList.at(sup1).begin(); i != supList.at(sup1).end(); ++i)
			{
				sup1_set.insert(i->first);
			}
			sup1_set.erase(n1_locaSup);
			for (auto i = adjSup[sup1].begin(); i != adjSup[sup1].end(); ++i)
			{
				int wAv = 0;
				if (adjNodes[n1_nbr].find(*i) != adjNodes[n1_nbr].end())
				{
					wAv = 1;
				}
				int i1 = nodesID.at(*i);
				vector<int> wBv_nodes;
				set_intersection(adjNodes[i1].begin(), adjNodes[i1].end(), sup1_set.begin(), sup1_set.end(), back_inserter(wBv_nodes));
				int wBv = wBv_nodes.size();
				fz = fz + min(wAv, wBv);
				fm = fm + max(wAv, wBv);
			}
			float fz_float = fz;
			result = fz_float / fm;
		}
	}
	//cout << "simialrity result=" << result << endl;
	return result;
}


//�Ա�MoSSo
void Graph::addNodeToSupG_MoSSo(int node)
{
	if (nodeSet.find(node) == nodeSet.end()) {          //���node�Ƿ����½ڵ�
		nodeSet.insert(node);
		nodesID[node] = numNodes;                       //������ҽڵ��Ӧ�ڽӱ�λ��,��0��ʼ

		supList[num_supNode][node] = 'a';               //��ʼ������
		vector<int> allAdj;
		adjSup.insert(make_pair(num_supNode, allAdj));      //��ʼ�����������нڵ���ڽӵ�
		nodeToSup.insert(make_pair(node, num_supNode));     //��ʼ���ڵ��Ӧ�ĳ���

		map<int, map<int, char>> map1;
		realNumEdges[num_supNode][num_supNode] = 0;           //��ʼ��node�������ڱ�����

		set<int> v;
		adjNodes.push_back(v);

		nodesHashList[node] = hashValue[numNodes];           //�����¼���ڵ�Ĺ�ϣֵ���ڵ�--��ϣֵ

		numNodes = numNodes + 1;
		num_supNode = num_supNode + 1;
	}
}

void Graph::addEdgeToSupG_MoSSo(int node1, int node2) 
{
	//cout << "addEdgeToSupG:" << "(" << node1 << "," << node2 << ")" << endl;
	ecnt = ecnt + 1;
	add_u = node1;
	add_v = node2;
	addNodeToSupG_MoSSo(node1);
 	addNodeToSupG_MoSSo(node2);
	adjKeyNodes1 = nodesID.at(node1);
	adjKeyNodes2 = nodesID.at(node2);
	adjNodes[adjKeyNodes1].insert(node2); //node1�ڽӵ������node2
	adjNodes[adjKeyNodes2].insert(node1); //node2�ڽӵ������node1


	//�¼���ߣ����½ڵ���飬minHash
	if (node_inGroup.find(node1) == node_inGroup.end())//node1���½ڵ㣬�������飬node1ֻ��node2һ���ڽӵ�
	{
		if (node_inGroup.find(node2) != node_inGroup.end()) //node2�����нڵ�
		{
			if (nodesHashList.at(node1) > nodesHashList.at(node2))//����node1�ķ��飬node2���鲻��
			{
				groupNodes[nodesHashList.at(node2)][node1] = 'a'; //minHash-�ڵ�
				node_inGroup[node1] = nodesHashList.at(node2);//�ڵ�-minHash
			}
			else //����node1�ķ��飬node2������ܸı�
			{
				groupNodes[nodesHashList.at(node1)][node1] = 'a'; //minHash-�ڵ�
				node_inGroup[node1] = nodesHashList.at(node1);//�ڵ�-minHash

				if (node_inGroup[node2] > nodesHashList.at(node1))//node2����ı䣬��node1��ϣֵ���浱ǰnode2��minHash
				{
					map<int, char>::iterator flag = groupNodes[node_inGroup.at(node2)].find(node2);
					groupNodes[node_inGroup.at(node2)].erase(flag);//ɾ��node2�����ķ���
					if (groupNodes[node_inGroup.at(node2)].size() == 0)
					{
						groupNodes.erase(node_inGroup.at(node2));
					}

					groupNodes[nodesHashList.at(node1)][node2] = 'a';//����node2����
					node_inGroup[node2] = nodesHashList.at(node1);
				}
			}
		}
		else //node2���½ڵ�
		{
			if (nodesHashList.at(node1) > nodesHashList.at(node2))//node2��ϣֵС��node1��node2��minHash����node2��ϣֵ
			{
				groupNodes[nodesHashList.at(node2)][node1] = 'a'; //minHash-�ڵ�
				node_inGroup[node1] = nodesHashList.at(node2);//�ڵ�-minHash

				groupNodes[nodesHashList.at(node2)][node2] = 'a'; //minHash-�ڵ�
				node_inGroup[node2] = nodesHashList.at(node2);//�ڵ�-minHash
			}
			else //node1��ϣֵС��node1��node2��minHash����node1��ϣֵ
			{
				groupNodes[nodesHashList.at(node1)][node1] = 'a'; //minHash-�ڵ�
				node_inGroup[node1] = nodesHashList.at(node1);//�ڵ�-minHash

				groupNodes[nodesHashList.at(node1)][node2] = 'a'; //minHash-�ڵ�
				node_inGroup[node2] = nodesHashList.at(node1);//�ڵ�-minHash
			}
		}
	}
	else//node1�����нڵ㣬����node1
	{
		if (node_inGroup.find(node2) != node_inGroup.end()) //node2�����нڵ�
		{//node1,node2�������нڵ㣬�ֱ�Ƚ�������ǰminHash��node2��node1�Ĺ�ϣֵ��С
			if (node_inGroup.at(node1) > nodesHashList.at(node2)) //node1��ǰminHash����node2�Ĺ�ϣֵ
			{
				map<int, char>::iterator flag = groupNodes[node_inGroup.at(node1)].find(node1);
				groupNodes[node_inGroup.at(node1)].erase(flag);//ɾ��node1�����ķ���
				if (groupNodes[node_inGroup.at(node1)].size() == 0)
				{
					groupNodes.erase(node_inGroup.at(node1));
				}

				groupNodes[nodesHashList.at(node2)][node1] = 'a';//����node1����
				node_inGroup[node1] = nodesHashList.at(node2);
			}

			if (node_inGroup.at(node2) > nodesHashList.at(node1)) //node2��ǰminHash����node1�Ĺ�ϣֵ
			{
				map<int, char>::iterator flag = groupNodes[node_inGroup.at(node2)].find(node2);
				groupNodes[node_inGroup.at(node2)].erase(flag);//ɾ��node2�����ķ���
				if (groupNodes[node_inGroup.at(node2)].size() == 0)
				{
					groupNodes.erase(node_inGroup.at(node2));
				}

				groupNodes[nodesHashList.at(node1)][node2] = 'a';//����node2����
				node_inGroup[node2] = nodesHashList.at(node1);
			}

		}
		else //node1�����нڵ㣬node2���½ڵ㡣����node1��minHash������node2��minHash
		{
			if (node_inGroup.at(node1) > nodesHashList.at(node2)) //node1��ǰminHash����node2�Ĺ�ϣֵ
			{
				map<int, char>::iterator flag = groupNodes[node_inGroup.at(node1)].find(node1);
				groupNodes[node_inGroup.at(node1)].erase(flag);//ɾ��node1�����ķ���
				if (groupNodes[node_inGroup.at(node1)].size() == 0)
				{
					groupNodes.erase(node_inGroup.at(node1));
				}

				groupNodes[nodesHashList.at(node2)][node1] = 'a';//����node1����
				node_inGroup[node1] = nodesHashList.at(node2);
			}


			if (nodesHashList.at(node1) > nodesHashList.at(node2))//node2��ϣֵС��node2��minHash����node2��ϣֵ
			{
				groupNodes[nodesHashList.at(node2)][node1] = 'a'; //minHash-�ڵ�
				node_inGroup[node1] = nodesHashList.at(node2);//�ڵ�-minHash

				groupNodes[nodesHashList.at(node2)][node2] = 'a'; //minHash-�ڵ�
				node_inGroup[node2] = nodesHashList.at(node2);//�ڵ�-minHash
			}
			else //node1��ϣֵС��node2��minHash��node1��ϣֵ
			{
				groupNodes[nodesHashList.at(node1)][node2] = 'a'; //minHash-�ڵ�
				node_inGroup[node2] = nodesHashList.at(node1);//�ڵ�-minHash
			}
		}
	}


	int vv1 = nodeToSup.at(node1);
	int mm1 = nodeToSup.at(node2);

	vector<int> vv2;  //����node1���ڳ�����ڽӵ�
	vv2.push_back(node2);
	vector<int> vv3;
	set_intersection(vv2.begin(), vv2.end(), adjSup[vv1].begin(), adjSup[vv1].end(), back_inserter(vv3));
	if (vv3.size() == 0)
	{
		vector<int> vv4;
		set_union(vv2.begin(), vv2.end(), adjSup[vv1].begin(), adjSup[vv1].end(), back_inserter(vv4));
		adjSup[vv1] = vv4;
	}

	vector<int> mm2; //����node2���ڳ�����ڽӵ�
	mm2.push_back(node1);
	vector<int> mm3;
	set_intersection(mm2.begin(), mm2.end(), adjSup[mm1].begin(), adjSup[mm1].end(), back_inserter(mm3));
	if (mm3.size() == 0)
	{
		vector<int> mm4;
		set_union(mm2.begin(), mm2.end(), adjSup[mm1].begin(), adjSup[mm1].end(), back_inserter(mm4));
		adjSup[mm1] = mm4;
	}

	addEdge_update(node1, node2, vv1, mm1);
}

set<int> Graph::getRandomNeighbor_MoSSo(int node)
{
	set<int> TP_set;//�ڵ㼯
	int supIndex = nodeToSup.at(node);//����P
	vector<int> P;//node���ڳ�����ڽӳ���
	int maxNum_w = 0;//����������w���������
	int Sn;

	if (supGraph.find(supIndex) != supGraph.end())
	{
		for (auto i = supGraph.at(supIndex).begin(); i != supGraph.at(supIndex).end(); ++i)
		{
			P.push_back(i->first);
			maxNum_w = maxNum_w + supList.at(i->first).size();
		}
		int Sn_Index = rand() % (P.size());
		Sn = P.at(Sn_Index);//Sn����
	}

	if (interDelC.find(node) != interDelC.end())
	{
		maxNum_w = maxNum_w - interDelC.at(node).size();
	}
	if (intraDelC.find(node) != intraDelC.end())
	{
		maxNum_w = maxNum_w - intraDelC.at(node).size();
	}

	vector<int> Cp;
	if (interAddC.find(node) != interAddC.end())
	{
		for (auto i = interAddC[node].begin(); i != interAddC[node].end(); ++i)
		{
			Cp.push_back(i->first);
		}
	}
	if (intraAddC.find(node) != intraAddC.end())
	{
		for (auto i = intraAddC[node].begin(); i != intraAddC[node].end(); ++i)
		{
			Cp.push_back(i->first);
		}
	}

	int n_ID = nodesID.at(node);

	float Cp_size = Cp.size();
	float re = Cp_size / adjNodes[n_ID].size();
	float min_left = 1.0;

	for (auto i = 0; i < c; ++i)
	{
		float sample_X1 = rand() % 100 / (double)101;

		if (sample_X1 < re)
		{
			if (Cp.size() != 0)
			{
				int CpIndex = rand() % (Cp.size());
				TP_set.insert(Cp.at(CpIndex));
				vector<int>::iterator it = Cp.begin() + CpIndex;
				Cp.erase(it);
			}
		}
		else
		{
			if (P.size() != 0)
			{
				while (true)
				{
					//cout << "while:addEdgeToSupG:" << "(" << add_u << "," << add_v << ")" << endl;
					int Sp_Index = rand() % (P.size());
					int Sp = P.at(Sp_Index);//Sp����

					if (Sp != Sn)
					{
						float sample_X2 = rand() % 100 / (double)101;
						float Sp_size = supList[Sp].size();
						float Sp_bi_Sn = Sp_size / supList[Sn].size();
						float min_S = min(min_left, Sp_bi_Sn);
						if (sample_X2 < min_S)
						{
							Sn = Sp;
						}
					}

					vector<int> Sn_nodes;
					for (auto i = supList.at(Sn).begin(); i != supList.at(Sn).end(); ++i)
					{
						Sn_nodes.push_back(i->first);
					}

					int w_Index = rand() % (Sn_nodes.size());
					int w = Sn_nodes.at(w_Index);

					if (TP_set.find(w) == TP_set.end())
					{
						if (interDelC.count(node) == 0 && w != node)
						{
							TP_set.insert(w);
							break;
						}
						else
						{
							if (interDelC[node].count(w) == 0)
							{
								TP_set.insert(w);
								break;
							}
						}

						if (intraDelC.count(node) == 0 && w != node)
						{
							TP_set.insert(w);
							break;
						}
						else
						{
							if (intraDelC[node].count(w) == 0)
							{
								TP_set.insert(w);
								break;
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}

		if (TP_set.size() == maxNum_w)
		{
			break;
		}
	}
	
	return TP_set;
}

void Graph::MoSSo(int node)
{
	set<int> TP = getRandomNeighbor_MoSSo(node);
	set<int> TN;//����Ҫ�ж��Ƿ��ƶ��������³���Ľڵ�
	for (auto w = TP.begin(); w != TP.end(); ++w)
	{
		float w_IntoTN = rand() % 100 / (double)101;
		int w_ID = nodesID.at(*w);
		float w_degree = adjNodes[w_ID].size();
		float w_Pr = 1 / w_degree;

		if (w_IntoTN <= w_Pr)
		{
			TN.insert(*w);
		}
	}

	for (auto y = TN.begin(); y != TN.end(); ++y)
	{
		int y_locateSup = nodeToSup.at(*y);
		movedNode_realEdges(*y);

		float X = rand() % 100 / (double)101;
		if (X <= e)//���ܳ�����µĳ��ڵ�
		{
			if (supList.at(y_locateSup).size() != 1) //y���ڳ����ж���ڵ�ʱ�������Ƴ�y��Ϊһ���³��㡣��ֻ��y�������в�����
			{
				vector<int> delta = getDelta_moveNode(y_locateSup, y_locateSup, false, true);
				if (delta[1] < 0)  //�����µĳ���
				{
					update(*y, y_locateSup, y_locateSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}

		}
		else //�����ƶ�������������
		{
			int y_minHash = node_inGroup.at(*y);
			set<int> R_y;//y������������нڵ�
			for (auto i = groupNodes.at(y_minHash).begin(); i != groupNodes.at(y_minHash).end(); ++i)
			{
				R_y.insert(i->first);
			}
			R_y.erase(*y);
			vector<int> CP_y;//�����ƶ�����Щ�ڵ����ڵĳ����� 
			set_intersection(TP.begin(), TP.end(), R_y.begin(), R_y.end(), back_inserter(CP_y));

			if (CP_y.size() != 0)
			{
				int z_Index = rand() % (CP_y.size());
				int z = CP_y.at(z_Index);
				if (nodeToSup.at(z) != y_locateSup)//z��y����ͬһ��������
				{
					vector<int> delta = getDelta_moveNode(y_locateSup, nodeToSup.at(z), true, false);
					if (delta[0] < 0)
					{
						update(*y, y_locateSup, nodeToSup.at(z), true);
					}
				}
			}
		}
		moveNode_realEdges.clear();
	}
}


//base
void Graph::addEdgeToSupG1(int node1, int node2)//�ӱߺ󣬸��µ�ǰ��ͼ
{
	//cout << "addEdgeToSupG:" << "(" << node1 << "," << node2 << ")" << endl;
	ecnt = ecnt + 1;
	add_u = node1;
	add_v = node2;
	addNodeToSupG(node1);
	addNodeToSupG(node2);
	adjKeyNodes1 = nodesID.at(node1);
	adjKeyNodes2 = nodesID.at(node2);
	adjNodes[adjKeyNodes1].insert(node2); //node1�ڽӵ������node2
	adjNodes[adjKeyNodes2].insert(node1); //node2�ڽӵ������node1

	int vv1 = nodeToSup.at(node1);
	int mm1 = nodeToSup.at(node2);

	vector<int> vv2;  //����node1���ڳ�����ڽӵ�
	vv2.push_back(node2);
	vector<int> vv3;
	set_intersection(vv2.begin(), vv2.end(), adjSup[vv1].begin(), adjSup[vv1].end(), back_inserter(vv3));
	if (vv3.size() == 0)
	{
		vector<int> vv4;
		set_union(vv2.begin(), vv2.end(), adjSup[vv1].begin(), adjSup[vv1].end(), back_inserter(vv4));
		adjSup[vv1] = vv4;
	}
	vector<int> mm2; //����node2���ڳ�����ڽӵ�
	mm2.push_back(node1);
	vector<int> mm3;
	set_intersection(mm2.begin(), mm2.end(), adjSup[mm1].begin(), adjSup[mm1].end(), back_inserter(mm3));
	if (mm3.size() == 0)
	{
		vector<int> mm4;
		set_union(mm2.begin(), mm2.end(), adjSup[mm1].begin(), adjSup[mm1].end(), back_inserter(mm4));
		adjSup[mm1] = mm4;
	}

	addEdge_update(node1, node2, vv1, mm1);//���£��ӱ����ڳ���ı�
	/*if (node1 == 2429 && node2 == 906)
	{
		cout << "-----------add-edge-before---update------" << endl;
		EndResult();
		cout << "-------------------------------------------" << endl;
	}*/
}

void Graph::changedSimilar1()
{
	//cout << "add a edge: (" << add_u << "," << add_v << ")" << endl;
	int x1 = nodesID.at(add_u);
	int e1 = nodesID.at(add_v);

	set<int> u_2hopLocaSup; //u��2-hop�ڵ����ڳ���
	for (auto i = adjNodes.at(x1).begin(); i != adjNodes.at(x1).end(); ++i)
	{
		int i2 = nodesID.at(*i);
		for (auto i3 = adjNodes.at(i2).begin(); i3 != adjNodes.at(i2).end(); ++i3)//2-hop
		{
			if (*i3 != add_u)
			{
				int sup = nodeToSup.at(*i3);
				u_2hopLocaSup.insert(sup);
			}
		}
	}
	if (u_2hopLocaSup.size() != 0)
	{
		reCalcSimilar_base[add_u] = u_2hopLocaSup;
	}

	set<int> v_2hopLocaSup;//v��
	for (auto i = adjNodes.at(e1).begin(); i != adjNodes.at(e1).end(); ++i)
	{
		int i2 = nodesID.at(*i);
		for (auto i3 = adjNodes.at(i2).begin(); i3 != adjNodes.at(i2).end(); ++i3)
		{
			if (*i3 != add_v)
			{
				int sup = nodeToSup.at(*i3);
				v_2hopLocaSup.insert(sup);
			}
		}
	}
	if (v_2hopLocaSup.size() != 0)
	{
		reCalcSimilar_base[add_v] = v_2hopLocaSup;
	}
}

void Graph::changedSimilar2()
{
	//cout << "add a edge: (" << add_u << "," << add_v << ")" << endl;
	int x1 = nodesID.at(add_u);
	int e1 = nodesID.at(add_v);

	set<int> u_2hopUpLocaSup; //u��2-hop���ڵ����ڳ���
	for (auto i = adjNodes.at(e1).begin(); i != adjNodes.at(e1).end(); ++i)
	{
		if (*i != add_u)
		{
			int sup = nodeToSup.at(*i);
			u_2hopUpLocaSup.insert(sup);
		}
	}
	if (u_2hopUpLocaSup.size() != 0)
	{
		reCalcSimilar_base[add_u] = u_2hopUpLocaSup;
	}

	set<int> v_2hopUpLocaSup;//v��
	for (auto i = adjNodes.at(x1).begin(); i != adjNodes.at(x1).end(); ++i)
	{
		if (*i != add_v)
		{
			int sup = nodeToSup.at(*i);
			v_2hopUpLocaSup.insert(sup);
		}
	}
	if (v_2hopUpLocaSup.size() != 0)
	{
		reCalcSimilar_base[add_v] = v_2hopUpLocaSup;
	}
}

void Graph::traverseReCaleSimi1()
{
	for (auto cs15 = reCalcSimilar_base.begin(); cs15 != reCalcSimilar_base.end(); ++cs15)
	{
		//cout << "may move: " << cs15->first << endl;
		float nMaxSupn = 0;
		int dstSup;
		for (auto i = cs15->second.begin(); i != cs15->second.end(); ++i)
		{
			float cs17 = nodeSimilarSup1(cs15->first, *i);
			//cout << "with supernode:"<< *i <<" has simialrity is:" << cs17 << endl;
			if (cs17 >= nMaxSupn)
			{
				nMaxSupn = cs17;
				dstSup = *i;
			}
		}


		//�ڵ��ƶ������ƶ�����������ڵ�����Ϊһ������
		int loca_sup = nodeToSup.at(cs15->first);
		movedNode_realEdges(cs15->first);
		if (dstSup == loca_sup) //Ŀ�ĵس���Ϊ�����㣬ֻ�����ܷ������³���
		{
			//cout << "dstSup is located supernode" << endl;
			if (supList.at(loca_sup).size() != 1)//��loca_sup�г����һ���³��㣬loca_sup,�³���IDΪnum_supNode����ر�����
			{
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, false, true);
				if (delta[1] < 0)  //�����µĳ���
				{
					//cout << "create a new supernode:" << num_supNode << endl;
					update1(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}
		}
		else //Ŀ�ĵس������������㣬�ȼ���ϲ������������savֵ
		{
			if (supList.at(loca_sup).size() != 1)//�ж��ǳ�����µĳ��㻹�Ǻϲ�����������
			{
				//cout << "dstSup is other sueprnode" << endl;
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, true);

				if (delta[0] < 0 && delta[0] < delta[1])//move node
				{
					//cout << "move node to other supernode:"<< dstSup << endl;
					update1(cs15->first, loca_sup, dstSup, true);
				}
				if (delta[1] < 0 && delta[1] < delta[0])//add new Supnode
				{
					//cout << "create a new supernode: " << num_supNode << endl;
					update1(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}
			else //�ж��Ƿ�ϲ�����������
			{
				//cout << "dstSup is other sueprnode" << endl;
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, false);
				if (delta[0] < 0)
				{
					//cout << "move node to other supernode:"<< dstSup << endl;
					update1(cs15->first, loca_sup, dstSup, true);

				}
			}
		}
		moveNode_realEdges.clear();
	}
	reCalcSimilar_base.clear();
}

void Graph::update1(int node, int s1, int s2, bool moveOrNew)
{
	int supID;
	if (moveOrNew == true) //move
	{
		supID = s2;
		update_realNumEdges(s1, s2, true);
		update_adjSup(node, s1, supID, true);
	}
	else                   //new
	{
		supID = num_supNode;
		update_realNumEdges(s1, s2, false);
		update_adjSup(node, s1, supID, false);
	}

	nodeToSup[node] = supID;//����nodeӳ��
	supList[supID][node] = 'a';//s2(��num_supNode)���node
	supList[s1].erase(node);//s1ɾ����node
	bool delete_s1 = false;
	if (supList[s1].size() == 0)
	{
		supList.erase(s1);
		if (supGraph.find(s1) != supGraph.end())
		{
			for (auto i = supGraph[s1].begin(); i != supGraph[s1].end(); ++i)
			{
				supGraph[i->first].erase(s1);//��ͼ�ﷴɾ
				if (supGraph[i->first].size() == 0)
				{
					supGraph.erase(i->first);
				}
			}

			costCounter = costCounter - supGraph.at(s1).size() * 2;
			supGraph.erase(s1);//��ɾ
		}
		adjSup.erase(s1);

		for (auto sh2 = realNumEdges[s1].begin(); sh2 != realNumEdges[s1].end(); ++sh2)
		{
			if (sh2->first != s1)
			{
				realNumEdges[sh2->first].erase(s1);
			}
		}
		realNumEdges.erase(s1);

		delete_s1 = true;
	}

	delete_node_suploop(node);
	delete_node_supEdge(node);
	if (delete_s1 == false)
	{
		for (auto i = realNumEdges[s1].begin(); i != realNumEdges[s1].end(); ++i)
		{
			if (i->first == s1)
			{
				update_suploop(node, s1, true, moveOrNew);//true��ʾ�������Դ������Ի�
			}
			else
			{
				if (i->first != supID) //Դ�����Ȳ������Ŀ�ĳ����ı�
				{
					update_supEdge(node, s1, i->first, true);//true��ʾ�������Դ�������س���
				}
			}
		}
	}

	for (auto i = realNumEdges[supID].begin(); i != realNumEdges[supID].end(); ++i)
	{
		if (i->first == supID)
		{
			update_suploop(node, supID, false, moveOrNew); //false��ʾ�������Ŀ�ĳ�����Ի�
		}
		else
		{
			update_supEdge(node, supID, i->first, false);//false��ʾ�������Ŀ�ĳ������س���

		}
	}
}


//���ѡȡ2hop�ڵ����ڳ���
void Graph::changedSimilar3()
{
	//cout << "add a edge: (" << add_u << "," << add_v << ")" << endl;
	int x1 = nodesID.at(add_u);
	int e1 = nodesID.at(add_v);

	set<int> u_2hop_Nv = adjNodes[e1]; //u��2-hop�ڵ��N(v)ȡ����
	u_2hop_Nv.erase(add_u);
	if (u_2hop_Nv.size() != 0)
	{
		reCalcSimilar3[add_u] = u_2hop_Nv;
	}

	set<int> v_2hop_Nu = adjNodes[x1];//v��2-hop�ڵ��N(u)ȡ����
	v_2hop_Nu.erase(add_v);
	if (v_2hop_Nu.size() != 0)
	{
		reCalcSimilar3[add_v] = v_2hop_Nu;
	}
	
	/*for (auto i = reCalcSimilar3.begin(); i != reCalcSimilar3.end(); i++)
	{
		cout << "reCalcSimilar3-----node----" << i->first;
		for (auto i1 = i->second.begin(); i1 != i->second.end(); i1++)
		{
			cout << ";;" << *i1 << ";;";
		}
		cout << endl;
	}*/
}

int Graph::selectDstSup(map<int, float>& allpro, float all) 
{
	float sample_sup = rand() % 100 / (double)101;
	//cout << "sample_sup::" << sample_sup << endl;

	float acc = 0;
	for (auto i = allpro.begin(); i != allpro.end();)
	{
		acc = acc + i->second; 
		float right = acc / all;  //���ʷֶ�ֵ
		//cout << "right::" << right << endl;

		if (sample_sup > right)
		{
			++i;
		}
		else
		{
			return i->first;
		}
	}
    return -1;
}

void Graph::traverseReCaleSimi3()
{
	for (auto cs15 = reCalcSimilar3.begin(); cs15 != reCalcSimilar3.end(); ++cs15)
	{
		//ԭʼ���������ֵ��ĳ��������2-hop�ڵ�����/�ó���size����greedyȡ���û��random����ѹ��Ч���ã�˵���ñ�ֵ���߱������ԣ�
		unordered_map<int, vector<int>> sup_nodes;//sup-nodes��2-hop�ڵ�������ڳ�����л���
		for (auto i = cs15->second.begin(); i != cs15->second.end(); ++i)
		{
			int sup = nodeToSup.at(*i);
			if (sup_nodes.find(sup) != sup_nodes.end())
			{
				sup_nodes[sup].push_back(*i);
			}
			else
			{
				vector<int> ls;
				ls.push_back(*i);
				sup_nodes[sup] = ls;
			}
		}
		map<int, float> sup_pro;//�泬���Ӧ�ĸ���
		float allpro = 0;
		float maxPro = 0;
		int dstSup;//��Ӧȡ���ֵ������dst����
		for (auto q1 = sup_nodes.begin(); q1 != sup_nodes.end(); ++q1)
		{
			float q2 = q1->second.size();
			float pro = q2 / supList.at(q1->first).size();//�ó����ڽ����ڵ����/�ó����ڽڵ�����
			sup_pro[q1->first] = pro;
			allpro = allpro + pro;//�������з�ĸ
			if (pro >= maxPro)//ȡ���ֵ�������̶�ѡ���pro�ĳ���
			{
				maxPro = pro;
				dstSup = q1->first;
			}
		}

		/*cout << "=====================" << endl;
		for (auto i = sup_nodes.begin(); i != sup_nodes.end(); i++)
		{
			cout << i->first << "---sup_nodes--";
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				cout << ";;" << *j << ";";
			}
			cout << endl;
		}
		for (auto i = num_sup.begin(); i != num_sup.end(); ++i)
		{
			cout << i->first << "---num_sup-";
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				cout << ";;" << j->first << ";;";
			}
			cout << endl;
		}
		for (auto i = sup_pro.begin(); i != sup_pro.end(); i++)
		{
			cout << "sup_pro::" << i->first << ": :" << i->second << endl;
		}
		cout << "=====================" << endl;*/


		//�ڵ��ƶ������ƶ�����������ڵ�����Ϊһ������
		int loca_sup = nodeToSup.at(cs15->first);
		movedNode_realEdges(cs15->first);
		if (dstSup == loca_sup) //Ŀ�ĵس���Ϊ�����㣬ֻ�����ܷ������³���
		{
			//cout << "dstSup is located supernode" << endl;
			if (supList.at(loca_sup).size() != 1)//��loca_sup�г����һ���³��㣬loca_sup,�³���IDΪnum_supNode����ر�����
			{
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, false, true);
				if (delta[1] < 0)  //�����µĳ���
				{
					//cout << "create a new supernode:" << num_supNode << endl;
					update(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}
		}
		else //Ŀ�ĵس������������㣬�ȼ���ϲ������������savֵ
		{    
			if (supList.at(loca_sup).size() != 1)//�ж��ǳ�����µĳ��㻹�Ǻϲ�����������
			{
				/*if (add_u == 2429 && add_v == 906)
				{
					cout << "dstSup is other sueprnode" << endl;
					cout << "from:" << loca_sup << " to:" << dstSup << endl;
				}*/
				//cout << "dstSup is other sueprnode" << endl;
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, true);
				//cout << "delta[0] " << delta[0] << endl;
				//cout << "delta[1] " << delta[1] << endl;

				if (delta[0] < 0 && delta[0]< delta[1])//move node
				{
					/*if (add_u == 2429 && add_v == 906)
					{
						cout << "move node to other supernode:" << dstSup << endl;
					}*/
					//cout << "move node to other supernode:"<< dstSup << endl;
					update(cs15->first, loca_sup, dstSup, true);
				}
				if (delta[1] < 0 && delta[1] < delta[0])//add new Supnode
				{
					/*if (add_u == 2429 && add_v == 906)
					{
						cout << "create a new supernode: " << num_supNode << endl;
					}*/
					//cout << "create a new supernode: " << num_supNode << endl;
					update(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}
			else //�ж��Ƿ�ϲ�����������
			{
				/*if (add_u == 2429 && add_v == 906)
				{
					cout << "dstSup is other sueprnode:"<< dstSup << endl;
				}*/
				//cout << "dstSup is other sueprnode" << endl;
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, false);
				//cout << "delta[0] " << delta[0] << endl;
				if (delta[0] < 0)
				{
					/*if (add_u == 2429 && add_v == 906)
					{
						cout << "move node to other supernode:" << dstSup << endl;
					}*/
					//cout << "move node to other supernode:"<< dstSup << endl;
					update(cs15->first, loca_sup, dstSup, true);
					
				}
			}
		}
		moveNode_realEdges.clear();
	}
	reCalcSimilar3.clear();
}

void Graph::traverseReCaleSimi4()
{
	for (auto cs15 = reCalcSimilar3.begin(); cs15 != reCalcSimilar3.end(); ++cs15)
	{
		//ԭʼ���������ֵ��ĳ��������2-hop�ڵ�����/�ó���size����greedyȡ���û��random����ѹ��Ч���ã�˵���ñ�ֵ���߱������ԣ�
		unordered_map<int, vector<int>> sup_nodes;//sup-nodes��2-hop�ڵ�������ڳ�����л���
		for (auto i = cs15->second.begin(); i != cs15->second.end(); ++i)
		{
			int sup = nodeToSup.at(*i);
			if (sup_nodes.find(sup) != sup_nodes.end())
			{
				sup_nodes[sup].push_back(*i);
			}
			else
			{
				vector<int> ls;
				ls.push_back(*i);
				sup_nodes[sup] = ls;
			}
		}
		map<int, float> sup_pro;//�泬���Ӧ�ĸ���
		float allpro = 0;
		float maxPro = 0;
		for (auto q1 = sup_nodes.begin(); q1 != sup_nodes.end(); ++q1)
		{
			float q2 = q1->second.size();
			float pro = q2 / supList.at(q1->first).size();//�ó����ڽ����ڵ����/�ó����ڽڵ�����
			sup_pro[q1->first] = pro;
			allpro = allpro + pro;//�������з�ĸ
		}
		int dstSup = selectDstSup(sup_pro, allpro);//���ѡȡ�������ƶ����ĳ���

		//�ڵ��ƶ������ƶ�����������ڵ�����Ϊһ������
		int loca_sup = nodeToSup.at(cs15->first);
		movedNode_realEdges(cs15->first);
		if (dstSup == loca_sup) //Ŀ�ĵس���Ϊ�����㣬ֻ�����ܷ������³���
		{
			if (supList.at(loca_sup).size() != 1)//��loca_sup�г���һ���³��㣬loca_sup,�³���IDΪnum_supNode����ر�����
			{
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, false, true);
				if (delta[1] < 0)  //�����µĳ���
				{
					update(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õ����µĳ���ID
				}
			}
		}
		else //Ŀ�ĵس������������㣬�ȼ���ϲ������������savֵ
		{
			if (supList.at(loca_sup).size() != 1)//�ж��ǳ�����µĳ��㻹�Ǻϲ�����������
			{
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, true);
				if (delta[0] < 0 && delta[0] < delta[1])//move node
				{
					update(cs15->first, loca_sup, dstSup, true);
				}
				if (delta[1] < 0 && delta[1] < delta[0])//add new Supnode
				{
					update(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}
			else //�ж��Ƿ�ϲ�����������
			{
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, false);
				if (delta[0] < 0)
				{
					update(cs15->first, loca_sup, dstSup, true);
				}
			}
		}
		moveNode_realEdges.clear();
	}
	reCalcSimilar3.clear();
}

void Graph::traverseReCaleSimi5() 
{
	for (auto cs15 = reCalcSimilar_base.begin(); cs15 != reCalcSimilar_base.end(); ++cs15)
	{
		set<int> kong;
		vector<int> ve;
		set_union(cs15->second.begin(), cs15->second.end(), kong.begin(), kong.end(), back_inserter(ve));
		int dst_index = rand() % ve.size();
		int dstSup = ve.at(dst_index); //�õ���ѡ���㼯�����ȡĿ�ĳ���
		if (supList.find(dstSup) == supList.end())
		{
			ve.erase(ve.begin()+ dst_index);//ɾ��ve[dst_index]
			dst_index = rand() % ve.size();
			dstSup = ve.at(dst_index);
		}

		//�ڵ��ƶ������ƶ�����������ڵ�����Ϊһ������
		int loca_sup = nodeToSup.at(cs15->first);
		movedNode_realEdges(cs15->first);
		if (dstSup == loca_sup) //Ŀ�ĵس���Ϊ�����㣬ֻ�����ܷ������³���
		{
			//cout << "dstSup is located supernode" << endl;
			if (supList.at(loca_sup).size() != 1)//��loca_sup�г����һ���³��㣬loca_sup,�³���IDΪnum_supNode����ر�����
			{
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, false, true);
				if (delta[1] < 0)  //�����µĳ���
				{
					//cout << "create a new supernode:" << num_supNode << endl;
					update(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}
		}
		else //Ŀ�ĵس������������㣬�ȼ���ϲ������������savֵ
		{
			if (supList.at(loca_sup).size() != 1)//�ж��ǳ�����µĳ��㻹�Ǻϲ�����������
			{
				/*if (add_u == 2429 && add_v == 906)
				{
					cout << "dstSup is other sueprnode" << endl;
					cout << "from:" << loca_sup << " to:" << dstSup << endl;
				}*/
				//cout << "dstSup is other sueprnode" << endl;
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, true);
				//cout << "delta[0] " << delta[0] << endl;
				//cout << "delta[1] " << delta[1] << endl;

				if (delta[0] < 0 && delta[0] < delta[1])//move node
				{
					/*if (add_u == 2429 && add_v == 906)
					{
						cout << "move node to other supernode:" << dstSup << endl;
					}*/
					//cout << "move node to other supernode:"<< dstSup << endl;
					update(cs15->first, loca_sup, dstSup, true);
				}
				if (delta[1] < 0 && delta[1] < delta[0])//add new Supnode
				{
					/*if (add_u == 2429 && add_v == 906)
					{
						cout << "create a new supernode: " << num_supNode << endl;
					}*/
					//cout << "create a new supernode: " << num_supNode << endl;
					update(cs15->first, loca_sup, dstSup, false);//����ѹ��ͼ
					num_supNode = num_supNode + 1;//�ټӱ�֮��addNodeToSupG��������½ڵ���Ϊ�³���ʱ���õľ����µĳ���ID
				}
			}
			else //�ж��Ƿ�ϲ�����������
			{
				/*if (add_u == 2429 && add_v == 906)
				{
					cout << "dstSup is other sueprnode:"<< dstSup << endl;
				}*/
				//cout << "dstSup is other sueprnode" << endl;
				vector<int> delta = getDelta_moveNode(loca_sup, dstSup, true, false);
				//cout << "delta[0] " << delta[0] << endl;
				if (delta[0] < 0)
				{
					/*if (add_u == 2429 && add_v == 906)
					{
						cout << "move node to other supernode:" << dstSup << endl;
					}*/
					//cout << "move node to other supernode:"<< dstSup << endl;
					update(cs15->first, loca_sup, dstSup, true);

				}
			}
		}
		moveNode_realEdges.clear();
	}
	reCalcSimilar_base.clear();
}


//��������
int Graph::getIntraPi(int sup)
{
	int size = supList[sup].size();
	int pi = 0;
	for (auto i = 1; i < size; ++i)
	{
		pi = pi + i;
	}
	return pi;
}

int Graph::getInterPi(int s1, int s2) 
{
	int pi = supList[s1].size() * supList[s2].size();
	return pi;
}

void Graph::movedNode_realEdges(int node)
{
	/*struct timeval step1;
	struct timeval step2;
	gettimeofday(&step1, NULL);*/

	int ad = nodesID.at(node);
	//cout << "adjNodes[ad]:";
	for (auto mn1 = adjNodes[ad].begin(); mn1 != adjNodes[ad].end(); ++mn1)
	{
		//cout << ";;" << *mn1 << ";;";
		int mn2 = nodeToSup.at(*mn1);
		if (moveNode_realEdges.find(mn2) != moveNode_realEdges.end())
		{
			moveNode_realEdges[mn2].push_back(*mn1);
		}
		else
		{
			vector<int> mn4;
			mn4.push_back(*mn1);
			moveNode_realEdges[mn2] = mn4;
		}
	}
	//cout << endl;

	/*gettimeofday(&step2, NULL);
	time_movedNode_realEdges = step2.tv_sec - step1.tv_sec + time_movedNode_realEdges;*/
}

vector<int> Graph::getDelta_moveNode(int s1, int s2, bool move, bool New) //�ƶ��ڵ��s1��s2
{
	int before = 0;
	int after = 0;
	int newReal = 0;
	int pi = 0;
	int newpi = 0;
	int cost = 0;
	int newcost = 0;
	int delta = 0;
	set<int> relatedSup_node;//���ȥs1,s2�����������س���

	for (auto i = realNumEdges[s1].begin(); i != realNumEdges[s1].end(); ++i)//s1���cost
	{
		if (i->first != s2)
		{
			if (i->first == s1)
			{
				if (moveNode_realEdges.find(s1) != moveNode_realEdges.end())
				{
					newReal = i->second - moveNode_realEdges[s1].size();
				}
				else
				{
					newReal = i->second;
				}
				pi = getIntraPi(s1);
				int moveEdges = supList.at(s1).size();
				newpi = pi - moveEdges + 1;
				cost = min(i->second, pi - i->second);
				newcost = min(newReal, newpi - newReal);
				before = before + cost;
				after = after + newcost;
			}
			else
			{
				if (moveNode_realEdges.find(i->first) != moveNode_realEdges.end())
				{
					relatedSup_node.insert(i->first);
					newReal = i->second - moveNode_realEdges[i->first].size();
				}
				else
				{
					newReal = i->second;
				}
				pi = getInterPi(s1, i->first);
				int moveEdges = supList.at(i->first).size();
				newpi = pi - moveEdges;
				cost = min(i->second, pi - i->second + 1);
				newcost = min(newReal, newpi - newReal + 1);
				before = before + cost;
				after = after + newcost;
			}
		}
	}
	/*if (add_u == 2429 && add_v == 906)
	{
		cout << "1--------before:"<<before << endl;
		cout << "1----------after:" << after << endl;
	}*/
	int before_newSup = before;//���������Ҫ�����������������õ�
	int after_newSup = after;
	/*if (add_u == 2429 && add_v == 906)
	{
		cout << "newsup----1--------before:" << before_newSup << endl;
		cout << "newsup----1----------after:" << after_newSup << endl;
	}*/

	if (move == true)
	{	
		for (auto i = realNumEdges[s2].begin(); i != realNumEdges[s2].end(); ++i)
		{
			if (i->first != s1)
			{
				if (i->first == s2)
				{
					if (moveNode_realEdges.find(s2) != moveNode_realEdges.end())
					{
						newReal = i->second + moveNode_realEdges[s2].size();
					}
					else
					{
						newReal = i->second;
					}
					pi = getIntraPi(s2);
					int moveEdges = supList.at(s2).size();
					newpi = pi + moveEdges;
					cost = min(i->second, pi - i->second);
					newcost = min(newReal, newpi - newReal);
					before = before + cost;
					after = after + newcost;
				}
				else
				{
					if (moveNode_realEdges.find(i->first) != moveNode_realEdges.end())
					{
						relatedSup_node.erase(i->first);
						newReal = i->second + moveNode_realEdges[i->first].size();
					}
					else
					{
						newReal = i->second;
					}
					pi = getInterPi(s2, i->first);
					int moveEdges = supList.at(i->first).size();
					newpi = pi + moveEdges;
					cost = min(i->second, pi - i->second + 1);
					newcost = min(newReal, newpi - newReal + 1);
					before = before + cost;
					after = after + newcost;
				}
			}
		}
		/*if (add_u == 2429 && add_v == 906)
		{
			cout << "2--------before:" << before << endl;
			cout << "2----------after:" << after << endl;
		}*/

		for (auto i = relatedSup_node.begin(); i != relatedSup_node.end(); ++i)//s2������صĳ��㣬������node����س���
		{
			newReal = moveNode_realEdges.at(*i).size();
			pi = getInterPi(s2, *i);
			int moveEdges = supList.at(*i).size();
			newpi = pi + moveEdges;
			newcost = min(newReal, newpi - newReal + 1);
			after = after + newcost;

		}
		/*if (add_u == 2429 && add_v == 906)
		{
			cout << "3--------before:" << before << endl;
			cout << "3----------after:" << after << endl;
		}*/

		//s1,s2֮��cost����
		int s1_delta = 0;//�ƶ��ڵ���ڽӵ����������е�����
		if (moveNode_realEdges.find(s1) != moveNode_realEdges.end())
		{
			s1_delta = moveNode_realEdges[s1].size();
		}
		int s2_delta = 0;//�ƶ��ڵ���ڽӵ���Ŀ�ĳ��������
		if (moveNode_realEdges.find(s2) != moveNode_realEdges.end())
		{
			s2_delta = moveNode_realEdges[s2].size();
		}
		int real_s1s2 = 0;
		if (realNumEdges[s1].find(s2) != realNumEdges[s1].end())
		{
			real_s1s2 = realNumEdges[s1][s2];
		}
		pi = getInterPi(s1, s2);
		newpi = pi + supList.at(s1).size() - supList.at(s2).size() - 1;
		newReal = real_s1s2 + s1_delta - s2_delta;
		cost = min(real_s1s2, pi - real_s1s2 + 1);
		newcost = min(newReal, newpi - newReal + 1);
		before = before + cost;
		after = after + newcost;
		delta = after - before;

		/*if (add_u == 2429 && add_v == 906)
		{
			cout << "4--------before:" << before << endl;
			cout << "4----------after:" << after << endl;
			cout << "delta:" << delta << endl;
		}*/
	}
	vector<int> returnDelta;
	returnDelta.push_back(delta);//returnDelta[0]��move�����delta

	//�������������µĳ��㣬��Ӧ��before��after
	if (New == true)
	{
		int newReal_newSup = 0;
		int pi_newSup = 0;
		int newpi_newSup = 0;
		int cost_newSup = 0;
		int newcost_newSup = 0;
		int delta_newSup = 0;
		

		if (s1 == s2)//Ŀ�ĳ����Դ������ͬʱ��ֻ���ǳ�����³��������S1�������Ի���cost
		{
			if (moveNode_realEdges.find(s1) != moveNode_realEdges.end()) 
			{
				newReal_newSup = realNumEdges[s1][s1] - moveNode_realEdges[s1].size();
			}
			else
			{
				newReal_newSup = realNumEdges[s1][s1];
			}
			pi_newSup = getIntraPi(s1);
			int moveEdges = supList.at(s1).size();
			newpi_newSup = pi_newSup - moveEdges + 1;
			cost_newSup = min(realNumEdges[s1][s1], pi_newSup - realNumEdges[s1][s1]);
			newcost_newSup = min(newReal_newSup, newpi_newSup - newReal_newSup);
			before_newSup = before_newSup + cost_newSup;
			after_newSup = after_newSup + newcost_newSup;
			/*if (add_u == 2429 && add_v == 906)
			{
				cout << "cost_newSup:" << cost_newSup << endl;
				cout << "newcost_newSup:" << newcost_newSup << endl;
				cout << "newsup----2--------before:" << before_newSup << endl;
				cout << "newsup----2----------after:" << after_newSup << endl;
			}*/
		}
		else //����ͬʱ��������move��new���������s1�������Ƴ��ڵ���s2֮�����ر�cost
		{
			if (realNumEdges[s1].find(s2) != realNumEdges[s1].end())
			{
				if (moveNode_realEdges.find(s2) != moveNode_realEdges.end())
				{
					newReal_newSup = realNumEdges[s1][s2] - moveNode_realEdges[s2].size();
				}
				else
				{
					newReal_newSup = realNumEdges[s1][s2];
				}
				pi_newSup = getInterPi(s1, s2);
				int moveEdges = supList.at(s2).size();
				newpi_newSup = pi_newSup - moveEdges;
				cost_newSup = min(realNumEdges[s1][s2], pi_newSup - realNumEdges[s1][s2] + 1);
				newcost_newSup = min(newReal_newSup, newpi_newSup - newReal_newSup + 1);
				before_newSup = before_newSup + cost_newSup;
				after_newSup = after_newSup + newcost_newSup;
			}		
		}
		

		for (auto i = moveNode_realEdges.begin(); i != moveNode_realEdges.end(); ++i)//�³�������س�����cost,���ܰ���s1
		{
			pi_newSup = (i->first == s1 ? supList.at(i->first).size() - 1 : supList.at(i->first).size());
			newcost_newSup = min(i->second.size(), pi_newSup - i->second.size() + 1);
			after_newSup = after_newSup + newcost_newSup;
		}
		
		delta_newSup = after_newSup - before_newSup;
		returnDelta.push_back(delta_newSup); //returnDelta[1]��New a supernode����� 
		/*if (add_u == 2429 && add_v == 906)
		{
			cout << "newsup----3--------before:" << before_newSup << endl;
			cout << "newsup----3----------after:" << after_newSup << endl;
			cout << "newsup----3----------delta_newSup:" << delta_newSup << endl;
		}*/
	}

	return returnDelta;
}

void Graph::update(int node, int s1, int s2, bool moveOrNew) 
{
	int supID;
	if (moveOrNew == true) //move
	{
		supID = s2;
		update_realNumEdges(s1, s2, true);
		//update_adjSup(node, s1, supID, true);
	}
	else                   //new
	{
		supID = num_supNode;
		update_realNumEdges(s1, s2, false);
		//update_adjSup(node, s1, supID, false);
	}

	nodeToSup[node] = supID;//����nodeӳ��
	supList[supID][node] = 'a';//s2(��num_supNode)���node
	supList[s1].erase(node);//s1ɾ����node
	bool delete_s1 = false;
	if (supList[s1].size() == 0)
	{
		supList.erase(s1);
		if (supGraph.find(s1) != supGraph.end())
		{
			for (auto i = supGraph[s1].begin(); i != supGraph[s1].end(); ++i)
			{
				supGraph[i->first].erase(s1);//��ͼ�ﷴɾ
				if (supGraph[i->first].size() == 0)
				{
					supGraph.erase(i->first);
				}
			}

			costCounter = costCounter - supGraph.at(s1).size() * 2;
			supGraph.erase(s1);//��ɾ
		}
		//adjSup.erase(s1);

		for (auto sh2 = realNumEdges[s1].begin(); sh2 != realNumEdges[s1].end(); ++sh2)
		{
			if (sh2->first != s1)
			{
				realNumEdges[sh2->first].erase(s1);
			}
		}
		realNumEdges.erase(s1);
		
		delete_s1 = true;
	}

	delete_node_suploop(node);
	delete_node_supEdge(node);
	if (delete_s1 == false)
	{
		for (auto i = realNumEdges[s1].begin(); i != realNumEdges[s1].end(); ++i)
		{
			if (i->first == s1)
			{
				update_suploop(node, s1, true, moveOrNew);//true��ʾ�������Դ������Ի�
			}
			else
			{
				if (i->first != supID) //Դ�����Ȳ������Ŀ�ĳ����ı�
				{
					update_supEdge(node, s1, i->first, true);//true��ʾ�������Դ�������س���
				}
			}
		}
	}

	for (auto i = realNumEdges[supID].begin(); i != realNumEdges[supID].end(); ++i)
	{
		if (i->first == supID)
		{
			update_suploop(node, supID, false, moveOrNew); //false��ʾ�������Ŀ�ĳ�����Ի�
		}
		else
		{
			update_supEdge(node, supID, i->first, false);//false��ʾ�������Ŀ�ĳ������س���
			
		}
	}
}

void Graph::update_realNumEdges(int s1, int s2, bool moveOrNew) 
{
	if (moveOrNew == true) //move
	{
		for (auto mn5 = moveNode_realEdges.begin(); mn5 != moveNode_realEdges.end(); ++mn5)
		{
			int mn6 = mn5->second.size();
			if (mn5->first == s1)
			{
				realNumEdges[s1][s1] = realNumEdges[s1][s1] - mn6;
				realNumEdges[s1][s2] = realNumEdges[s1][s2] + mn6;
				realNumEdges[s2][s1] = realNumEdges[s1][s2];
			}
			else
			{
				if (mn5->first == s2)
				{
					realNumEdges[s1][s2] = realNumEdges[s1][s2] - mn6;
					realNumEdges[s2][s1] = realNumEdges[s1][s2];

					realNumEdges[s2][s2] = realNumEdges[s2][s2] + mn6;
				}
				else
				{
					realNumEdges[s1][mn5->first] = realNumEdges[s1][mn5->first] - mn6;
					realNumEdges[mn5->first][s1] = realNumEdges[s1][mn5->first];

					if (realNumEdges[s2].find(mn5->first) != realNumEdges[s2].end())
					{
						realNumEdges[s2][mn5->first] = realNumEdges[s2][mn5->first] + mn6;
						realNumEdges[mn5->first][s2] = realNumEdges[s2][mn5->first];
					}
					else
					{
						realNumEdges[s2][mn5->first] = mn6;
						realNumEdges[mn5->first][s2] = mn6;
					}
				}
			}
		}
	}
	else  //new
	{
		realNumEdges[num_supNode][num_supNode] = 0;

		for (auto mn5 = moveNode_realEdges.begin(); mn5 != moveNode_realEdges.end(); ++mn5)
		{
			int mn6 = mn5->second.size();
			if (mn5->first == s1)
			{
				realNumEdges[s1][s1] = realNumEdges[s1][s1] - mn6;
				realNumEdges[s1][num_supNode] = mn6;
				realNumEdges[num_supNode][s1] = mn6; //��node��Ϊһ���µĳ��㣬���������������ر�����
			}
			else
			{
				realNumEdges[s1][mn5->first] = realNumEdges[s1][mn5->first] - mn6;
				realNumEdges[mn5->first][s1] = realNumEdges[s1][mn5->first];

				realNumEdges[num_supNode][mn5->first] = mn6;
				realNumEdges[mn5->first][num_supNode] = mn6;
			}
		}
	}
}

void Graph::update_adjSup(int node, int s1, int supID, bool moveOrNew)
{
	vector<int> m2;
	if (moveOrNew == true) //move
	{
		m2 = adjSup.at(supID);
	}
	int lo = nodesID.at(node);
	vector<int> m3;
	set_union(adjNodes[lo].begin(), adjNodes[lo].end(), m2.begin(), m2.end(), back_inserter(m3));
	adjSup[supID] = m3;//����s2((��num_supNode))���ڽӵ�

	if (supList[s1].size()-1 != 0)//s1����node�󣬳����Դ���
	{
		vector<int> mo9;//����s1�ڽӵ�
		for (auto mo8 = supList[s1].begin(); mo8 != supList[s1].end(); ++mo8)//�ӳ���s1���ڽӵ�ɾ��node1���ڽӵ�
		{
			set<int> mo10 = adjNodes[nodesID.at(mo8->first)];
			mo9.insert(mo9.end(), mo10.begin(), mo10.end());
		}
		sort(mo9.begin(), mo9.end());
		mo9.erase(unique(mo9.begin(), mo9.end()), mo9.end());
		adjSup[s1] = mo9;
	}
}

void Graph::delete_node_suploop(int node) 
{
	//�Ի��У�node�ڵ��s1���ƶ���ȥ����ɾ��node���
	if (intraDelC.find(node) != intraDelC.end())
	{
		costCounter = costCounter - intraDelC.at(node).size() * 2;
		for (auto i = intraDelC[node].begin(); i != intraDelC[node].end(); ++i)
		{
			intraDelC[i->first].erase(node);

			if (intraDelC[i->first].size() == 0)
			{
				unordered_map<int, map<int, char>>::iterator flag = intraDelC.find(i->first);
				intraDelC.erase(flag);
			}
		}
		intraDelC.erase(node);
	}

	if (intraAddC.find(node) != intraAddC.end())
	{
		costCounter = costCounter - intraAddC.at(node).size() * 2;
		for (auto i = intraAddC[node].begin(); i != intraAddC[node].end(); ++i)
		{
			intraAddC[i->first].erase(node);

			if (intraAddC[i->first].size() == 0)
			{
				unordered_map<int, map<int, char>>::iterator flag = intraAddC.find(i->first);
				intraAddC.erase(flag);
			}
		}
		intraAddC.erase(node);
	}
}

void Graph::update_suploop(int node, int supnode, bool srcOrdst, bool moveOrNew) 
{
	if (srcOrdst == true)
	{
		if (realNumEdges[supnode][supnode] <= getIntraPi(supnode) - realNumEdges[supnode][supnode])  //�ƶ���C+
		{
			if (supLoop.find(supnode) != supLoop.end()) //�ƶ�ǰP,C-
			{
				supLoop.erase(supnode);
				int num = 0;
				for (auto i = supList.at(supnode).begin(); i != supList.at(supnode).end(); ++i)
				{
					if (intraDelC.find(i->first) != intraDelC.end())
					{
						num = num + intraDelC.at(i->first).size();
						intraDelC.erase(i->first);
					}
					int adj = nodesID.at(i->first);

					for (auto j = supList.at(supnode).begin(); j != supList.at(supnode).end(); ++j)
					{
						if (i->first == j->first || i->first > j->first)
						{
							continue;
						}

						if (adjNodes[adj].find(j->first) != adjNodes[adj].end())
						{
							intraAddC[i->first][j->first] = 'a';//���C+
							intraAddC[j->first][i->first] = 'a';
							
							costCounter = costCounter + 2;
						}
					}
				}
				costCounter = costCounter - num * 2;
			}
		}
		else  //�ƶ���P,C-
		{
			if (supLoop.find(supnode) == supLoop.end()) //�ƶ�ǰC+
			{
				supLoop.insert(supnode);
				int num = 0;
				for (auto i = supList.at(supnode).begin(); i != supList.at(supnode).end(); ++i)
				{
					if (intraAddC.find(i->first) != intraAddC.end())
					{
						num = num + intraAddC.at(i->first).size();
						intraAddC.erase(i->first);
					}
					int adj = nodesID.at(i->first);

					for (auto j = supList.at(supnode).begin(); j != supList.at(supnode).end(); ++j)
					{
						if (i->first == j->first || i->first > j->first)
						{
							continue;
						}

						if (adjNodes[adj].find(j->first) == adjNodes[adj].end())
						{
							intraDelC[i->first][j->first] = 'a';//���C-
							intraDelC[j->first][i->first] = 'a';

							costCounter = costCounter + 2;
						}
					}
				}
				costCounter = costCounter - num * 2;
			}
		}
	}
	else
	{
		if (moveOrNew == true) //move������s2�Ի��������ɳ��㲻�����Ի����
		{
			int lo = nodesID.at(node);
			if (realNumEdges[supnode][supnode] <= getIntraPi(supnode) - realNumEdges[supnode][supnode]) //�ƶ���C+
			{
				if (supLoop.find(supnode) == supLoop.end()) //�ƶ�ǰC+����ΪC+��ֻ�����node���C+
				{
					for (auto i = supList.at(supnode).begin(); i != supList.at(supnode).end(); ++i)
					{
						if (node != i->first && adjNodes[lo].find(i->first) != adjNodes[lo].end())
						{
							intraAddC[node][i->first] = 'a';
							intraAddC[i->first][node] = 'a';

							costCounter = costCounter + 2;
						}
					}
				}
				else  //�ƶ�ǰP,C-
				{
					int num = 0;
					supLoop.erase(supnode);
					for (auto i = supList.at(supnode).begin(); i != supList.at(supnode).end(); ++i)
					{
						if (intraDelC.find(i->first) != intraDelC.end())
						{
							num = num + intraDelC.at(i->first).size();
							intraDelC.erase(i->first);
						}

						int adj = nodesID.at(i->first);
						for (auto j = supList.at(supnode).begin(); j != supList.at(supnode).end(); ++j)
						{
							if (i->first == j->first || i->first > j->first)
							{
								continue;
							}

							if (adjNodes[adj].find(j->first) != adjNodes[adj].end())
							{
								intraAddC[i->first][j->first] = 'a';
								intraAddC[j->first][i->first] = 'a';

								costCounter = costCounter + 2;
							}
						}
					}
					costCounter = costCounter - num * 2;
				}
			}
			else   //�ƶ���P,C-
			{
				if (supLoop.find(supnode) == supLoop.end()) //�ƶ�ǰC+
				{
					int num = 0;
					supLoop.insert(supnode);
					for (auto i = supList.at(supnode).begin(); i != supList.at(supnode).end(); ++i)
					{
						if (intraAddC.find(i->first) != intraAddC.end())
						{
							num = num + intraAddC.at(i->first).size();
							intraAddC.erase(i->first);
						}

						int adj = nodesID.at(i->first);
						for (auto j = supList.at(supnode).begin(); j != supList.at(supnode).end(); ++j)
						{
							if (i->first == j->first || i->first > j->first)
							{
								continue;
							}

							if (adjNodes[adj].find(j->first) == adjNodes[adj].end())
							{
								intraDelC[i->first][j->first] = 'a';
								intraDelC[j->first][i->first] = 'a';
								
								costCounter = costCounter + 2;
							}
						}
					}
					costCounter = costCounter - num * 2;
				}
				else  //�ƶ�ǰP,C-
				{
					for (auto i = supList.at(supnode).begin(); i != supList.at(supnode).end(); ++i)
					{
						if (node != i->first && adjNodes[lo].find(i->first) == adjNodes[lo].end())
						{
							intraDelC[node][i->first] = 'a';
							intraDelC[i->first][node] = 'a';

							costCounter = costCounter + 2;
						}
					}
				}
			}
		}
	}
}

void Graph::delete_node_supEdge(int node) 
{
	//���³�����ǰ����ɾ��node��ر�
	int numA = 0;
	if (interAddC.find(node) != interAddC.end())
	{
		numA = numA + interAddC.at(node).size();
		for (auto i = interAddC[node].begin(); i != interAddC[node].end(); ++i)
		{
			interAddC[i->first].erase(node);

			if (interAddC[i->first].size() == 0)
			{
				unordered_map<int, map<int, char>>::iterator flag = interAddC.find(i->first);
				interAddC.erase(flag);
			}
		}
		interAddC.erase(node);
	}
	costCounter = costCounter - numA * 2;

	int numD = 0;
	if (interDelC.find(node) != interDelC.end())
	{
		numD = numD + interDelC.at(node).size();
		for (auto i = interDelC[node].begin(); i != interDelC[node].end(); ++i)
		{
			interDelC[i->first].erase(node);

			if (interDelC[i->first].size() == 0)
			{
				unordered_map<int, map<int, char>>::iterator flag = interDelC.find(i->first);
				interDelC.erase(flag);
			}
		}
		interDelC.erase(node);
	}
	costCounter = costCounter - numD * 2;
}

void Graph::update_supEdge(int node, int s1, int s2, bool srcOrdst) 
{
	if (realNumEdges[s1][s2] <= getInterPi(s1, s2) - realNumEdges[s1][s2] + 1) //�ƶ���C+
	{
		if (supGraph.find(s1) != supGraph.end())
		{
			if (supGraph[s1].find(s2) != supGraph[s1].end())  //�ƶ�ǰ��P,C-
			{
				deleteSupedge(node, s1, s2);
			}
			else   //�ƶ�ǰ��C+
			{
				remain_CAdd(node, s2, srcOrdst);
			}
		}
		else   //�ƶ�ǰ��C+
		{
			remain_CAdd(node, s2, srcOrdst);
		}
	}
	else  //�ƶ���P,C-
	{
		if (supGraph.find(s1) != supGraph.end())
		{
			if (supGraph[s1].find(s2) != supGraph[s1].end())  //�ƶ�ǰ��P,C-
			{
				remain_CDel(node, s2, srcOrdst);
			}
			else   //�ƶ�ǰ��C+
			{
				addSupedge(node, s1, s2);
			}
		}
		else   //�ƶ�ǰ��C+
		{
			addSupedge(node, s1, s2);
		}
	}
}

void Graph::deleteSupedge(int n1, int S1, int S2) //P,C-��C+
{
	supGraph[S1].erase(S2);
	if (supGraph.at(S1).size() == 0)
	{
		unordered_map<int, map<int, char>>::iterator flag = supGraph.find(S1);
		supGraph.erase(flag);
	}

	supGraph[S2].erase(S1);
	if (supGraph.at(S2).size() == 0)
	{
		unordered_map<int, map<int, char>>::iterator flag = supGraph.find(S2);
		supGraph.erase(flag);
	}
	costCounter = costCounter - 2;

	for (auto j = supList.at(S1).begin(); j != supList.at(S1).end(); ++j)
	{
		if (j->first != n1)
		{
			for (auto k = supList.at(S2).begin(); k != supList.at(S2).end(); ++k)
			{
				if (interDelC.find(j->first) != interDelC.end())
				{
					if (interDelC[j->first].find(k->first) != interDelC[j->first].end())//C-���ڣ�ɾ��
					{
						interDelC[j->first].erase(k->first);
						if (interDelC.at(j->first).size() == 0)
						{
							unordered_map<int, map<int, char>>::iterator flag = interDelC.find(j->first);
							interDelC.erase(flag);
						}

						interDelC[k->first].erase(j->first);
						if (interDelC.at(k->first).size() == 0)
						{
							unordered_map<int, map<int, char>>::iterator flag = interDelC.find(k->first);
							interDelC.erase(flag);
						}

						costCounter = costCounter - 2;
					}
					else//C-�����ڣ����C+
					{
						interAddC[j->first][k->first] = 'a';
						interAddC[k->first][j->first] = 'a';

						costCounter = costCounter + 2;
					}
				}
				else
				{
					interAddC[j->first][k->first] = 'a';
					interAddC[k->first][j->first] = 'a';

					costCounter = costCounter + 2;
				}
			}
		}
		else
		{
			int adj = nodesID.at(n1);

			for (auto k = supList.at(S2).begin(); k != supList.at(S2).end(); ++k)
			{
				if (adjNodes[adj].find(k->first) != adjNodes[adj].end())
				{
					interAddC[n1][k->first] = 'a';
					interAddC[k->first][n1] = 'a';

					costCounter = costCounter + 2;
				}
			}
		}
	}
}

void Graph::remain_CAdd(int n1, int S2, bool srcOrdst) //��ΪC+
{
	if (srcOrdst == false)//����Ŀ�Ľڵ��������س��㱣��C+ʱ������������Ŀ�Ľڵ���Դ�ڵ��C+״̬
	{
		if (moveNode_realEdges.find(S2) != moveNode_realEdges.end())
		{
			for (auto i = moveNode_realEdges[S2].begin(); i != moveNode_realEdges[S2].end(); ++i)
			{
				interAddC[n1][*i] = 'a';
				interAddC[*i][n1] = 'a';

				costCounter = costCounter + 2;
			}
		}
	}
}

void Graph::addSupedge(int n1, int S1, int S2) //C+��P,C-
{
	supGraph[S1][S2] = 'a';
	supGraph[S2][S1] = 'a';
	
	costCounter = costCounter + 2;

	for (auto j = supList.at(S1).begin(); j != supList.at(S1).end(); ++j)
	{
		if (j->first != n1)
		{
			for (auto k = supList.at(S2).begin(); k != supList.at(S2).end(); ++k)
			{
				if (interAddC.find(j->first) != interAddC.end())
				{
					if (interAddC[j->first].find(k->first) != interAddC[j->first].end())//C+���ڣ�ɾ��
					{
						interAddC[j->first].erase(k->first);
						if (interAddC.at(j->first).size() == 0)
						{
							unordered_map<int, map<int, char>>::iterator flag = interAddC.find(j->first);
							interAddC.erase(flag);
						}

						interAddC[k->first].erase(j->first);
						if (interAddC.at(k->first).size() == 0)
						{
							unordered_map<int, map<int, char>>::iterator flag = interAddC.find(k->first);
							interAddC.erase(flag);
						}

						costCounter = costCounter - 2;
					}
					else//C+�����ڣ����C-
					{
						interDelC[j->first][k->first] = 'a';
						interDelC[k->first][j->first] = 'a';

						costCounter = costCounter + 2;
					}
				}
				else
				{
					interDelC[j->first][k->first] = 'a';
					interDelC[k->first][j->first] = 'a';

					costCounter = costCounter + 2;
				}
			}
		}
		else
		{
			int adj = nodesID.at(n1);

			for (auto k = supList.at(S2).begin(); k != supList.at(S2).end(); ++k)
			{
				if (adjNodes[adj].find(k->first) == adjNodes[adj].end())
				{
					interDelC[n1][k->first] = 'a';
					interDelC[k->first][n1] = 'a';

					costCounter = costCounter + 2;
				}
			}
		}
	}
}

void Graph::remain_CDel(int n1, int S2, int srcOrdst) //��ΪC-
{
	if (srcOrdst == false)//����Ŀ�Ľڵ��������س��㱣��C-ʱ������������Ŀ�Ľڵ���Դ�ڵ��C-״̬
	{
		int adj = nodesID.at(n1);
		for (auto i = supList[S2].begin(); i != supList[S2].end(); ++i)
		{
			if (adjNodes[adj].find(i->first) == adjNodes[adj].end())
			{
				interDelC[n1][i->first] = 'a';
				interDelC[i->first][n1] = 'a';

				costCounter = costCounter + 2;
			}
		}
	}	
}

bool Graph::check_Correctness()
{
	/*
		����ÿ���ڵ���ڽӵ�
		����䣺1.���ڳ��ߣ��ж�C-���Ƿ���ڸ��ڽӵ㣬�����ڣ�wrong���������ڣ�right
		        2. �����ڳ��ߣ��ж�C+�Ƿ���ڸ��ڽӵ㣬�����ڣ�right���������ڣ�wrong
		�����ڣ�1.���Ի����ж�C-�У������ڣ�wrong���������ڣ�right
		        2.���Ի����ж�C+�У������ڣ�right���������ڣ�wrong
	*/
	bool flag = false;

	for (auto i = nodeSet.begin(); i != nodeSet.end(); ++i)
	{
		int nbr_i = nodesID.at(*i);
		int locSup_i = nodeToSup.at(*i);
		for (auto j = adjNodes[nbr_i].begin(); j != adjNodes[nbr_i].end(); ++j)
		{
			int locSup_j = nodeToSup.at(*j);
			if (locSup_i != locSup_j)//�����
			{
				if (supGraph.find(locSup_i) != supGraph.end()) //locSup_i���ڳ���
				{
					if (supGraph[locSup_i].find(locSup_j) != supGraph[locSup_i].end())//���ڽӵ�֮���г���
					{
						if (interDelC.find(*i) != interDelC.end())
						{
							if (interDelC[*i].find(*j) != interDelC[*i].end())//C-�д���
							{
								cout << "wrong1, interDelC["<<*i<<"].find("<<*j<<")"<< endl;
								flag = false;
							}
							else
							{
								flag = true;
							}
						}
					}
					else //�޳���
					{
						if (interAddC.find(*i) != interAddC.end())
						{
							if (interAddC[*i].find(*j) != interAddC[*i].end())//C+�д���
							{
								flag = true;
							}
							else
							{
								cout << "wrong2, interAddC[" << *i << "].find(" << *j << ")" << endl;
								flag = false;
							}
						}
					}
				}
				else //locSup_i�����ڳ���
				{
					if (interAddC.find(*i) != interAddC.end())
					{
						if (interAddC[*i].find(*j) != interAddC[*i].end())//C+�д���
						{
							flag = true;
						}
						else
						{
							cout << "wrong3, interAddC[" << *i << "].find(" << *j << ")" << endl;
							flag = false;
						}
					}
				}
			}
			else//������
			{
				if (supLoop.find(locSup_i) != supLoop.end()) //���Ի�
				{
					if (intraDelC.find(*i) != intraDelC.end() || intraDelC.find(*j) != intraDelC.end())
					{
						if (intraDelC[*i].find(*j) != intraDelC[*i].end() || intraDelC[*j].find(*i) != intraDelC[*j].end())//C-�д���
						{
							cout << "wrong4, intraDelC[" << *i << "].find(" << *j << ")" << endl;
							flag = false;
						}
						else
						{
							
							flag = true;
						}
					}
				}
				else  //���Ի�
				{
					if (intraAddC.find(*i) != intraAddC.end() || intraAddC.find(*j) != intraAddC.end())
					{
						if (intraAddC[*i].find(*j) != intraAddC[*i].end() || intraAddC[*j].find(*i) != intraAddC[*j].end())//C+�д���
						{
							flag = true;
						}
						else
						{
							cout << "wrong5, intraAddC[" << *i << "].find(" << *j << ")" << endl;
							flag = false;
						}
					}
				}
			}
		}
	}

	if (flag == false)
	{
		return false;
	}
	else
	{
		return true;
	}
}

float Graph::output() 
{
	float ratio = costCounter/(2 * ecnt);
	//cout << "costCounter:" << costCounter/2 << endl;
	return ratio;
}