#include <iostream>

int main( int argc, char **argv ) {
  
  if (argc != 7) {
    std::cerr << "Usage: ./csim <sets> <blocks> <write-allocate OR no--write allocate> <write-through OR write-back> <lru OR fifo>" << std::endl;
    return 1;
  }

  return 0;
}
