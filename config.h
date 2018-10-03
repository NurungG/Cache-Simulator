#ifndef __CONFIG_H__
#define __CONFIG_H__

#define K 1024

#define CAPACITY   4*K
#define BLOCK_BITS 4
#define WAY_BITS   2

#define BLOCK_SIZE ( 1 << BLOCK_BITS )  // 2^BLOCK_BITS
#define N_WAY      ( 1 << WAY_BITS )    // 2^WAY_BITS

#define N_ENTRY    ( CAPACITY / BLOCK_SIZE )
#define N_SET      ( N_ENTRY / N_WAY )

#define B_INDEX    N_SET
#define B_OFFSET   BLOCK_BITS
#define B_TAG      ( 32 - B_INDEX - B_OFFSET )

#endif
