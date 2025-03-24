#include "cacheSimFuncs.h"

Cache::Cache(char* args[]) {
    setCount = std::stoi(args[1]);
    blockCount = std::stoi(args[2]);
    isWriteAllocate = (std::string(args[4]) == "write-allocate");
    isWriteThrough = (std::string(args[5]) == "write-through"); // if false, then it's write-back
    isLRU = (std::string(args[6]) == "lru");

    // Initialize cache sets
    sets.resize(setCount);
}

unsigned int Cache::getTag(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits) {
    return address >> (byteOffsetBits + indexBits);
}

unsigned int Cache::getIndex(std::uint32_t address, unsigned byteOffsetBits) {
    return (address >> byteOffsetBits) & (setCount - 1);
}

void Cache::loadAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits) {
    // Implement load operation
    if (isLRU) {
        if (isWriteAllocate) {

        } else {

        }
        if (isWriteThrough) {

        } else {

        }
    }
}

void Cache::storeAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits) {
    // Implement store operation
    if (isLRU) {
        if (isWriteAllocate) {

        } else {

        }
        if (isWriteThrough) {

        } else {
            
        }
    }
}

void Cache::evict(unsigned int index, unsigned int tag) {
    // Implement LRU eviction policy
}

void Cache::addBlock(unsigned int index, unsigned int tag) {
    // Implement adding a cache slot
}