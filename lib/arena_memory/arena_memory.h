#ifndef AERNA_MEMORY_H
#define AERNA_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ARENA_BLOCK_SIZE  (1024 * 1024)   //size of block currently 1MB

/* Dynamic size Arena Block Allocation */ //if over the defualt size will make a block for that size
typedef struct ArenaBlock
{
    uint8_t* memory;            //raw memory -> 1 per byte
    size_t size;                //total block size
    size_t used;                //how much of the block is used
    struct ArenaBlock* next;    //link to next block if we need more space
} ArenaBlock;

typedef struct
{
    ArenaBlock* current;        //currently allocating block
    ArenaBlock* first;          //start of our block
    size_t totalAllocated;      //Total byte allocated
    size_t defualtBlockSize;    // size of new block
    size_t alignment;            //number of bits the Arena should be aligned to
} Arena;

Arena* arena_init(size_t defualtBlockSize, size_t alignment);
void arena_destroy(Arena* arena);
void* arena_alloc(Arena* arena, size_t size);
void arena_reset(Arena* arena); //just restting all the allocated counters to zero and ptr to the start for the blocks

#endif // AERNA_MEMORY_H
