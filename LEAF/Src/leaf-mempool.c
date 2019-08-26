
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

#include "../Inc/leaf-mempool.h"


#define NUM_BLOCKS 50

char memory[MPOOL_POOL_SIZE];
mpool_pool_t blocks[NUM_BLOCKS]; // 
mpool_t leaf_pool;

/**
 * private function
 */
static inline size_t mpool_align(size_t size);


/**
 * create memory pool
 */
void mpool_create (size_t size, mpool_t* pool)
{
    if (size > MPOOL_POOL_SIZE) size = MPOOL_POOL_SIZE;
    
    pool->mpool = &blocks[0];
    
    pool->mpool->pool = (void*)memory;
    pool->mpool->size = size;
    pool->mpool->used = 0;
    
    pool->usize  = 0;
    pool->msize  = size;
    
    for (int i = 0; i < MPOOL_POOL_SIZE; i++) memory[i]=0;
}

void leaf_pool_init(size_t size)
{
    mpool_create(size, &leaf_pool);
}

/**
 * allocate memory from memory pool
 */
void* mpool_alloc(size_t size, mpool_t* pool)
{
    pool->next += 1;
    
    if (pool->next >= NUM_BLOCKS) return NULL;
    
    mpool_pool_t* new_chunk = &blocks[pool->next];
    new_chunk->size = mpool_align(size);
    
    int idx = pool->next - 1;
    for (mpool_pool_t *old_chunk = &blocks[idx]; idx >= 0; old_chunk = &blocks[--idx])
    {
        if (old_chunk->used == 0)
        {
            if (new_chunk->size < old_chunk->size)
            {
                // set memory start pointer of this chunk to that of old chunk, mark new chunk used
                new_chunk->pool = old_chunk->pool;
                new_chunk->used = 1;
                
                // increment memory start pointer of old chunk
                char* ptr = (char*)old_chunk->pool;
                ptr += new_chunk->size;
                old_chunk->pool = (void*)ptr;
                
                // decrement size of old chunk by size of new chunk
                old_chunk->size -= new_chunk->size;
                
                // increment used size of whole mempool by size of new chunk
                pool->usize += new_chunk->size;
                
                // return memory pointer of new chunk
                return new_chunk->pool;
            }
        }
    }
    
    // if not enough space anywhere, return NULL
    return NULL;
}

void* leaf_alloc(size_t size)
{
    //printf("alloc %i\n", size);
	void* block = mpool_alloc(size, &leaf_pool);

	if (block == NULL) leaf_mempool_overrun();

    return block;
}

void mpool_free(void* ptr, mpool_t* pool)
{
    //printf("finding %p\n", ptr);
    
    int idx = 0;
    for (mpool_pool_t *block = &blocks[0]; idx < NUM_BLOCKS; block = &blocks[++idx])
    {
        if (block->pool == ptr)
        {
            // Mark unused
            block->used = 0;
            
            
            // Format to all zeros (kinda arbitrary, but possibly handy / needed)
            char* buff = (char*)block->pool;
            for (int i = 0; i < block->size; i++)
            {
                buff[i] = 0;
            }
            
            // decrement overall pool size
            pool->usize -= block->size;
        
            break;
        }
    }
}

void leaf_free(void* ptr)
{
    mpool_free(ptr, &leaf_pool);
}

size_t mpool_get_size(mpool_t* pool)
{
    return pool->msize;
}

size_t leaf_pool_get_size(void)
{
    return mpool_get_size(&leaf_pool);
}

size_t mpool_get_used(mpool_t* pool)
{
    return pool->usize;
}

size_t leaf_pool_get_used(void)
{
    return mpool_get_used(&leaf_pool);
}

void* leaf_pool_get_pool(void)
{
    float* buff = (float*)memory;
    
    return buff;
}

/**
 * align byte boundary
 */
static inline size_t mpool_align(size_t size) {
    return (size + (MPOOL_ALIGN_SIZE - 1)) & ~(MPOOL_ALIGN_SIZE - 1);
}

void leaf_mempool_overrun(void)
{

}

