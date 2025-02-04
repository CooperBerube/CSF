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
    char *hex = (char*) malloc(65 * sizeof(char));
    if (hex == NULL) {
        return NULL; 
    }

    char buf[9];
    int index = 0;

    for (int i = 7; i >= 0; i--) {
        uint32_t num = uint256_get_bits(val, i);
        sprintf(buf, "%08x", num);

        for (int j = 0; j < 8; j++) {
            hex[index++] = buf[j];
        }
    }

    hex[index] = '\0';

    char *result = hex;
    while (*result == '0' && *(result + 1) != '\0') {
        result++;
    }

    char *result_no_zeros = strdup(result);
    free(hex);

    return result_no_zeros;
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
  UInt256 sum = {0};
  uint32_t carry = 0;

  for (int i = 0; i < 8; i++) {
    uint64_t temp = (uint64_t)left.data[i] + right.data[i] + carry;
    sum.data[i] = (uint32_t)temp;
    carry = temp >> 32;
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

    for (int i = 0; i < 256; i++) {
        if (uint256_is_bit_set(left, i)) {
            product = uint256_add(product, uint256_lshift(right, i));
        }
    }

    return product;
}

UInt256 uint256_lshift( UInt256 val, unsigned shift ) {
  assert(shift < 256); // Ensure shift is within the valid range (less than 256)

  UInt256 result = val; // Initialize result to 0

  for (int i = 0; i < shift; i++) {
    uint32_t next = 0;
    uint32_t curr = 0;
    for (int j = 0; j < 8; j++) {
      curr = result.data[j] & (1 << 31);
      if (curr != 0) {
        curr = 1;
      } else {
        curr = 0;
      }
      result.data[j] = (result.data[j] << 1) + next;
      next = curr;
    }
  }
  return result;
}