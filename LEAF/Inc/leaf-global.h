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
    
    /*!
     * @internal
     * @defgroup LEAF LEAF 
     * @brief The LEAF instance. Contains global references and settings.
     * @{
     */
    typedef struct LEAF
    {
        ///@{ @internal
        float   sampleRate; //!< The current audio sample rate.
        float   invSampleRate; //!< The inverse of the current sample rate.
        int     blockSize; //!< The size of the audio block and delay in sample between input and output.
        float   twoPiTimesInvSampleRate; //!<  Two-pi times the inverse of the current sample rate.
        float   (*random)(void); //!< A pointer to a random() function provided on initialization.
        int     clearOnAllocation; //!< A flag that determines whether memory allocated from the LEAF memory pool will be cleared.
        ///@}
    } LEAF;
    
    /*! @} */
    
    extern LEAF leaf;
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_GLOBAL_H_INCLUDED

//==============================================================================


