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
    
    typedef struct _LEAF
    {
        float   sampleRate;
        float   invSampleRate;
        int     blockSize;
        float   twoPiTimesInvSampleRate;
        float   (*random)(void);
        int     clearOnAllocation;
    } LEAF;
    
    extern LEAF leaf; // The global instance of LEAF.
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_GLOBAL_H_INCLUDED

//==============================================================================

