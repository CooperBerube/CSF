#include "cacheSimFuncs.h"
#include <algorithm>

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

    ++totalCycles;
    ++loads;

    int currIndex = getIndex(address, byteOffsetBits);
    int currTag = getTag(address, byteOffsetBits, currIndex);

    if (sets[indexBits].tags.find(currTag) != sets[indexBits].tags.end()) {
        loadHits++;
        if (isLRU) { 
            evict(currIndex, currTag);
        }
    } else { 
        loadMisses++;
        addBlock(currIndex, currTag); 
    }
    
}

void Cache::storeAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits) {
    // Implement store operation

    ++totalCycles;
    ++stores;

    int currIndex = getIndex(address, byteOffsetBits);
    int currTag = getTag(address, byteOffsetBits, currIndex);


    if (sets[currIndex].tags.find(currTag) != sets[currIndex].tags.end()) {
        storeHits++;
        if (isWriteThrough) {
            ++totalCycles;
        } else {
            sets[currIndex].slots[sets[currIndex].tags[currTag]->load_ts].dirty = true;
        }
        if (isLRU) {
            evict(currIndex, currTag);
        }
    } else {
        storeMisses++;
        if (isWriteAllocate) {
            addBlock(currIndex, currTag);
        }
    }

}

void Cache::evict(unsigned int index, unsigned int tag) {
    Set &s = sets[index];
    if (isLRU) {
        s.tags[tag]->access_ts = totalCycles;
    } else {
        // FIFO policy does not require any additional operations here
    }
}


void Cache::addBlock(unsigned int index, unsigned int tag) {
    Set &s = sets[index];
    if (s.tags.size() == blockCount) {
        auto it = std::min_element(s.tags.begin(), s.tags.end(), 
            [](const auto &a, const auto &b) { return a.second->access_ts < b.second->access_ts; });
        s.tags.erase(it->first);
    }
    Block b;
    b.tag = tag;
    b.valid = true;
    b.dirty = false;
    b.load_ts = totalCycles;
    b.access_ts = totalCycles;
    s.slots.push_back(b);
    // s.tags[tag] = s.slots.end();
}