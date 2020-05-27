
/** mpool source significantly modified by Mike Mulshine, Jeff Snyder, et al., Princeton University Music Department **/

/**
 In short, mpool is distributed under so called "BSD license",
 
 Copyright (c) 2009-2010 Tatsuhiko Kubo <cubicdaiya@gmail.com>
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of the authors nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* written with C99 style */

#if _WIN32 || _WIN64

#include "..\Inc\leaf-mempool.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-mempool.h"
#include "../leaf.h"

#endif

/**
 * private function
 */
static inline size_t mpool_align(size_t size);
static inline mpool_node_t* create_node(char* block_location, mpool_node_t* next, mpool_node_t* prev, size_t size);
static inline void delink_node(mpool_node_t* node);

/**
 * create memory pool
 */
void mpool_create (char* memory, size_t size, _tMempool* pool)
{
    leaf.header_size = mpool_align(sizeof(mpool_node_t));
    
    pool->mpool = (char*)memory;
    pool->usize  = 0;
    pool->msize  = size;
    
    pool->head = create_node(pool->mpool, NULL, NULL, pool->msize-leaf.header_size);
    
    /*
    for (int i = 0; i < pool->head->size; i++)
    {
        memory[i+leaf.header_size]=0;
    }
    */
    //is zeroing out the memory necessary? This takes a long time on large pools - JS
}

void leaf_pool_init(char* memory, size_t size)
{
    mpool_create(memory, size, &leaf._mempool);
    
    leaf.mempool = &leaf._mempool;
}

/**
 * allocate memory from memory pool
 */
char* mpool_alloc(size_t asize, _tMempool* pool)
{
    // If the head is NULL, the mempool is full
    if (pool->head == NULL)
    {
        leaf_mempool_overrun();
        return NULL;
    }
    
    // Should we alloc the first block large enough or check all blocks and pick the one closest in size?
    size_t size_to_alloc = mpool_align(asize);
    mpool_node_t* node_to_alloc = pool->head;
    
    // Traverse the free list for a large enough block
    while (node_to_alloc->size < size_to_alloc)
    {
        node_to_alloc = node_to_alloc->next;
        
        // If we reach the end of the free list, there
        // are no blocks large enough, return NULL
        if (node_to_alloc == NULL)
        {
            leaf_mempool_overrun();
            return NULL;
        }
    }
    
    // Create a new node after the node to be allocated if there is enough space
    mpool_node_t* new_node;
    size_t leftover = node_to_alloc->size - size_to_alloc;
    node_to_alloc->size = size_to_alloc;
    if (leftover > leaf.header_size)
    {
        long offset = (char*) node_to_alloc - (char*) pool->mpool;
        offset += leaf.header_size + node_to_alloc->size;
        new_node = create_node(&pool->mpool[offset],
                               node_to_alloc->next,
                               node_to_alloc->prev,
                               leftover - leaf.header_size);
    }
    else
    {
        // Add any leftover space to the allocated node to avoid fragmentation
        node_to_alloc->size += leftover;
        
        new_node = node_to_alloc->next;
    }
    
    // Update the head if we are allocating the first node of the free list
    // The head will be NULL if there is no space left
    if (pool->head == node_to_alloc)
    {
        pool->head = new_node;
    }
    
    // Remove the allocated node from the free list
    delink_node(node_to_alloc);
    
    pool->usize += leaf.header_size + node_to_alloc->size;
    
    if (leaf.clearOnAllocation > 0)
    {
        char* new_pool = (char*)node_to_alloc->pool;
        for (int i = 0; i < node_to_alloc->size; i++) new_pool[i] = 0;
    }
    
    // Return the pool of the allocated node;
    return node_to_alloc->pool;
}


/**
 * allocate memory from memory pool and also clear that memory to be blank
 */
char* mpool_calloc(size_t asize, _tMempool* pool)
{
    // If the head is NULL, the mempool is full
    if (pool->head == NULL)
    {
        leaf_mempool_overrun();
        return NULL;
    }
    
    // Should we alloc the first block large enough or check all blocks and pick the one closest in size?
    size_t size_to_alloc = mpool_align(asize);
    mpool_node_t* node_to_alloc = pool->head;
    
    // Traverse the free list for a large enough block
    while (node_to_alloc->size < size_to_alloc)
    {
        node_to_alloc = node_to_alloc->next;
        
        // If we reach the end of the free list, there
        // are no blocks large enough, return NULL
        if (node_to_alloc == NULL)
        {
            leaf_mempool_overrun();
            return NULL;
        }
    }
    
    // Create a new node after the node to be allocated if there is enough space
    mpool_node_t* new_node;
    size_t leftover = node_to_alloc->size - size_to_alloc;
    node_to_alloc->size = size_to_alloc;
    if (leftover > leaf.header_size)
    {
        long offset = (char*) node_to_alloc - (char*) pool->mpool;
        offset += leaf.header_size + node_to_alloc->size;
        new_node = create_node(&pool->mpool[offset],
                               node_to_alloc->next,
                               node_to_alloc->prev,
                               leftover - leaf.header_size);
    }
    else
    {
        // Add any leftover space to the allocated node to avoid fragmentation
        node_to_alloc->size += leftover;
        
        new_node = node_to_alloc->next;
    }
    
    // Update the head if we are allocating the first node of the free list
    // The head will be NULL if there is no space left
    if (pool->head == node_to_alloc)
    {
        pool->head = new_node;
    }
    
    // Remove the allocated node from the free list
    delink_node(node_to_alloc);
    
    pool->usize += leaf.header_size + node_to_alloc->size;
    // Format the new pool
    char* new_pool = (char*)node_to_alloc->pool;
    for (int i = 0; i < node_to_alloc->size; i++) new_pool[i] = 0;
    // Return the pool of the allocated node;
    return node_to_alloc->pool;
}

char* leaf_alloc(size_t size)
{
    //printf("alloc %i\n", size);
    char* block = mpool_alloc(size, &leaf._mempool);
    
    if (block == NULL) leaf_mempool_overrun();
    
    return block;
}

char* leaf_calloc(size_t size)
{
    //printf("alloc %i\n", size);
    char* block = mpool_calloc(size, &leaf._mempool);
    
    if (block == NULL) leaf_mempool_overrun();
    
    
    return block;
}

void mpool_free(char* ptr, _tMempool* pool)
{
    //if (ptr < pool->mpool || ptr >= pool->mpool + pool->msize)
    // Get the node at the freed space
    mpool_node_t* freed_node = (mpool_node_t*) (ptr - leaf.header_size);
    
    pool->usize -= leaf.header_size + freed_node->size;
    
    // Check each node in the list against the newly freed one to see if it's adjacent in memory
    mpool_node_t* other_node = pool->head;
    mpool_node_t* next_node;
    while (other_node != NULL)
    {
        next_node = other_node->next;
        // Check if a node is directly after the freed node
        if ((long) freed_node + (leaf.header_size + freed_node->size) == (long) other_node)
        {
            // Increase freed node's size
            freed_node->size += leaf.header_size + other_node->size;
            // If we are merging with the head, move the head forward
            if (other_node == pool->head) pool->head = pool->head->next;
            // Delink the merged node
            delink_node(other_node);
        }
        
        // Check if a node is directly before the freed node
        else if ((long) other_node + (leaf.header_size + other_node->size) == (long) freed_node)
        {
            // Increase the merging node's size
            other_node->size += leaf.header_size + freed_node->size;
            
            if (other_node != pool->head)
            {
                // Delink the merging node
                delink_node(other_node);
                // Attach the merging node to the head
                other_node->next = pool->head;
                // Merge
                freed_node = other_node;
            }
            else
            {
                // If we are merging with the head, move the head forward
                pool->head = pool->head->next;
                // Merge
                freed_node = other_node;
            }
        }
        
        other_node = next_node;
    }
    
    // Ensure the freed node is attached to the head
    freed_node->next = pool->head;
    if (pool->head != NULL) pool->head->prev = freed_node;
    pool->head = freed_node;
    
    // Format the freed pool
    //    char* freed_pool = (char*)freed_node->pool;
    //    for (int i = 0; i < freed_node->size; i++) freed_pool[i] = 0;
}

void leaf_free(char* ptr)
{
    mpool_free(ptr, &leaf._mempool);
}

size_t mpool_get_size(_tMempool* pool)
{
    return pool->msize;
}

size_t mpool_get_used(_tMempool* pool)
{
    return pool->usize;
}

size_t leaf_pool_get_size(void)
{
    return mpool_get_size(&leaf._mempool);
}

size_t leaf_pool_get_used(void)
{
    return mpool_get_used(&leaf._mempool);
}

char* leaf_pool_get_pool(void)
{
    char* buff = leaf._mempool.mpool;
    
    return buff;
}

/**
 * align byte boundary
 */
static inline size_t mpool_align(size_t size) {
    return (size + (MPOOL_ALIGN_SIZE - 1)) & ~(MPOOL_ALIGN_SIZE - 1);
}

static inline mpool_node_t* create_node(char* block_location, mpool_node_t* next, mpool_node_t* prev, size_t size)
{
    mpool_node_t* node = (mpool_node_t*)block_location;
    node->pool = block_location + leaf.header_size;
    node->next = next;
    node->prev = prev;
    node->size = size;
    
    return node;
}

static inline void delink_node(mpool_node_t* node)
{
    // If there is a node after the node to remove
    if (node->next != NULL)
    {
        // Close the link
        node->next->prev = node->prev;
    }
    // If there is a node before the node to remove
    if (node->prev != NULL)
    {
        // Close the link
        node->prev->next = node->next;
    }
    
    node->next = NULL;
    node->prev = NULL;
}

void leaf_mempool_overrun(void)
{
    LEAF_error(1);
    //TODO: we should make a set of real error codes that are in an enum type
}

void tMempool_init(tMempool* const mp, char* memory, size_t size)
{
    tMempool_initToPool(mp, memory, size, &leaf.mempool);
}

void tMempool_free(tMempool* const mp)
{
    tMempool_freeFromPool(mp, &leaf.mempool);
}

void    tMempool_initToPool     (tMempool* const mp, char* memory, size_t size, tMempool* const mem)
{
    _tMempool* mm = *mem;
    _tMempool* m = *mp = (_tMempool*) mpool_alloc(sizeof(_tMempool), mm);
    
    mpool_create (memory, size, m);
}

void    tMempool_freeFromPool   (tMempool* const mp, tMempool* const mem)
{
    _tMempool* mm = *mem;
    _tMempool* m = *mp;
    
    mpool_free((char*)m, mm);
}
