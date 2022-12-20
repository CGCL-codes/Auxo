#ifndef _PARAMS_H
#define _PARAMS_H
#include <iostream>
#include <stdlib.h>

#define M 80000

#define INC 739
#define MOD 1048576
#define MUL 5

#ifdef ROOM1
#define ROOM 1	// This is the parameter to controll the maximum number of rooms in a bucket. 
#else
#define ROOM 1  // This is the parameter to controll the maximum number of rooms in a bucket.
#endif

#ifdef SR2
#define SLOTROOM 2
#elif SR4
#define SLOTROOM 4
#elif SR8
#define SLOTROOM 8
#else
#define SLOTROOM 16
#endif

typedef uint16_t fp_type;
// typedef uint16_t w_type;
typedef uint32_t w_type;
typedef uint32_t key_type;
typedef uint32_t addr_type;
typedef uint32_t hash_type;
typedef uint32_t bit_vector_type;

typedef uint32_t slot_key_type;
typedef uint32_t slot_value_type;

#endif //_PARAMS_H
