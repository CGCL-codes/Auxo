#include "querysupportstruct.h"
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <set>
using namespace std;
int main(int argc, char* argv[])
{
    string dataset="../../data/lkml/rawdata.txt";
    string nodeOut="../../data/lkml/nodeOutQueryTest.txt";
    string nodeIn="../../data/lkml/nodeInQueryTest.txt";
    string edge="../../data/lkml/edgeQueryTest.txt";
    string result="../result/lkml.txt";

    int fpl=16,cols=4,candiNum=16,width=100;

    for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-dataset") == 0) {
			dataset = argv[++i];
		}
		if (strcmp(argv[i], "-result") == 0) {
			result = argv[++i];
		}
		if (strcmp(argv[i], "-fpl") == 0) {
			fpl = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-width") == 0) {
			width = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-cols") == 0) {
			cols = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-candiNum") == 0) {
			candiNum = atoi(argv[++i]);
		}
    }
    cout<<"The parameters are shown as below:\n"<<"dataset: "<<dataset<<" result: "<<result<<" cols: "<<cols<<" candiNum: "<<candiNum<<" fpl: "<<fpl<<endl;

    Auxo *auxo=new Auxo(width,cols,candiNum,fpl);//side width of the matrix, length of the hash address, number of the candidate bucket, length of the fingerprint
    AuxoPro *auxopro=new AuxoPro(width,cols,candiNum,fpl);
    GSS_Chain *gsschain=new GSS_Chain(width,cols,candiNum,fpl);
    int num=0;

    ofstream out1(result);

    ifstream ifs;
    ifs.open(dataset);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << dataset << endl;
		return -1;
	}

	int64_t s, d;
    int w;
    unsigned int t;
    double AuxoMatrixTime=0,
           ComMatrixTime=0,
           GSSMatrixTime=0;
    cout<<"Insertion starts\n";
    while(!ifs.eof())
    {
        ifs >> s >> d >> w >> t;

        auxo->insert(to_string(s), to_string(d),w,AuxoMatrixTime);
        auxopro->insert(to_string(s), to_string(d),w,ComMatrixTime);
        gsschain->insert(to_string(s), to_string(d),w,GSSMatrixTime);
        num++;
    }
    out1<<"The over all throughput of Auxo: "<<num/(AuxoMatrixTime/1e6)<<"/s\n";
    out1<<"The over all throughput of proportional Auxo: "<<num/(ComMatrixTime/1e6)<<"/s\n";
    out1<<"The over all throughput of GSS_Chain: "<<num/(GSSMatrixTime/1e6)<<"/s\n";
    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<"The consumed memory of fingerprints for Auxo: "<<(double)(auxo->memoryAllocated2)/131072<<" M\n";
    out1<<"The consumed memory of fingerprints for proportional Auxo: "<<(double)(auxopro->memoryAllocated2)/131072<<" M\n";
    out1<<"The consumed memory of fingerprints for GSS_Chain: "<<(double)(gsschain->memoryAllocated2)/131072<<" M\n";
    return 0;
}
