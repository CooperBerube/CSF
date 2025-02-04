#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32( uint32_t val ) {
  UInt256 result = {0};
  result.data[0] = val;
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 7 is the most significant.
UInt256 uint256_create( const uint32_t data[8] ) {
  UInt256 result;
  for (int i = 0; i < 8; i++) {
      result.data[i] = data[i]; // copies data
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex( const char *hex ) {
  UInt256 result = {0};
  size_t len = strlen(hex);
  
  for (size_t i = 0; i < len; i++) {
    char c = hex[len - 1 - i];
    uint32_t value;

    if (c >= '0' && c <= '9') {
      value = c - '0';
    } else if (c >= 'a' && c <= 'f') {
      value = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      value = c - 'A' + 10;
    } else {
      exit(EXIT_FAILURE);
    }

    size_t index = i / 8;
    size_t shift = (i % 8) * 4;
    result.data[index] |= value << shift;
  }

  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex( UInt256 val ) {
  char *hex = NULL;
  // TODO: implement
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits( UInt256 val, unsigned index ) {
  // Fail if index is out of bounds
  if(index >= 8) {
    exit(EXIT_FAILURE);
  }
  uint32_t bits = val.data[index];
  return bits;
}

// Return 1 if bit at given index is set, 0 otherwise.
int uint256_is_bit_set( UInt256 val, unsigned index ) {
  // Fail if index is out of bounds
  if (index >= 256) {
    exit(EXIT_FAILURE);
  }
  unsigned word = index / 32;
  unsigned bit = index % 32;
  int is_bit_set = (val.data[word] & (1U << bit)) != 0;
  return is_bit_set;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add( UInt256 left, UInt256 right ) {
  UInt256 sum;
  uint32_t pieceSum;
  uint32_t carry = 0; // Initialize carry to 0
  for (int i = 0; i < 8; i++) {
    pieceSum = left.data[i] + right.data[i] + carry;
    if (pieceSum < left.data[i] || pieceSum < right.data[i]) {
      carry = 1; // Set carry flag if there's an overflow
    } else {
      carry = 0; // No carry if the sum doesn't overflow
    }
    sum.data[i] = pieceSum;
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub( UInt256 left, UInt256 right ) {
  UInt256 neg_right = uint256_negate(right);
  return uint256_add(left, neg_right);
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate( UInt256 val ) {
  UInt256 result;
  uint32_t one = 1;
  UInt256 increment = uint256_create_from_u32(one);
  for (int i = 0; i < 8; i++) {
    result.data[i] = ~(val.data[i]);
  }
  result = uint256_add(result, increment);
  return result;
}

// Compute the product of two UInt256 values.
UInt256 uint256_mul( UInt256 left, UInt256 right ) {
  UInt256 product = {0};
  // TODO: implement
  return product;
}

UInt256 uint256_lshift( UInt256 val, unsigned shift ) {
  assert(shift < 256); // Ensure shift is within the valid range (less than 256)

  UInt256 result = {0}; // Initialize result to 0
  return result;
}