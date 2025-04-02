#include "cacheSimFuncs.h"
#include <algorithm>

// Constructor for Cache class
// Initializes cache properties based on input arguments.
Cache::Cache(char* args[])
    : setCount(std::stoi(args[1])), blockCount(std::stoi(args[2])), byteCount(std::stoi(args[3])),
      isWriteAllocate(std::string(args[4]) == "write-allocate"), 
      isWriteThrough(std::string(args[5]) == "write-through"),
      isLRU(std::string(args[6]) == "lru"), 
      totalCycles(0), loads(0), loadHits(0),
      loadMisses(0), stores(0), storeHits(0), storeMisses(0) {
    sets.resize(setCount); // Allocate cache sets
}

// Processes a load operation on the given memory address
void Cache::loadAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits) {
    processAddress(address, byteOffsetBits, indexBits, false);
}

// Processes a store operation on the given memory address
void Cache::storeAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits) {
    processAddress(address, byteOffsetBits, indexBits, true);
}

// Handles both load and store operations by determining if the access results in a hit or miss
void Cache::processAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits, bool isStore) {
    unsigned int currIndex = (address >> byteOffsetBits) & (setCount - 1); // Extract set index
    unsigned int currTag = address >> (byteOffsetBits + indexBits); // Extract tag
    Set& set = sets[currIndex]; // Retrieve the corresponding set

    // Check if the tag exists in the set (cache hit)
    if (set.tags.find(currTag) != set.tags.end()) {
        handleHit(set, currTag, isStore);
    } else { // Cache miss
        handleMiss(set, currTag, isStore);
    }

    totalCycles++; // Increment cycle count
    isStore ? stores++ : loads++; // Update operation counters
}

// Handles cache hit: updates LRU, dirty bit, and cycles if needed
void Cache::handleHit(Set& set, unsigned tag, bool isStore) {
    isStore ? storeHits++ : loadHits++; // Increment hit counter

    // If it's a store operation and the policy is write-back, mark block as dirty
    if (isStore && !isWriteThrough) {
        set.tags[tag]->dirty = true;
    }

    // Update LRU order if applicable
    if (isLRU) {
        updateLRU(set, tag);
    }

    // If write-through is enabled, each store incurs additional memory write cycles
    if (isStore && isWriteThrough) {
        totalCycles += 100;
    }
}

// Handles cache miss: loads data into cache and updates cycles accordingly
void Cache::handleMiss(Set& set, unsigned tag, bool isStore) {
    isStore ? storeMisses++ : loadMisses++; // Increment miss counter

    // If it's a load or a write-allocate store, bring the block into the cache
    if (!isStore || isWriteAllocate) {
        addBlock(set, tag);
    }

    // If it's a store with no write-allocate, it incurs direct memory write cycles
    if (isStore && !isWriteAllocate) {
        totalCycles += (byteCount / 4) * 100;
    }
}

// Updates LRU ordering by moving the accessed block to the most recently used position
void Cache::updateLRU(Set& set, unsigned tag) {
    auto it = set.tags[tag]; // Locate the block in the linked list
    set.blocks.splice(set.blocks.end(), set.blocks, it); // Move block to end (most recently used)
    it->access_ts = totalCycles; // Update access timestamp
}

// Adds a new block to the cache, evicting an old one if necessary
void Cache::addBlock(Set& set, unsigned tag) {
    // If the set is full, evict a block using the chosen replacement policy
    if (set.blocks.size() == blockCount) {
        evictBlock(set);
    }

    // Insert the new block into the cache
    Block block{tag, true, false, totalCycles, totalCycles};
    set.blocks.push_back(block);
    set.tags[tag] = --set.blocks.end(); // Update tag map to point to the new block

    // Fetching a block from memory incurs additional cycles
    totalCycles += (byteCount / 4) * 100;
}

// Evicts a block from the cache, writing back if necessary
void Cache::evictBlock(Set& set) {
    auto it = set.blocks.begin(); // Select block to evict (FIFO or LRU)
    Block& evicted = *it;
    set.tags.erase(evicted.tag); // Remove tag mapping

    // If the evicted block is dirty, write it back to memory
    if (evicted.dirty) {
        totalCycles += (byteCount / 4) * 100;
    }

    set.blocks.erase(it); // Remove block from cache
}
