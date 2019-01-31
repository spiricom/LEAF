/*==============================================================================

    leaf-sample.h
    Created: 23 Jan 2019 11:22:09am
    Author:  Mike Mulshine

==============================================================================*/


#ifndef LEAF_SAMPLE_H_INCLUDED
#define LEAF_SAMPLE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
//==============================================================================
 
#include "leaf-globals.h"
#include "leaf-math.h"
    
//==============================================================================
    
    typedef enum RecordMode
    {
        RecordOneShot = 0,
        RecordLoop,
        RecordModeNil
    } RecordMode;
    
    typedef struct _tSample
    {
        float* buff;
        
        uint32_t idx;
        uint32_t length;
        
        RecordMode mode;
        
        int active;
        
    } tSample;

    void  tSample_init (tSample* const, uint32_t length);
    void  tSample_free (tSample* const);
    
    void  tSample_tick (tSample* const, float sample);
    
    void  tSample_read(tSample* const, float* buff, uint32_t len);
    
    float tSample_get (tSample* const, int idx);
    
    void  tSample_start (tSample* const);
    void  tSample_stop (tSample* const);
    
    void  tSample_setRecordMode (tSample* const, RecordMode mode);
    
    void  tSample_clear (tSample* const);
    
//==============================================================================
    
    typedef enum Mode
    {
        Normal,
        Loop,
        BackAndForth,
        SampleModeNil
    } Mode;
    
    typedef struct _tSamplePlayer
    {
        tSample* samp;
        
        float idx;
        float inc;
        int8_t dir;
        int8_t flip;
        
        int32_t start;
        int32_t end;
        uint32_t len;
        uint32_t cnt;
    
        Mode mode;
        
        int active;
        
        float g1;
        float g2;
    } tSamplePlayer;
    
    void    tSamplePlayer_init      (tSamplePlayer* const, tSample* s);
    void    tSamplePlayer_free      (tSamplePlayer* const);
    
    float   tSamplePlayer_tick      (tSamplePlayer* const);

    void    tSamplePlayer_setSample (tSamplePlayer* const, tSample* s);
    
    void    tSamplePlayer_setMode   (tSamplePlayer* const, Mode mode);
    
    void    tSamplePlayer_play      (tSamplePlayer* const);
    void    tSamplePlayer_stop      (tSamplePlayer* const);
    
    void    tSamplePlayer_setStart  (tSamplePlayer* const, int32_t start);
    void    tSamplePlayer_setEnd    (tSamplePlayer* const, int32_t end);
    
    void    tSamplePlayer_setRate   (tSamplePlayer* const, float rate);
    
//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_SAMPLE_H_INCLUDED

//==============================================================================
