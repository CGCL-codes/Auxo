#ifndef GS4_H_INCLUDED
#define GS4_H_INCLUDED
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include<map>
#include<cmath>
#include<stdlib.h>
#include<bitset>
#include<memory.h>
#include <sys/time.h>
#include "omp.h"
#include<time.h>
#include <string.h>
#include <math.h>
#include <algorithm>
using namespace std;
typedef long long ll;

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

template <class T1, class T2>
class pqnode{
public:
    T1 val1,val2;
    T2 diff;

    pqnode(T1 _val1, T1 _val2, T2 _diff){
        val1=_val1;
        val2=_val2;
        diff=_diff;
    }
    bool operator < (const pqnode<T1,T2> & other) const{
        return this->diff<other.diff;
    }

    bool operator == (const pqnode<T1,T2> & other) const{
        return ((this->val1==other.val1)||(this->val2==other.val2)||(this->val2==other.val1)||(this->val1==other.val2));
    }
};

template <class T>
class heap{
private:
    vector<T> nodes;
    //record the size of the nodes
    int pq_size;
public:
    void clear(){
        nodes.clear();
        pq_size=0;
    }
    heap(int _size){
        nodes=vector<T>(_size);
        pq_size=0;
    }
    heap(){
        pq_size=0;
    }
    void heapSubTree(int k){
        int n=pq_size-1;
        while((2*k+1)<=n){
            int temp=2*k+1;
            if((2*k+2)<=n&&nodes[2*k+2]<nodes[2*k+1]){
                temp=2*k+2;
            }
            if(nodes[temp]<nodes[k]){
                T t=nodes[temp];
                nodes[temp]=nodes[k];
                nodes[k]=t;
                k=temp;
            }
            else break;
        }
    }
    int size(){
        return pq_size;
    }
    void push(T node){
        if(nodes.size()==pq_size){
            nodes.emplace_back(node);
        }
        else{
            nodes[pq_size]=node;
        }
        pq_size++;
        int temp=(pq_size-2)/2,child=pq_size-1;
        while(temp>=0){
            if(nodes[child]<nodes[temp]){
                T t=nodes[child];
                nodes[child]=nodes[temp];
                nodes[temp]=t;
                child=temp;
                temp=(temp-1)/2;
            }
            else break;
        }
    }

    void pop(){
        if(pq_size==0)return ;
        nodes[0]=nodes[pq_size-1];
        pq_size--;
        heapSubTree(0);
    }
    T top(){
        return nodes[0];
    }
    bool isEmpty(){
        return (pq_size==0);
    }

    void remove(int k){
        int n=--pq_size;
        while((2*k+1)<=n){
            int temp=2*k+1;
            if((2*k+2)<=n&&nodes[2*k+2]<nodes[2*k+1]){
                temp=2*k+2;
            }
            nodes[k]=nodes[temp];
            k=temp;
        }
        nodes[k]=nodes[pq_size];
    }

    void del(T node){
        int pos=0;
        while(pos<pq_size){
            if(node==nodes[pos]){
                remove(pos);
            }
            else{
                pos++;
            }
        }
    }
};

struct csr_node{
    string nei;
    int next;
};
class graph {
public:
    unordered_map<string,int> node_head;
    vector<csr_node> csr;

    heap<pqnode<int,double>> pq;

    unordered_map<int, vector<string>> super_map;
    unordered_map<string,int> node_group;

    graph(){
    }
    void insert_edge(string src,string dst){
        csr_node new_node = csr_node();
        new_node.nei=dst;
        if(node_head.find(src)==node_head.end()){
            node_head[src]=csr.size();
            new_node.next=-1;
        }
        else{
            new_node.next=node_head[src];
            node_head[src]=csr.size();
        }
        csr.emplace_back(new_node);
    }
    void get_nei(string root,vector<string>& nei, graph & gra){
        if(gra.node_head.find(root)==gra.node_head.end())return;
        int now_nei=gra.node_head[root];
        while(now_nei!=-1){
            nei.push_back(gra.csr[now_nei].nei);
            now_nei=gra.csr[now_nei].next;
        }
    }

    void calE(vector<string>& nei, vector<double>& e, graph & gra){
        double cnt=nei.size();
        unordered_map<int,int> mp;
        for(string tmp:nei){
            mp[gra.node_group[tmp]]++;
        }
        for(auto it=mp.begin();it!=mp.end();it++){
            e.push_back(it->second/cnt);
        }
    }

    //calculate the difference of two super nodes
    double diffCal(int src,int dst, graph & gra1,graph & gra2){
        vector<string> nei1,nei2;
        for(string tmp:super_map[src]){
            get_nei(tmp,nei1,gra1);
        }
        for(string tmp:super_map[dst]){
            get_nei(tmp,nei2,gra2);
        }
        int n1=super_map[src].size(),n2=super_map[dst].size();
        int d1=nei1.size(),d2=nei2.size();
        vector<double> e1,e2;
        calE(nei1,e1,gra1);
        calE(nei2,e2,gra2);
        double res=abs(n1-n2)/(double)max(n1,n2)+abs(d1-d2)/(double)max(d1,d2);
        int len=max(e1.size(),e2.size());
        double tmp1,tmp2;
        for(int i=0;i<len;i++){
            tmp1=0;tmp2=0;
            if(i<e1.size())tmp1=e1[i];
            if(i<e2.size())tmp2=e2[i];
            res+=(tmp1>tmp2? (tmp1-tmp2):(tmp2-tmp1));
        }
        return res;
    }

    void clear_summarize(){
        pq.clear();
        super_map.clear();
        node_group.clear();
    }

    void summarize(int target_size){
        clear_summarize();
        cout<<"\nprocess bar: ";
        int node_size=node_head.size(),super_node_count=0;
        int interval=(node_size-target_size)/10,cnt=0;
        //record a super node's children
        auto it=node_head.begin();
        while(it!=node_head.end()){
            vector<string> children;
            children.emplace_back(it->first);
            super_map[super_node_count]=children;
            node_group[it->first]=super_node_count;
            super_node_count++;
            it++;
        }
        for(int i=0;i<super_node_count;i++){
            for(int j=i+1;j<super_node_count;j++){
                pq.push(pqnode<int,double>(i,j,diffCal(i,j,*this,*this)));
            }
        }
        int src,dst;
        while(node_size>target_size){
            src=pq.top().val1;
            dst=pq.top().val2;
            pq.pop();
            pq.del(pqnode<int,double>(src,dst,-1));

            //update the super_map and the node_group
            vector<string> new_super_node_nei;
            for(string tmp:super_map[src]){
                new_super_node_nei.emplace_back(tmp);
                node_group[tmp]=super_node_count;
            }
            for(string tmp:super_map[dst]){
                new_super_node_nei.emplace_back(tmp);
                node_group[tmp]=super_node_count;
            }

            super_map.erase(src);
            super_map.erase(dst);

            //update the difference heap
            for(auto it=super_map.begin();it!=super_map.end();it++){
                pq.push(pqnode<int,double>(it->first,super_node_count,diffCal(it->first,super_node_count,*this,*this)));
            }
            super_map[super_node_count]=new_super_node_nei;

            super_node_count++;
            node_size--;
            cnt++;
            if(cnt%interval==0)cout<<"#";
        }
        cout<<endl;
    }

    void print(string out){
        ofstream out1(out);
        for(auto it=super_map.begin();it!=super_map.end();it++){
            out1<<it->first<<": ";
            for(string tmp:it->second)out1<<tmp<<" ";
            out1<<endl;
            out1.flush();
        }
        out1.close();
    }

    double calDiff2Graph(graph & other){
        heap<pqnode<int,double>> diff_pair;
        unordered_map<int, vector<string>> mp1=this->super_map;
        unordered_map<int, vector<string>> mp2=other.super_map;
        for(auto itout=mp1.begin();itout!=mp1.end();itout++){
            for(auto it=mp2.begin();it!=mp2.end();it++){
                diff_pair.push(pqnode<int,double>(itout->first,it->first,diffCal(itout->first,it->first,*this,other)));
            }
        }
        double res=0;
        while(!diff_pair.isEmpty()){
            res+=diff_pair.top().diff;
            diff_pair.del(pqnode<int,double>(diff_pair.top().val1,diff_pair.top().val2,-1));
            diff_pair.pop();
        }
        return res;
    }
};


class gs4{
private:
    graph ref_graph,now_graph;
    int interval,super_node_count;
    double diff_threshold;
    ll edge_count;
    string result_dir;
public:
    gs4(int _interval,int _super_node_count,double _diff_threshold, string _result_dir){
        ref_graph=graph();
        now_graph=graph();
        interval=_interval;
        super_node_count=_super_node_count;
        diff_threshold=_diff_threshold;
        edge_count=0;
        result_dir=_result_dir;
    }
    void insert(string src, string dst){
        if(edge_count<interval){
            ref_graph.insert_edge(src,dst);
            ref_graph.insert_edge(dst,src);
        }
        else if(edge_count==interval){
            ref_graph.summarize(super_node_count);
        }
        else{
            now_graph.insert_edge(src,dst);
            now_graph.insert_edge(dst,src);
            if(edge_count%interval==0){
                now_graph.summarize(super_node_count);
                if(ref_graph.calDiff2Graph(now_graph)>diff_threshold){
                    ref_graph=now_graph;
                }
                now_graph=graph();
            }
            //print("./"+result_dir+"_"+to_string(edge_count));
        }
        edge_count++;
    }
    void print(string out){
        ref_graph.print(out);
    }
};


#endif // GS4_H_INCLUDED
