#include "querysupportstruct.h"
using namespace std;
edgeval::edgeval(string _from,string _to,unsigned int _val){
    from=_from;to=_to;val=_val;
}

bool edgeval::operator < (const edgeval& b)const
{
    return val<b.val;
}

void heap::heapSubTree(int k,int n){
    while((2*k+1)<=n){
        int temp=2*k+1;
        if((2*k+2)<=n&&(*num[2*k+2])<(*num[2*k+1])){
            temp=2*k+2;
        }
        if((*num[temp])<(*num[k])){
            edgeval *t=num[temp];
            num[temp]=num[k];
            num[k]=t;
            k=temp;
        }
        else break;
    }
}

void heap::initialHeap(int l){
    s=l;
    num=new edgeval*[s];
    for(int i=0;i<s;i++)num[i]=new edgeval("null","null",0);
    for(int j=(s-2)/2;j>=0;j--){
        heapSubTree(j,s-1);
    }
}

int heap::find(string s1,string s2){
    for(int i=0;i<s;i++){
        if(num[i]->from==s1&&num[i]->to==s2){
            return num[i]->val;
        }
    }
    return -1;
}

heap::~heap(){
    for(int i=0;i<s;i++){
        delete num[i];
    }
}

void heap::pop(){
    delete num[0];
    num[0]=num[s-1];
    s--;
    heapSubTree(0,s-1);
}

void heap::push(string s1,string s2,unsigned int weight){
    num[s]=new edgeval(s1,s2,weight);s++;
    int temp=(s-2)/2,child=s-1;
    while(temp>=0){
        if((*num[child])<(*num[temp])){
            edgeval *t=num[child];
            num[child]=num[temp];
            num[temp]=t;
            child=temp;
            temp=(temp-1)/2;
        }
        else break;
    }
}

bool heap:: increase(string s1,string s2,unsigned int weight){
    for(int i=0;i<s;i++){
        if(num[i]->from==s1&&num[i]->to==s2){
            int now=i;
            num[now]->val=weight;
            heapSubTree(now,s-1);
            return true;
        }
    }
    return false;
}

unsigned int heap:: top(){
    return num[0]->val;
}

void heap::topres(string &s1,string &s2){
    s1=num[0]->from;
    s2=num[0]->to;
}

bool heap::empty(){
    if(!s)return true;
    return false;
}

int heap::size(){
    return s;
}

Blizzard_MPQ::Blizzard_MPQ(){
    ul seed =0x00100001, index1 =0, index2 =0, i;
    for(index1 = 0; index1 < 0x100; ++index1){
        for(index2 = index1, i = 0; i < 5; ++i, index2 += 0x100){
          ul temp1, temp2;
          seed = (seed * 125 + 3) % 0x2AAAAB;
          temp1 = (seed & 0xFFFF) << 0x10;
          seed = (seed * 125 + 3) % 0x2AAAAB;
          temp2 = (seed & 0xFFFF);
          MagicTable[index2] = ( temp1 | temp2 );
        }
    }
}

ul Blizzard_MPQ::MPQHash(const char* str, ul HashType){
      uc *key = (uc*)str;
      ul seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
      int ch;
      while(*key != 0){
        ch = toupper(*key++);
        seed1 = MagicTable[(HashType <<8) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) +3;
      }
      return seed1;
}


Gbasket::Gbasket(uint64* _srcFig,uint64*_dstFig,uint64* _idx,weightSize* _weight){
    srcFig=_srcFig;dstFig=_dstFig;
    idx=_idx;
    weight=_weight;
}

Gbasket::~Gbasket(){
    delete [] dstFig;
    delete [] srcFig;
    delete [] idx;
    delete [] weight;
}

basketNode::basketNode(){
    for(int i=0;i<2;i++){
        next[i]=NULL;
    }
    value=NULL;
    bound=0;
    level=0;
}

basketNode::~basketNode(){
    delete value;
}

ul calHash(ul tmp_g,ul tmp_s,int k,int w){
    ul shifter = tmp_g;
    for (int v = 0; v < tmp_s; v++)
        shifter = (shifter*timer + prime) % bigger_p;
    ul tmp_h = k;
    ul left=0,right=(shifter/w)+1;
    while(left<right){
        ul mid=(left+right)>>1;
        if(tmp_h+w*mid<shifter){
            left=mid+1;
        }
        else right=mid;
    }
    tmp_h+=w*left;
    tmp_h -= shifter;
    //cout<<"i\n";
    return tmp_h;
}


bool mycomp(const hashvalue &hv1, const hashvalue &hv2)
{
	return hv1.key<hv2.key;
}
bool operator==(const hashvalue &hv1, const hashvalue &hv2)
{
	return hv1.key==hv2.key;
}
int countjoin( vector<hashvalue> &V1, vector<hashvalue> &V2)
{
 int i1=0,i2=0;
 int count=0;
 while(i1<V1.size())
 {
 	if (i2>=V2.size()) return count;
 	while(V2[i2].key<V1[i1].key)
 	{
 		i2++;
 		if (i2>=V2.size()) return count;
 	}
 	if(V2[i2].key==V1[i1].key)
 	{
 		count+=V1[i1].IDnum;
 	    i1++;
 		i2++;
 		continue;
	 }
	else if(V2[i2].key>V1[i1].key)
	{
 			i1++;
	}
  }
  return count;
}

bool comp_degree(const degree_node &dn1, const degree_node &dn2)
{
	return dn1.degree<dn2.degree;
}

void GSketch::insert_node(unsigned int h1, int IDnum)
{
    if(nodeSet.find(h1)!=nodeSet.end())return;
    nodeSet.insert(h1);
	index[h1]=nodenum;
	degree[h1]=0;
	idnums[h1]=IDnum;
	GSnode* np=new GSnode;
	np->h=h1;
	np->next=NULL;
	sketch.push_back(np);
	nodenum++;
}
void GSketch::insert_edge(unsigned int h1, unsigned int h2)
{
	degree[h1]++;
	degree[h2]++;
	GSnode* np = sketch[index[h1]];
	for(;np!=NULL;np=np->next)
	{
		if(np->h==h2)
			break;
		if(np->next==NULL)
		{
			GSnode* newn=new GSnode;
			newn->h=h2;
			newn->next=NULL;
			np->next=newn;
			edge_num++;
			break;
		}
	}
	np = sketch[index[h2]];
	for(;np!=NULL;np=np->next)
	{
		if(np->h==h1)
			break;
		if(np->next==NULL)
		{
			GSnode* newn=new GSnode;
			newn->h=h1;
			newn->next=NULL;
			np->next=newn;
			edge_num++;
			break;
		}
	}
}
void GSketch::clean()
{
	map<unsigned int, int> sequence;
	map<unsigned int, int>::iterator it;
	for(it=degree.begin();it!=degree.end();it++)
	{
		degree_node dn;
		dn.h=it->first;
		dn.degree=it->second;
		sorted_nodes.push_back(dn);
	}
	sort(sorted_nodes.begin(),sorted_nodes.end(),comp_degree);
	for(int i=0;i<nodenum;i++)
		sequence[sorted_nodes[i].h]=i;
	for(int i=0;i<nodenum;i++)
	{
		GSnode* np=sketch[i];
		int seq_d=sequence[np->h];
		GSnode* parent=np;
		np=np->next;
		while(np!=NULL)
		{
			if(sequence[np->h]<=seq_d)
			{
				parent->next=np->next;
				delete np;
				np=parent->next;
				edge_num--;
			}
			else
			{
				parent=np;
				np=np->next;
			}
		}
	}
 }
 void GSketch::get_neighbors(unsigned int h, vector<hashvalue>& hv)
 {
 	map<unsigned int, int>::iterator it;
 	it=index.find(h);
 	if(it==index.end()) return;
 	GSnode* np=sketch[it->second];
 	np=np->next;
 	for(;np!=NULL;np=np->next)
 	{
 	//	cout<<"in"<<endl;
 		if(np->h==h)
 			cout<<"loop!"<<endl;
 		hashvalue new_hv;
 		new_hv.key=np->h;
 		new_hv.IDnum=idnums[np->h];
 		hv.push_back(new_hv);
	 }
	 return;
  }
  int GSketch::countTriangle()
  {
  	int count=0;
  	for(int i=0;i<nodenum;i++)
  	{
  		unsigned int h1=sorted_nodes[i].h;
  		int num1=idnums[h1];
  		vector<hashvalue> set1;
  		get_neighbors(h1, set1);
  		sort(set1.begin(),set1.end(),mycomp);
  	//	cout<<set1.size()<<endl;
  		for(int j=0;j<set1.size();j++)
  		{
  			unsigned int h2=set1[j].key;
  			int num2=set1[j].IDnum;
  			vector<hashvalue> set2;
  			get_neighbors(h2,set2);
  			sort(set2.begin(),set2.end(),mycomp);
  			int num3=countjoin(set1,set2);
  		//	cout<<num1<<' '<<num2<<' '<<num3<<endl;
  			count+=num1*num2*num3;
		  }
	  }
	  return count;
  }

