/*==============================================================================

 leaf-sampling.h
 Created: 23 Jan 2019 11:22:09am
 Author:  Mike Mulshine

 ==============================================================================*/

#ifndef LEAF_SAMPLING_H_INCLUDED
#define LEAF_SAMPLING_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

    //==============================================================================

#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-envelopes.h"
#include "leaf-mempool.h"
#include "leaf-analysis.h"

    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */

    //==============================================================================

    /*!
     @defgroup tbuffer tBuffer
     @ingroup sampling
     @brief Buffer for use in Sampler objects.
     @details Buffer for use in Sampler objects. Can selectively record audio and keeps track of the length of recorded audio.
     @{

     @fn void  tBuffer_init                  (tBuffer* const, uint32_t length, LEAF* const leaf)
     @brief Initialize a tBuffer to the default mempool of a LEAF instance.
     @param sampler A pointer to the tBuffer to initialize.
     @param length The length of the buffer in samples.
     @param leaf A pointer to the leaf instance.

     @fn void  tBuffer_initToPool            (tBuffer* const, uint32_t length, tMempool* const)
     @brief Initialize a tBuffer to a specified mempool.
     @param sampler A pointer to the tBuffer to initialize.
     @param length The length of the buffer in samples.
     @param mempool A pointer to the tMempool to use.

     @fn void  tBuffer_free                  (tBuffer* const)
     @brief Free a tBuffer from its mempool.
     @param sampler A pointer to the tBuffer to free.

     @fn void  tBuffer_tick                  (tBuffer* const, Lfloat sample)
     @brief If recording, add a sample to the buffer. Otherwise do nothing.
     @param sampler A pointer to the relevant tBuffer.
     @param input The input sample.

     @fn void  tBuffer_read                  (tBuffer* const, Lfloat* buff, uint32_t len)
     @brief Read an input buffer into the buffer.
     @param sampler A pointer to the relevant tBuffer.
     @param inputBuffer The input buffer.
     @param length The length of the input buffer.

     @fn Lfloat tBuffer_get                   (tBuffer* const, int idx)
     @brief Get the sample recorded at a given position in the buffer.
     @param sampler A pointer to the relevant tBuffer.
     @param position The position to get a sample from.
     @return The recorded sample.

     @fn void  tBuffer_record                (tBuffer* const)
     @brief Start recording samples into the buffer.
     @param sampler A pointer to the relevant tBuffer.

     @fn void  tBuffer_stop                  (tBuffer* const)
     @brief Stop recordings samples into the buffer.
     @param sampler A pointer to the relevant tBuffer.

     @fn int   tBuffer_getRecordPosition     (tBuffer* const)
     @brief Get the recording position, from where the buffer will next add samples.
     @param sampler A pointer to the relevant tBuffer.
     @return The recording position.

     @fn void   tBuffer_setRecordPosition    (tBuffer* const, int pos)
     @brief Set the recording position, from where the buffer will next add samples.
     @param sampler A pointer to the relevant tBuffer.
     @param index The new recording position.

     @fn void  tBuffer_setRecordMode         (tBuffer* const, RecordMode mode)
     @brief Set the recording mode.
     @param sampler A pointer to the relevant tBuffer.
     @param mode The new mode, either RecordOneShot to record one buffer length or RecordLoop to record on loop, overwriting old samples.

     @fn void  tBuffer_clear                 (tBuffer* const)
     @brief Clear the buffer.
     @param sampler A pointer to the relevant tBuffer.

     @fn uint32_t tBuffer_getBufferLength    (tBuffer* const)
     @brief Get the length of the buffer.
     @param sampler A pointer to the relevant tBuffer.
     @return The length of the buffer.

     @fn uint32_t tBuffer_getRecordedLength  (tBuffer* const sb)
     @brief Get the length of recorded audio in the buffer.
     @param sampler A pointer to the relevant tBuffer.
     @return The length in samples of recorded audio.

     @fn void     tBuffer_setRecordedLength    (tBuffer* const sb, int length)
     @brief Set the length of what is considered recorded audio in the buffer.
     @param sampler A pointer to the relevant tBuffer.
     @param length The new recorded length.

     @fn int     tBuffer_isActive            (tBuffer* const sb)
     @brief Check if the buffer is recording
     @param sampler A pointer to the relevant tBuffer.
     @return 1 if recording, 0 if not.

     @} */

    typedef enum RecordMode
    {
        RecordOneShot = 0,
        RecordLoop,
        RecordModeNil
    } RecordMode;

    typedef struct tBuffer
    {

        tMempool mempool;

        Lfloat *buff;

        uint32_t idx;
        uint32_t bufferLength;
        uint32_t recordedLength;
        uint32_t channels;
        uint32_t sampleRate;
        RecordMode mode;

        int active;
    } tBuffer;

    void     tBuffer_init              (tBuffer**const, uint32_t length, LEAF *const leaf);
    void     tBuffer_initToPool        (tBuffer**const sb, uint32_t length, tMempool *const mp);
    void     tBuffer_free              (tBuffer**const);

    void     tBuffer_tick              (tBuffer* const, Lfloat sample);

    void     tBuffer_read              (tBuffer* const, Lfloat *buff, uint32_t len);
    Lfloat   tBuffer_get               (tBuffer* const, int idx);
    void     tBuffer_record            (tBuffer* const);
    void     tBuffer_stop              (tBuffer* const);
    void     tBuffer_setBuffer         (tBuffer* const sb, Lfloat *externalBuffer, int length, int channels, int sampleRate);
    int      tBuffer_getRecordPosition (tBuffer* const);
    void     tBuffer_setRecordPosition (tBuffer* const, int pos);
    void     tBuffer_setRecordMode     (tBuffer* const, RecordMode mode);
    void     tBuffer_clear             (tBuffer* const);
    uint32_t tBuffer_getBufferLength   (tBuffer* const);
    uint32_t tBuffer_getRecordedLength (tBuffer* const sb);
    void     tBuffer_setRecordedLength (tBuffer* const sb, int length);
    int      tBuffer_isActive          (tBuffer* const sb);

    //==============================================================================

    /*!
     @defgroup tsampler tSampler
     @ingroup sampling
     @brief Loopable input sampler with crossfading.
     @{

     @fn void    tSampler_init               (tSampler* const, tBuffer* const, LEAF* const leaf)
     @brief Initialize a tSampler to the default mempool of a LEAF instance.
     @param sampler A pointer to the tSampler to initialize.
     @param buffer A pointer to a tBuffer to playback audio from. Multiple tSamplers can share one tBuffer.
     @param leaf A pointer to the leaf instance.

     @fn void    tSampler_initToPool         (tSampler* const, tBuffer* const, tMempool* const)
     @brief Initialize a tSampler to a specified mempool.
     @param sampler A pointer to the tSampler to initialize.
     @param buffer A pointer to a tBuffer to playback audio from. Multiple tSamplers can share one tBuffer.
     @param mempool A pointer to the tMempool to use.

     @fn void    tSampler_free               (tSampler* const)
     @brief Free a tSampler from its mempool.
     @param sampler A pointer to the tSampler to free.

     @fn Lfloat   tSampler_tick               (tSampler* const)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_setSample          (tSampler* const, tBuffer* const)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_setMode            (tSampler* const, PlayMode mode)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_play               (tSampler* const)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_stop               (tSampler* const)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_setStart           (tSampler* const, int32_t start)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_setEnd             (tSampler* const, int32_t end)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_setLength             (tSampler* const, int32_t length)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_setCrossfadeLength (tSampler* const sp, uint32_t length)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @fn void    tSampler_setRate            (tSampler* const, Lfloat rate)
     @brief
     @param sampler A pointer to the relevant tSampler.

     @} */

    typedef enum PlayMode
    {
        PlayNormal,
        PlayLoop,
        PlayBackAndForth,
        PlayModeNil
    } PlayMode;

    typedef struct tSampler
    {
        tMempool mempool;

        tBuffer samp;

        Lfloat invSampleRate;
        Lfloat sampleRate;
        Lfloat ticksPerSevenMs;
        Lfloat rateFactor;
        uint32_t channels;

        tRamp gain;

        Lfloat idx;
        Lfloat inc;
        Lfloat last;
        Lfloat iinc;
        int8_t dir;
        int8_t flip;
        int8_t bnf;

        int32_t start, targetstart;
        int32_t end, targetend;

        uint32_t len;
        uint32_t cfxlen;
        Lfloat numticks;
        PlayMode mode;
        int retrigger;

        int active;

        uint8_t inCrossfade;

        Lfloat flipStart;
        Lfloat flipIdx;
    } tSampler;

    void    tSampler_init               (tSampler**const, tBuffer *const, LEAF *const leaf);
    void    tSampler_initToPool         (tSampler**const sp, tBuffer *const b, tMempool *const mp, LEAF *const leaf);
    void    tSampler_free               (tSampler**const);

    Lfloat  tSampler_tick               (tSampler* const);
    Lfloat  tSampler_tickStereo         (tSampler* const sp, Lfloat *outputArray);

    void    tSampler_setSample          (tSampler* const, tBuffer const);
    void    tSampler_setMode            (tSampler* const, PlayMode mode);
    void    tSampler_play               (tSampler* const);
    void    tSampler_stop               (tSampler* const);
    void    tSampler_setStart           (tSampler* const, int32_t start);
    void    tSampler_setEnd             (tSampler* const, int32_t end);
    void    tSampler_setEndUnsafe       (tSampler* const, int32_t end);
    void    tSampler_setLength          (tSampler* const, int32_t length);
    void    tSampler_setCrossfadeLength (tSampler* const, uint32_t length);
    void    tSampler_setRate            (tSampler* const, Lfloat rate);
    void    tSampler_setSampleRate      (tSampler* const, Lfloat sr);

    //==============================================================================

    /*!
     @defgroup tautosampler tAutoSampler
     @ingroup sampling
     @brief Automatic sampler based on power of input.
     @{

     @fn void    tAutoSampler_init               (tAutoSampler* const, tBuffer* const, LEAF* const leaf)
     @brief Initialize a tAutoSampler to the default mempool of a LEAF instance.
     @param sampler A pointer to the tAutoSampler to initialize.
     @param leaf A pointer to the leaf instance.

     @fn void    tAutoSampler_initToPool         (tAutoSampler* const, tBuffer* const, tMempool* const)
     @brief Initialize a tAutoSampler to a specified mempool.
     @param sampler A pointer to the tAutoSampler to initialize.
     @param mempool A pointer to the tMempool to use.

     @fn void    tAutoSampler_free               (tAutoSampler* const)
     @brief Free a tAutoSampler from its mempool.
     @param sampler A pointer to the tAutoSampler to free.

     @fn Lfloat   tAutoSampler_tick               (tAutoSampler* const, Lfloat input)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_setBuffer          (tAutoSampler* const, tBuffer* const)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_setMode            (tAutoSampler* const, PlayMode mode)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_play               (tAutoSampler* const)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_stop               (tAutoSampler* const)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_setThreshold       (tAutoSampler* const, Lfloat thresh)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_setWindowSize      (tAutoSampler* const, uint32_t size)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_setCrossfadeLength (tAutoSampler* const, uint32_t length)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @fn void    tAutoSampler_setRate            (tAutoSampler* const, Lfloat rate)
     @brief
     @param sampler A pointer to the relevant tAutoSampler.

     @} */

    typedef struct tAutoSampler
    {

        tMempool mempool;
        tSampler sampler;
        tEnvelopeFollower ef;
        uint32_t windowSize;
        Lfloat threshold;
        Lfloat previousPower;
        uint32_t sampleCounter;
        uint32_t powerCounter;
        uint8_t sampleTriggered;
    } tAutoSampler;

    void    tAutoSampler_init               (tAutoSampler**const, tBuffer *const, LEAF *const leaf);
    void    tAutoSampler_initToPool         (tAutoSampler**const as, tBuffer *const b, tMempool *const mp, LEAF *const leaf);
    void    tAutoSampler_free               (tAutoSampler**const);

    Lfloat  AutoSampler_tick                (tAutoSampler* const, Lfloat input);

    void    tAutoSampler_setBuffer          (tAutoSampler* const, tBuffer const);
    void    tAutoSampler_setMode            (tAutoSampler* const, PlayMode mode);
    void    tAutoSampler_play               (tAutoSampler* const);
    void    tAutoSampler_stop               (tAutoSampler* const);
    void    tAutoSampler_setThreshold       (tAutoSampler* const, Lfloat thresh);
    void    tAutoSampler_setWindowSize      (tAutoSampler* const, uint32_t size);
    void    tAutoSampler_setCrossfadeLength (tAutoSampler* const, uint32_t length);
    void    tAutoSampler_setRate            (tAutoSampler* const, Lfloat rate);
    void    tAutoSampler_setSampleRate      (tAutoSampler* const, Lfloat sr);

    /*!
     @defgroup tmbsampler tMBSampler
     @ingroup sampling
     @brief Loopable input sampler using minBLEP to remove discontinuities.
     @{

     @fn void    tMBSampler_init               (tMBSampler* const, tBuffer* const, LEAF* const leaf)
     @brief Initialize a tMBSampler to the default mempool of a LEAF instance.
     @param sampler A pointer to the tMBSampler to initialize.
     @param leaf A pointer to the leaf instance.

     @fn void    tMBSampler_initToPool         (tMBSampler* const, tBuffer* const, tMempool* const)
     @brief Initialize a tMBSampler to a specified mempool.
     @param sampler A pointer to the tMBSampler to initialize.
     @param mempool A pointer to the tMempool to use.

     @fn void    tMBSampler_free               (tMBSampler* const)
     @brief Free a tMBSampler from its mempool.
     @param sampler A pointer to the tMBSampler to free.

     @fn Lfloat   tMBSampler_tick               (tMBSampler* const)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_setSample          (tMBSampler* const, tBuffer* const)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_setMode            (tMBSampler* const, PlayMode mode)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_play               (tMBSampler* const)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_stop               (tMBSampler* const)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_setStart           (tMBSampler* const, int32_t start)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_setEnd             (tMBSampler* const, int32_t end)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_setLength             (tMBSampler* const, int32_t length)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_setCrossfadeLength (tMBSampler* const sp, uint32_t length)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @fn void    tMBSampler_setRate            (tMBSampler* const, Lfloat rate)
     @brief
     @param sampler A pointer to the relevant tMBSampler.

     @} */
#ifndef FILLEN
#define FILLEN 128
#endif

    typedef struct tMBSampler
    {

        tMempool mempool;

        tBuffer samp;
        PlayMode mode;
        int active;

        tExpSmooth gain;

        Lfloat out;
        Lfloat last, beforeLast;
        Lfloat amp;
        Lfloat last_amp;
        Lfloat syncin;
        Lfloat _p, _w, _z;
        Lfloat _last_w;
        Lfloat _f[FILLEN + STEP_DD_PULSE_LENGTH];
        int _j;

        int start, end;
        int currentLoopLength;
    } tMBSampler;

    void    tMBSampler_init       (tMBSampler**const, tBuffer *const, LEAF *const leaf);
    void    tMBSampler_initToPool (tMBSampler**const, tBuffer *const, tMempool *const);
    void    tMBSampler_free       (tMBSampler**const);

    Lfloat  tMBSampler_tick       (tMBSampler* const);

    void    tMBSampler_setSample  (tMBSampler* const, tBuffer *const);
    void    tMBSampler_setMode    (tMBSampler* const, PlayMode mode);
    void    tMBSampler_play       (tMBSampler* const);
    void    tMBSampler_stop       (tMBSampler* const);
    void    tMBSampler_setStart   (tMBSampler* const, int32_t start);
    void    tMBSampler_setEnd     (tMBSampler* const, int32_t end);
    void    tMBSampler_setLength  (tMBSampler* const, int32_t length);
    void    tMBSampler_setRate    (tMBSampler* const, Lfloat rate);

#ifdef __cplusplus
}
#endif

#endif // LEAF_SAMPLING_H_INCLUDED

//==============================================================================
