#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
} TestObjs;

// Helper functions for implementing tests
void set_all( UInt256 *val, uint32_t wordval );

#define ASSERT_SAME( expected, actual ) \
do { \
  ASSERT( expected.data[0] == actual.data[0] ); \
  ASSERT( expected.data[1] == actual.data[1] ); \
  ASSERT( expected.data[2] == actual.data[2] ); \
  ASSERT( expected.data[3] == actual.data[3] ); \
  ASSERT( expected.data[4] == actual.data[4] ); \
  ASSERT( expected.data[5] == actual.data[5] ); \
  ASSERT( expected.data[6] == actual.data[6] ); \
  ASSERT( expected.data[7] == actual.data[7] ); \
} while ( 0 )

#define INIT_FROM_ARR( val, arr ) \
do { \
  for ( int i = 0; i < 8; ++i ) \
    val.data[i] = arr[i]; \
} while ( 0 )

// Functions to create and cleanup the test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Declarations of test functions
void test_get_bits( TestObjs *objs );
void test_is_bit_set( TestObjs *objs );
void test_create_from_u32( TestObjs *objs );
void test_create( TestObjs *objs );
void test_create_from_hex( TestObjs *objs );
void test_format_as_hex( TestObjs *objs );
void test_add( TestObjs *objs );
void test_sub( TestObjs *objs );
void test_negate( TestObjs *objs );
void test_neg_overflow( TestObjs *objs );
void test_mul( TestObjs *objs );
void test_lshift( TestObjs *objs );

void test_add_zero( TestObjs *objs );
void test_sub_zero( TestObjs *objs );
void test_mul_zero( TestObjs *objs );
void test_mul_one( TestObjs *objs );
void test_lshift_zero( TestObjs *objs );

void test_add_overflow( TestObjs *objs );
void test_sub_overflow( TestObjs *objs );

void test_add_genfact();
void test_sub_genfact();
void test_mul_genfact();

int main( int argc, char **argv ) {
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST( test_get_bits );
  TEST( test_is_bit_set );
  TEST( test_create_from_u32 );
  TEST( test_create );
  TEST( test_create_from_hex );
  TEST( test_format_as_hex );
  TEST( test_add );
  TEST( test_sub );
  TEST( test_negate );
  TEST( test_neg_overflow );
  TEST( test_mul );
  TEST( test_lshift );

  TEST( test_add_zero );
  TEST( test_sub_zero );
  TEST( test_mul_zero );
  TEST( test_lshift_zero );

  TEST ( test_add_overflow );
  TEST ( test_sub_overflow );

  TEST ( test_add_genfact );
  TEST ( test_mul_genfact );
  TEST ( test_sub_genfact );
  TEST ( test_mul_one );

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all( UInt256 *val, uint32_t wordval ) {
  for ( unsigned i = 0; i < 8; ++i ) {
    val->data[i] = wordval;
  }
}

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs ) );

  // initialize several UInt256 values "manually"
  set_all( &objs->zero, 0 );
  set_all( &objs->one, 0 );
  objs->one.data[0] = 1U;
  set_all( &objs->max, 0xFFFFFFFFU );

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR( objs->msb_set, msb_set_data );

  return objs;
}

void cleanup( TestObjs *objs ) {
  free( objs );
}

void test_get_bits( TestObjs *objs ) {
  ASSERT( 0U == uint256_get_bits( objs->zero, 0 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 1 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 2 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 3 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 4 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 5 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 6 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 7 ) );

  ASSERT( 1U == uint256_get_bits( objs->one, 0 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 1 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 2 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 3 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 4 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 5 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 6 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 7 ) );

  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 0 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 1 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 2 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 3 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 4 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 5 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 6 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 7 ) );
}

void test_is_bit_set( TestObjs *objs ) {
  // no bits should be set in the zero value
  for ( int i = 0; i < 256; ++i )
    ASSERT( !uint256_is_bit_set( objs->zero, i ) );

  // only bit 0 should be set in the one value
  ASSERT( uint256_is_bit_set( objs->one, 0 ) );
  for ( int i = 1; i < 256; ++i )
    ASSERT( !uint256_is_bit_set( objs->one, i ) );

  // only bit 255 should be set in the msb_set value
  for ( int i = 0; i < 255; ++i )
    ASSERT( !uint256_is_bit_set( objs->msb_set, i ) );
  ASSERT( uint256_is_bit_set( objs->msb_set, 255 ) );

  // all bits should be set in the max value
  for ( int i = 0; i < 256; ++i )
    ASSERT( uint256_is_bit_set( objs->max, i ) );
}

void test_create_from_u32( TestObjs *objs ) {
  UInt256 zero = uint256_create_from_u32( 0U );
  UInt256 one = uint256_create_from_u32( 1U );

  ASSERT_SAME( objs->zero, zero );
  ASSERT_SAME( objs->one, one );
}

void test_create( TestObjs *objs ) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create( data1 );
  ASSERT( 1U == val1.data[0] );
  ASSERT( 2U == val1.data[1] );
  ASSERT( 3U == val1.data[2] );
  ASSERT( 4U == val1.data[3] );
  ASSERT( 5U == val1.data[4] );
  ASSERT( 6U == val1.data[5] );
  ASSERT( 7U == val1.data[6] );
  ASSERT( 8U == val1.data[7] );
}

void test_create_from_hex( TestObjs *objs ) {
  UInt256 zero = uint256_create_from_hex( "0" );
  ASSERT_SAME( objs->zero, zero );

  UInt256 one = uint256_create_from_hex( "1" );
  ASSERT_SAME( objs->one, one );

  UInt256 max = uint256_create_from_hex( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" );
  ASSERT_SAME( objs->max, max );
}

void test_format_as_hex( TestObjs *objs ) {
  char *s;

  s = uint256_format_as_hex( objs->zero );
  ASSERT( 0 == strcmp( "0", s ) );
  free( s );

  s = uint256_format_as_hex( objs->one );
  ASSERT( 0 == strcmp( "1", s ) );
  free( s );

  s = uint256_format_as_hex( objs->max );
  ASSERT( 0 == strcmp( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s ) );
  free( s );
}

void test_add( TestObjs *objs ) {
  UInt256 result;

  result = uint256_add( objs->zero, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_add( objs->zero, objs->one );
  ASSERT_SAME( objs->one, result );

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR( two, two_data );
  result = uint256_add( objs->one, objs->one );
  ASSERT_SAME( two, result );

  result = uint256_add( objs->max, objs->one );
  ASSERT_SAME( objs->zero, result );

  
  //added tests
  result = uint256_add( objs->msb_set, objs->msb_set );
  ASSERT_SAME( objs->zero, result );

  
  UInt256 shifted_one = { { 0,1,0,0,0,0,0,0 } };
  UInt256 max_in_first_box = { {0x80000000U,0U,0U,0U,0U,0U,0U,0U} };
  result = uint256_add(max_in_first_box, max_in_first_box);
  ASSERT_SAME(shifted_one, result);
}

void test_sub( TestObjs *objs ) {
  UInt256 result;

  result = uint256_sub( objs->zero, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_sub( objs->one, objs->one );
  ASSERT_SAME( objs->zero, result );

  result = uint256_sub( objs->zero, objs->one );
  ASSERT_SAME( objs->max, result );

  //Added tests
  UInt256 shifted_one = { { 0,1,0,0,0,0,0,0 } };
  UInt256 max_in_first_box = { {0x80000000U,0U,0U,0U,0U,0U,0U,0U} };
  result = uint256_sub(shifted_one, max_in_first_box);
  ASSERT_SAME(max_in_first_box,result);
}

void test_negate( TestObjs *objs ) {
  UInt256 result;

  result = uint256_negate( objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_negate( objs->one );
  ASSERT_SAME( objs->max, result );

  result = uint256_negate( objs->max );
  ASSERT_SAME( objs->one, result );

  //Added tests
  UInt256 little_number = { {0xffffffffU,0xffffffffU,0xffffffffU,0xffffffffU,0U,0U,0U,0U} };
  UInt256 big_number = { {0U,0U,0U,0U,0xffffffffU,0xffffffffU,0xffffffffU,0xffffffffU} };
  UInt256 inv_ln = { {1U,0U,0U,0U,0xffffffffU,0xffffffffU,0xffffffffU,0xffffffffU} };
  UInt256 inv_bn = { {0U,0U,0U,0U,1U,0U,0U,0U} };

  result = uint256_negate(little_number);
  ASSERT_SAME(inv_ln,result);

  result = uint256_negate(big_number);
  ASSERT_SAME(inv_bn,result);
  
}

void test_neg_overflow( TestObjs *objs ) {
  UInt256 result;

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR( two, two_data );

  result = uint256_sub( objs->one, objs->max );
  ASSERT_SAME( two, result );
}

void test_mul( TestObjs *objs ) {
  UInt256 left, right, result;

  // some very basic tests

  result = uint256_mul( objs->one, objs->one );
  ASSERT_SAME( objs->one, result );

  result = uint256_mul( objs->one, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_mul( objs->zero, objs->one );
  ASSERT_SAME( objs->zero, result );

  UInt256 two = { { 2,0,0,0,0,0,0,0 } }, four = { { 4,0,0,0,0,0,0,0 } };
  result = uint256_mul( two, two );
  ASSERT_SAME( four, result );

  // a more complicated test

  // fc42c691d6284761fb49dd54f3a13eb * 4cfd2c7d8790c50c280ff0ff77617a8e = 4bdd4cc8b6067f7617c05917f828d17a26046ba5f436cb7df595f6c68c00a5a
  left.data[0] = 0x4f3a13ebU;
  left.data[1] = 0x1fb49dd5U;
  left.data[2] = 0x1d628476U;
  left.data[3] = 0xfc42c69U;
  left.data[4] = 0x0U;
  left.data[5] = 0x0U;
  left.data[6] = 0x0U;
  left.data[7] = 0x0U;
  right.data[0] = 0x77617a8eU;
  right.data[1] = 0x280ff0ffU;
  right.data[2] = 0x8790c50cU;
  right.data[3] = 0x4cfd2c7dU;
  right.data[4] = 0x0U;
  right.data[5] = 0x0U;
  right.data[6] = 0x0U;
  right.data[7] = 0x0U;
  result = uint256_mul(left, right);
  ASSERT(0x68c00a5aU == result.data[0]);
  ASSERT(0xdf595f6cU == result.data[1]);
  ASSERT(0x5f436cb7U == result.data[2]);
  ASSERT(0xa26046baU == result.data[3]);
  ASSERT(0x7f828d17U == result.data[4]);
  ASSERT(0x617c0591U == result.data[5]);
  ASSERT(0x8b6067f7U == result.data[6]);
  ASSERT(0x4bdd4ccU == result.data[7]);
}

void test_lshift( TestObjs *objs ) {
  UInt256 result;

  UInt256 two = { { 2,0,0,0,0,0,0,0 } }, four = { { 4,0,0,0,0,0,0,0 } }, shifted_one = { { 0,1,0,0,0,0,0,0 } };

  // some very basic tests

  result = uint256_lshift( objs->one, 0 );
  ASSERT_SAME( objs->one, result );

  result = uint256_lshift( objs->one, 1 );
  ASSERT_SAME( two, result );

  result = uint256_lshift( objs->one, 2 );
  ASSERT_SAME( four, result );

  //Added tests
  result = uint256_lshift( objs->one, 255 );
  ASSERT_SAME( objs->msb_set, result);

  result = uint256_lshift( objs->one, 32);
  ASSERT_SAME( shifted_one, result);

  result = uint256_lshift( objs->one, 0);
  ASSERT_SAME( objs->one, result);

  // a more complicated test
  {
    // Test shifting 727767d07ccff5fe25cd125b4523e8c7db1b8d1a2c8a2830284d72bb872c33a5 left by 50 bit(s)
    uint32_t arr[8] = {0x872c33a5U, 0x284d72bbU, 0x2c8a2830U, 0xdb1b8d1aU, 0x4523e8c7U, 0x25cd125bU, 0x7ccff5feU, 0x727767d0U};
    UInt256 val;
    INIT_FROM_ARR( val, arr );
    uint32_t expected_arr[8] = {0x00000000U, 0xce940000U, 0xcaee1cb0U, 0xa0c0a135U, 0x3468b228U, 0xa31f6c6eU, 0x496d148fU, 0xd7f89734U};
    UInt256 expected;
    INIT_FROM_ARR( expected, expected_arr );
    result = uint256_lshift( val, 50U );
    ASSERT_SAME( expected, result );
  }
}

// added tests

void test_add_zero( TestObjs *objs ) {
  UInt256 result;
  result = uint256_add( objs->zero, objs->one );
  ASSERT_SAME( objs->one, result );
}

void test_sub_zero( TestObjs *objs ) {
  UInt256 result;
  result = uint256_sub( objs->one, objs->zero );
  ASSERT_SAME( objs->one, result );
}

void test_mul_zero( TestObjs *objs ) {
  UInt256 result;
  result = uint256_mul( objs->one, objs->zero );
  ASSERT_SAME( objs->zero, result );
}

void test_mul_one( TestObjs *objs ) {
  UInt256 result;
  result = uint256_mul( objs->one, objs->one );
  ASSERT_SAME( objs->one, result);
}

void test_lshift_zero( TestObjs *objs ) {
  UInt256 result;
  result = uint256_lshift( objs->one, 0);
  ASSERT_SAME( objs->one, result );
}

void test_add_overflow( TestObjs *objs ) {
  UInt256 result;
  result = uint256_add( objs->max, objs->one);
  ASSERT_SAME( objs->zero, result );
}

void test_sub_overflow( TestObjs *objs ) {
  UInt256 result;
  result = uint256_sub( objs->zero, objs->one);
  ASSERT_SAME( objs->max, result );
}

// 50a3b0bc4719744b525c1526194488054b5feaa98f8991d5daade3febd3f2275 + 996fc7bd0d5717312594becc2b92cedeb3da172d39edfa60f3e33d1490066acb = ea13787954708b7c77f0d3f244d756e3ff3a01d6c9778c36ce9121134d458d40
void test_add_genfact() {
  UInt256 left, right, result;

  left.data[0] = 0xbd3f2275U;
  left.data[1] = 0xdaade3feU;
  left.data[2] = 0x8f8991d5U;
  left.data[3] = 0x4b5feaa9U;
  left.data[4] = 0x19448805U;
  left.data[5] = 0x525c1526U;
  left.data[6] = 0x4719744bU;
  left.data[7] = 0x50a3b0bcU;
  right.data[0] = 0x90066acbU;
  right.data[1] = 0xf3e33d14U;
  right.data[2] = 0x39edfa60U;
  right.data[3] = 0xb3da172dU;
  right.data[4] = 0x2b92cedeU;
  right.data[5] = 0x2594beccU;
  right.data[6] = 0x0d571731U;
  right.data[7] = 0x996fc7bdU;
  result = uint256_add(left, right);
  ASSERT(0x4d458d40U == result.data[0]);
  ASSERT(0xce912113U == result.data[1]);
  ASSERT(0xc9778c36U == result.data[2]);
  ASSERT(0xff3a01d6U == result.data[3]);
  ASSERT(0x44d756e3U == result.data[4]);
  ASSERT(0x77f0d3f2U == result.data[5]);
  ASSERT(0x54708b7cU == result.data[6]);
  ASSERT(0xea137879U == result.data[7]);
}
// 919c9e39f502d60f98a2336af243a1d * 58fb5703128bec6e774a6df57bd6127 = 329ccc71083459f44740017e4c9ba0292d0d1055787cc37ea920732af2d76b
void test_mul_genfact() {
  UInt256 left, right, result;

  left.data[0] = 0xaf243a1dU;
  left.data[1] = 0xf98a2336U;
  left.data[2] = 0x9f502d60U;
  left.data[3] = 0x0919c9e3U;
  left.data[4] = 0x00000000U;
  left.data[5] = 0x00000000U;
  left.data[6] = 0x00000000U;
  left.data[7] = 0x00000000U;
  right.data[0] = 0x57bd6127U;
  right.data[1] = 0xe774a6dfU;
  right.data[2] = 0x3128bec6U;
  right.data[3] = 0x058fb570U;
  right.data[4] = 0x00000000U;
  right.data[5] = 0x00000000U;
  right.data[6] = 0x00000000U;
  right.data[7] = 0x00000000U;
  result = uint256_mul(left, right);
  ASSERT(0x2af2d76bU == result.data[0]);
  ASSERT(0x7ea92073U == result.data[1]);
  ASSERT(0x55787cc3U == result.data[2]);
  ASSERT(0x292d0d10U == result.data[3]);
  ASSERT(0x7e4c9ba0U == result.data[4]);
  ASSERT(0xf4474001U == result.data[5]);
  ASSERT(0x71083459U == result.data[6]);
  ASSERT(0x00329cccU == result.data[7]);
}
// 75fc96fc9f84c32e71396eec6e8d205b3804664645d8aa94a96cddb4485f957 - 20049d006157fca28de5db3b938bf1d4e958ecbc040cc7613d766679361a13d = 
// 55f7f9fc3e2cc68be35393b0db012e864eab798a41cbe3336bf6773b124581a
void test_sub_genfact () {
  UInt256 left, right, result;

  left.data[0] = 0x4485f957U;
  left.data[1] = 0x4a96cddbU;
  left.data[2] = 0x645d8aa9U;
  left.data[3] = 0xb3804664U;
  left.data[4] = 0xc6e8d205U;
  left.data[5] = 0xe71396eeU;
  left.data[6] = 0xc9f84c32U;
  left.data[7] = 0x075fc96fU;
  right.data[0] = 0x9361a13dU;
  right.data[1] = 0x13d76667U;
  right.data[2] = 0xc040cc76U;
  right.data[3] = 0x4e958ecbU;
  right.data[4] = 0xb938bf1dU;
  right.data[5] = 0x28de5db3U;
  right.data[6] = 0x06157fcaU;
  right.data[7] = 0x020049d0U;
  result = uint256_sub(left, right);
  ASSERT(0xb124581aU == result.data[0]);
  ASSERT(0x36bf6773U == result.data[1]);
  ASSERT(0xa41cbe33U == result.data[2]);
  ASSERT(0x64eab798U == result.data[3]);
  ASSERT(0x0db012e8U == result.data[4]);
  ASSERT(0xbe35393bU == result.data[5]);
  ASSERT(0xc3e2cc68U == result.data[6]);
  ASSERT(0x055f7f9fU == result.data[7]);
}