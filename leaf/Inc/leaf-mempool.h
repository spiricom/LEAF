/*==============================================================================
 
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
 
 written by C99 style
 ==============================================================================*/

#ifndef LEAF_MPOOL_H_INCLUDED
#define LEAF_MPOOL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
    
    //==============================================================================
    
#define MPOOL_ALIGN_SIZE (8)
    
    typedef struct LEAF LEAF;
    
    typedef enum LEAFErrorType
    {
        LEAFMempoolOverrun = 0,
        LEAFMempoolFragmentation,
        LEAFInvalidFree,
        LEAFErrorNil
    } LEAFErrorType;
    
    /*!
     * @defgroup tmempool tMempool
     * @ingroup mempool
     * An object that can contain an additional mempool for the allocation of LEAF objects.
     * @{
     */
    
    // node of free list
    typedef struct mpool_node_t {
        char                *pool;     // memory pool field
        struct mpool_node_t *next;     // next node pointer
        struct mpool_node_t *prev;     // prev node pointer
        size_t size;
    } mpool_node_t;
    
    typedef struct _tMempool _tMempool;
    typedef _tMempool* tMempool;
    struct _tMempool
    {
        tMempool      mempool;
        LEAF*         leaf;
        char*         mpool;       // start of the mpool
        size_t        usize;       // used size of the pool
        size_t        msize;       // max size of the pool
        mpool_node_t* head;        // first node of memory pool free list
    };
    
    //! Initialize a tMempool for a given memory location and size to the default LEAF mempool.
    /*!
     @param pool A pointer to the tMempool to be initialized.
     @param memory A pointer to the chunk of memory to be used as a mempool.
     @param size The size of the chunk of memory to be used as a mempool.
     */
    void    tMempool_init           (tMempool* const pool, char* memory, size_t size, LEAF* const leaf);
    
    
    //! Free a tMempool from its mempool.
    /*!
     @param pool A pointer to the tMempool to be freed.
     */
    void    tMempool_free           (tMempool* const pool);
    
    
    //! Initialize a tMempool for a given memory location and size to a specified mempool.
    /*!
     @param pool A pointer to the tMempool to be initialized.
     @param memory A pointer to the chunk of memory to be used as a mempool.
     @param size The size of the chuck of memory to be used as a mempool.
     @param poolTo A pointer to the tMempool to which a tMempool should be initialized.
     */
    void    tMempool_initToPool     (tMempool* const mp, char* memory, size_t size, tMempool* const mem, LEAF* const leaf);

    /*!￼￼￼
     @} */
    
    //==============================================================================

    //    typedef struct mpool_t {
    //        char*         mpool;       // start of the mpool
    //        size_t        usize;       // used size of the pool
    //        size_t        msize;       // max size of the pool
    //        mpool_node_t* head;        // first node of memory pool free list
    //    } mpool_t;
    
    void mpool_create (char* memory, size_t size, _tMempool* pool);
    
    char* mpool_alloc(size_t size, _tMempool* pool);
    char* mpool_calloc(size_t asize, _tMempool* pool);
    
    void mpool_free(char* ptr, _tMempool* pool);
    
    size_t mpool_get_size(_tMempool* pool);
    size_t mpool_get_used(_tMempool* pool);
    
    void leaf_pool_init(LEAF* const leaf, char* memory, size_t size);
    
    char* leaf_alloc(LEAF* const leaf, size_t size);
    char* leaf_calloc(LEAF* const leaf, size_t size);
    
    void leaf_free(LEAF* const leaf, char* ptr);
    
    size_t leaf_pool_get_size(LEAF* const leaf);
    size_t leaf_pool_get_used(LEAF* const leaf);
    
    char* leaf_pool_get_pool(LEAF* const leaf);
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_MPOOL_H

//==============================================================================



