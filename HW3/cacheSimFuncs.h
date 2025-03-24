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

class set {
    std::vector<set> slots;
};

class cache {
    private:
    
    unsigned int setCount;
    unsigned int blockCount;
    unsigned int byteCount;
    bool isWriteAllocate; // true = write-allocate, false = no-write-allocate
    bool isWriteThrough; // true = write-through, false = write-back
    bool isLRU;

    unsigned int totalCycles = 0;
    unsigned int loads = 0;
    unsigned int loadHits = 0;
    unsigned int loadMisses = 0;
    unsigned int stores = 0;
    unsigned int storeHits = 0;
    unsigned int storeMisses = 0;

    std::vector<cache> sets;

    public:

    unsigned int getSetCount() const { return setCount; }
    unsigned int getBlockCount() const { return blockCount; }
    unsigned int getByteCount() const { return byteCount; }
    bool getIsWriteAllocate() const { return isWriteAllocate; }
    bool getIsWriteThrough() const { return isWriteThrough; }
    bool getIsLRUEviction() const { return isLRU; }
    unsigned int getTotalLoads() const { return loads; }
    unsigned int getTotalStores() const { return stores; }
    unsigned int getLoadHits() const { return loadHits; }
    unsigned int getLoadMisses() const { return loadMisses; }
    unsigned int getStoreHits() const { return storeHits; }
    unsigned int getStoreMisses() const { return storeMisses; }
    unsigned int getTotalCycles() const { return totalCycles; }

    unsigned int getTag(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);
    unsigned int getIndex(std::uint32_t address, unsigned byteOffsetBits);
    void loadAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);
    void storeAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);
    void evictLRU(unsigned int index, unsigned int tag);
    void addCacheSlot(unsigned int index, unsigned int tag);

};

#endif // CACHESIMFUNCS_H
