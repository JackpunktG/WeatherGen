#include "arena_memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//static mean its only for internal use, and can't be accidentially assess by somewhere else the same function being declared
static ArenaBlock* arena_add_block(Arena *arena, size_t minimumSize)
{
    //checking if size fits in one block
    size_t blockSize = minimumSize > arena->defualtBlockSize ? minimumSize : arena->defualtBlockSize;

    // allocate block struct
    ArenaBlock* block = malloc(sizeof(ArenaBlock));
    if(!block)
    {
        printf("ERROR - could not allocate block struct\n");
        return NULL;
    }
    // allocating the actual memory
    block->memory = malloc(blockSize);
    if(!block->memory)
    {
        free(block);
        printf("ERROR - could not allocate block memory\n");
        return NULL;
    }

    //init the block members
    block->size = blockSize;
    block->used = 0;
    block->next = NULL;

    //link into arena
    if (arena->current)   //if its not the first block, linking it to the last block before allocating it to current
        arena->current->next = block;
    else
        arena->first = block;

    arena->current = block;

    return block;
}

//alows for different alignment
static size_t align_to(size_t size, size_t alignment)
{
    return (size + (alignment -1)) & ~(alignment -1);
}

Arena* arena_init(size_t blockSize, size_t alignment)
{
    //checking for 0 and must be a power of two
    if (alignment == 0 || (alignment & (alignment -1)))
    {
        printf("ERROR - alignment cannot be 0 and must be a power of two\n");
        return NULL;
    }

    if (blockSize != align_to(blockSize, alignment))
    {
        printf("WARNING - Mismatch with defualt Block Size and alignment\n");
        blockSize = align_to(blockSize, alignment);
        printf("New defualtBlockSize based on your alignment: %lu\n", blockSize);
    }

    //Init arena controller struct
    Arena* arena = malloc(sizeof(Arena));
    if (!arena)
    {
        printf("ERROR - arena_init malloc failed\n");
        return NULL;
    }


    //init fields
    arena->defualtBlockSize = blockSize;
    arena->totalAllocated = 0;
    arena->alignment = alignment;
    arena->current = NULL;
    arena->first = NULL;

    // Create first block
    if(!arena_add_block(arena, blockSize))
    {
        free(arena);
        printf("ERROR - Failed to create first block");
        return NULL;
    }

    return arena;
}



void* arena_alloc(Arena* arena, size_t size)
{
    if (!arena || !size)
    {
        printf("ERROR - arena or size are NULL\n");
        return NULL;
    }

    //align size to next mulitiple of 8 bytes
    size = align_to(size, arena->alignment);

    //check if current block has enough space
    if (!arena->current || arena->current->used + size > arena->current->size)
    {
        //new block if size is not enough
        if(!arena_add_block(arena, size))
            return NULL;
    }


    //get the pointer to the space
    void* ptr = arena->current->memory + arena->current->used;


    //## Perhaps some way to verify alignment and fix if not aligned properly??


    //updated current used and total used
    arena->current->used += size;
    arena->totalAllocated += size;

    return ptr;
}

void arena_reset(Arena* arena)
{
    if (!arena)
    {
        printf("ERROR - arena is already NULL before the reset\n");
        return;
    }

    //Marking all blocks as empty
    ArenaBlock* block = arena->first;
    while(block)
    {
        block->used = 0;
        block = block->next;
    }

    arena->current = arena->first;
    arena->totalAllocated = 0;
}

void arena_destroy(Arena* arena)
{
    if (!arena)
    {
        printf("ERROR - arena is already NULL before the destroy\n");
        return;
    }

    //free all blocks
    ArenaBlock* block = arena->first;
    while(block)
    {
        ArenaBlock* next = block->next;
        free(block->memory);
        free(block);
        block = next;
    }

    free(arena);
}
