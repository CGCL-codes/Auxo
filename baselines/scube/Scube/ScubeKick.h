#ifndef _SCUBEKICK_H
#define _SCUBEKICK_H
#define MAX_LOAD 85
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <cstdlib>
#include <bitset>
#include <memory.h>
#include <malloc.h>
#include <ctime>
#include "Scube.h"
#include "../headers/HashFunction.h"
#include "../headers/params.h"
#include "../DegDetector/DegDetectorSlot2bit.h"

using namespace std;

#ifdef H1
#define HASH 1
#elif H2
#define HASH 2
#elif H3
#define HASH 3
#elif H4
#define HASH 4
#elif H5
#define HASH 5
#elif H6
#define HASH 6
#else
#define HASH 0
#endif

class ScubeKick: public Scube
{
private:
    struct mapnode
    {
        addr_type addr;
        fp_type fp;
    };
    int n = 0;
    const uint8_t kick_times = 10;
    const uint32_t slot_num = 1000;
    //basket* value = NULL;
    vector<basket*> values;
    //DegDetector* degDetector = NULL;
    vector<DegDetector*> degDetectors;
    uint32_t s_extend = 0, d_extend = 0, max_kick_extend = 0;
    bool slot2bit = false;
    double exp_deg;
    int ignore_bits;
    int reserved_bits;
    double alpha;
    int matrxi_num;
    int max_num, inserted_num;

public:
    // DegSlot2bit
    ScubeKick(uint32_t width, uint32_t depth, uint8_t fingerprint_length, uint32_t kick_times, uint32_t slot_num, double exp_deg, int ignore_bits, int reserved_bits, double alpha);
    ~ScubeKick() override;
    bool insert(string s, string d, w_type w) override;
    bool  insert(string s, string d, w_type w, double& kick_time, double& detector_ins_time) override;
    w_type edgeWeightQuery(string s, string d) override;
    uint32_t nodeWeightQuery(string v, int type) override;
    uint32_t nodeWeightQuery(string v, int type, double& matrix_time, double& addr_time) override;
    bool reachabilityQuery(string s, string d) override;
    bool reachabilityQuery(string s, string d, int& points, int& lines) override;
    void printUsageInfo() override;
    void printExtendAddrInfo() override
    {
        cout << "s_extend = " << s_extend << endl;
        cout << "d_extend = " << d_extend << endl;
        cout << "max_kick_extend = " << max_kick_extend << endl;
    }
    void extendMatrx(){
        cout<<"we extend a new matrix "<<matrxi_num<<endl;
        inserted_num=0;
        matrxi_num++;
        auto* value = (basket *) memalign(64, sizeof(basket) * width * depth);	            // 64-byte alignment of the requested space
        memset(value, 0, sizeof(basket) * width * depth);
        values.push_back(value);
        degDetectors.push_back(new DegDetectorSlot2bit(width, depth, slot_num, exp_deg, ignore_bits, reserved_bits, alpha));
    }

private:
    int getMinIndex(uint16_t * a, int length);
    uint32_t recoverAddrSeed(fp_type fp, uint16_t idx, addr_type addr, uint32_t mod);

    int insert_matrix(int cou, addr_type& addr_s, fp_type& fp_s, addr_type& addr_d, fp_type& fp_d, w_type& w, addr_type& idx_s,
                      addr_type& idx_d, addr_type* addr_rows, addr_type* addr_cols,
                      int& back_pos, int & back_k, int &back_idx, int& back_num, bool insert_flag = false);
    bool kick_out(addr_type& addr_s, fp_type& fp_s, addr_type& addr_d, fp_type& fp_d, w_type& w, addr_type& idx_s, addr_type& idx_d, addr_type* addr_row, addr_type* addr_col, int& row_addrs, int& col_addrs);
};

// DegSlot2bit
ScubeKick::ScubeKick(uint32_t width, uint32_t depth, uint8_t fingerprint_length, uint32_t kick_times, uint32_t slot_num, double exp_deg, int ignore_bits, int reserved_bits, double alpha):
Scube(width, depth, fingerprint_length), kick_times(kick_times)
{
#if defined(DEBUG)
    cout << "ScubeKick::ScubeKick(width: " <<  width << ", depth: " << depth << ", fplen: " << (uint32_t) fingerprint_length
         << ", kick_times: " << kick_times << ", slot_num: " << slot_num << ", exp_deg: " << exp_deg
         << ", ignore_bits: " << ignore_bits << ", reserved_bits: " << reserved_bits << ", alpha: " << alpha << ")" << endl;
#endif
    this->slot2bit = true;
    this->exp_deg=exp_deg;
    this->ignore_bits=ignore_bits;
    this->reserved_bits=reserved_bits;
    this->alpha=alpha;
    s_extend = 0, d_extend = 0, max_kick_extend = 0;
    matrxi_num=0;
    max_num = (width * depth * MAX_LOAD*ROOM)/100;
    extendMatrx();
//    this->degDetector = new DegDetectorSlot2bit(width, depth, slot_num, exp_deg, ignore_bits, reserved_bits, alpha);
//    // this->value = new basket[width * depth];
//    // posix_memalign((void**)&(this->value), 64, sizeof(basket) * width * depth);		// 64-byte alignment of the requested space
//    // this->value = (basket *) aligned_alloc(64, sizeof(basket) * width * depth);	    // 64-byte alignment of the requested space
//    this->value = (basket *) memalign(64, sizeof(basket) * width * depth);	            // 64-byte alignment of the requested space
//    memset(value, 0, sizeof(basket) * width * depth);
}

ScubeKick::~ScubeKick()
{
    #if defined(DEBUG)
	cout << "ScubeKick::~ScubeKick()" << endl;
    #endif
    for(auto degDetector:degDetectors){
        delete degDetector;
    }
    // delete[] value;
    for(auto value : values){
        free(value);
    }
}

/***************private functions***************/
// return the min value index of array a
int ScubeKick::getMinIndex(uint16_t *a, int length)
{
    uint16_t min = a[0];
    int index = 0;
    for(int i = 1; i < length; i++)
    {
        if(a[i] < min)
        {
            min = a[i];
            index = i;
        }
    }
    return index;
}
// recover the address seed by the fingerprint and the address index
uint32_t ScubeKick::recoverAddrSeed(fp_type fp, uint16_t idx, addr_type addr, uint32_t mod)
{
    uint32_t res = 0;
    uint32_t shifter = fp;
    for (int v = 0; v < idx; v++)
    {
        shifter = (shifter * MUL + INC) % MOD;
    }
    uint32_t tmp_h = addr;
    while (tmp_h < shifter)
        tmp_h += mod;
    res = tmp_h - shifter;
    return res;
}
// -1: insert failed
// 0: insert succeed and the edge does not exist (update)
// 1: insert succeed and the edge exists
int ScubeKick::insert_matrix(int cou, addr_type& addr_s, fp_type& fp_s, addr_type& addr_d, fp_type& fp_d, w_type& w,
                             addr_type& idx_s, addr_type& idx_d, addr_type* addr_rows,
                             addr_type* addr_cols, int& back_pos, int & back_k, int &back_idx, int& back_num, bool insert_flag)
{
    //if(idx_d>60||idx_s>60)return -1;
    basket* value=values[cou];
    //  calculate the seeds
    uint32_t* seed1 = new uint32_t[idx_s];
    uint32_t* seed2 = new uint32_t[idx_d];
    seed1[0] = fp_s;
    for(int i = 1; i < idx_s; i++)
    {
        seed1[i] = (seed1[i - 1] * MUL + INC) % MOD;
    }
    seed2[0] = fp_d;
    for(int i = 1; i < idx_d; i++)
    {
        seed2[i] = (seed2[i - 1] * MUL + INC) % MOD;
    }

    // determine whether the current edge already exists in one of all the current candidate addresses
    bool empty = false;         // determine whether there is any empty address in the current alternative addresses
    uint16_t temp_idx = 0;
    int temp_k = -1;
    addr_type temp_pos = 0;

    for (int i = 0; i < idx_s; i++)
    {
        addr_rows[i] = (addr_s + seed1[i]) % depth;
        for (int j = 0; j < idx_d; j++)
        {
            addr_cols[j] = (addr_d + seed2[j]) % width;
            addr_type pos = addr_rows[i] * width + addr_cols[j];
            uint16_t idx = (i << 8) |  j;
            for (int k = 0; k < ROOM; ++k)
            {
                if((value[pos].fp_src[k] == fp_s) && (value[pos].fp_dst[k] == fp_d) && (value[pos].ext[k] == idx))
                {
                    value[pos].weight[k] += w;
                    delete[] seed1;
                    delete[] seed2;
                    return 1;
                }
                if(value[pos].fp_src[k] == 0 && !empty) // find an empty address
                {
                    empty = true;
                    temp_idx = idx;
                    temp_k = k;
                    temp_pos = pos;
                }
            }
        }
    }


    if(empty)
    {
        if(insert_flag){
            value[temp_pos].ext[temp_k] = temp_idx;
            value[temp_pos].fp_src[temp_k] = fp_s;
            value[temp_pos].fp_dst[temp_k] = fp_d;
            value[temp_pos].weight[temp_k] = w;
        }
        else{
            back_idx = temp_idx;
            back_pos = temp_pos;
            back_k = temp_k;
            back_num = cou;
        }
        delete[] seed1;
        delete[] seed2;
        return 0;
    }
    delete[] seed1;
    delete[] seed2;
    return -1;
}
// kick-out strategy
bool ScubeKick::kick_out(addr_type& addr_s, fp_type& fp_s, addr_type& addr_d, fp_type& fp_d, w_type& w,
                         addr_type& idx_s, addr_type& idx_d, addr_type* rows, addr_type* cols, int& row_kicks, int& col_kicks)
{
    basket* value=values[matrxi_num-1];
    DegDetector* degDetector = degDetectors[matrxi_num-1];
    uint16_t ext = ((idx_s - row_kicks) << 8) | (idx_d - col_kicks);
    int max_kick = 1000;
    bool inserted = false;
    uint32_t pre_row = -1, pre_col = -1;    // the address of the current kicking edge
    key_type key_s = ((addr_s << fingerprint_length) + fp_s);
    key_type key_d = ((addr_d << fingerprint_length) + fp_d);

    int back_pos=-1, back_k=-1, back_idx=-1, back_num=-1;

    for (int kick_num = 0; !inserted; kick_num++)
    {
        /* When the number of kicks reaches kick_times, then each time it is determined whether the kicked edge contains a high-degree node,
         * if so, we just directly extend the address; otherwise, continue to kick.
         * When the number of kicks reaches max_kick, we just directly extend the address
         */
        if(kick_num > kick_times && kick_num <= max_kick)       // determine whether the kicked edge contains a high-degree node
        {
            bool brk = false;
            // determine whether cur_key_s, cur_key_d are high-degree nodes
            addr_type n1 = degDetector->addrQuery(key_s, 0);
            addr_type n2 = degDetector->addrQuery(key_d, 1);

            if (n1 > 2)
            {
                // extend the row address
                bool ext = degDetector->extendAddr(key_s, n1 + 1, 0);     // record the node into the detector
                if (!ext)
                    cout << "extend address error! " << endl;
                brk = true;
                s_extend++;
            }
            if (n2 > 2)
            {
                // extend the column address
                bool ext = degDetector->extendAddr(key_d, n2 + 1, 1);     // record the node into the detector
                if (!ext)
                    cout << "extend address error!" << endl;
                brk = true;
                d_extend++;
            }
            if (brk)
                break;
        }
        else if (kick_num > max_kick)
        {
            max_kick_extend++;
            // directly treat the current node as a high-degree node and just extend the addresses (increase 1)
            addr_type n1 = degDetector->addrQuery(key_s, 0);
            addr_type n2 = degDetector->addrQuery(key_d, 1);

            // extend the row address
            bool ext1 = degDetector->extendAddr(key_s, n1 + 1, 0);    // record the node into the detector
            if (!ext1)
                cout << "extend address error!" << endl;

            // extend the column address
            bool ext2 = degDetector->extendAddr(key_d, n2 + 1, 1);    // record the node into the detector
            if (!ext2)
                cout << "extend address error!" << endl;
            break;
        }

        // find the one with the smallest index among the (Roomnum * row_addrs * col_addrs) alternative addresses
        uint16_t* kick_index = new uint16_t[ROOM * row_kicks * col_kicks];
        memset(kick_index, -1, ROOM * row_kicks * col_kicks * sizeof(uint16_t));
        for (int i = 0; i < row_kicks; i++)
        {
            for (int j = 0; j < col_kicks; j++)
            {
                if ((rows[i] == pre_row) && (cols[j] == pre_col))
                    continue;
                uint32_t pos = rows[i] * width + cols[j];
                for (int k = 0; k < ROOM; k++)
                {
                    kick_index[col_kicks * ROOM * i + ROOM * j + k]
                    = value[pos].
                            ext[k];
                }
            }
        }

        uint16_t idx = getMinIndex(kick_index, ROOM * row_kicks * col_kicks);
        if (idx == -1) {
            cout << "Error: ScubeKick:kick_out()~line 288" << endl;
        }
        delete[] kick_index;
        uint16_t cur_i = (idx & 4) >> 2;
        uint16_t cur_j = (idx & 2) >> 1;
        uint16_t cur_k = (idx & 1);

        pre_row = rows[cur_i];
        pre_col = cols[cur_j];

        // put the kicked edge into the "kick_" field
        addr_type kick_pos = rows[cur_i] * width + cols[cur_j];
        fp_type kick_fp_s = value[kick_pos].fp_src[cur_k];
        fp_type kick_fp_d = value[kick_pos].fp_dst[cur_k];
        w_type kick_fp_w = value[kick_pos].weight[cur_k];
        uint16_t kick_ext = value[kick_pos].ext[cur_k];

        // insert the current edge into the matrix
        value[kick_pos].fp_src[cur_k] = fp_s;
        value[kick_pos].fp_dst[cur_k] = fp_d;
        value[kick_pos].weight[cur_k] = w;
        value[kick_pos].ext[cur_k] = ((ext & 0xff00) + (cur_i << 8)) | ((ext & 0xff) + cur_j);

        // put the kicked edge information into the (fp_s, fp_d, w) fields, and continue to look for the bucket to insert
        fp_s = kick_fp_s;
        fp_d = kick_fp_d;
        w = kick_fp_w;

        // restore addr_seed according to the fingerprint and address rounds
        addr_s = recoverAddrSeed(kick_fp_s, (kick_ext >> 8), rows[cur_i], depth);
        addr_d = recoverAddrSeed(kick_fp_d, (kick_ext & 0xff), cols[cur_j], width);

        key_s = ((addr_s << fingerprint_length) + fp_s);
        key_d = ((addr_d << fingerprint_length) + fp_d);
        idx_s = degDetector->addrQuery(key_s, 0);
        idx_d = degDetector->addrQuery(key_d, 1);

        addr_type* addr_rows = new addr_type[idx_s];
        addr_type* addr_cols = new addr_type[idx_d];
        // insert to the hash matrix
        int res = insert_matrix(matrxi_num-1,addr_s, fp_s, addr_d, fp_d, w, idx_s,
                                idx_d, addr_rows, addr_cols, back_pos, back_k, back_idx, back_num, true);
        if (res != -1)
        {
            inserted = true;
        }
        // kick in the current "latest" two lines
        row_kicks = 2;
        col_kicks = 2;

        ext = ((idx_s - row_kicks) << 8) | (idx_d - col_kicks);

        for (int i = 0; i < row_kicks; i++) {
            rows[i] = addr_rows[idx_s - row_kicks + i];
        }
        for (int i = 0; i < col_kicks; i++) {
            cols[i] = addr_cols[idx_d - col_kicks + i];
        }
        delete[] addr_rows;
        delete[] addr_cols;
    }
    if (!inserted)
    {
        addr_type* addr_rows = new addr_type[idx_s];
        addr_type* addr_cols = new addr_type[idx_d];
        // insert to the hash matrix
        int res = insert_matrix(matrxi_num-1,addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, addr_rows, addr_cols,back_pos, back_k, back_idx, back_num, true);
        if (res == -1)
        {
            // kick in the current "latest" two lines
            int row_kicks = 2;
            int col_kicks = 2;
            addr_type* rows = new addr_type[255];
            addr_type* cols = new addr_type[255];

            for (int i = 0; i < row_kicks; i++) {
                rows[i] = addr_rows[idx_s - row_kicks + i];
            }
            for (int i = 0; i < col_kicks; i++) {
                cols[i] = addr_cols[idx_d - col_kicks + i];
            }
            inserted = kick_out(addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, rows, cols, row_kicks, col_kicks);
            delete[] rows;
            delete[] cols;
        }
        else
        {
            inserted = true;
        }
        delete[] addr_rows;
        delete[] addr_cols;
    }
    return inserted;
}
/***********************************************/
// s is the ID of the source node, d is the ID of the destination node, w is the edge weight.
bool ScubeKick::insert(string s, string d, w_type w)
{
    hash_type hash_s = (*hfunc[HASH])((unsigned char*)(s.c_str()), s.length());
    hash_type hash_d = (*hfunc[HASH])((unsigned char*)(d.c_str()), d.length());
    hash_type mask = (1 << fingerprint_length) - 1;

    addr_type addr_s = (hash_s >> fingerprint_length) % depth;
    addr_type addr_d = (hash_d >> fingerprint_length) % width;

    fp_type fp_s = hash_s & mask;
    fp_type fp_d = hash_d & mask;
    if(fp_s == 0) fp_s = 1;
    if(fp_d == 0) fp_d = 1;

    key_type key_s = (addr_s << fingerprint_length) + fp_s;
    key_type key_d = (addr_d << fingerprint_length) + fp_d;

    if (!this->slot2bit)
    {
//        // insert to the degDetector
//        if (!degDetector->insert(key_s, key_d, hash_s, hash_d))
//        {
//            cout << "Failed in inserting (" << s << ", " << d << ") to degree detector!" << endl;
//        }
//        // query the number of alternative addresses of each node
//        addr_type idx_s = degDetector->addrQuery(key_s, 0);
//        addr_type idx_d = degDetector->addrQuery(key_d, 1);
//        addr_type* addr_rows = new addr_type[idx_s];
//        addr_type* addr_cols = new addr_type[idx_d];
//        // insert to the hash matrix
//        int res = insert_matrix(addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, addr_rows, addr_cols);
//        bool inserted = false;
//        if (res == -1)
//        {
//            int row_kicks = 2;
//            int col_kicks = 2;
//            addr_type* rows = new addr_type[255];
//            addr_type* cols = new addr_type[255];
//
//            for (int i = 0; i < row_kicks; i++) {
//                rows[i] = addr_rows[idx_s - row_kicks + i];
//            }
//            for (int i = 0; i < col_kicks; i++) {
//                cols[i] = addr_cols[idx_d - col_kicks + i];
//            }
//            inserted = kick_out(addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, rows, cols, row_kicks, col_kicks);
//            delete[] rows;
//            delete[] cols;
//            if (!inserted)
//            {
//                cout << "Failed in inserting edge(" << s << ", " << d << ", " << w << ")!" << endl;
//            }
//        }
//        else
//        {
//            inserted = true;
//        }
//        delete[] addr_rows;
//        delete[] addr_cols;
//        return inserted;
    }
    else
    {
        // query the number of alternative addresses of each node

        addr_type idx_s;
        addr_type idx_d;
        addr_type* addr_rows = nullptr;
        addr_type* addr_cols = nullptr;

        int back_pos=-1, back_k=-1, back_idx=-1, back_num=-1;
        bool inserted = false;
        for(int i=0;i<matrxi_num;i++){
            if(addr_cols){
                free(addr_cols);
            }
            if(addr_rows){
                free(addr_rows);
            }
            idx_s = degDetectors[i]->addrQuery(key_s, 0);
            idx_d = degDetectors[i]->addrQuery(key_d, 1);
            addr_rows = new addr_type[idx_s];
            addr_cols = new addr_type[idx_d];

            // insert to the hash matrix
            int res = insert_matrix(i,addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, addr_rows, addr_cols, back_pos,back_k, back_idx,back_num);
            if(res==1){
                inserted=true;
                break;
            }
        }
        if(!inserted){
            if(back_num!=-1){
                inserted=true;
                basket * value=values[back_num];
                value[back_pos].ext[back_k] = back_idx;
                value[back_pos].fp_src[back_k] = fp_s;
                value[back_pos].fp_dst[back_k] = fp_d;
                value[back_pos].weight[back_k] = w;
                if(back_num == matrxi_num-1){
                    // update the degDetector
                    if (!degDetectors[back_num]->insert(key_s, key_d, hash_s, hash_d))
                    {
                        cout << "Failed in inserting (" << s << ", " << d << ") to degree detector!" << endl;
                    }
                    inserted_num++;
                }
            }
            else{
                if(inserted_num>max_num){
                    extendMatrx();
                    if(addr_cols){
                        free(addr_cols);
                    }
                    if(addr_rows){
                        free(addr_rows);
                    }
                    idx_s = degDetectors[matrxi_num-1]->addrQuery(key_s, 0);
                    idx_d = degDetectors[matrxi_num-1]->addrQuery(key_d, 1);
                    addr_rows = new addr_type[idx_s];
                    addr_cols = new addr_type[idx_d];

                    // insert to the hash matrix
                    insert_matrix(matrxi_num-1,addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, addr_rows, addr_cols, back_pos,back_k, back_idx,back_num,true);
                    inserted=true;
                }
                else{
                    int row_kicks = 2;
                    int col_kicks = 2;
                    auto* rows = new addr_type[255];
                    auto* cols = new addr_type[255];

                    for (int i = 0; i < row_kicks; i++) {
                        rows[i] = addr_rows[idx_s - row_kicks + i];
                    }
                    for (int i = 0; i < col_kicks; i++) {
                        cols[i] = addr_cols[idx_d - col_kicks + i];
                    }
                    inserted = kick_out(addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, rows, cols, row_kicks, col_kicks);
                    delete[] rows;
                    delete[] cols;
                    if (!inserted)
                    {
                        cout << "Failed in inserting edge(" << s << ", " << d << ", " << w << ")!" << endl;
                    }
                    else{
                        inserted_num++;
                    }
                }
            }
        }
        delete[] addr_rows;
        delete[] addr_cols;
        return inserted;
    }
    return true;
}


// s is the ID of the source node, d is the ID of the destination node, w is the edge weight.
bool ScubeKick::insert(string s, string d, w_type w, double& kick_time, double& detector_ins_time)
{
    hash_type hash_s = (*hfunc[HASH])((unsigned char*)(s.c_str()), s.length());
    hash_type hash_d = (*hfunc[HASH])((unsigned char*)(d.c_str()), d.length());
    hash_type mask = (1 << fingerprint_length) - 1;

    addr_type addr_s = (hash_s >> fingerprint_length) % depth;
    addr_type addr_d = (hash_d >> fingerprint_length) % width;

    fp_type fp_s = hash_s & mask;
    fp_type fp_d = hash_d & mask;
    if(fp_s == 0) fp_s = 1;
    if(fp_d == 0) fp_d = 1;

    key_type key_s = (addr_s << fingerprint_length) + fp_s;
    key_type key_d = (addr_d << fingerprint_length) + fp_d;

    if (!this->slot2bit)
    {
//        // insert to the degDetector
//        if (!degDetector->insert(key_s, key_d, hash_s, hash_d))
//        {
//            cout << "Failed in inserting (" << s << ", " << d << ") to degree detector!" << endl;
//        }
//        // query the number of alternative addresses of each node
//        addr_type idx_s = degDetector->addrQuery(key_s, 0);
//        addr_type idx_d = degDetector->addrQuery(key_d, 1);
//        addr_type* addr_rows = new addr_type[idx_s];
//        addr_type* addr_cols = new addr_type[idx_d];
//        // insert to the hash matrix
//        int res = insert_matrix(addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, addr_rows, addr_cols);
//        bool inserted = false;
//        if (res == -1)
//        {
//            int row_kicks = 2;
//            int col_kicks = 2;
//            addr_type* rows = new addr_type[255];
//            addr_type* cols = new addr_type[255];
//
//            for (int i = 0; i < row_kicks; i++) {
//                rows[i] = addr_rows[idx_s - row_kicks + i];
//            }
//            for (int i = 0; i < col_kicks; i++) {
//                cols[i] = addr_cols[idx_d - col_kicks + i];
//            }
//            inserted = kick_out(addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, rows, cols, row_kicks, col_kicks);
//            delete[] rows;
//            delete[] cols;
//            if (!inserted)
//            {
//                cout << "Failed in inserting edge(" << s << ", " << d << ", " << w << ")!" << endl;
//            }
//        }
//        else
//        {
//            inserted = true;
//        }
//        delete[] addr_rows;
//        delete[] addr_cols;
//        return inserted;
    }
    else
    {
        // query the number of alternative addresses of each node

        addr_type idx_s;
        addr_type idx_d;
        addr_type* addr_rows = nullptr;
        addr_type* addr_cols = nullptr;

        int back_pos=-1, back_k=-1, back_idx=-1, back_num=-1;
        bool inserted = false;
        for(int i=0;i<matrxi_num;i++){
            if(addr_cols){
                free(addr_cols);
            }
            if(addr_rows){
                free(addr_rows);
            }
            idx_s = degDetectors[i]->addrQuery(key_s, 0);
            idx_d = degDetectors[i]->addrQuery(key_d, 1);
            addr_rows = new addr_type[idx_s];
            addr_cols = new addr_type[idx_d];

            // insert to the hash matrix
            int res = insert_matrix(i,addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, addr_rows, addr_cols, back_pos,back_k, back_idx,back_num);
            if(res==1){
                inserted=true;
                break;
            }
        }
        if(!inserted){
            if(back_num!=-1){
                inserted=true;
                basket * value=values[back_num];
                value[back_pos].ext[back_k] = back_idx;
                value[back_pos].fp_src[back_k] = fp_s;
                value[back_pos].fp_dst[back_k] = fp_d;
                value[back_pos].weight[back_k] = w;
                if(back_num == matrxi_num-1){
                    // update the degDetector
                    if (!degDetectors[back_num]->insert(key_s, key_d, hash_s, hash_d))
                    {
                        cout << "Failed in inserting (" << s << ", " << d << ") to degree detector!" << endl;
                    }
                    inserted_num++;
                }
            }
            else{
                if(inserted_num>max_num){
                    extendMatrx();
                    if(addr_cols){
                        free(addr_cols);
                    }
                    if(addr_rows){
                        free(addr_rows);
                    }
                    idx_s = degDetectors[matrxi_num-1]->addrQuery(key_s, 0);
                    idx_d = degDetectors[matrxi_num-1]->addrQuery(key_d, 1);
                    addr_rows = new addr_type[idx_s];
                    addr_cols = new addr_type[idx_d];

                    // insert to the hash matrix
                    insert_matrix(matrxi_num-1,addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, addr_rows, addr_cols, back_pos,back_k, back_idx,back_num,true);
                    inserted=true;
                }
                else{
                    int row_kicks = 2;
                    int col_kicks = 2;
                    auto* rows = new addr_type[255];
                    auto* cols = new addr_type[255];

                    for (int i = 0; i < row_kicks; i++) {
                        rows[i] = addr_rows[idx_s - row_kicks + i];
                    }
                    for (int i = 0; i < col_kicks; i++) {
                        cols[i] = addr_cols[idx_d - col_kicks + i];
                    }
                    inserted = kick_out(addr_s, fp_s, addr_d, fp_d, w, idx_s, idx_d, rows, cols, row_kicks, col_kicks);
                    delete[] rows;
                    delete[] cols;
                    if (!inserted)
                    {
                        cout << "Failed in inserting edge(" << s << ", " << d << ", " << w << ")!" << endl;
                    }
                    else{
                        inserted_num++;
                    }
                }
            }
        }
        delete[] addr_rows;
        delete[] addr_cols;
        return inserted;
    }
    return true;
}

w_type ScubeKick::edgeWeightQuery(string s, string d) // s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
{
    hash_type hash_s = (*hfunc[HASH])((unsigned char*)(s.c_str()), s.length());
    hash_type hash_d = (*hfunc[HASH])((unsigned char*)(d.c_str()), d.length());
    hash_type mask = (1 << fingerprint_length) - 1;
    addr_type addr_s = (hash_s >> fingerprint_length) % depth;
    addr_type addr_d = (hash_d >> fingerprint_length) % width;

    fp_type fp_s = hash_s & mask;
    fp_type fp_d = hash_d & mask;
    if(fp_s == 0) fp_s = 1;
    if(fp_d == 0) fp_d = 1;

    key_type key_s = (addr_s << fingerprint_length) + fp_s;
    key_type key_d = (addr_d << fingerprint_length) + fp_d;

    // query the address number of the current node
    basket* value;
    DegDetector *degDetector;
    addr_type map_idx_s;
    addr_type map_idx_d;
    uint32_t* seed1 = nullptr;
    uint32_t* seed2 = nullptr;
    w_type res = 0;
    int hit = 0;
    for(int cou=0; cou<matrxi_num; cou++){
        value= values[cou];
        degDetector = degDetectors[cou];
        if(seed1){
            free(seed1);
        }
        if(seed2){
            free(seed2);
        }
        map_idx_s = degDetector->addrQuery(key_s, 0);
        map_idx_d = degDetector->addrQuery(key_d, 1);

        // calculate the seeds
        seed1 = new uint32_t[map_idx_s];
        seed2 = new uint32_t[map_idx_d];
        seed1[0] = fp_s;
        for(int i = 1; i < map_idx_s; i++)
        {
            seed1[i] = (seed1[i - 1] * MUL + INC) % MOD;
        }
        seed2[0] = fp_d;
        for(int i = 1; i < map_idx_d; i++)
        {
            seed2[i] = (seed2[i - 1] * MUL + INC) % MOD;
        }

        // query
        for (int i = 0; i < map_idx_s; ++i)
        {
            uint32_t addr_row = (addr_s + seed1[i]) % depth;
            for (int j = 0; j < map_idx_d; ++j)
            {
                uint32_t addr_col = (addr_d + seed2[j]) % width;
                uint32_t pos = addr_row * width + addr_col;
                uint16_t idx = (i << 8) |  j;
                for (int k = 0; k < ROOM; ++k)
                {
                    if((value[pos].fp_src[k] == fp_s) && (value[pos].fp_dst[k] == fp_d) && (value[pos].ext[k] == idx))
                    {
                        // delete[] seed1;
                        // delete[] seed2;
                        // return value[pos].weight[k];
                        hit++;
                        res += value[pos].weight[k];
                    }
                }
            }
        }
    }

    delete[] seed1;
    delete[] seed2;
    // if (hit == 0 || hit > 1)
    //     cout << "hit = " << hit << endl;
    return res;
}
/* type 0 is for successor query, type 1 is for precusor query */
uint32_t ScubeKick::nodeWeightQuery(string v, int type)
{
    uint32_t sum_weight = 0;
    hash_type hash_v = (*hfunc[HASH])((unsigned char*)(v.c_str()), v.length());
    hash_type mask = (1 << fingerprint_length) - 1;
    fp_type fp_v = hash_v & mask;
    if(fp_v == 0) fp_v = 1;

    for(int cou=0;cou<matrxi_num;cou++) {
        basket *value = values[cou];
        DegDetector *degDetector = degDetectors[cou];

        if(type == 0)
        {
            addr_type addr_v = (hash_v >> fingerprint_length) % depth;
            key_type key_v = (addr_v << fingerprint_length) + fp_v;
            // query the address number of the current node
            addr_type map_idx_s = degDetector->addrQuery(key_v, 0);
            // calculate the seeds
            uint32_t* seed1 = new uint32_t[map_idx_s];
            seed1[0] = fp_v;
            for(int i = 1; i < map_idx_s; i++)
            {
                seed1[i] = (seed1[i - 1] * MUL + INC) % MOD;
            }

            for (int i = 0; i < map_idx_s; i++)
            {
                addr_type addr_row = ((addr_v + seed1[i]) % depth);
                for (int j = 0; j < width; j++)
                {
                    addr_type pos = addr_row * width + j;
                    for (int k = 0; k < ROOM; k++)
                    {
                        if((value[pos].fp_src[k] == fp_v) && ((value[pos].ext[k] >> 8) == i)) {
                            sum_weight += value[pos].weight[k];
                        }
                    }
                }
            }
            delete[] seed1;
        }
        else if (type == 1)
        {
            addr_type addr_v = (hash_v >> fingerprint_length) % width;
            key_type key_v = (addr_v << fingerprint_length) + fp_v;
            // query the address number of the current node
            addr_type map_idx_d = degDetector->addrQuery(key_v, 1);
            // calculate the seeds
            uint32_t* seed2 = new uint32_t[map_idx_d];
            seed2[0] = fp_v;
            for(int i = 1; i < map_idx_d; i++)
            {
                seed2[i] = (seed2[i - 1] * MUL + INC) % MOD;
            }

            for (int i = 0; i < depth; i++)
            {
                for (int j = 0; j < map_idx_d; ++j)
                {
                    addr_type addr_col = (addr_v + seed2[j]) % width;
                    addr_type pos = i * width + addr_col;
                    for (int k = 0; k < ROOM; k++)
                    {
                        if((value[pos].fp_dst[k] == fp_v) && ((value[pos].ext[k] & 0xff) == j)) {
                            sum_weight += value[pos].weight[k];
                        }
                    }
                }
            }
            delete[] seed2;
        }
    }

    return sum_weight;
}

/* type 0 is for successor query, type 1 is for precusor query */
// v is the ID of the queried node, function for node query.
uint32_t ScubeKick::nodeWeightQuery(string v, int type, double& matrix_time, double& addr_time)
{
    uint32_t sum_weight = 0;
    hash_type hash_v = (*hfunc[HASH])((unsigned char*)(v.c_str()), v.length());
    hash_type mask = (1 << fingerprint_length) - 1;
    fp_type fp_v = hash_v & mask;
    if(fp_v == 0) fp_v = 1;

    for(int cou=0;cou<matrxi_num;cou++) {
        basket *value = values[cou];
        DegDetector *degDetector = degDetectors[cou];

        if(type == 0)
        {
            addr_type addr_v = (hash_v >> fingerprint_length) % depth;
            key_type key_v = (addr_v << fingerprint_length) + fp_v;
            // query the address number of the current node
            addr_type map_idx_s = degDetector->addrQuery(key_v, 0);
            // calculate the seeds
            uint32_t* seed1 = new uint32_t[map_idx_s];
            seed1[0] = fp_v;
            for(int i = 1; i < map_idx_s; i++)
            {
                seed1[i] = (seed1[i - 1] * MUL + INC) % MOD;
            }

            for (int i = 0; i < map_idx_s; i++)
            {
                addr_type addr_row = ((addr_v + seed1[i]) % depth);
                for (int j = 0; j < width; j++)
                {
                    addr_type pos = addr_row * width + j;
                    for (int k = 0; k < ROOM; k++)
                    {
                        if((value[pos].fp_src[k] == fp_v) && ((value[pos].ext[k] >> 8) == i)) {
                            sum_weight += value[pos].weight[k];
                        }
                    }
                }
            }
            delete[] seed1;
        }
        else if (type == 1)
        {
            addr_type addr_v = (hash_v >> fingerprint_length) % width;
            key_type key_v = (addr_v << fingerprint_length) + fp_v;
            // query the address number of the current node
            addr_type map_idx_d = degDetector->addrQuery(key_v, 1);
            // calculate the seeds
            uint32_t* seed2 = new uint32_t[map_idx_d];
            seed2[0] = fp_v;
            for(int i = 1; i < map_idx_d; i++)
            {
                seed2[i] = (seed2[i - 1] * MUL + INC) % MOD;
            }

            for (int i = 0; i < depth; i++)
            {
                for (int j = 0; j < map_idx_d; ++j)
                {
                    addr_type addr_col = (addr_v + seed2[j]) % width;
                    addr_type pos = i * width + addr_col;
                    for (int k = 0; k < ROOM; k++)
                    {
                        if((value[pos].fp_dst[k] == fp_v) && ((value[pos].ext[k] & 0xff) == j)) {
                            sum_weight += value[pos].weight[k];
                        }
                    }
                }
            }
            delete[] seed2;
        }
    }

    return sum_weight;
}

// s is the ID of the source node, d is the ID of the destination node, return whether reachable.
bool ScubeKick::reachabilityQuery(string s, string d)
{
    hash_type hash_s = (*hfunc[HASH])((unsigned char*)(s.c_str()), s.length());
    hash_type hash_d = (*hfunc[HASH])((unsigned char*)(d.c_str()), d.length());
    hash_type mask = (1 << fingerprint_length) - 1;

    addr_type addr_s = (hash_s >> fingerprint_length) % depth;
    addr_type addr_d = (hash_d >> fingerprint_length) % width;

    fp_type fp_s = hash_s & mask;
    fp_type fp_d = hash_d & mask;
    if(fp_s == 0) fp_s = 1;
    if(fp_d == 0) fp_d = 1;

    key_type key_s = (addr_s << fingerprint_length) + fp_s;
    key_type key_d = (addr_d << fingerprint_length) + fp_d;

	int pos;
	map<key_type, bool> checked;
	queue<mapnode> q;
	mapnode e;
	e.addr = addr_s;
	e.fp = fp_s;
	q.push(e);
	checked[key_s] = true;
	map<unsigned int, bool>::iterator IT;

    addr_type temp_addr;
    fp_type temp_fp;
    addr_type ad_1, ad_2;

	while (!q.empty())
	{
		e = q.front();
		temp_addr = e.addr;
		temp_fp = e.fp;
        key_type temp_key = (temp_addr << fingerprint_length) + temp_fp;
        basket *value;
        DegDetector *degDetector;
        for(int cou=0;cou<matrxi_num;cou++){
            value=values[cou];
            degDetector=degDetectors[cou];

            ad_1 = degDetector->addrQuery(temp_key, 0);
            ad_2 = degDetector->addrQuery(key_d, 1);

            int* tmp1 = new int[ad_1];
            int* tmp2 = new int[ad_2];
            tmp1[0] = temp_fp;
            tmp2[0] = fp_d;

            for (int i = 1; i < ad_1; i++)
            {
                tmp1[i] = (tmp1[i - 1] * MUL + INC) % MOD;
            }
            for (int i = 1; i < ad_2; i++)
            {
                tmp2[i] = (tmp2[i - 1] * MUL + INC) % MOD;
            }
            for (int i1 = 0; i1 < ad_1; i1++)
            {
                int p1 = (temp_addr + tmp1[i1]) % depth;
                for (int i2 = 0; i2 < ad_2; i2++)
                {
                    int p2 = (addr_d + tmp2[i2]) % width;
                    int pos = p1 * width + p2;
                    uint16_t idx = (i1 << 8) | i2;
                    for (int i3 = 0; i3 < ROOM; i3++)
                    {
                        if ((value[pos].ext[i3] == idx) && (value[pos].fp_src[i3] == temp_fp) && (value[pos].fp_dst[i3] == fp_d))
                        {
                            delete []tmp1;
                            delete []tmp2;
                            return true;
                        }
                    }
                }
            }
            // find in matrix
            for (int i1 = 0; i1 < ad_1; i1++)
            {
                int p1 = (temp_addr + tmp1[i1]) % depth;
                for (int i2 = 0; i2 < width; i2++)
                {
                    int pos = p1 * width + i2;
                    for (int i3 = 0; i3 < ROOM; i3++)
                    {
                        if (value[pos].fp_src[i3] == temp_fp && ((value[pos].ext[i3] >> 8) == i1))
                        {
                            uint32_t tmp_g = value[pos].fp_dst[i3];
                            int tmp_s = value[pos].ext[i3] & 0xff;

                            uint32_t shifter = tmp_g;
                            for (int v = 0; v < tmp_s; v++)
                                shifter = (shifter * MUL + INC) % MOD;
                            uint32_t tmp_h = i2;
                            while (tmp_h < shifter)
                                tmp_h += width;
                            tmp_h -= shifter;

                            key_type val = (tmp_h << fingerprint_length) + tmp_g;

                            IT = checked.find(val);
                            if (IT == checked.end())
                            {
                                mapnode tmp_e;
                                tmp_e.addr = tmp_h;
                                tmp_e.fp = tmp_g;
                                q.push(tmp_e);
                                checked[val] = true;
                            }
                        }

                    }
                }
            }
            delete[] tmp1;
            delete[] tmp2;
        }

		q.pop();
	}
	return false;
}

bool ScubeKick::reachabilityQuery(string s, string d, int& points, int& lines)
{
    points = 0;
    lines = 0;
    hash_type hash_s = (*hfunc[HASH])((unsigned char*)(s.c_str()), s.length());
    hash_type hash_d = (*hfunc[HASH])((unsigned char*)(d.c_str()), d.length());
    hash_type mask = (1 << fingerprint_length) - 1;

    addr_type addr_s = (hash_s >> fingerprint_length) % depth;
    addr_type addr_d = (hash_d >> fingerprint_length) % width;

    fp_type fp_s = hash_s & mask;
    fp_type fp_d = hash_d & mask;
    if(fp_s == 0) fp_s = 1;
    if(fp_d == 0) fp_d = 1;

    key_type key_s = (addr_s << fingerprint_length) + fp_s;
    key_type key_d = (addr_d << fingerprint_length) + fp_d;

    int pos;
    map<key_type, bool> checked;
    queue<mapnode> q;
    mapnode e;
    e.addr = addr_s;
    e.fp = fp_s;
    q.push(e);
    checked[key_s] = true;
    map<unsigned int, bool>::iterator IT;

    addr_type temp_addr;
    fp_type temp_fp;
    addr_type ad_1, ad_2;

    while (!q.empty())
    {
        e = q.front();
        points++;
        temp_addr = e.addr;
        temp_fp = e.fp;
        key_type temp_key = (temp_addr << fingerprint_length) + temp_fp;
        basket *value;
        DegDetector *degDetector;
        for(int cou=0;cou<matrxi_num;cou++){
            value=values[cou];
            degDetector=degDetectors[cou];

            ad_1 = degDetector->addrQuery(temp_key, 0);
            ad_2 = degDetector->addrQuery(key_d, 1);
            lines+=ad_1;

            int* tmp1 = new int[ad_1];
            int* tmp2 = new int[ad_2];
            tmp1[0] = temp_fp;
            tmp2[0] = fp_d;

            for (int i = 1; i < ad_1; i++)
            {
                tmp1[i] = (tmp1[i - 1] * MUL + INC) % MOD;
            }
            for (int i = 1; i < ad_2; i++)
            {
                tmp2[i] = (tmp2[i - 1] * MUL + INC) % MOD;
            }
            for (int i1 = 0; i1 < ad_1; i1++)
            {
                int p1 = (temp_addr + tmp1[i1]) % depth;
                for (int i2 = 0; i2 < ad_2; i2++)
                {
                    int p2 = (addr_d + tmp2[i2]) % width;
                    int pos = p1 * width + p2;
                    uint16_t idx = (i1 << 8) | i2;
                    for (int i3 = 0; i3 < ROOM; i3++)
                    {
                        if ((value[pos].ext[i3] == idx) && (value[pos].fp_src[i3] == temp_fp) && (value[pos].fp_dst[i3] == fp_d))
                        {
                            delete []tmp1;
                            delete []tmp2;
                            return true;
                        }
                    }
                }
            }
            // find in matrix
            for (int i1 = 0; i1 < ad_1; i1++)
            {
                int p1 = (temp_addr + tmp1[i1]) % depth;
                for (int i2 = 0; i2 < width; i2++)
                {
                    int pos = p1 * width + i2;
                    for (int i3 = 0; i3 < ROOM; i3++)
                    {
                        if (value[pos].fp_src[i3] == temp_fp && ((value[pos].ext[i3] >> 8) == i1))
                        {
                            uint32_t tmp_g = value[pos].fp_dst[i3];
                            int tmp_s = value[pos].ext[i3] & 0xff;

                            uint32_t shifter = tmp_g;
                            for (int v = 0; v < tmp_s; v++)
                                shifter = (shifter * MUL + INC) % MOD;
                            uint32_t tmp_h = i2;
                            while (tmp_h < shifter)
                                tmp_h += width;
                            tmp_h -= shifter;

                            key_type val = (tmp_h << fingerprint_length) + tmp_g;

                            IT = checked.find(val);
                            if (IT == checked.end())
                            {
                                mapnode tmp_e;
                                tmp_e.addr = tmp_h;
                                tmp_e.fp = tmp_g;
                                q.push(tmp_e);
                                checked[val] = true;
                            }
                        }

                    }
                }
            }
            delete[] tmp1;
            delete[] tmp2;
        }

        q.pop();
    }
    return false;
}


void ScubeKick::printUsageInfo()
{
    for(int cou=0;cou<matrxi_num;cou++){
        basket *value=values[cou];
        DegDetector * degDetector = degDetectors[cou];
        // print the deg detect info
        cout << "ScubeKick::printUsageInfo()" << endl;
        degDetector->printUsage();
        // print the matrix usage
        cout << "-----------------------Matrix-----------------------" << endl;
        uint32_t room_count = 0;
        uint32_t bucket_count = 0;
        for (uint32_t i = 0; i < width * depth; i++)
        {
            if ((value[i].fp_src[0] != 0) && (value[i].weight[0] != 0))
            {
                bucket_count++;
            }
            for (uint8_t j = 0; j < ROOM; j++)
            {
                if ((value[i].fp_src[j] != 0) && (value[i].weight[j] != 0))
                {
                    room_count++;
                }
            }
        }
        cout << "Scube room_count = " << room_count << ", total room = " << (width * depth * ROOM) << ", space usage is " <<
             (double)room_count / (double)(width * depth * ROOM) * 100 << "%" << endl;
        cout << "Scube bucket_count = " << bucket_count << ", total bucket = " << (width * depth) << ", space usage is " <<
             (double)bucket_count / (double)(width * depth) * 100 << "%" << endl;
        cout << "----------------------------------------------------" << endl;
    }
}

#endif // _SCUBEKICK_H
