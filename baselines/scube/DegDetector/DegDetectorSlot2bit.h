#ifndef _DegDetectorSlot2bit_H
#define _DegDetectorSlot2bit_H
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <map>
#include <set>
#include <random>
#include <memory.h>
#include "DegDetector.h"

// slot + linkedlist
// just maintains reserved_bits for each key
using namespace std;

class DegDetectorSlot2bit: public DegDetector
{
private:
    const int BITVECBITS = 2;      // take the high 2 bits of slot.value as bitvector
    const int UPDATEBITS = 8;      // take the another 8 bits of slot.value as update_times
    const int ADDRBITS = 10;        // take the low 6 bits of slot.value as address number
    struct slot
    {
        key_type slot_key;          // the least significant bit is marked as a pointer
        slot_value_type value;      // split the field into three parts. [bitvector(2 bits), update_times(8 bits), address number(6 bits)]
    };
    slot_value_type addr_mask;
    slot_value_type update_mask;
    slot_value_type bitvec_mask;

    const uint32_t slot_num;
    slot* out_slots = NULL;
    slot* in_slots = NULL;
    int ignored_bits;
    int reserved_bits;
    uint32_t hash_mask = 0;
    double alpha = 0.8;
    double exp_deg;

    set<uint32_t> extend_s_index;
    set<uint32_t> extend_d_index;

public:
    DegDetectorSlot2bit(uint32_t matrix_width, uint32_t matrix_depth, uint32_t slotNum, double exp_deg, int ignored_bits, int reserved_bits = 2, double alpha = 0.8, int update_bits = 8, int addr_bits = 10);
    ~DegDetectorSlot2bit();
    bool insert(key_type s, key_type d, hash_type hash_s, hash_type hash_d);
    uint32_t degQuery(key_type n, int type);
    addr_type addrQuery(key_type n, int type);   // type = 0 for out_addr, 1 for in_addr
    bool extendAddr(key_type key, addr_type n, int type);
    void printUsage();
    
private:
    slot* recoverPointer(slot s);
    bool addSlot(uint32_t addr, int type, key_type key, slot_value_type slot_value);
    bool decayStrategy(uint32_t addr, int type, key_type key, int update_bit);
    bool insertSlot(key_type key, int update_bit, int type);
};

DegDetectorSlot2bit::DegDetectorSlot2bit(uint32_t matrix_width, uint32_t matrix_depth, uint32_t slotNum, double exp_deg, int ignored_bits, int reserved_bits, double alpha, int update_bits, int addr_bits): 
DegDetector(matrix_width, matrix_depth), slot_num(slotNum), exp_deg(exp_deg), ignored_bits(ignored_bits), reserved_bits(reserved_bits), BITVECBITS(reserved_bits), UPDATEBITS(update_bits), ADDRBITS(addr_bits)
{
#if defined(DEBUG)
    cout << "DegDetectorSlot2bit::DegDetectorSlot2bit(matrix_width: " << matrix_width << ", matrix_depth: " << matrix_depth 
         << ", slot_num: " << slotNum << ", exp_deg: " << exp_deg << ", ignored_bits: " << ignored_bits << ", reserved_bits: " << reserved_bits 
         << ", alpha: " << alpha  << ", BITVECBITS: " << BITVECBITS  << ", UPDATEBITS: " << UPDATEBITS << ", ADDRBITS: " << ADDRBITS << ")" << endl;
#endif
    this->hash_mask = ((1 << ignored_bits) - 1);
    this->addr_mask = ((1 << ADDRBITS) - 1);
    this->update_mask = (((1 << UPDATEBITS) - 1) << ADDRBITS);
    this->bitvec_mask = (((1 << BITVECBITS) - 1) << (ADDRBITS + UPDATEBITS));

    // this->out_slots = new slot[slotNum * SLOTROOM];
    // this->in_slots = new slot[slotNum * SLOTROOM];
    // posix_memalign((void**)&(this->out_slots), 64, sizeof(slot) * slotNum * SLOTROOM);		// 64-byte alignment of the requested space
    // posix_memalign((void**)&(this->in_slots), 64, sizeof(slot) * slotNum * SLOTROOM);		// 64-byte alignment of the requested space
    // this->out_slots = (slot *) aligned_alloc(64, sizeof(slot) * slotNum * SLOTROOM);		    // 64-byte alignment of the requested space
    // this->in_slots = (slot *) aligned_alloc(64, sizeof(slot) * slotNum * SLOTROOM);		    // 64-byte alignment of the requested space
    this->out_slots = (slot *) memalign(64, sizeof(slot) * slotNum * SLOTROOM);		            // 64-byte alignment of the requested space
    this->in_slots = (slot *) memalign(64, sizeof(slot) * slotNum * SLOTROOM);		            // 64-byte alignment of the requested space
    memset(this->out_slots, 0, sizeof(slot) * slotNum * SLOTROOM);
    memset(this->in_slots, 0, sizeof(slot) * slotNum * SLOTROOM);
}

DegDetectorSlot2bit::~DegDetectorSlot2bit()
{
#if defined(DEBUG)
    cout << "DegDetectorSlot2bit::~DegDetectorSlot2bit()" << endl;
#endif
    for (set<uint32_t>::iterator iter = extend_s_index.begin(); iter != extend_s_index.end(); iter++) {
        uint32_t index = *iter;
        slot* head = recoverPointer(out_slots[index * SLOTROOM + SLOTROOM - 1]);
        cout << "index = " << index << ", head = ";
        slot* ptr = head;
        while (ptr != NULL) 
        {
            cout << ptr;
            if ((ptr[SLOTROOM - 2].slot_key & 1) == 1) 
            {
                slot* ptr1 = recoverPointer(ptr[SLOTROOM - 1]);
                delete[] ptr;
                ptr = ptr1;
                cout << " -> ";
            }
            else 
            {
                delete[] ptr;
                ptr = NULL;
                cout << endl;
            }
        }
    }
    for (set<uint32_t>::iterator iter = extend_d_index.begin(); iter != extend_d_index.end(); iter++)
    {
        uint32_t index = *iter;
        
        slot* head = recoverPointer(in_slots[index * SLOTROOM + SLOTROOM - 1]);
        cout << "index = " << index << ", head = ";
        slot* ptr = head;
        while (ptr != NULL)
        {
            cout << ptr;
            if ((ptr[SLOTROOM - 2].slot_key & 1) == 1)
            {
                slot* ptr1 = recoverPointer(ptr[SLOTROOM - 1]);
                delete[] ptr;
                ptr = ptr1;
                cout << " -> ";
            }
            else
            {
                delete[] ptr;
                ptr = NULL;
                cout << endl;
            }
        }
    }
    // delete[] this->out_slots;
    // delete[] this->in_slots;
    free(this->out_slots);
    free(this->in_slots);
}

DegDetectorSlot2bit::slot* DegDetectorSlot2bit::recoverPointer(slot s) 
{
    return (slot *) ((((uint64_t)s.slot_key) << 32) | s.value);
}

bool DegDetectorSlot2bit::addSlot(uint32_t addr, int type, key_type key, slot_value_type slot_value)
{
    if (type == 0)
        this->extend_s_index.insert(addr);
    else
        this->extend_d_index.insert(addr);
    
    slot* slots = (type == 0) ? out_slots : in_slots;
    slot* new_slot = new slot[SLOTROOM];
    memset(new_slot, 0, sizeof(slot) * SLOTROOM);
    // cout << "addr = " << addr << ", new_slot = " << new_slot << ", type = " << type << endl;
    
    if (new_slot == NULL)
        return false;
    
    if ((slots[addr * SLOTROOM + SLOTROOM - 2].slot_key & 1) == 1)      // not extend for the first time
    { 
        slot* rec = recoverPointer(slots[addr * SLOTROOM + SLOTROOM - 1]);
        while (true) 
        {
            if ((rec[SLOTROOM - 2].slot_key & 1) == 1)
                rec = recoverPointer(rec[SLOTROOM - 1]);
            else 
                break;
        }
        rec[SLOTROOM - 2].slot_key |= 1;
        new_slot[0].slot_key = rec[SLOTROOM - 1].slot_key;
        new_slot[0].value = rec[SLOTROOM - 1].value;
        new_slot[1].slot_key = (key << 1);
        new_slot[1].value = slot_value;
        // set the two fields of the (SLOTNUM - 1)th cell in the array to pointers
        rec[SLOTROOM - 1].slot_key = ((uint64_t) new_slot >> 32);       
        rec[SLOTROOM - 1].value = ((uint64_t) new_slot & 0xffffffff);
    }
    else   // extend for the first time
    {    
        slots[addr * SLOTROOM + SLOTROOM - 2].slot_key |= 1;
        new_slot[0].slot_key = slots[addr * SLOTROOM + SLOTROOM - 1].slot_key;
        new_slot[0].value = slots[addr * SLOTROOM + SLOTROOM - 1].value;
        new_slot[1].slot_key = (key << 1);
        new_slot[1].value = slot_value;
        // set the two fields of the (SLOTNUM - 1)th cell in the array to pointers
        slots[addr * SLOTROOM + SLOTROOM - 1].slot_key = ((uint64_t) new_slot >> 32);       
        slots[addr * SLOTROOM + SLOTROOM - 1].value = ((uint64_t) new_slot & 0xffffffff);
    }
    return true;
}

bool DegDetectorSlot2bit::decayStrategy(uint32_t addr, int type, key_type key, int update_bit)
{
    slot* slots = (type == 0) ? out_slots : in_slots;
    slot* decay_slot = NULL;
    if ((slots[addr * SLOTROOM + SLOTROOM - 2].slot_key & 1) == 1)      // the slot has a pointer, directly get the tail slot
    {
        slot* rec = recoverPointer(slots[addr * SLOTROOM + SLOTROOM - 1]);
        while (true) 
        {
            if ((rec[SLOTROOM - 2].slot_key & 1) == 1)
                rec = recoverPointer(rec[SLOTROOM - 1]);
            else 
                break;
        }
        decay_slot = rec;
    }
    else
    {
        decay_slot = &(slots[addr * SLOTROOM]);
    }
    
    // we kick out the minimum element in the slot according to the hash value by probability
    int index = -1;
    int min_update = INT32_MAX;
    for (int i = 0; i < SLOTROOM; i++) 
    {        
        if (((decay_slot[i].value & addr_mask) == 2))
        {
            int update_val = ((decay_slot[i].value & update_mask) >> ADDRBITS);
            if (update_val < min_update) 
            {
                min_update = update_val;
                index = i;
            }
        }
    }
    // all the points in the ROOM are high-degree nodes, we just do the expanded operation directly.
    if (index == -1)    
    {
        if (!addSlot(addr, type, key, ((1 << (ADDRBITS + UPDATEBITS + update_bit)) | 2)))
        {
            cout << "addSlot() error!" << endl;
            return false;
        }
    }
    else
    {
        if ((decay_slot[index].value & bitvec_mask) == 0)
        {
            if ((decay_slot[index].value & update_mask) == 0)
            {
                decay_slot[index].slot_key = (key << 1);
                decay_slot[index].value = ((1 << (ADDRBITS + UPDATEBITS + update_bit)) | 2);   // the initial address of potential high degree node is set to 2
            }
            else
            {
                decay_slot[index].value -= (1 << ADDRBITS);
            }
        }
        else
        {
            if (((decay_slot[index].value & (1 << (ADDRBITS + UPDATEBITS + update_bit))) != 0))
            {
                decay_slot[index].value -= (1 << (ADDRBITS + UPDATEBITS + update_bit));
            }
            else
            {
                decay_slot[index].value &= (~bitvec_mask);
            }
        }
    }
    return true;
}

// update slot
bool DegDetectorSlot2bit::insertSlot(key_type key, int update_bit, int type)
{
    slot* slots = (type == 0) ? out_slots : in_slots;
    uint32_t idx = key % slot_num;
    uint32_t len = (type == 0) ? matrix_width : matrix_depth;
    double theta = len * ROOM * alpha;

    // look up if the node is existing or the slot is empty
    slot* ptr = &slots[idx * SLOTROOM];
    bool flag = true;
    while (flag)
    {
        for (int i = 0; i < SLOTROOM; i++) 
        {
            if (i == SLOTROOM - 1)
            {
                if ((ptr[SLOTROOM - 2].slot_key & 1) == 1)              // the last ROOM stores a pointer
                {
                    // restore the pointer
                    ptr = recoverPointer(ptr[SLOTROOM - 1]);
                    break;
                }
                else
                {
                    flag = false;
                }
            }
            // if the fingerprint is the same
            if ((ptr[i].slot_key >> 1) == key) 
            {
                // update bit
                ptr[i].value |= (1 << (ADDRBITS + UPDATEBITS + update_bit));
                // check all bit
                bool update = true;
                for (int j = 0; j < reserved_bits; j++) 
                {
                    if ((ptr[i].value & (1 << (ADDRBITS + UPDATEBITS + j))) == 0) 
                    {
                        update = false;
                        break;
                    }
                }
                if (update)
                {
                    // clear the bit vector and add update_times
                    ptr[i].value &= (~bitvec_mask);
                    ptr[i].value += (1 << ADDRBITS);
                    // update addrs
                    int upd_times = ((ptr[i].value & update_mask) >> ADDRBITS);
                    if (upd_times == 0)
                    {
                        cout << "upd_times is out of range! Considering add more bits to UPDATEBITS." << endl;
                    }
                    uint32_t addrs =  (ptr[i].value & addr_mask);
                    if (addrs < 2) 
                    {
                        cout << "error! DegDetectorSlot2bit-line 340" << endl;
                    }
                    addr_type ad = (addr_type) max (ceil((double)((double)(upd_times * exp_deg) / theta)), (double)addrs);
                    if (ad > addr_mask) 
                    {
                        cout << "ad is out of range! Considering add more bits to ADDRBITS." << endl;
                    }
                    if (ad > addrs)
                    {
                        // update addr
                        ptr[i].value &= (~addr_mask);
                        ptr[i].value |= ad;
                    }
                }
                return true;
            }
            // if the slot is empty
            else if ((ptr[i].slot_key == 0) && (ptr[i].value == 0)) 
            {
                ptr[i].slot_key = (key << 1);
                ptr[i].value = ((1 << (ADDRBITS + UPDATEBITS + update_bit)) | 2);   // the initial address of potential high degree node is set to 2
                return true;
            }
        }
    }
    

    // 前面的slot中已经满了，而且没有fingerprint匹配的，因此进行decay
    // we kick out the minimum element in the slot according to the hash value by probability
    return addSlot(idx, type, key, ((1 << (ADDRBITS + UPDATEBITS + update_bit)) | 2));
    // return decayStrategy(idx, type, key, update_bit);
}

// insert elements to the degree detector
bool DegDetectorSlot2bit::insert(key_type s, key_type d, hash_type hash_s, hash_type hash_d)
{
    bool res = true;
    if ((hash_s & hash_mask) == 0) 
    {
        int update_bit = -1;
        for (int i = 0; i < reserved_bits; i++) 
        {
            if (((1 << (ignored_bits + i)) & hash_s) != 0)
            {
                update_bit = i;
                break;
            }
        }
        if (update_bit != -1)
        {
            res = res && insertSlot(d, update_bit, 1);
        }
        // int update_bit = -1;
        // for (int i = 0; i < reserved_bits - 1; i++) 
        // {
        //     if (((1 << (ignored_bits + i)) & hash_s) != 0)
        //     {
        //         update_bit = i;
        //         break;
        //     }
        // }
        // if (update_bit == -1) 
        // {
        //     if ((hash_s >> (ignored_bits + reserved_bits - 1)) != 0) 
        //     {
        //         update_bit = reserved_bits - 1;
        //     }
        // }
        // if (update_bit != -1)
        // {
        //     res = res && insertSlot(d, update_bit, 1);
        // }
    }

    if ((hash_d & hash_mask) == 0) 
    {
        int update_bit = -1;
        for (int i = 0; i < reserved_bits; i++) 
        {
            if (((1 << (ignored_bits + i)) & hash_d) != 0)
            {
                update_bit = i;
                break;
            }
        }
        if (update_bit != -1)
        {
            res = res && insertSlot(s, update_bit, 0);
        }
        // int update_bit = -1;
        // for (int i = 0; i < reserved_bits - 1; i++) 
        // {
        //     if (((1 << (ignored_bits + i)) & hash_d) != 0)
        //     {
        //         update_bit = i;
        //         break;
        //     }
        // }
        // if (update_bit == -1) 
        // {
        //     if ((hash_d >> (ignored_bits + reserved_bits - 1)) != 0) 
        //     {
        //         update_bit = reserved_bits - 1;
        //     }
        // }
        // if (update_bit != -1)
        // {
        //     res = res && insertSlot(s, update_bit, 0);
        // }
    }

    return res;
}

// return the estimate degree of the node
uint32_t DegDetectorSlot2bit::degQuery(key_type key, int type) 
{
    slot* slots = (type == 0) ? out_slots : in_slots;
    uint32_t slot_index = key % slot_num;

    slot* ptr = &slots[slot_index * SLOTROOM];
    bool flag = true;
    while (flag)
    {
        for (int i = 0; i < SLOTROOM; i++)         // query the addr in slots
        {
            if (i == SLOTROOM - 1)
            {
                if ((ptr[SLOTROOM - 2].slot_key & 1) == 1)              // the last ROOM stores a pointer
                {
                    // restore the pointer
                    ptr = recoverPointer(ptr[SLOTROOM - 1]);
                    break;
                }
                else
                {
                    flag = false;
                }
            }
            // if the fingerprint is the same
            if ((ptr[i].slot_key >> 1) == key) 
            {
                // get the addr info
                // res_ad = (ptr[i].value & addr_mask);
                uint32_t res = (uint32_t)(((ptr[i].value & update_mask) >> ADDRBITS) * exp_deg);
                return res;
            }
        }
    }
    return 0;
}

addr_type DegDetectorSlot2bit::addrQuery(key_type key, int type) 
{
    // the initial address for all nodes is set to 2
    // addr_type res_ad = 2;
    slot* slots = (type == 0) ? out_slots : in_slots;
    uint32_t slot_index = key % slot_num;

    slot* ptr = &slots[slot_index * SLOTROOM];
    bool flag = true;
    while (flag)
    {
        for (int i = 0; i < SLOTROOM; i++)         // query the addr in slots
        {
            if (i == SLOTROOM - 1)
            {
                if ((ptr[SLOTROOM - 2].slot_key & 1) == 1)              // the last ROOM stores a pointer
                {
                    // restore the pointer
                    ptr = recoverPointer(ptr[SLOTROOM - 1]);
                    break;
                }
                else
                {
                    flag = false;
                }
            }
            // if the fingerprint is the same
            if ((ptr[i].slot_key >> 1) == key) 
            {
                // get the addr info
                // res_ad = (ptr[i].value & addr_mask);
                return max(2, (int)(ptr[i].value & addr_mask));
            }
        }
    }
    return 2;
}

bool DegDetectorSlot2bit::extendAddr(key_type key, addr_type n, int type) 
{
    bool res = false;
    if (n < 2)
    {
        cout << "DegDetectorSlot2bit::extendAddr error!" << endl;
        return false;
    }
    if (n > addr_mask) 
    {
        cout << "extendAddr(n = " << n << ") is out of range! Considering add more bits to ADDRBITS." << endl;
    }
    // first find out if there is a key in the slots
    slot* slots = (type == 0) ? out_slots : in_slots;
    uint32_t slot_index = key % slot_num;

    slot* ptr = &slots[slot_index * SLOTROOM];
    bool flag = true;
    while (flag)
    {
        for (int i = 0; i < SLOTROOM; i++)
        {
            if (i == SLOTROOM - 1)
            {
                if ((ptr[SLOTROOM - 2].slot_key & 1) == 1)              // the last ROOM stores a pointer
                {
                    // restore the pointer
                    ptr = recoverPointer(ptr[SLOTROOM - 1]);
                    break;
                }
                else
                {
                    flag = false;
                }
            }
            // if the key is the same
            if ((ptr[i].slot_key >> 1) == key) 
            {
                // extend addrs to n
                ptr[i].value &= (~addr_mask);
                ptr[i].value |= n;
                return true;
            }
            // look up if there exists empty slot
            if ((ptr[i].slot_key == 0) && (ptr[i].value == 0)) 
            {
                ptr[i].slot_key = (key << 1);
                ptr[i].value = n;   // the address number is set to n
                return true;
            }
        }
    }

    // there is no empty room, we just do the extended operation
    if (!addSlot(slot_index, type, key, n))
    {
        cout << "addSlot() error!" << endl;
        return false;
    }
    return true;
}

void DegDetectorSlot2bit::printUsage() 
{
    map<addr_type, uint32_t> out_ad, in_ad;
    for(int idx = 0; idx < slot_num; idx++) {
        slot* ptr1 = &out_slots[idx * SLOTROOM];
        bool flag = true;
        while (flag)
        {
            for (int i = 0; i < SLOTROOM; i++)
            {
                if (i == SLOTROOM - 1)
                {
                    if ((ptr1[SLOTROOM - 2].slot_key & 1) == 1)
                    {
                        ptr1 = recoverPointer(ptr1[SLOTROOM - 1]);
                        break;
                    }
                    else
                        flag = false;
                }
                addr_type ad = (ptr1[i].value & addr_mask);
                map<addr_type, uint32_t>::iterator iter = out_ad.find(ad);
                if (iter != out_ad.end())
                {
                    out_ad[ad]++;
                }
                else
                {
                    out_ad[ad] = 1;
                }
            }
        }

        slot* ptr2 = &in_slots[idx * SLOTROOM];
        flag = true;
        while (flag)
        {
            for (int i = 0; i < SLOTROOM; i++)
            {
                if (i == SLOTROOM - 1)
                {
                    if ((ptr2[SLOTROOM - 2].slot_key & 1) == 1)
                    {
                        ptr2 = recoverPointer(ptr2[SLOTROOM - 1]);
                        break;
                    }
                    else
                        flag = false;
                }
                addr_type ad = (ptr2[i].value & addr_mask);
                map<addr_type, uint32_t>::iterator iter = in_ad.find(ad);
                if (iter != in_ad.end())
                {
                    in_ad[ad]++;
                }
                else
                {
                    in_ad[ad] = 1;
                }
            }
        }
    }
    cout << "out addrs" << endl;
    for (map<addr_type, uint32_t>::iterator iter = out_ad.begin(); iter != out_ad.end(); iter++) {
        cout << "addr-" << iter->first << ": " << iter->second << endl;
    }
    cout << endl << "in addrs" << endl;
    for (map<addr_type, uint32_t>::iterator iter = in_ad.begin(); iter != in_ad.end(); iter++) {
        cout << "addr-" << iter->first << ": " << iter->second << endl;
    }

    uint32_t in_count = 0, out_count = 0;
    // uint32_t in_pointers = 0, out_pointers = 0;
    for (int i = 0; i < slot_num * SLOTROOM; i++) 
    {
        if (in_slots[i].slot_key != 0)
            in_count++;
        if (out_slots[i].slot_key != 0) 
            out_count++;
    }

    cout << "------------------DegDeteSlot----------------------" << endl;
    cout << "in_count = " << in_count << ", in_slot size = " << (slot_num * SLOTROOM)
         << ", in_slot usage = " << (((double)in_count / (double)(slot_num * SLOTROOM)) * 100) << "%" << endl;
    cout << "out_count = " << out_count << ", out_slot size = " << (slot_num * SLOTROOM)
         << ", out_slot usage = " << (((double)out_count / (double)(slot_num * SLOTROOM)) * 100) << "%" << endl;
    cout << "----------------------------------------------------" << endl;
}

#endif // _DegDetectorSlot2bit_H
