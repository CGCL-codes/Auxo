#include "querysupportstruct.h"
using namespace std;

Auxo::Auxo(int width, int range, int p_num, int f_num)
{
    checkMatrices=vector<basketNode*>(1500);
    posfind=vector<int>(1500);
    weiRes=vector<int>(1500);
    matrixLevel=vector<int>(200);
    fullCount=0;
    loadFactor=0;
    mpq=new Blizzard_MPQ();
    valueCou=1;
    insertNum=0;
    memoryAllocated1=0;
    memoryAllocated2=0;
    memoryAllocated3=0;
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

	mainTree=new basketNode();
	mainTree->value=initialGbasket(f,f);
}
void Auxo::freeValueTree(basketNode* root){
    if(root){
        freeValueTree(root->next[0]);
        freeValueTree(root->next[1]);
        delete root;
    }
}
Gbasket* Auxo::initialGbasket(int fsize1,int fsize2){
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
void Auxo::MatrixAppend(basketNode* basketCurr,int level){
    int oldSrcFigSize=f-((level+1)>>1),oldDstFigSize=f-((level+1)>>1)-((level+1)&0x1);
    valueCou+=2;
    loadFactor+=(double)basketCurr->bound/matrixSize;
    fullCount+=1;
    //cout<<(double)basketCurr->bound/matrixSize<<" "<<insertNum/(matrixSize*valueCou)<<endl;
    Gbasket *valueNow=basketCurr->value;

    basketNode *appendBasket0=new basketNode(),*appendBasket1=new basketNode();
    basketCurr->next[0]=appendBasket0;basketCurr->next[1]=appendBasket1;

    appendBasket1->value=initialGbasket(oldSrcFigSize,oldDstFigSize);

    appendBasket0->value=initialGbasket(oldSrcFigSize,oldDstFigSize);
    return;
}
bool Auxo::insert(string s1, string s2,int weight,double& insertMatrix)// s1 is the ID of the source node, s2 is the ID of the destination node, weight is the edge weight.
{
        timeval t_start, t_end;
        gettimeofday( &t_start, NULL);
		ul hash1 = mpq->MPQHash(s1.c_str(), Type);
		ul hash2 = mpq->MPQHash(s2.c_str(), Type);
		unsigned int g1 = hash1 & fvalue[f];
		if(g1==0) g1+=1;
		unsigned int h1 = (hash1>>f)%w;
		unsigned int g2 = hash2 & fvalue[f];
		if(g2==0) g2+=1;
		unsigned int h2 = (hash2>>f)%w;
		unsigned int k1 = (h1<<f)+g1;
		unsigned int k2 = (h2<<f)+g2;

		unsigned int* tmp1 = new unsigned int[r];
		unsigned int* tmp2 = new unsigned int[r];
		tmp1[0] = g1;
		tmp2[0] = g2;
		for(int i=1;i<r;i++)
		{
			tmp1[i]=(tmp1[i-1]*timer+prime)%bigger_p;
			tmp2[i]=(tmp2[i-1]*timer+prime)%bigger_p;
		}

		long *keys=new long[p+1];
		keys[0] = g1+g2;
		for(int i=1;i<p;i++)keys[i] = (keys[i-1]*timer+prime)%bigger_p;


        bool inserted=false;
        //we first check if we have recorded the edge in the main value tree
        basketNode *mainBasketNow=mainTree,*targetBasket=NULL,*tempMainBasket=NULL;
        Gbasket *value;

        int emptyNum=-1,mainLevelNow=0;
        unsigned int targetLevel=-1;
        unsigned int srcFig=g1,dstFig=g2;
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
        while(mainBasketNow){
            tempMainBasket=mainBasketNow;
            value=mainBasketNow->value;
            int srcFigSize=f-(mainLevelNow>>1),dstFigSize=f-(mainLevelNow>>1)-(mainLevelNow&0x1);
            for(int i=0;i<p;i++)
            {
                int index1 = indexs1[i];
                int index2 = indexs2[i];
                int pos = poses[i];

                if (value->weight[pos]== 0)
                {
                    if(emptyNum==-1){
                        emptyNum=i;
                        targetBasket=mainBasketNow;
                        targetLevel=mainLevelNow;
                    }
                    continue;
                }

                //get the fingerprint and the idx
                int posIdx,posOffset;
                posIdx=pos/fper[srcFigSize];
                posOffset=pos%fper[srcFigSize];
                unsigned int srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
                if(srcFigNow!=srcFig)continue;

                posIdx=pos/fper[dstFigSize];
                posOffset=pos%fper[dstFigSize];
                unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
                if(dstFigNow!=dstFig)continue;

                posIdx=pos/rper;
                posOffset=pos%rper;
                unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);
                if(idxNow!=((index1<<halfR)|index2))continue;

                value->weight[pos]+= weight;
                inserted = true;
                break;

            }
            if(inserted)break;

            unsigned int tempfig;
            if(mainLevelNow&0x1){
                tempfig=srcFig;
                srcFig>>=1;
            }
            else{
                tempfig=dstFig;
                dstFig>>=1;
            }
            mainBasketNow=mainBasketNow->next[tempfig&0x1];
            mainLevelNow++;
        }
        mainLevelNow--;

        if(!inserted&&emptyNum!=-1){
            insertNum+=1;
            value=targetBasket->value;
            int index1 = indexs1[emptyNum];
            int index2 = indexs2[emptyNum];
            int pos = poses[emptyNum];

			int srcFigSize=f-(targetLevel>>1),dstFigSize=f-(targetLevel>>1)-(targetLevel&0x1);
            unsigned int srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));

            int newPosIdx,newPosOffset;
            //cout<<targetLevel<<endl;
            newPosIdx=pos/fper[srcFigSize];
            newPosOffset=pos%fper[srcFigSize];
            value->srcFig[newPosIdx]|=((uint64)srcFig<<(newPosOffset*srcFigSize));

            newPosIdx=pos/fper[dstFigSize];
            newPosOffset=pos%fper[dstFigSize];
            value->dstFig[newPosIdx]|=((uint64)dstFig<<(newPosOffset*dstFigSize));

            newPosIdx=pos/rper;
            newPosOffset=pos%rper;
            value->idx[newPosIdx]|=((uint64)((index1<<halfR)|index2)<<(newPosOffset*rfnum));


            value->weight[pos]= weight;
            targetBasket->bound+=1;
            inserted = true;
        }

        int levelNow=mainLevelNow+1;
        if(!inserted){
            //append node down flow
            MatrixAppend(tempMainBasket,mainLevelNow);
            int srcFigSize=f-(mainLevelNow>>1),dstFigSize=f-(mainLevelNow>>1)-(mainLevelNow&0x1);
            //cout<<srcFigSize<<" "<<dstFigSize<<endl;
            g1>>=(f-srcFigSize);
            g2>>=(f-dstFigSize);

            if(levelNow&0x1){
                tempMainBasket=tempMainBasket->next[g2&0x1];
                g2>>=1;
                dstFigSize-=1;
            }
            else{
                tempMainBasket=tempMainBasket->next[g1&0x1];
                g1>>=1;
                srcFigSize-=1;
            }

            value=tempMainBasket->value;
//            int srcFigSize=f-(levelNow>>1),dstFigSize=f-(levelNow>>1)-(levelNow&0x1);
//            unsigned int srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));
            for(int i=0;i<p;i++)
            {
                int index1 = indexs1[i];
                int index2 = indexs2[i];
                int pos = poses[i];
                if (value->weight[pos] == 0)
                {
                    int newPosIdx,newPosOffset;

                    newPosIdx=pos/fper[srcFigSize];
                    newPosOffset=pos%fper[srcFigSize];
                    value->srcFig[newPosIdx]|=((uint64)srcFig<<(newPosOffset*srcFigSize));

                    newPosIdx=pos/fper[dstFigSize];
                    newPosOffset=pos%fper[dstFigSize];
                    value->dstFig[newPosIdx]|=((uint64)dstFig<<(newPosOffset*dstFigSize));

                    newPosIdx=pos/rper;
                    newPosOffset=pos%rper;
                    value->idx[newPosIdx]|=((uint64)((index1<<halfR)|index2)<<(newPosOffset*rfnum));

                    value->weight[pos]= weight;
                    tempMainBasket->bound+=1;
                    inserted=true;
                    insertNum+=1;
                    break;
                }
            }
        }
		delete [] tmp1;
		delete [] tmp2;
		delete [] keys;
		gettimeofday( &t_end, NULL);
        insertMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
	    return inserted;
}

int Auxo::edgeQuery(string s1, string s2,double& queryMatrix)// s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
{
    int res=0;
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);
	ul hash1 = mpq->MPQHash(s1.c_str(), Type);
    ul hash2 = mpq->MPQHash(s2.c_str(), Type);
	unsigned int g1 = hash1 & fvalue[f];
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	unsigned int g2 = hash2 & fvalue[f];
	if (g2 == 0) g2 += 1;
	unsigned int h2 = (hash2 >> f) % w;
	unsigned int* tmp1 = new unsigned int[r];
	unsigned int* tmp2 = new unsigned int[r];
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


    //we first check if we have recorded the edge in the main value tree
    basketNode *mainBasketNow=mainTree,*tempMainBasket=NULL;
    Gbasket *value;

    int mainLevelNow=0;
    unsigned int srcFig=g1,dstFig=g2;

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
    while(mainBasketNow){
        tempMainBasket=mainBasketNow;
        value=mainBasketNow->value;
        int srcFigSize=f-(mainLevelNow>>1),dstFigSize=f-(mainLevelNow>>1)-(mainLevelNow&0x1);
        for(int i=0;i<p;i++)
        {
            int index1 = indexs1[i];
            int index2 = indexs2[i];
            int pos = poses[i];

            //get the fingerprint and the idx
            int posIdx,posOffset;
            posIdx=pos/fper[srcFigSize];
            posOffset=pos%fper[srcFigSize];
            unsigned int srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
            if(srcFig!=srcFigNow)continue;

            posIdx=pos/fper[dstFigSize];
            posOffset=pos%fper[dstFigSize];
            unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
            if(dstFig!=dstFigNow)continue;

            posIdx=pos/rper;
            posOffset=pos%rper;
            unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);
            if(idxNow!=((index1<<halfR)|index2))continue;

            gettimeofday( &t_end, NULL);
            queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
            delete [] keys;
            delete [] tmp1;
            delete [] tmp2;
            return value->weight[pos];

        }

        unsigned int tempfig;
        if(mainLevelNow&0x1){
            tempfig=srcFig;
            srcFig>>=1;
        }
        else{
            tempfig=dstFig;
            dstFig>>=1;
        }
        if(tempfig&0x1)mainBasketNow=mainBasketNow->next[1];
        else mainBasketNow=mainBasketNow->next[0];
        mainLevelNow++;
    }

    delete [] keys;
    delete [] tmp1;
    delete [] tmp2;
    gettimeofday( &t_end, NULL);
    queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
        (t_end.tv_usec-t_start.tv_usec);
    return res;
}
/*type 0 is for successor query, type 1 is for preccessor query*/
unsigned int Auxo::getNodeValue(basketNode* basketCurr,int level,unsigned int *poses, unsigned int g1,bool type){
    unsigned int weight=0;
    if(!basketCurr)return weight;
    Gbasket *value=basketCurr->value;
    int srcFigSize=f-(level>>1),dstFigSize=f-(level>>1)-(level&0x1);
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
                    weight += value->weight[pos];
                }
            }
       }
    }
    else{
        for (int i = 0; i < r; i++){
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
                    weight += value->weight[pos];
                }
            }
        }
    }

    if((level&0x1)!=type){
        int chose=g1&0x1;
        g1>>=1;
        weight+=getNodeValue(basketCurr->next[chose],level+1,poses,g1,type);
    }
    else{
        weight+=getNodeValue(basketCurr->next[0],level+1,poses,g1,type);
        weight+=getNodeValue(basketCurr->next[1],level+1,poses,g1,type);
    }
    return weight;
}

int Auxo::nodeValueQuery(string s1, int type, double& queryMatrix) // s1 is the ID of the queried node, function for node query.
{
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);
	int weight = 0;
	ul hash1 = mpq->MPQHash(s1.c_str(), Type);
	unsigned int g1 = hash1 & fvalue[f];
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	unsigned int* tmp1 = new unsigned int[r];

	unsigned int* poses=new unsigned int[r];
	tmp1[0] = g1;
	poses[0]=(h1 + tmp1[0]) % w;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
		poses[i]=(h1 + tmp1[i]) % w;
	}


    weight+=getNodeValue(mainTree,0,poses,g1,type);
	gettimeofday( &t_end, NULL);
	queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
	delete []tmp1;
	delete []poses;
	return weight;
}

void Auxo::sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, ul g1,vector<ul>& neis,ul prefix,int preLen,bool type){
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

    if((level&0x1)!=type){
        int chose=g1&0x1;
        g1>>=1;
        sucessorQuery(basketCurr->next[chose],level+1,poses,g1,neis,prefix,preLen,type);
    }
    else{
        sucessorQuery(basketCurr->next[0],level+1,poses,g1,neis,prefix,preLen+1,type);
        sucessorQuery(basketCurr->next[1],level+1,poses,g1,neis,prefix|(0x1<<preLen),preLen+1,type);
    }
}

bool Auxo::reachQuery(string s1,string s2,double &queryTime){
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
        sucessorQuery(mainTree,0,poses1,g1,neis,0,0,false);
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
        sucessorQuery(mainTree,0,poses2,g2,neis,0,0,true);
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
