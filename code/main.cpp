#include <iostream>
#include "querysupportstruct.h"
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <set>
using namespace std;
int main()
{
    char data[100]="../data/lkml/rawdata.txt";
    char nodeOut[100]="../data/lkml/nodeOutQueryTest.txt";
    char nodeIn[100]="../data/lkml/nodeInQueryTest.txt";
    char edge[100]="../data/lkml/edgeQueryTest.txt";
    Auxo *auxo=new Auxo(126,16,80,15);//side width of the matrix, length of the hash address, number of the candidate bucket, length of the fingerprint
    ComAuxo *comauxo=new ComAuxo(126,16,80,15);
    GSS_Chain *gsschain=new GSS_Chain(126,16,80,15);
    int num=0;
    FILE *fp;
    fp = fopen(data, "r");
    if(fp == NULL)
    {
        printf("cannot open file1!\n");
        return -1;
    }
    double AuxoMatrixTime=0,
           ComMatrixTime=0,
           GSSMatrixTime=0;
    cout<<"Insertion starts\n";
    while(!feof(fp))
    {
        unsigned int temp1,temp2,weight,timeStamp;
        fscanf(fp, "%d ", &temp1);
        fscanf(fp, "%d ", &temp2);
        fscanf(fp, "%d ", &weight);
        fscanf(fp, "%d", &timeStamp);
        ostringstream from,to;
        from<<temp1;to<<temp2;

        auxo->insert(from.str(),to.str(),weight,AuxoMatrixTime);
        comauxo->insert(from.str(),to.str(),weight,ComMatrixTime);
        gsschain->insert(from.str(),to.str(),weight,GSSMatrixTime);
        num++;
    }
    cout<<"The over all throughput of Auxo: "<<num/(AuxoMatrixTime/1e6)<<"/s\n";
    cout<<"The over all throughput of compacted Auxo: "<<num/(ComMatrixTime/1e6)<<"/s\n";
    cout<<"The over all throughput of GSS_Chain: "<<num/(GSSMatrixTime/1e6)<<"/s\n";
    cout<<"\n-----------------------------------------------------------------------------------\n\n";
    cout<<"The consumed memory of fingerprints for Auxo: "<<(double)(auxo->memoryAllocated2)/131672<<" M\n";
    cout<<"The consumed memory of fingerprints for compacted Auxo: "<<(double)(comauxo->memoryAllocated2)/131672<<" M\n";
    cout<<"The consumed memory of fingerprints for GSS_Chain: "<<(double)(gsschain->memoryAllocated2)/131672<<" M\n";
    return 0;
}
