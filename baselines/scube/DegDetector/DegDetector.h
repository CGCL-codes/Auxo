#ifndef _DegDetector_H
#define _DegDetector_H
#include "../headers/params.h"

using namespace std;

class DegDetector
{
public:
    const uint32_t matrix_width = 0;        // the width of the matrix
    const uint32_t matrix_depth = 0;        // the depth of the matrix
    DegDetector() { cout << "DegDetector::DegDetector()" << endl; }
    DegDetector(uint32_t width, uint32_t depth): matrix_width(width), matrix_depth(depth) 
    {
        #if defined(DEBUG)
        cout << "DegDetector::DegDetector(width: " <<  width << ", depth: " << depth << ")" << endl;
        #endif
    }
    virtual ~DegDetector() 
    {
        #if defined(DEBUG)
        cout << "DegDetector::~DegDetector()" << endl;
        #endif
    }
    // the insert process of each tuple (s, d)
    virtual bool insert(key_type s, key_type d, hash_type hash_s, hash_type hash_d) = 0;
    // return the estimation in/out degree of the node n
    // virtual uint32_t degQuery(key_type key, int type) = 0;
    // return the row/column addr numbers of the node n
    virtual addr_type addrQuery(key_type key, int type) = 0;   // type = 0 for out_addr, 1 for in_addr
    // when kick-out failed, extend more addresses for potential high degree nodes
    virtual bool extendAddr(key_type key, addr_type n, int type) = 0;
    virtual void printUsage() = 0;
};

#endif // _DegDetector_H