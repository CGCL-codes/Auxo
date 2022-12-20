 #include<iostream>
#include <fstream>
#include <time.h>
#include<string>
#include<sstream>
#include<cmath>
#include "graph.h"
#include <sys/time.h>
//#include <sys/time.h>

using namespace std;

// ʵʱ��ȡ����ռ�õ��ڴ棬��λ��kb��
 size_t physical_memory_used_by_process()
 {
     FILE* file = fopen("/proc/self/status", "r");
     int result = -1;
     char line[128];

     while (fgets(line, 128, file) != nullptr) {
         if (strncmp(line, "VmSize:", 6) == 0) {
             int len = strlen(line);

             const char* p = line;
             for (; std::isdigit(*p) == false; ++p) {}

             line[len - 3] = 0;
             result = atoi(p);

             break;
         }
     }

     fclose(file);

     return result;
 }

vector<string> SplitStr(const string& str) {
	vector<string> result;
	string tmp_str;
	int i = 0;
	for (; i < str.size(); i++) {
		if (str[i] == '\t' || str[i] == ' ' || str[i] == 0x20) {
			if (tmp_str.empty())
				continue;
			result.push_back(tmp_str);
			tmp_str.clear();
		}
		else {
			tmp_str += str[i];
		}
	}

	if (!tmp_str.empty())
		result.push_back(tmp_str);
	return result;
}

/**
 * ��ȡ�ı��ļ��ض��е����ݽ��д���
 * @param b ��ʼ�к�
 * @param e �����к�
 * @param F ����������Ա������
 * @param g F������������
 * @return
 */
template<typename T>
void Process(std::string file_name, int b, int e, void(T::* F)(int, int), T& g)
{
	ifstream file(file_name);
	int index = 0;
	string line;
	if (file)
	{
		while (getline(file, line))
		{
			if (index >= b && index <= e)
			{
				auto result = SplitStr(line);    //�����ַ����ָ��
				int a = atoi(result[0].c_str());
				int b = atoi(result[1].c_str());
				//cout << "( " << a << ", " << b << " )" << endl;
				if (a != b) //�����Ի���u,u��
				{
					(g.*F)(a, b);
				}
			}
			index++;
			if (index > e)
			{
				break;
			}
		}
	}

	//file.close();//������ر��ļ�������getline����Ŷ���
}


int main(int argc, char* argv[]) {
	Graph graph = Graph();
    std:string file_name,result_name,time_name;
    int test = 1;//1��1+2��2��3+2��3��3+4(ֱ�����ȡrefined candidate supernodes)��4��3+5(��ֵ-ȡ���)��5��3+6(��ֵ-���ȡ)��6��KDD
    int interval = 1000;
    // command line parameters
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-dataset") == 0) {
            file_name = argv[++i];
        }
        if (strcmp(argv[i], "-result") == 0) {
            result_name = argv[++i];
        }
        if (strcmp(argv[i], "-interval") == 0) {
            interval = atoi(argv[++i]);;
        }
        if (strcmp(argv[i], "-strategy") == 0) {
            test = atoi(argv[++i]);
        }
    }
    cout<<"dataset: "<<file_name<<endl;
    cout<<"interval: "<<interval<<endl;
    cout<<"strategy "<<test<<endl;
    cout<<"result: "<<result_name<<endl;

    time_t t1, t2;
    ifstream file(file_name, ios::in);    //��ȡ�ļ�Ŀ¼


	//timeval micro_seed;
	//gettimeofday(&micro_seed, NULL);
	//srand(micro_seed.tv_usec * micro_seed.tv_sec); //��ʼ��΢�뼶ʱ������

	string line;
	vector<string> result;
	int beginline2 = 1;
	int endline2 = 3782447;
	int last_result = 0;
	long iteration = 0;
	double x = 2, y = 1;//xΪ������yΪָ��
	struct timeval step1;//����һ���ߵĿ�ʼʱ��
	struct timeval step2;//����һ���ߵĽ���ʱ��
	double alltime = 0;
	t1 = time(NULL);

    ofstream out1(result_name);

    cout<<"we are testing strategy "<<test<<endl;
	if (test == 1)
	{
		if (file)
		{
			while (getline(file, line))
			{
				//if (index >= beginline2 && index <= endline2)// && line[0] != '#'
				{
					//cout << line << endl;
					result = SplitStr(line);    //�����ַ����ָ��
					//cout << result[0] << "   " << result[1] << endl;
					int a = atoi(result[0].c_str());
					int b = atoi(result[1].c_str());
					//int chkedge = graph.checkEdge(a, b);
					//if (chkedge != 1 && a != b)  //�����±ߣ������Ի����ظ���
					iteration = iteration + 1;

					gettimeofday(&step1, NULL);
					graph.addEdgeToSupG1(a, b);
					graph.changedSimilar1();
					graph.traverseReCaleSimi1();
					gettimeofday(&step2, NULL);
					alltime = (step2.tv_sec-step1.tv_sec)*1000000.0 +
                              (step2.tv_usec-step1.tv_usec) + alltime;

					//int current_result= graph.printEndResult();  //����1��ѹ������������1�����
					//if (current_result-1 > last_result)
					//{
					//	cout << "addedge ( " << a << "," << b << ")" << endl;
					//	cout << "current_result:" << current_result << endl;
					//	cout << "last_result:" << last_result << endl;
					//	cout << "wrong;;;" << endl;
					//	break;
					//}
					//last_result = current_result;

					/*if (iteration == pow(x, y))
					{
						t2 = time(NULL);
						y = y + 1;
						out2 << t2 - t1 << endl;
					}*/
					if (iteration % interval == 0)
					{
                        out1<<iteration<<" ";
                        out1<<"memory cost: "<<physical_memory_used_by_process()/1024.0<<"M";
                        out1<<" update time: "<<alltime/interval<<" us\n";
                        out1.flush();
                        alltime=0;
					}
				}
			}
		}
	}
	else if (test == 2)
	{
        if (file)
        {
            while (getline(file, line))
            {
                //if (index >= beginline2 && index <= endline2)// && line[0] != '#'
                {
                    //cout << line << endl;
                    result = SplitStr(line);    //�����ַ����ָ��
                    //cout << result[0] << "   " << result[1] << endl;
                    int a = atoi(result[0].c_str());
                    int b = atoi(result[1].c_str());
                    //int chkedge = graph.checkEdge(a, b);
                    //if (chkedge != 1 && a != b)  //�����±ߣ������Ի����ظ���
                    iteration = iteration + 1;

                    gettimeofday(&step1, NULL);
                    graph.addEdgeToSupG1(a, b);
                    graph.changedSimilar2();
                    graph.traverseReCaleSimi1();
                    gettimeofday(&step2, NULL);
                    alltime = (step2.tv_sec-step1.tv_sec)*1000000.0 +
                              (step2.tv_usec-step1.tv_usec) + alltime;

                    //int current_result= graph.printEndResult();  //����1��ѹ������������1�����
                    //if (current_result-1 > last_result)
                    //{
                    //	cout << "addedge ( " << a << "," << b << ")" << endl;
                    //	cout << "current_result:" << current_result << endl;
                    //	cout << "last_result:" << last_result << endl;
                    //	cout << "wrong;;;" << endl;
                    //	break;
                    //}
                    //last_result = current_result;

                    /*if (iteration == pow(x, y))
                    {
                        t2 = time(NULL);
                        y = y + 1;
                        out2 << t2 - t1 << endl;
                    }*/
                    if (iteration % interval == 0)
                    {
                        out1<<iteration<<" ";
                        out1<<"memory cost: "<<physical_memory_used_by_process()/1024.0<<"M";
                        out1<<" update time: "<<alltime/interval<<" us\n";
                        out1.flush();
                        alltime=0;
                    }
                }
            }
        }
	}

	else if (test == 3)
	{
		if (file)
		{
			while (getline(file, line))
			{
                //if (index >= beginline2 && index <= endline2)// && line[0] != '#'
                {
                    //cout << line << endl;
                    result = SplitStr(line);    //�����ַ����ָ��
                    //cout << result[0] << "   " << result[1] << endl;
                    int a = atoi(result[0].c_str());
                    int b = atoi(result[1].c_str());
                    //int chkedge = graph.checkEdge(a, b);
                    //if (chkedge != 1 && a != b)  //�����±ߣ������Ի����ظ���
                    iteration = iteration + 1;

                    gettimeofday(&step1, NULL);
                    graph.addEdgeToSupG(a, b);
                    graph.changedSimilar2();
                    graph.traverseReCaleSimi5();
                    gettimeofday(&step2, NULL);
                    alltime = (step2.tv_sec-step1.tv_sec)*1000000.0 +
                              (step2.tv_usec-step1.tv_usec) + alltime;

                    //int current_result= graph.printEndResult();  //����1��ѹ������������1�����
                    //if (current_result-1 > last_result)
                    //{
                    //	cout << "addedge ( " << a << "," << b << ")" << endl;
                    //	cout << "current_result:" << current_result << endl;
                    //	cout << "last_result:" << last_result << endl;
                    //	cout << "wrong;;;" << endl;
                    //	break;
                    //}
                    //last_result = current_result;

                    /*if (iteration == pow(x, y))
                    {
                        t2 = time(NULL);
                        y = y + 1;
                        out2 << t2 - t1 << endl;
                    }*/
                    if (iteration % interval == 0)
                    {
                        out1<<iteration<<" ";
                        out1<<"memory cost: "<<physical_memory_used_by_process()/1024.0<<"M";
                        out1<<" update time: "<<alltime/interval<<" us\n";
                        out1.flush();
                        alltime=0;
                    }
                }
			}
		}
	}

	else if (test == 4)
	{
		if (file)
		{
			while (getline(file, line))
			{
                {
                    //cout << line << endl;
                    result = SplitStr(line);    //�����ַ����ָ��
                    //cout << result[0] << "   " << result[1] << endl;
                    int a = atoi(result[0].c_str());
                    int b = atoi(result[1].c_str());
                    //int chkedge = graph.checkEdge(a, b);
                    //if (chkedge != 1 && a != b)  //�����±ߣ������Ի����ظ���
                    iteration = iteration + 1;

                    gettimeofday(&step1, NULL);
                    graph.addEdgeToSupG(a, b);
                    graph.changedSimilar3();
                    graph.traverseReCaleSimi3();
                    gettimeofday(&step2, NULL);
                    alltime = (step2.tv_sec-step1.tv_sec)*1000000.0 +
                              (step2.tv_usec-step1.tv_usec) + alltime;

                    //int current_result= graph.printEndResult();  //����1��ѹ������������1�����
                    //if (current_result-1 > last_result)
                    //{
                    //	cout << "addedge ( " << a << "," << b << ")" << endl;
                    //	cout << "current_result:" << current_result << endl;
                    //	cout << "last_result:" << last_result << endl;
                    //	cout << "wrong;;;" << endl;
                    //	break;
                    //}
                    //last_result = current_result;

                    /*if (iteration == pow(x, y))
                    {
                        t2 = time(NULL);
                        y = y + 1;
                        out2 << t2 - t1 << endl;
                    }*/
                    if (iteration % interval == 0)
                    {
                        out1<<iteration<<" ";
                        out1<<"memory cost: "<<physical_memory_used_by_process()/1024.0<<"M";
                        out1<<" update time: "<<alltime/interval<<" us\n";
                        out1.flush();
                        alltime=0;
                    }
                }
			}
		}
	}

	else if (test == 5)
	{
		if (file)
		{
			while (getline(file, line))
			{
                {
                    //cout << line << endl;
                    result = SplitStr(line);    //�����ַ����ָ��
                    //cout << result[0] << "   " << result[1] << endl;
                    int a = atoi(result[0].c_str());
                    int b = atoi(result[1].c_str());
                    //int chkedge = graph.checkEdge(a, b);
                    //if (chkedge != 1 && a != b)  //�����±ߣ������Ի����ظ���
                    iteration = iteration + 1;

                    gettimeofday(&step1, NULL);
                    graph.addEdgeToSupG(a, b);
                    graph.changedSimilar3();
                    graph.traverseReCaleSimi4();
                    gettimeofday(&step2, NULL);
                    alltime = (step2.tv_sec-step1.tv_sec)*1000000.0 +
                              (step2.tv_usec-step1.tv_usec) + alltime;

                    //int current_result= graph.printEndResult();  //����1��ѹ������������1�����
                    //if (current_result-1 > last_result)
                    //{
                    //	cout << "addedge ( " << a << "," << b << ")" << endl;
                    //	cout << "current_result:" << current_result << endl;
                    //	cout << "last_result:" << last_result << endl;
                    //	cout << "wrong;;;" << endl;
                    //	break;
                    //}
                    //last_result = current_result;

                    /*if (iteration == pow(x, y))
                    {
                        t2 = time(NULL);
                        y = y + 1;
                        out2 << t2 - t1 << endl;
                    }*/
                    if (iteration % interval == 0)
                    {
                        out1<<iteration<<" ";
                        out1<<"memory cost: "<<physical_memory_used_by_process()/1024.0<<"M";
                        out1<<" update time: "<<alltime/interval<<" us\n";
                        out1.flush();
                        alltime=0;
                    }
                }
			}
		}
	}
    else{
        cout<<"no such a strategy\n";
    }
    out1.close();

	//t2 = time(NULL);
	//alltime = t2 - t1;
	//cout << "time:" << alltime << endl;
	int allEdges=graph.printEndResult();
	cout <<" allEdges: "<< allEdges << endl;

	/*cout << "--time_step1--" << time_step1<< endl;
	cout << "--time_step2--" << time_step2 << endl;
	cout << "--time_step3--" << time_step3 << endl;*/

	//bool verify_result = graph.check_Correctness();  //����2��ѹ��ͼ�ع��Ա�original graph
	//cout << "verify_result:" << verify_result << endl;
}
