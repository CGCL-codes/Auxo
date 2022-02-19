#include "querysupportstruct.h"
using namespace std;

ComAuxo::ComAuxo(int width, int range, int p_num, int f_num){
    mpq=new Blizzard_MPQ();
    valueCou=1;
    insertNum=0;
    memoryAllocated=0;
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
	deputyTree=new basketNode();

}

void ComAuxo::freeValueTree(basketNode* root){
    if(root){
        freeValueTree(root->next[0]);
        freeValueTree(root->next[1]);
        delete root;
    }
}

Gbasket* ComAuxo::initialGbasket(int fsize1,int fsize2){
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

basketNode* ComAuxo::getDeputyBasketNode(unsigned int srcFig,unsigned int dstFig,int& resLevel){
    basketNode *res=deputyTree,*helper=NULL;
    resLevel=-1;
    int level=0;
    while(res){
        if(res->value){
            helper=res;
            resLevel=level;
        }
        unsigned int tempfig;
        if(level&0x1){
            tempfig=srcFig;
            srcFig>>=1;
        }
        else{
            tempfig=dstFig;
            dstFig>>=1;
        }
        if(tempfig&0x1)res=res->next[1];
        else res=res->next[0];
        level++;
    }
    return helper;
}

void ComAuxo::MatrixAppend(basketNode* basketCurr,int& level){
    valueCou+=1;
    basketNode *tempBasketNode=basketCurr;
    int oldSrcFigSize=f-(level>>1),oldDstFigSize=f-(level>>1)-(level&0x1);
    while(true){
        level++;
        if((basketCurr->next[0]==NULL||basketCurr->next[0]->value==NULL)&&(basketCurr->next[1]==NULL||basketCurr->next[1]->value==NULL)){

            basketNode *appendBasket0,*appendBasket1;
            if(!basketCurr->next[0]){
                appendBasket0=new basketNode();
                basketCurr->next[0]=appendBasket0;
            }
            if(!basketCurr->next[1]){
                appendBasket1=new basketNode();
                basketCurr->next[1]=appendBasket1;
            }
            appendBasket0=basketCurr->next[0];
            appendBasket1=basketCurr->next[1];
            appendBasket0->bound=0;
            appendBasket1->bound=0;
            appendBasket0->level=level;
            appendBasket1->level=level;

            int newSrcFigSize=f-(level>>1),newDstFigSize=f-(level>>1)-(level&0x1);
            appendBasket0->value=initialGbasket(newSrcFigSize,newDstFigSize);
            appendBasket1->value=initialGbasket(newSrcFigSize,newDstFigSize);

            Gbasket *value=tempBasketNode->value;
            for(int i=0;i<matrixSize;i++){
                if(value->weight[i]==0)continue;

                int posIdx,posOffset;

                posIdx=i/fper[oldSrcFigSize];
                posOffset=i%fper[oldSrcFigSize];
                uint64 srcFig=(((value->srcFig[posIdx])>>(posOffset*oldSrcFigSize))&fvalue[oldSrcFigSize]);

                posIdx=i/fper[oldDstFigSize];
                posOffset=i%fper[oldDstFigSize];
                uint64  dstFig=(((value->dstFig[posIdx])>>(posOffset*oldDstFigSize))&fvalue[oldDstFigSize]);

                posIdx=i/rper;
                posOffset=i%rper;
                uint64  idx=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);

                unsigned int next;
                if((level-1)&0x1){
                    next=((srcFig>>(oldSrcFigSize-newSrcFigSize-1))&0x1);
                }
                else{
                    next=((dstFig>>(oldDstFigSize-newDstFigSize-1))&0x1);
                }

                Gbasket *valueNow;
                if(next){//move the element to next[1]
                    valueNow=appendBasket1->value;
                    appendBasket1->bound+=1;
                }
                else{//move the element to next[0]
                    valueNow=appendBasket0->value;
                    appendBasket0->bound+=1;
                }

                srcFig>>=(oldSrcFigSize-newSrcFigSize);
                dstFig>>=(oldDstFigSize-newDstFigSize);

                int newPosIdx,newPosOffset;

                newPosIdx=i/fper[newSrcFigSize];
                newPosOffset=i%fper[newSrcFigSize];
                valueNow->srcFig[newPosIdx]|=((uint64)srcFig<<(newPosOffset*newSrcFigSize));

                newPosIdx=i/fper[newDstFigSize];
                newPosOffset=i%fper[newDstFigSize];
                valueNow->dstFig[newPosIdx]|=((uint64)dstFig<<(newPosOffset*newDstFigSize));

                newPosIdx=i/rper;
                newPosOffset=i%rper;
                valueNow->idx[newPosIdx]|=((uint64)idx<<(newPosOffset*rfnum));

                valueNow->weight[i]=value->weight[i];


            }
            delete value;

            memoryAllocated-=matrixSize*sizeof(weightSize)/8;
            int num;
            num=matrixSize/fper[oldSrcFigSize]+1;
            memoryAllocated-=((matrixSize*oldSrcFigSize)/64);
            num=matrixSize/fper[oldDstFigSize]+1;
            memoryAllocated-=((matrixSize*oldDstFigSize)/64);
            num=matrixSize/rper+1;
            memoryAllocated-=num;

            tempBasketNode->value=NULL;
            tempBasketNode->bound=0;
            level--;
            return;

        }
        else{
            if(basketCurr->next[0]&&basketCurr->next[0]->value){
                basketCurr=basketCurr->next[1];
            }
            else{
                basketCurr=basketCurr->next[0];
            }
        }
    }
}

bool ComAuxo::insert(string s1, string s2,int weight,double& hashTime,double& insertMatrix){
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

		gettimeofday( &t_end, NULL);
        hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
        gettimeofday( &t_start, NULL);

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
                if(srcFig!=srcFigNow)continue;

                posIdx=pos/fper[dstFigSize];
                posOffset=pos%fper[dstFigSize];
                unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
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

        int deputyLevelNow=0;
        basketNode *deputyBasketNow=NULL;
        if(!inserted){//we check if we have recorded the edge in the deputy value tree
            //get last node with value on the deputy tree
            deputyBasketNow=getDeputyBasketNode(g1,g2,deputyLevelNow);
            if(deputyBasketNow==NULL&&emptyNum==-1){//we create a deputy node only if we need
                deputyTree->value=initialGbasket(f,f);
                deputyBasketNow=deputyTree;
                deputyLevelNow=0;
                valueCou+=1;
                //cout<<"a\n";
            }
            if(deputyBasketNow){
                value=deputyBasketNow->value;
                int srcFigSize=f-(deputyLevelNow>>1),dstFigSize=f-(deputyLevelNow>>1)-(deputyLevelNow&0x1);
                unsigned int srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));
                for(int i=0;i<p;i++)
                {
                    int index1 = indexs1[i];
                    int index2 = indexs2[i];
                    int pos = poses[i];
                    if (value->weight[pos] == 0)
                    {
                        if(emptyNum==-1){
                            emptyNum=i;
                            targetBasket=deputyBasketNow;
                            targetLevel=deputyLevelNow;
                        }
                        continue;
                    }

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

                    value->weight[pos]+= weight;
                    inserted = true;
                    break;

                }
            }
        }
        //if we find an empty bucket for the edge
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

        bool newTreeFlag=false;
        int levelNow;
        basketNode *basketNow;

        while(!inserted){

            //append node down flow
            if(newTreeFlag){
                deputyBasketNow=deputyTree;
                deputyLevelNow=0;
                deputyTree->value=initialGbasket(f,f);
            }
            else{
               gettimeofday( &t_end, NULL);
               hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                            (t_end.tv_usec-t_start.tv_usec);
               gettimeofday( &t_start, NULL);
               MatrixAppend(deputyBasketNow,deputyLevelNow);
               gettimeofday( &t_end, NULL);
               hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                            (t_end.tv_usec-t_start.tv_usec);
               gettimeofday( &t_start, NULL);

            }
            basketNow=getDeputyBasketNode(g1,g2,levelNow);
            value=basketNow->value;

            int srcFigSize=f-(levelNow>>1),dstFigSize=f-(levelNow>>1)-(levelNow&0x1);
            unsigned int srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));
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
                    basketNow->bound+=1;
                    inserted=true;
                    insertNum+=1;
                    break;
                }
            }
            if(deputyLevelNow==mainLevelNow){//we cut the value of the deputy off and stick it on the main value tree
                tempMainBasket->next[0]=new basketNode();
                tempMainBasket->next[0]->value=deputyBasketNow->next[0]->value;
                deputyBasketNow->next[0]->value=NULL;

                tempMainBasket->next[1]=new basketNode();
                tempMainBasket->next[1]->value=deputyBasketNow->next[1]->value;
                deputyBasketNow->next[1]->value=NULL;
                mainLevelNow++;
                newTreeFlag=true;
            }
            deputyBasketNow=basketNow;
            deputyLevelNow++;
        }

//		if(!inserted){
//            insert(s1,s2,weight,hashTime,insertMatrix);
//		}
		delete [] tmp1;
		delete [] tmp2;
		delete [] keys;
		gettimeofday( &t_end, NULL);
        insertMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
	    return inserted;
}

int ComAuxo::edgeQuery(string s1, string s2,double & hashTime, double& queryMatrix){
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
    gettimeofday( &t_end, NULL);
    hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
    gettimeofday( &t_start, NULL);
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

    int deputyLevelNow=0;
    basketNode *deputyBasketNow;

    deputyBasketNow=getDeputyBasketNode(g1,g2,deputyLevelNow);



    if(!deputyBasketNow){
        gettimeofday( &t_end, NULL);
        queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                (t_end.tv_usec-t_start.tv_usec);
        delete [] keys;
        delete [] tmp1;
        delete [] tmp2;
        return res;
    }
    value=deputyBasketNow->value;
    int srcFigSize=f-(deputyLevelNow>>1),dstFigSize=f-(deputyLevelNow>>1)-(deputyLevelNow&0x1);
    srcFig=(g1>>(f-srcFigSize));dstFig=(g2>>(f-dstFigSize));
    for(int i=0;i<p;i++)
    {
        int index1 = indexs1[i];
        int index2 = indexs2[i];
        int pos = poses[i];
        if (value->weight[pos]== 0)
        {
            continue;
        }


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

    delete [] keys;
    delete [] tmp1;
    delete [] tmp2;
    gettimeofday( &t_end, NULL);
    queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
        (t_end.tv_usec-t_start.tv_usec);
    return res;
}

int ComAuxo::getMainNodeValue(basketNode* basketCurr,int level,unsigned int *poses, unsigned int g1,bool type){
    int weight=0;
    if(!basketCurr)return weight;

    if(basketCurr->value){
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
    }

    if((level&0x1)!=type){
        int chose=g1&0x1;
        g1>>=1;
        weight+=getMainNodeValue(basketCurr->next[chose],level+1,poses,g1,type);
    }
    else{
        weight+=getMainNodeValue(basketCurr->next[0],level+1,poses,g1,type);
        weight+=getMainNodeValue(basketCurr->next[1],level+1,poses,g1,type);
    }
    return weight;
}

int ComAuxo::nodeValueQuery(string s1, int type, double& hashTime,double& queryMatrix){
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
	gettimeofday( &t_end, NULL);
	hashTime+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
    gettimeofday( &t_start, NULL);
    num1=0;
    num2=0;
    weight+=getMainNodeValue(mainTree,0,poses,g1,type);
    weight+=getMainNodeValue(deputyTree,0,poses,g1,type);

	gettimeofday( &t_end, NULL);
	queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
	delete []tmp1;
	delete []poses;
	return weight;
}

void ComAuxo::sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, unsigned int g1,vector<unsigned int> neis,unsigned int prefix,int preLen){
    if(!basketCurr)return;

    if(basketCurr->value){
        Gbasket *value=basketCurr->value;
        int srcFigSize=f-(level>>1),dstFigSize=f-(level>>1)-(level&0x1);
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
                    posIdx=pos/fper[dstFigSize];
                    posOffset=pos%fper[dstFigSize];
                    unsigned int dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
                    dstFigNow<<=preLen;
                    dstFigNow|=prefix;
                    unsigned int idx=(idxNow&fvalue[halfR]);
                    unsigned int originalH=calHash(dstFigNow,idx,k,w);
                    neis.push_back((originalH<<f)|dstFigNow);
                }
            }
        }
    }

    if(level&0x1){
        int chose=g1&0x1;
        g1>>=1;
        sucessorQuery(basketCurr->next[chose],level+1,poses,g1,neis,prefix,preLen);
    }
    else{
        sucessorQuery(basketCurr->next[0],level+1,poses,g1,neis,prefix,preLen+1);
        sucessorQuery(basketCurr->next[1],level+1,poses,g1,neis,prefix|(0x1<<preLen),preLen+1);
    }
}

bool ComAuxo::reachQuery(string s1,string s2){
    if(s1==s2)return true;
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
	unsigned int* poses=new unsigned int[r];
    tmp1[0] = g1;
    poses[0]=(h1 + tmp1[0]) % w;
    for (int i = 1; i < r; i++)
    {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
        poses[i]=(h1 + tmp1[i]) % w;
    }

    unsigned int maxNum=fvalue[f]*w;
    bool *visited=new bool[maxNum];
    memset(visited,0,sizeof(bool)*maxNum);
    queue<unsigned int> reader;
    reader.push(k1);
    visited[k1]=true;
    while(!reader.empty()){
        unsigned int neiNow=reader.front();
        reader.pop();
        unsigned int h=(neiNow>>f),g=(neiNow&fvalue[f]);
        tmp1[0] = g;
        poses[0]=(h + tmp1[0]) % w;
        for (int i = 1; i < r; i++)
        {
            tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
            poses[i]=(h + tmp1[i]) % w;
        }
        vector<unsigned int>neis;
        sucessorQuery(mainTree,0,poses,g,neis,0,0);
        sucessorQuery(deputyTree,0,poses,g,neis,0,0);
        for(vector<unsigned int>:: iterator it=neis.begin();it!=neis.end();it++){
            unsigned int now=*it;
            if(!visited[now]){
                if(now==k2){
                    delete []poses;
                    delete []tmp1;
                    delete []visited;
                    return true;
                }
                visited[now]=true;
                reader.push(now);
            }
        }
    }
    delete []poses;
    delete []tmp1;
    delete []visited;
    return false;
}
