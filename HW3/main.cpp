#include "cacheSimFuncs.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

/**
 * Checks if a given value is a power of two.
 * This is essential for cache configurations, as cache parameters
 * (number of sets, blocks per set, and block size) must be powers of two.
 *
 * @param value Integer to check.
 * @return True if value is a power of two, otherwise false.
 */
bool isPowerOfTwo(int value) {
    return value > 0 && (value & (value - 1)) == 0;
}

/**
 * Validates the command-line arguments for the cache simulator.
 * Ensures that the correct number of arguments is provided and that
 * the cache parameters meet the required constraints.
 *
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 * @throws invalid_argument If arguments are missing, incorrectly formatted,
 *         or violate logical constraints.
 */
void validateArguments(int argc, char **argv) {
    if (argc != 7) {
        throw invalid_argument("Usage: ./csim <sets> <blocks> <block size> <write-allocate OR no-write-allocate> <write-through OR write-back> <lru OR fifo>");
    }

    int cacheSets = stoi(argv[1]);  // Number of sets in the cache
    int setBlocks = stoi(argv[2]);  // Number of blocks per set
    int blockBytes = stoi(argv[3]); // Size of each block in bytes

    // Ensure the first three arguments are powers of two (as required for cache indexing)
    if (!isPowerOfTwo(cacheSets) || !isPowerOfTwo(setBlocks) || !isPowerOfTwo(blockBytes)) {
        throw invalid_argument("First three arguments must be powers of 2.");
    }

    string writeAllocate = argv[4];       // "write-allocate" or "no-write-allocate"
    string writePolicy = argv[5];         // "write-through" or "write-back"
    string replacementPolicy = argv[6];   // "lru" or "fifo"

    // Validate write-allocate policy
    if (writeAllocate != "write-allocate" && writeAllocate != "no-write-allocate") {
        throw invalid_argument("Fourth argument must be 'write-allocate' or 'no-write-allocate'.");
    }

    // Validate write policy
    if (writePolicy != "write-through" && writePolicy != "write-back") {
        throw invalid_argument("Fifth argument must be 'write-through' or 'write-back'.");
    }

    // Validate replacement policy
    if (replacementPolicy != "lru" && replacementPolicy != "fifo") {
        throw invalid_argument("Sixth argument must be 'lru' or 'fifo'.");
    }

    // Logical constraint: "write-back" must be used with "write-allocate"
    if (writePolicy == "write-back" && writeAllocate == "no-write-allocate") {
        throw invalid_argument("'write-back' and 'no-write-allocate' cannot both be selected.");
    }
}

/**
 * Main function for cache simulation.
 * Reads memory access traces from standard input and processes them.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS if execution is successful, EXIT_FAILURE otherwise.
 */
int main(int argc, char **argv) {
    try {
        // Validate command-line arguments before proceeding
        validateArguments(argc, argv);

        // Initialize the cache based on input parameters
        Cache cache(argv);
        string line;

        // Read memory accesses from standard input line by line
        while (getline(cin, line)) {
            istringstream iss(line);
            vector<string> wordVector(3);
            
            // Read three parts of the input line (operation, address, size)
            for (auto &word : wordVector) {
                iss >> word;
            }

            // Convert the memory address from hex string to uint32_t
            uint32_t address = stoul(wordVector[1], nullptr, 16);

            // Calculate the number of bits for block offset and index
            unsigned blockOffsetBits = log2(cache.byteCount);
            unsigned indexBits = log2(cache.setCount);

            // Determine whether the access is a load or store operation
            if (wordVector[0] == "l") {
                cache.loadAddress(address, blockOffsetBits, indexBits);
            } else {
                cache.storeAddress(address, blockOffsetBits, indexBits);
            }
        }

        // Print final cache performance statistics
        cout << "Total loads: " << cache.loads << "\n"
             << "Total stores: " << cache.stores << "\n"
             << "Load hits: " << cache.loadHits << "\n"
             << "Load misses: " << cache.loadMisses << "\n"
             << "Store hits: " << cache.storeHits << "\n"
             << "Store misses: " << cache.storeMisses << "\n"
             << "Total cycles: " << cache.totalCycles << endl;
             
    } catch (const exception &e) {
        // Print error message if an exception occurs and exit with failure
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
