#ifndef CACHESIMFUNCS_H
#define CACHESIMFUNCS_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <cstdint>

// Cache simulation functions and data structures
// This header file defines the structures and functions used for simulating a cache memory system.

class Block {
public:
    unsigned int tag = 0; // Tag of the memory block
    bool valid = false; // Validity bit to check if the block is valid
    bool dirty = false; // Dirty bit to check if the block is modified
    unsigned int load_ts = 0; // Timestamp of when the block was loaded
    unsigned int access_ts = 0; // Timestamp of the last access to the block
};

class Set {
public:
    std::map<unsigned, std::list<Block>::iterator> tags; // Map of tags to block iterators
    std::list<Block> blocks; // List of blocks in the set
};

class Cache {
public:
    Cache() = default; // Default constructor
    explicit Cache(char* args[]); // Constructor with command line arguments

    std::vector<Set> sets; // Vector of sets in the cache
    unsigned int setCount = 0; // Number of sets
    unsigned int blockCount = 0; // Number of blocks per set
    unsigned int byteCount = 0; // Number of bytes per block
    bool isWriteAllocate = false; // Write-allocate policy
    bool isWriteThrough = false; // Write-through policy
    bool isLRU = false; // LRU replacement policy

    unsigned int totalCycles = 0; // Total cycles for the simulation
    unsigned int loads = 0; // Total number of load operations
    unsigned int loadHits = 0; // Total number of load hits
    unsigned int loadMisses = 0; // Total number of load misses
    unsigned int stores = 0; // Total number of store operations
    unsigned int storeHits = 0; // Total number of store hits
    unsigned int storeMisses = 0; // Total number of store misses

    // Load an address into the cache
    void loadAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);

    // Store an address into the cache
    void storeAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits);

    // Process an address for load or store operation
    void processAddress(std::uint32_t address, unsigned byteOffsetBits, unsigned indexBits, bool isStore);

    // Functionality for a hit
    void handleHit(Set& set, unsigned tag, bool isStore);

    // Functionality for a miss
    void handleMiss(Set& set, unsigned tag, bool isStore);

    // Update the LRU status of a block
    void updateLRU(Set& set, unsigned tag);

    // Add a block to the set
    void addBlock(Set& set, unsigned tag);

    // Update the LRU status of a block
    void evict(unsigned int index, unsigned int tag);

    // Evict a block from the set
    void evictBlock(Set& set);
};

#endif // CACHESIMFUNCS_H