/*
 ==============================================================================
 
 leaf-global.h
 Created: 24 Oct 2019 2:24:38pm
 Author:  Matthew Wang
 
 ==============================================================================
 */

#ifndef LEAF_GLOBAL_H_INCLUDED
#define LEAF_GLOBAL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
#include "leaf-mempool.h"
    
#if _WIN32 || _WIN64
#include "..\leaf-config.h"
#else
#include "../leaf-config.h"
#endif
    
    /*!
     * @ingroup leaf
     * @brief Struct for an instance of LEAF.
     */
    
    struct LEAF
    {
        ///@{ 
        float   sampleRate; //!< The current audio sample rate. Set with LEAF_setSampleRate().
        float   invSampleRate; //!< The inverse of the current sample rate.
        int     blockSize; //!< The audio block size.
        float   twoPiTimesInvSampleRate; //!<  Two-pi times the inverse of the current sample rate.
        float   (*random)(void); //!< A pointer to the random() function provided on initialization.
        int     clearOnAllocation; //!< A flag that determines whether memory allocated from the LEAF memory pool will be cleared.
        tMempool mempool; //!< The default LEAF mempool object.
        _tMempool _internal_mempool;
        size_t header_size; //!< The size in bytes of memory region headers within mempools.
        void (*errorCallback)(LEAF* const, LEAFErrorType); //!< A pointer to the callback function for LEAF errors. Can be set by the user.
        int     errorState[LEAFErrorNil]; //!< An array of flags that indicate which errors have occurred.
        
        ///@}
    };
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_GLOBAL_H_INCLUDED

//==============================================================================


