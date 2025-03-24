#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>

int main( int argc, char **argv ) {
  
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

  if (strcmp(argv[1], "write-allocate")) {
    
  }

  return 0;
}
