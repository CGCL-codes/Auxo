#include <sstream>
#include "./querysupportstruct.h"
int main(){
    char data[100]="../dblp/rawdata.txt";
    char nodeOut[100]="../dblp/nodeOutQueryTest.txt";
    char nodeIn[100]="../dblp/nodeInQueryTest.txt";
    char edge[100]="../dblp/edgeQueryTest.txt";
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
    double AuxoHashTime=0, AuxoMatrixTime=0,
           ComHashTime=0, ComMatrixTime=0,
           GSSHashTime=0, GSSMatrixTime=0;
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

        auxo->insert(from.str(),to.str(),weight,AuxoHashTime,AuxoMatrixTime);
        comauxo->insert(from.str(),to.str(),weight,ComHashTime,ComMatrixTime);
        gsschain->insert(from.str(),to.str(),weight,GSSHashTime,GSSMatrixTime);
        num++;
    }
    cout<<"The over all throughput of Auxo: "<<num/((AuxoHashTime+AuxoMatrixTime)/1e6)<<"/s\n";
    cout<<"The over all throughput of compacted Auxo: "<<num/((ComHashTime+ComMatrixTime)/1e6)<<"/s\n";
    cout<<"The over all throughput of GSS_Chain: "<<num/((GSSHashTime+GSSMatrixTime)/1e6)<<"/s\n";
    cout<<"\n-----------------------------------------------------------------------------------\n\n";
    cout<<"The consumed memory of Auxo: "<<auxo->matrixSize/131672<<" M\n";
    cout<<"The consumed memory of compacted Auxo: "<<comauxo->matrixSize/131672<<" M\n";
    cout<<"The consumed memory of GSS_Chain: "<<gsschain->matrixSize/131672<<" M\n";
    return 0;
}