#include "querysupportstruct.h"
using namespace std;

GSS_Chain::GSS_Chain(int width, int range, int p_num, int f_num){
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
    memoryAllocated+=matrixSize*sizeof(weightSize)/8;

    int num;
    num=matrixSize/fper[fsize1]+1;

    uint64 *srcFig=new uint64[num];
    memset(srcFig,0,sizeof(uint64)*num);
    memoryAllocated+=((matrixSize*fsize1)/64);

    num=matrixSize/fper[fsize2]+1;

    uint64 *dstFig=new uint64[num];
    memset(dstFig,0,sizeof(uint64)*num);
    memoryAllocated+=((matrixSize*fsize2)/64);

    num=matrixSize/rper+1;

    uint64 *idx=new uint64[num];
    memset(idx,0,sizeof(uint64)*num);
    memoryAllocated+=num;

    Gbasket *res=new Gbasket(srcFig,dstFig,idx,weight);


    return res;
}

void GSS_Chain::MatrixAppend(basketNode* basketCurr){
    valueCou+=1;

    basketNode *appendBasket0=new basketNode();
    basketCurr->next[0]=appendBasket0;

    appendBasket0->value=initialGbasket(f,f);
    return;
}

void GSS_Chain::insert(string s1, string s2,int weight,double& hashTime,double& insertMatrix){
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
        hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
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
        while(mainBasketNow){
            tempMainBasket=mainBasketNow;
            value=mainBasketNow->value;
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
            if(inserted)break;

            mainBasketNow=mainBasketNow->next[0];
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

int GSS_Chain::edgeQuery(string s1, string s2,double & hashTime, double& queryMatrix){
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
    hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
    gettimeofday( &t_start, NULL);

    basketNode *mainBasketNow=valueTree,*tempMainBasket;
    Gbasket *value;
    unsigned srcFig=g1,dstFig=g2;
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
        for(int i=0;i<p;i++)
        {
            int index1 = indexs1[i];
            int index2 = indexs2[i];
            int pos = poses[i];

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

            gettimeofday( &t_end, NULL);
            queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
            delete [] tmp1;
		    delete [] tmp2;
		    delete [] keys;
            return value->weight[pos];

        }

        mainBasketNow=mainBasketNow->next[0];
    }
    gettimeofday( &t_end, NULL);
    queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
        (t_end.tv_usec-t_start.tv_usec);
    delete [] tmp1;
    delete [] tmp2;
	delete [] keys;
    return res;
}

int GSS_Chain::nodeValueQuery(string s1, int type, double& hashTime,double& queryMatrix){
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
	hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
    gettimeofday( &t_start, NULL);

    basketNode *mainBasketNow=valueTree;
    Gbasket *value;
    unsigned srcFigSize=f,dstFigSize=f;
    while(mainBasketNow){
        value=mainBasketNow->value;
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
                        weight += value->weight[pos];
                    }
                }
            }
        }


        mainBasketNow=mainBasketNow->next[0];
    }

	gettimeofday( &t_end, NULL);
	queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
	delete []tmp1;
	delete []poses;
	return weight;
}
