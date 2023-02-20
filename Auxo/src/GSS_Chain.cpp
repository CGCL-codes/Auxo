#include "querysupportstruct.h"
using namespace std;
GSS_Chain::GSS_Chain(int width, int range, int p_num,int f_num)
{
    checkMatrices=vector<basketNode*>(1500);
    posfind=vector<int>(1500);
    weiRes=vector<int>(1500);
    matrixLevel=vector<int>(200);
    memoryAllocated1=0;memoryAllocated2=0;memoryAllocated3=0;
    fullCount=0;
    loadFactor=0;
    mpq=new Blizzard_MPQ();
    valueCou=1;
	w = width;
	r = range; /* r x r mapped baskets */
	p = p_num; /*candidate buckets*/
	f = f_num; /*finger print length*/
	rfnum=2*ceil(log(r)/log(2));//the number of bit index need
	halfR=rfnum/2;
	rper=64/rfnum;//the number of index a uint64 can contain
	matrixSize=w*w;
	fvalue=new unsigned int[f+1];
	fper=new int[f+1];
	for(int i=1;i<=f;i++){
        fvalue[i]=((0x1<<i)-1);
        fper[i]=64/i;
	}

	valueTree=new basketNode();
	valueTree->value=initialGbasket(f,f);
}
void GSS_Chain::freeValueTree(basketNode* root){
    if(root){
        freeValueTree(root->next[0]);
        delete root;
    }
}
Gbasket* GSS_Chain::initialGbasket(int fsize1,int fsize2){
    weightSize *weight=new weightSize[matrixSize];
    memset(weight,0,sizeof(weightSize)*matrixSize);
    memoryAllocated1+=matrixSize*sizeof(weightSize)/8;

    int num;
    num=matrixSize/fper[fsize1]+1;

    uint64 *srcFig=new uint64[num];
    memset(srcFig,0,sizeof(uint64)*num);
    memoryAllocated2+=((matrixSize*fsize1)/64);

    num=matrixSize/fper[fsize2]+1;

    uint64 *dstFig=new uint64[num];
    memset(dstFig,0,sizeof(uint64)*num);
    memoryAllocated2+=((matrixSize*fsize2)/64);

    num=matrixSize/rper+1;

    uint64 *idx=new uint64[num];
    memset(idx,0,sizeof(uint64)*num);
    memoryAllocated3+=num;

    Gbasket *res=new Gbasket(srcFig,dstFig,idx,weight);


    return res;
}
void GSS_Chain::MatrixAppend(basketNode* basketCurr){
    valueCou+=1;
    loadFactor+=(double)basketCurr->bound/matrixSize;
    fullCount+=1;
    //cout<<(double)basketCurr->bound/matrixSize<<endl;

    basketNode *appendBasket0=new basketNode();
    basketCurr->next[0]=appendBasket0;

    appendBasket0->value=initialGbasket(f,f);
    return;
}
void GSS_Chain::insert(string s1, string s2,int weight,double& insertMatrix)// s1 is the ID of the source node, s2 is the ID of the destination node, weight is the edge weight.
{
        timeval t_start, t_end;
        gettimeofday( &t_start, NULL);
		ul hash1 = mpq->MPQHash(s1.c_str(), Type);
		ul hash2 = mpq->MPQHash(s2.c_str(), Type);
		unsigned int tmp = pow(2,f)-1;
		unsigned short g1 = hash1 & tmp;
		if(g1==0) g1+=1;
		unsigned int h1 = (hash1>>f)%w;
		unsigned short g2 = hash2 & tmp;
		if(g2==0) g2+=1;
		unsigned int h2 = (hash2>>f)%w;
		unsigned int k1 = (h1<<f)+g1;
		unsigned int k2 = (h2<<f)+g2;

		int* tmp1 = new int[r];
		int* tmp2 = new int[r];
		tmp1[0] = g1;
		tmp2[0] = g2;
		for(int i=1;i<r;i++)
		{
			tmp1[i]=(tmp1[i-1]*timer+prime)%bigger_p;
			tmp2[i]=(tmp2[i-1]*timer+prime)%bigger_p;
		}
		bool inserted=false;
		long *keys=new long[p+1];
		keys[0] = g1+g2;
		for(int i=1;i<p;i++)keys[i] = (keys[i-1]*timer+prime)%bigger_p;
		gettimeofday( &t_end, NULL);
        insertMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
        gettimeofday( &t_start, NULL);

        basketNode *mainBasketNow=valueTree,*tempMainBasket,*targetBasket;
        Gbasket *value;
        unsigned srcFig=g1,dstFig=g2;
        int emptyNum=-1;
        int poses[p],indexs1[p],indexs2[p];
        for(int i=0;i<p;i++)
        {
            int index = keys[i]%(r*r);
            indexs1[i] = index/r;
            indexs2[i] = index%r;
            int p1 = (h1+tmp1[indexs1[i]])%w;
            int p2 = (h2+tmp2[indexs2[i]])%w;
            poses[i] = p1*w + p2;
        }
        int cnt=0;
        while(mainBasketNow){
            tempMainBasket=mainBasketNow;

            checkMatrices[cnt]=mainBasketNow;
            posfind[cnt++]=-1;


            mainBasketNow=mainBasketNow->next[0];
        }

        #pragma omp parallel for
        for(int i=0;i<cnt;i++){
            if(inserted)continue;
            Gbasket *value=checkMatrices[i]->value;

            for(int j=0;j<p;j++)
            {
                int index1 = indexs1[j];
                int index2 = indexs2[j];
                int pos = poses[j];

                if (value->weight[pos]== 0)
                {
                    if(posfind[i]==-1){
                        posfind[i]=j;
                    }
                    continue;
                }

                //get the fingerprint and the idx
                int posIdx,posOffset;
                posIdx=pos/fper[f];
                posOffset=pos%fper[f];
                unsigned int srcFigNow=(((value->srcFig[posIdx])>>(posOffset*f))&fvalue[f]);
                if(srcFig!=srcFigNow)continue;

                posIdx=pos/fper[f];
                posOffset=pos%fper[f];
                unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*f))&fvalue[f]);
                if(dstFig!=dstFigNow)continue;

                posIdx=pos/rper;
                posOffset=pos%rper;
                unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);
                if(idxNow!=((index1<<halfR)|index2))continue;

                value->weight[pos]+= weight;
                inserted = true;
                break;

            }
        }

        if(!inserted){
            for(int i=0;i<cnt;i++){
                if(posfind[i]!=-1){
                    emptyNum=posfind[i];
                    targetBasket=checkMatrices[i];
                    break;
                }
            }
        }

        if(!inserted&&emptyNum!=-1){
            insertNum+=1;
            value=targetBasket->value;
            int index1 = indexs1[emptyNum];
            int index2 = indexs2[emptyNum];
            int pos = poses[emptyNum];

            int newPosIdx,newPosOffset;
            //cout<<targetLevel<<endl;
            newPosIdx=pos/fper[f];
            newPosOffset=pos%fper[f];
            value->srcFig[newPosIdx]|=((uint64)srcFig<<(newPosOffset*f));

            newPosIdx=pos/fper[f];
            newPosOffset=pos%fper[f];
            value->dstFig[newPosIdx]|=((uint64)dstFig<<(newPosOffset*f));

            newPosIdx=pos/rper;
            newPosOffset=pos%rper;
            value->idx[newPosIdx]|=((uint64)((index1<<halfR)|index2)<<(newPosOffset*rfnum));


            value->weight[pos]= weight;
            targetBasket->bound+=1;
            inserted = true;
        }

        if(!inserted){
            insertNum+=1;
            MatrixAppend(tempMainBasket);
            value=tempMainBasket->next[0]->value;
            for(int i=0;i<p;i++)
            {
                int index1 = indexs1[i];
                int index2 = indexs2[i];
                int pos = poses[i];
                if (value->weight[pos] == 0)
                {
                    int newPosIdx,newPosOffset;
                    //cout<<targetLevel<<endl;
                    newPosIdx=pos/fper[f];
                    newPosOffset=pos%fper[f];
                    value->srcFig[newPosIdx]|=((uint64)srcFig<<(newPosOffset*f));

                    newPosIdx=pos/fper[f];
                    newPosOffset=pos%fper[f];
                    value->dstFig[newPosIdx]|=((uint64)dstFig<<(newPosOffset*f));

                    newPosIdx=pos/rper;
                    newPosOffset=pos%rper;
                    value->idx[newPosIdx]|=((uint64)((index1<<halfR)|index2)<<(newPosOffset*rfnum));


                    value->weight[pos]= weight;
                    tempMainBasket->bound+=1;

                    inserted=true;
                }

            }
        }
		delete [] tmp1;
		delete [] tmp2;
		delete [] keys;
		gettimeofday( &t_end, NULL);
        insertMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
	    return;
}

int GSS_Chain::edgeQuery(string s1, string s2,double& queryMatrix)// s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
{
    int res=0;
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);
	ul hash1 = mpq->MPQHash(s1.c_str(), Type);
    ul hash2 = mpq->MPQHash(s2.c_str(), Type);
	int tmp = pow(2, f) - 1;
	unsigned short g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	unsigned short g2 = hash2 & tmp;
	if (g2 == 0) g2 += 1;
	unsigned int h2 = (hash2 >> f) % w;
	int* tmp1 = new int[r];
	int* tmp2 = new int[r];
	tmp1[0] = g1;
	tmp2[0] = g2;
	for (int i = 1; i<r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
		tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
	}
	long *keys=new long[p];
	keys[0] = g1 + g2;
	for(int i=1;i<p;i++)keys[i]=(keys[i-1] * timer + prime) % bigger_p;
	unsigned int k1 = (h1 << f) + g1;
	unsigned int k2 = (h2 << f) + g2;
	gettimeofday( &t_end, NULL);
    queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
    gettimeofday( &t_start, NULL);

    basketNode *mainBasketNow=valueTree;
    Gbasket *value;
    unsigned srcFig=g1,dstFig=g2;
    int poses[p],indexs1[p],indexs2[p],cnt=0;
    for(int i=0;i<p;i++)
    {
        int index = keys[i]%(r*r);
        indexs1[i] = index/r;
        indexs2[i] = index%r;
        int p1 = (h1+tmp1[indexs1[i]])%w;
        int p2 = (h2+tmp2[indexs2[i]])%w;
        poses[i] = p1*w + p2;
    }
    while(mainBasketNow){
        checkMatrices[cnt++]=mainBasketNow;

        mainBasketNow=mainBasketNow->next[0];
    }

    bool findEdge=false;

    #pragma omp parallel for
    for(int i=0;i<cnt;i++){
        if(findEdge)continue;
        Gbasket *value=checkMatrices[i]->value;

        for(int j=0;j<p;j++)
        {
            int index1 = indexs1[j];
            int index2 = indexs2[j];
            int pos = poses[j];

            //get the fingerprint and the idx
            int posIdx,posOffset;
            posIdx=pos/fper[f];
            posOffset=pos%fper[f];
            unsigned int srcFigNow=(((value->srcFig[posIdx])>>(posOffset*f))&fvalue[f]);
            if(srcFig!=srcFigNow)continue;

            posIdx=pos/fper[f];
            posOffset=pos%fper[f];
            unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*f))&fvalue[f]);
            if(dstFig!=dstFigNow)continue;

            posIdx=pos/rper;
            posOffset=pos%rper;
            unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);
            if(idxNow!=((index1<<halfR)|index2))continue;

            res=value->weight[pos];
            findEdge = true;
            break;

        }
    }

    gettimeofday( &t_end, NULL);
    queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
        (t_end.tv_usec-t_start.tv_usec);
    delete [] tmp1;
    delete [] tmp2;
	delete [] keys;
    return res;
}
int GSS_Chain::nodeValueQuery(string s1, int type, double& queryMatrix) // s1 is the ID of the queried node, function for node query.
{
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);
	int weight = 0;
	ul hash1 = mpq->MPQHash(s1.c_str(), Type);
	int tmp = pow(2, f) - 1;
	unsigned short g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	int* tmp1 = new int[r];
	unsigned int* poses=new unsigned int[r];
	tmp1[0] = g1;
	poses[0]=(h1 + tmp1[0]) % w;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
		poses[i]=(h1 + tmp1[i]) % w;
	}
	gettimeofday( &t_end, NULL);
	queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
    gettimeofday( &t_start, NULL);

    basketNode *mainBasketNow=valueTree;
    Gbasket *value;
    unsigned srcFigSize=f,dstFigSize=f;

    int cnt=0;
    while(mainBasketNow){
        checkMatrices[cnt++]=mainBasketNow;
        mainBasketNow=mainBasketNow->next[0];
    }

    #pragma omp parallel for
    for(int j=0;j<cnt;j++){
        weiRes[j]=0;
        Gbasket *value=checkMatrices[j]->value;
        if(!type){
            for (int i = 0; i < r; i++)
            {
                int p1 = poses[i];
                for (int k = 0; k < w; k++)
                {
                    int pos = p1*w + k;
                    int posIdx,posOffset;

                    posIdx=pos/fper[srcFigSize];
                    posOffset=pos%fper[srcFigSize];
                    unsigned int srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
                    if(srcFigNow!=g1)continue;

                    posIdx=pos/rper;
                    posOffset=pos%rper;
                    unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);
                    if ((idxNow>>halfR) == i)
                    {
                        weiRes[j]+= value->weight[pos];
                    }
                }
            }
        }
        else{
            for (int i = 0; i < r; i++)
            {
                int p1 = poses[i];
                for (int k = 0; k < w; k++)
                {
                    int pos = p1 + k*w;
                    int posIdx,posOffset;

                    posIdx=pos/fper[dstFigSize];
                    posOffset=pos%fper[dstFigSize];
                    unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
                    if(dstFigNow!=g1)continue;

                    posIdx=pos/rper;
                    posOffset=pos%rper;
                    unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);

                    if ((idxNow&fvalue[halfR]) == i)
                    {
                        weiRes[j]+= value->weight[pos];
                    }
                }
            }
        }
    }
    for(int i=0;i<cnt;i++)weight+=weiRes[i];

	gettimeofday( &t_end, NULL);
	queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
	delete []tmp1;
	delete []poses;
	return weight;
}

void GSS_Chain::sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, ul g1,vector<ul>& neis,ul prefix,int preLen,bool type){
    //cout<<level<<endl;
    if(!basketCurr)return;

    if(basketCurr->value){
        Gbasket *value=basketCurr->value;
        int srcFigSize=f-(level>>1),dstFigSize=f-(level>>1)-(level&0x1);
        if(!type){
            for (int i = 0; i < r; i++)
            {
                int p1 = poses[i];
                for (int k = 0; k < w; k++)
                {
                    //cout<<k<<endl;
                    int pos = p1*w + k;
                    int posIdx,posOffset;

                    posIdx=pos/fper[srcFigSize];
                    posOffset=pos%fper[srcFigSize];
                    ul srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
                    if(srcFigNow!=g1)continue;

                    posIdx=pos/rper;
                    posOffset=pos%rper;
                    unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);

                    if ((idxNow>>halfR) == i)
                    {
                        posIdx=pos/fper[dstFigSize];
                        posOffset=pos%fper[dstFigSize];
                        ul dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
                        dstFigNow<<=preLen;
                        dstFigNow|=prefix;
                        unsigned int idx=(idxNow&fvalue[halfR]);
                        ul originalH=calHash(dstFigNow,idx,k,w);
                        neis.push_back((originalH<<f)|dstFigNow);
                    }
                }
            }
        }
        else{
            for (int i = 0; i < r; i++)
            {
                int p1 = poses[i];
                for (int k = 0; k < w; k++)
                {
                    int pos = p1 + k*w;
                    int posIdx,posOffset;

                    posIdx=pos/fper[dstFigSize];
                    posOffset=pos%fper[dstFigSize];
                    unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
                    if(dstFigNow!=g1)continue;

                    posIdx=pos/rper;
                    posOffset=pos%rper;
                    unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);

                    if ((idxNow&fvalue[halfR]) == i)
                    {
                        posIdx=pos/fper[srcFigSize];
                        posOffset=pos%fper[srcFigSize];
                        ul srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
                        srcFigNow<<=preLen;
                        srcFigNow|=prefix;
                        unsigned int idx=(idxNow>>halfR);
                        ul originalH=calHash(srcFigNow,idx,k,w);
                        neis.push_back((originalH<<f)|srcFigNow);
                    }
                }
            }
        }
    }

    sucessorQuery(basketCurr->next[0],level,poses,g1,neis,prefix,preLen,type);
}
bool GSS_Chain::reachQuery(string s1,string s2,double &queryTime){
    if(s1==s2)return true;
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);

    ul hash1 = mpq->MPQHash(s1.c_str(), Type);
    ul hash2 = mpq->MPQHash(s2.c_str(), Type);
    //取低f位
    ul g1 = hash1 & fvalue[f];
    if(g1==0) g1+=1;
    ul h1 = (hash1>>f)%w;
    ul g2 = hash2 & fvalue[f];
    if(g2==0) g2+=1;
    ul h2 = (hash2>>f)%w;
    //到时候还原到k
    ul k1 = (h1<<f)+g1;
    ul k2 = (h2<<f)+g2;

    ul* tmp1 = new ul[r],* tmp2 = new ul[r];
	unsigned int* poses1=new unsigned int[r],* poses2=new unsigned int[r];
    tmp1[0] = g1;
    poses1[0]=(h1 + tmp1[0]) % w;
    for (int i = 1; i < r; i++)
    {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
        poses1[i]=(h1 + tmp1[i]) % w;

        tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
        poses2[i]=(h2 + tmp2[i]) % w;
    }



    set<ul> visited1,visited2;
    queue<ul> reader1,reader2;
    reader1.push(k1);
    visited1.insert(k1);
    reader2.push(k2);
    visited2.insert(k2);

    while(!reader1.empty()&&!reader2.empty()){
        ul neiNow=reader1.front();
        reader1.pop();
        h1=(neiNow>>f);g1=(neiNow&fvalue[f]);
        tmp1[0] = g1;
        poses1[0]=(h1 + tmp1[0]) % w;
        for (int i = 1; i < r; i++)
        {
            tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
            poses1[i]=(h1 + tmp1[i]) % w;
        }
        vector<ul>neis;
        sucessorQuery(valueTree,0,poses1,g1,neis,0,0,false);
        //cout<<"f\n";

        for(vector<ul>:: iterator it=neis.begin();it!=neis.end();it++){
            ul now=*it;
            if(visited1.find(now)==visited1.end()){
                if(visited2.find(now)!=visited2.end()){
                    delete []poses1;
                    delete []tmp1;
                    delete []poses2;
                    delete []tmp2;

                    gettimeofday( &t_end, NULL);
                    queryTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                            (t_end.tv_usec-t_start.tv_usec);
                    return true;
                }
                visited1.insert(now);
                reader1.push(now);
                //cout<<now<<endl;
            }
        }

        neiNow=reader2.front();
        reader2.pop();
        h2=(neiNow>>f);g2=(neiNow&fvalue[f]);
        tmp2[0] = g2;
        poses2[0]=(h2 + tmp2[0]) % w;
        for (int i = 1; i < r; i++)
        {
            tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
            poses2[i]=(h2 + tmp2[i]) % w;
        }
        neis.clear();
        sucessorQuery(valueTree,0,poses2,g2,neis,0,0,true);
        //cout<<"f\n";

        for(vector<ul>:: iterator it=neis.begin();it!=neis.end();it++){
            ul now=*it;
            if(visited2.find(now)==visited2.end()){
                if(visited1.find(now)!=visited1.end()){
                    delete []poses1;
                    delete []tmp1;
                    delete []poses2;
                    delete []tmp2;

                    gettimeofday( &t_end, NULL);
                    queryTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                            (t_end.tv_usec-t_start.tv_usec);
                    return true;
                }
                visited2.insert(now);
                reader2.push(now);
                //cout<<now<<endl;
            }
        }
    }
    delete []poses1;
    delete []tmp1;
    delete []poses2;
    delete []tmp2;
    gettimeofday( &t_end, NULL);
    queryTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
            (t_end.tv_usec-t_start.tv_usec);
    return false;
}

