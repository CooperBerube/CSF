#include "cacheSimFuncs.h"
#include <algorithm>

Cache::Cache(char* args[]) {
    setCount = std::stoi(args[1]);
    blockCount = std::stoi(args[2]);
    byteCount = std::stoi(args[3]);
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

    int currIndex = getIndex(address, byteOffsetBits);
    int currTag = getTag(address, byteOffsetBits, indexBits);

    if (sets[currIndex].tags.find(currTag) != sets[currIndex].tags.end()) {
        loadHits++;
        if (isLRU) {
            evict(currIndex, currTag);
        }
    } else {
        loadMisses++;
        addBlock(currIndex, currTag);
    }

    totalCycles++;
    loads++;
}

void Cache::storeAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits) {

    int currIndex = getIndex(address, byteOffsetBits);
    int currTag = getTag(address, byteOffsetBits, indexBits);

    if (sets[currIndex].tags.find(currTag) != sets[currIndex].tags.end()) {
        storeHits++;
        if (isWriteThrough) {
            ++totalCycles;
        } else {
            sets[currIndex].tags[currTag]->dirty = true;
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

    totalCycles++;
    stores++;
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

    // Check if the slot is full
    if (s.slots.size() == (size_t) blockCount) { // Slot is full
        s.tags.erase(s.tags.find(s.slots.front().tag)); // Remove the evicted slot
        if (s.slots.front().dirty) {
            totalCycles += (byteCount / 4) * 100;
        }
        s.slots.pop_front(); // Evict the least recently used slot from the block
    }

    Block block;
    block.tag = tag;
    block.valid = true;
    block.dirty = false;
    block.load_ts = totalCycles;
    block.access_ts = totalCycles;
    s.slots.push_back(block);
    s.tags[tag] = --s.slots.end();
    totalCycles += (byteCount / 4) * 100;
}