#include "querysupportstruct.h"
using namespace std;

AuxoPro::AuxoPro(int width, int range, int p_num, int f_num)
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
    memoryAllocated1=0;memoryAllocated2=0;memoryAllocated3=0;
	w = width;
	r = range; /* r x r mapped baskets */
	p = p_num; /*candidate buckets*/
	f = f_num; /*finger print length*/
	rfnum=2*ceil(log(r)/log(2));//the number of bit index need
	halfR=rfnum/2;
	rper=64/rfnum;//the number of index a uint64 can contain
	matrixSize=w*w;
	fvalue=new ul[f+1];
	fper=new int[f+1];
	for(int i=1;i<=f;i++){
        fvalue[i]=(((ul)0x1<<i)-1);
        fper[i]=64/i;
	}

	mainTree=new basketNode();
	mainTree->value=initialGbasket(f,f);
	deputyTree=new basketNode();

}
void AuxoPro::freeValueTree(basketNode* root){
    if(root){
        freeValueTree(root->next[0]);
        freeValueTree(root->next[1]);
        delete root;
    }
}
Gbasket* AuxoPro::initialGbasket(int fsize1,int fsize2){
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
basketNode* AuxoPro::getDeputyBasketNode(unsigned int srcFig,unsigned int dstFig,int& resLevel){
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
void AuxoPro::MatrixAppend(basketNode* basketCurr,int& level){
    valueCou+=1;
    loadFactor+=(double)basketCurr->bound/matrixSize;
    fullCount+=1;
    basketNode *tempBasketNode=basketCurr;
    //cout<<(double)basketCurr->bound/matrixSize<<" "<<insertNum/(matrixSize*valueCou)<<endl;
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
            delete tempBasketNode->value;
            tempBasketNode->value=NULL;

            memoryAllocated1-=matrixSize*sizeof(weightSize)/8;
            int num;
            num=matrixSize/fper[oldSrcFigSize]+1;
            memoryAllocated2-=((matrixSize*oldSrcFigSize)/64);
            num=matrixSize/fper[oldDstFigSize]+1;
            memoryAllocated2-=((matrixSize*oldDstFigSize)/64);
            num=matrixSize/rper+1;
            memoryAllocated3-=num;

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


int AuxoPro::insert(string s1, string s2,int weight,double& insertMatrix)// s1 is the ID of the source node, s2 is the ID of the destination node, weight is the edge weight.
{
        int res;
        timeval t_start, t_end;
        gettimeofday( &t_start, NULL);
		ul hash1 = mpq->MPQHash(s1.c_str(), Type);
		ul hash2 = mpq->MPQHash(s2.c_str(), Type);
		ul g1 = hash1 & fvalue[f];
		if(g1==0) g1+=1;
		ul h1 = (hash1>>f)%w;
		ul g2 = hash2 & fvalue[f];
		if(g2==0) g2+=1;
		ul h2 = (hash2>>f)%w;
		ul k1 = (h1<<f)+g1;
		ul k2 = (h2<<f)+g2;

		ul* tmp1 = new ul[r];
		ul* tmp2 = new ul[r];
		tmp1[0] = g1;
		tmp2[0] = g2;
		for(int i=1;i<r;i++)
		{
			tmp1[i]=(tmp1[i-1]*timer+prime)%bigger_p;
			tmp2[i]=(tmp2[i-1]*timer+prime)%bigger_p;
		}

		ul *keys=new ul[p+1];
		keys[0] = g1+g2;
		for(int i=1;i<p;i++)keys[i] = (keys[i-1]*timer+prime)%bigger_p;

		gettimeofday( &t_end, NULL);
        insertMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                    (t_end.tv_usec-t_start.tv_usec);
        gettimeofday( &t_start, NULL);

        volatile bool inserted=false;
        //we first check if we have recorded the edge in the main value tree
        basketNode *mainBasketNow=mainTree,*targetBasket=NULL,*tempMainBasket=NULL;
        Gbasket *value;

        int emptyNum=-1,mainLevelNow=0;
        unsigned int targetLevel=-1;
        ul srcFig=g1,dstFig=g2;
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

        cnt=0;
        while(mainBasketNow){
            tempMainBasket=mainBasketNow;
            //value=mainBasketNow->value;
            int srcFigSize=f-(mainLevelNow>>1),dstFigSize=f-(mainLevelNow>>1)-(mainLevelNow&0x1);
            checkMatrices[cnt]=mainBasketNow;
            posfind[cnt]=-1;
            matrixLevel[cnt++]=mainLevelNow;

            ul tempfig;
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
        deputyBasketNow=getDeputyBasketNode(g1,g2,deputyLevelNow);
        bool lastDeputy=false;
        if(deputyBasketNow){
            checkMatrices[cnt]=deputyBasketNow;
            posfind[cnt]=-1;
            matrixLevel[cnt++]=deputyLevelNow;
            lastDeputy=true;
        }

        #pragma omp parallel for
        for (int i = 0; i < cnt; i++){
            if(inserted)continue;
            Gbasket *value=checkMatrices[i]->value;
            int level=matrixLevel[i];
            int srcFigSize=f-(level>>1),dstFigSize=f-(level>>1)-(level&0x1);
            ul srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));
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

                posIdx=pos/fper[srcFigSize];
                posOffset=pos%fper[srcFigSize];
                ul srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
                if(srcFig!=srcFigNow)continue;

                posIdx=pos/fper[dstFigSize];
                posOffset=pos%fper[dstFigSize];
                ul dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
                if(dstFig!=dstFigNow)continue;

                posIdx=pos/rper;
                posOffset=pos%rper;
                unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);
                if(idxNow!=((index1<<halfR)|index2))continue;

                value->weight[pos]+= weight;
                inserted = true;
                res=value->weight[pos];
                break;

            }
        }
        if(!inserted){
            for(int i=0;i<cnt;i++){
                if(posfind[i]!=-1){
                    emptyNum=posfind[i];
                    targetBasket=checkMatrices[i];
                    targetLevel=matrixLevel[i];
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

			int srcFigSize=f-(targetLevel>>1),dstFigSize=f-(targetLevel>>1)-(targetLevel&0x1);
            ul srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));

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
            res=value->weight[pos];
            inserted = true;
        }

        if(!inserted){//we create a deputy node only if we need
            if(deputyBasketNow==NULL){
                deputyTree->value=initialGbasket(f,f);
                deputyBasketNow=deputyTree;
                deputyLevelNow=0;
                valueCou+=1;

                emptyNum=0;
                targetBasket=deputyBasketNow;
                targetLevel=0;


                insertNum+=1;
                value=targetBasket->value;
                int index1 = indexs1[emptyNum];
                int index2 = indexs2[emptyNum];
                int pos = poses[emptyNum];

                int srcFigSize=f-(targetLevel>>1),dstFigSize=f-(targetLevel>>1)-(targetLevel&0x1);
                ul srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));

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
                res=value->weight[pos];
                inserted = true;
            }
        }
        //if we find an empty bucket for the edge


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
               insertMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                            (t_end.tv_usec-t_start.tv_usec);
               gettimeofday( &t_start, NULL);
               MatrixAppend(deputyBasketNow,deputyLevelNow);
               gettimeofday( &t_end, NULL);
               insertMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
                            (t_end.tv_usec-t_start.tv_usec);
               gettimeofday( &t_start, NULL);

            }
            basketNow=getDeputyBasketNode(g1,g2,levelNow);
            value=basketNow->value;

            int srcFigSize=f-(levelNow>>1),dstFigSize=f-(levelNow>>1)-(levelNow&0x1);
            ul srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));
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
                    res=value->weight[pos];
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
	    return res;
}

int AuxoPro::edgeQuery(string s1, string s2,double& queryMatrix)// s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
{
    int res=0;
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);
	ul hash1 = mpq->MPQHash(s1.c_str(), Type);
    ul hash2 = mpq->MPQHash(s2.c_str(), Type);
	ul g1 = hash1 & fvalue[f];
	if (g1 == 0) g1 += 1;
	ul h1 = (hash1 >> f) % w;
	ul g2 = hash2 & fvalue[f];
	if (g2 == 0) g2 += 1;
	ul h2 = (hash2 >> f) % w;
	ul* tmp1 = new ul[r];
	ul* tmp2 = new ul[r];
	tmp1[0] = g1;
	tmp2[0] = g2;
	for (int i = 1; i<r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
		tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
	}
	ul *keys=new ul[p];
	keys[0] = g1 + g2;
	for(int i=1;i<p;i++)keys[i]=(keys[i-1] * timer + prime) % bigger_p;
	ul k1 = (h1 << f) + g1;
	ul k2 = (h2 << f) + g2;


    //we first check if we have recorded the edge in the main value tree
    basketNode *mainBasketNow=mainTree,*tempMainBasket=NULL;
    Gbasket *value;

    int mainLevelNow=0;
    ul srcFig=g1,dstFig=g2;
    gettimeofday( &t_end, NULL);
    queryMatrix+=(t_end.tv_sec-t_start.tv_sec)*1000000.0 +
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

    cnt=0;

    while(mainBasketNow){
        tempMainBasket=mainBasketNow;
        //value=mainBasketNow->value;
        int srcFigSize=f-(mainLevelNow>>1),dstFigSize=f-(mainLevelNow>>1)-(mainLevelNow&0x1);
        if(mainBasketNow->value){
            checkMatrices[cnt]=mainBasketNow;
            matrixLevel[cnt++]=mainLevelNow;
        }

        ul tempfig;
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

    int deputyLevelNow=0;
    basketNode *deputyBasketNow;

    deputyBasketNow=getDeputyBasketNode(g1,g2,deputyLevelNow);
    if(deputyBasketNow){
        checkMatrices[cnt]=deputyBasketNow;
        matrixLevel[cnt++]=deputyLevelNow;
    }

    bool founded=false;

    #pragma omp parallel for
    for (int i = 0; i < cnt; i++){
        if(founded)continue;
        Gbasket *value=checkMatrices[i]->value;
        int level=matrixLevel[i];
        int srcFigSize=f-(level>>1),dstFigSize=f-(level>>1)-(level&0x1);
        ul srcFig=(g1>>(f-srcFigSize)),dstFig=(g2>>(f-dstFigSize));
        for(int j=0;j<p;j++)
        {
            int index1 = indexs1[j];
            int index2 = indexs2[j];
            int pos = poses[j];


            //get the fingerprint and the idx
            int posIdx,posOffset;

            posIdx=pos/fper[srcFigSize];
            posOffset=pos%fper[srcFigSize];
            ul srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
            if(srcFig!=srcFigNow)continue;

            posIdx=pos/fper[dstFigSize];
            posOffset=pos%fper[dstFigSize];
            ul dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
            if(dstFig!=dstFigNow)continue;

            posIdx=pos/rper;
            posOffset=pos%rper;
            unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);
            if(idxNow!=((index1<<halfR)|index2))continue;

            founded=true;
            res=value->weight[pos];
            break;
        }
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
void AuxoPro::getMainNodeValue(basketNode* basketCurr,int level, ul g1,
                            bool type,int& cnt){
    if(!basketCurr)return;

    if(basketCurr->value){
        checkMatrices[cnt]=basketCurr;
        matrixLevel[cnt++]=level;
    }

    if((level&0x1)!=type){
        int chose=g1&0x1;
        g1>>=1;
        getMainNodeValue(basketCurr->next[chose],level+1,g1,type,cnt);
    }
    else{
        getMainNodeValue(basketCurr->next[0],level+1,g1,type,cnt);
        getMainNodeValue(basketCurr->next[1],level+1,g1,type,cnt);
    }
    return;
}

int AuxoPro::nodeValueQuery(string s1, int type, double& queryMatrix) // s1 is the ID of the queried node, function for node query.
{
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);
	int weight = 0;
	ul hash1 = mpq->MPQHash(s1.c_str(), Type);


	ul g1 = hash1 & fvalue[f];
	if (g1 == 0) g1 += 1;
	ul h1 = (hash1 >> f) % w;
	ul* tmp1 = new ul[r];
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
    num1=0;
    num2=0;

    cnt=0;

    getMainNodeValue(mainTree,0,g1,type,cnt);
    getMainNodeValue(deputyTree,0,g1,type,cnt);


    for(int i=0;i<cnt;i++)weiRes[i]=0;
    #pragma omp parallel for
    for (int j = 0; j < cnt; j++){
        Gbasket *value=checkMatrices[j]->value;
        int level=matrixLevel[j];

        if(!type){
           int srcFigSize=f-(level>>1);
           ul figNow=(g1>>(f-srcFigSize));
           for (int i = 0; i < r; i++)
           {
                int p1 = poses[i];
                for (int k = 0; k < w; k++)
                {
                    int pos = p1*w + k;
                    int posIdx,posOffset;

                    posIdx=pos/fper[srcFigSize];
                    posOffset=pos%fper[srcFigSize];
                    ul srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);
                    if(srcFigNow!=figNow)continue;

                    posIdx=pos/rper;
                    posOffset=pos%rper;
                    unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);

                    if ((idxNow>>halfR) == i)
                    {
                        weiRes[j]+=value->weight[pos];
                    }
                }
           }
        }
        else{
            int dstFigSize=f-(level>>1)-(level&0x1);
            ul figNow=(g1>>(f-dstFigSize));
            for (int i = 0; i < r; i++){
                int p1 = poses[i];
                for (int k = 0; k < w; k++)
                {
                    int pos = p1 + k*w;
                    int posIdx,posOffset;

                    posIdx=pos/fper[dstFigSize];
                    posOffset=pos%fper[dstFigSize];
                    ul dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);
                    if(dstFigNow!=figNow)continue;

                    posIdx=pos/rper;
                    posOffset=pos%rper;
                    unsigned int idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);


                    if ((idxNow&fvalue[halfR]) == i)
                    {
                        weiRes[j]+=value->weight[pos];
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
void AuxoPro::sucessorQuery(basketNode* basketCurr,int level,unsigned int *poses, ul g1,vector<ul>& neis,ul prefix,int preLen,bool type){
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
void AuxoPro::nodeSuccessorQuery(string s1, vector<ul> &neis){
    ul hash1 = mpq->MPQHash(s1.c_str(), Type);
    ul g1 = hash1 & fvalue[f];
    if(g1==0) g1+=1;
    ul h1 = (hash1>>f)%w;

    ul k1 = (h1<<f)+g1;

    ul* tmp1 = new ul[r];
	unsigned int* poses=new unsigned int[r];
    tmp1[0] = g1;
    poses[0]=(h1 + tmp1[0]) % w;
    for (int i = 1; i < r; i++)
    {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
        poses[i]=(h1 + tmp1[i]) % w;
    }

    ul h=h1,g=g1;
    tmp1[0] = g;
    poses[0]=(h + tmp1[0]) % w;
    for (int i = 1; i < r; i++)
    {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
        poses[i]=(h + tmp1[i]) % w;
    }
    sucessorQuery(mainTree,0,poses,g,neis,0,0,true);
    sucessorQuery(deputyTree,0,poses,g,neis,0,0,true);

    return ;
}

bool AuxoPro::reachQuery(string s1,string s2,double &queryTime){
    if(s1==s2)return true;
    timeval t_start, t_end;
    gettimeofday( &t_start, NULL);

    ul hash1 = mpq->MPQHash(s1.c_str(), Type);
    ul hash2 = mpq->MPQHash(s2.c_str(), Type);
    ul g1 = hash1 & fvalue[f];
    if(g1==0) g1+=1;
    ul h1 = (hash1>>f)%w;
    ul g2 = hash2 & fvalue[f];
    if(g2==0) g2+=1;
    ul h2 = (hash2>>f)%w;
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
        sucessorQuery(deputyTree,0,poses1,g1,neis,0,0,false);
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
        sucessorQuery(deputyTree,0,poses2,g2,neis,0,0,true);
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

void AuxoPro::findAllEdgeForTriangle(GSketch* gs,ul preLen1,ul preLen2,ul prefix1, ul prefix2, int level,basketNode* basketCurr){
    if(!basketCurr)return;

    int srcFigSize=f-(level>>1),dstFigSize=f-(level>>1)-(level&0x1);
    //ul oriSrcFig=0,oriDstFig=0;
    if(basketCurr->value){
        Gbasket *value=basketCurr->value;
        for(int i=0;i<w;i++){
            for(int j=0;j<w;j++){
                int pos=i*w+j;
                if(value->weight[pos]){
                    //recover the hash value
                    int posIdx,posOffset;

                    posIdx=pos/fper[srcFigSize];
                    posOffset=pos%fper[srcFigSize];
                    ul srcFigNow=(((value->srcFig[posIdx])>>(posOffset*srcFigSize))&fvalue[srcFigSize]);

                    posIdx=pos/rper;
                    posOffset=pos%rper;
                    ul idxNow=(((value->idx[posIdx])>>(posOffset*rfnum))&fvalue[rfnum]);


                    srcFigNow<<=preLen1;
                    srcFigNow|=prefix1;
                    unsigned int idx1=(idxNow>>halfR);
                    ul originalH1=((calHash(srcFigNow,idx1,i,w)<<f)|srcFigNow);


                    posIdx=pos/fper[dstFigSize];
                    posOffset=pos%fper[dstFigSize];
                    ul dstFigNow=(((value->dstFig[posIdx])>>(posOffset*dstFigSize))&fvalue[dstFigSize]);


                    dstFigNow<<=preLen2;
                    dstFigNow|=prefix2;
                    unsigned int idx2=(idxNow&fvalue[halfR]);
                    ul originalH2=((calHash(dstFigNow,idx2,j,w)<<f)|dstFigNow);

                    gs->insert_node(originalH1,1);
                    gs->insert_node(originalH2,1);
                    gs->insert_edge(originalH1,originalH2);
                }
            }
        }
    }
    if(level&0x1){
        findAllEdgeForTriangle(gs,preLen1,preLen2+1,prefix1,prefix2,level+1,basketCurr->next[0]);
        findAllEdgeForTriangle(gs,preLen1,preLen2+1,prefix1,prefix2|(0x1<<preLen2),level+1,basketCurr->next[1]);
    }
    else{
        findAllEdgeForTriangle(gs,preLen1+1,preLen2,prefix1,prefix2,level+1,basketCurr->next[0]);
        findAllEdgeForTriangle(gs,preLen1+1,preLen2,prefix1|(0x1<<preLen1),prefix2,level+1,basketCurr->next[1]);
    }
}


int AuxoPro::triangleCounting()
{
    int res;
	GSketch* gs = new GSketch;
	findAllEdgeForTriangle(gs,0,0,0,0,0,mainTree);
	findAllEdgeForTriangle(gs,0,0,0,0,0,deputyTree);

	gs->clean();
	res=gs->countTriangle();
	delete gs;
	return res;
}
