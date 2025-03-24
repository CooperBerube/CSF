#include "cacheSimFuncs.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <cstring>

using std::string;

  /*
   *  This program simulates a cache memory system based on the parameters provided
   *  as command line arguments. The user must specify the number of sets, blocks,
   *  block size, write policy (write-allocate or no-write-allocate), 
   *  write-through or write-back policy, and the replacement policy (LRU or FIFO).
   *
   *  Usage: ./csim <sets> <blocks> <block size> <write-allocate OR no-write-allocate>
   *               <write-through OR write-back> <lru OR fifo>
   *
   *  Example: ./csim 16 4 64 write-allocate write-through lru < input_file
  */

int main( int argc, char **argv ) {

  // Validation for command line arguements

  if (argc != 7) {
    std::cerr << "Usage: ./csim <sets> <blocks> <write-allocate OR no--write allocate> <write-through OR write-back> <lru OR fifo>" << std::endl;
    return 1;
  }

  int cacheSets;
  int setBlocks;
  int blockBytes;

  try {
    cacheSets = std::stoi(argv[1]);
    setBlocks = std::stoi(argv[2]);
    blockBytes = std::stoi(argv[3]);
  } catch (const std::exception& e) {
    std::cout << "integer value incorrect, first three inputs need to be " << std::endl;
    return 1;
  } 

  if (cacheSets <= 0 || log2(cacheSets) != floor(log2(cacheSets))) {
    std::cout << "variable 1 is not a power of 2" << std::endl;
    return 1;
  }
  if (cacheSets <= 0 || log2(setBlocks) != floor(log2(setBlocks))) {
    std::cout << "variable 2 is not a power of 2" << std::endl;
    return 1;
  }
  if (cacheSets <= 0 || log2(blockBytes) != floor(log2(blockBytes))) {
    std::cout << "variable 3 is not a power of 2" << std::endl;
    return 1;
  }

  if (strcmp(argv[4], "write-allocate") != 0 || strcmp(argv[4], "no-write-allocate") != 0) {
    std::cout << "fourth argument is write-allocate or no-write-allocate" << std::endl;
    return 1;
  }

  if (strcmp(argv[5], "write-through") != 0 || strcmp(argv[5], "write-back") != 0) {
    std::cout << "fifth argument needs to be write-through or write-back" << std::endl;
    return 1;
  }

  if (strcmp(argv[6], "lru") != 0 || strcmp(argv[6], "fifo") != 0) {
    std::cout << "sixth argument needs to be lru or fifo" << std::endl;
    return 1;
  }

  if (strcmp(argv[5], "write-back") == 0 && strcmp(argv[4], "no-write-allocate") == 0) {
    std::cout << "write-back and no-write-allocate can't both be declared" << std::endl;
    return 1;
  }

  // If all validations pass, we can proceed to create the cache simulation

  Cache cache(argv);
  string line;

  while (std::getline(std::cin, line)) {
    // TODO: Implement
  } 

  return 0;
}