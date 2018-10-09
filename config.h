#ifndef __CONFIG_H__
#define __CONFIG_H__

#define K 1024

#define CAPACITY_BITS   4
#define BLOCK_BITS      4
#define WAY_BITS        2

#define CAPACITY    ( (1 << CAPACITY_BITS) * K ) // 2^CAPACITY_BITS * 1024
#define BLOCK_SIZE  ( 1 << BLOCK_BITS )  // 2^BLOCK_BITS
#define N_WAY       ( 1 << WAY_BITS )    // 2^WAY_BITS

#define N_ENTRY     ( CAPACITY / BLOCK_SIZE )
#define N_SET       ( N_ENTRY / N_WAY )

#define B_OFFSET    BLOCK_BITS
#define B_INDEX     ( ( CAPACITY_BITS + 10 ) - BLOCK_BITS - WAY_BITS )
#define B_TAG       ( 64 - B_INDEX - B_OFFSET )

#define OFFSET_MASK ( UINT64_MAX >> ( 64 - B_OFFSET ) )
#define TAG_MASK    ( UINT64_MAX << ( 64 - B_TAG ) )
#define INDEX_MASK  ( UINT64_MAX & (!OFFSET_MASK) & (!TAG_MASK) )

#endif
