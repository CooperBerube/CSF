#ifndef CACHESIMFUNCS_H
#define CACHESIMFUNCS_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdint>

// Cache simulation functions and data structures
// This header file defines the structures and functions used for simulating a cache memory system.

class Block {
    unsigned int tag;
    bool valid, dirty;
    unsigned int load_ts, access_ts;
};

class Set {
    std::vector<Set> slots;
};

class Cache {
    public:

    Cache(char* args[]);
    std::vector<Cache> sets;
    
    unsigned int setCount;
    unsigned int blockCount;
    unsigned int byteCount;
    bool isWriteAllocate;
    bool isWriteThrough;
    bool isLRU;

    unsigned int totalCycles = 0;
    unsigned int loads = 0;
    unsigned int loadHits = 0;
    unsigned int loadMisses = 0;
    unsigned int stores = 0;
    unsigned int storeHits = 0;
    unsigned int storeMisses = 0;

    unsigned int getTag(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);
    unsigned int getIndex(std::uint32_t address, unsigned byteOffsetBits);
    void loadAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);
    void storeAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);
    void evict(unsigned int index, unsigned int tag);
    void addBlock(unsigned int index, unsigned int tag);

};

#endif // CACHESIMFUNCS_H
