/*
 * leaf-electrical.h
 *
 *  Created on: Sep 25, 2019
 *      Author: jeffsnyder
 */

#ifndef LEAF_INC_LEAF_ELECTRICAL_H_
#define LEAF_INC_LEAF_ELECTRICAL_H_

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-mempool.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /*!
     @defgroup twdf tWDF
     @ingroup electrical
     @brief Wave digital filter component.
     @{
     
     @fn void    tWDF_init                   (tWDF* const, WDFComponentType type, float value, tWDF* const rL, tWDF* const rR, LEAF* const leaf)
     @brief Initialize a tWDF to the default mempool of a LEAF instance.
     @param wdf A pointer to the tWDF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWDF_initToPool             (tWDF* const, WDFComponentType type, float value, tWDF* const rL, tWDF* const rR, tMempool* const)
     @brief Initialize a tWDF to a specified mempool.
     @param wdf A pointer to the tWDF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWDF_free                   (tWDF* const)
     @brief Free a tWDF from its mempool.
     @param wdf A pointer to the tWDF to free.
     
     @fn float   tWDF_tick                   (tWDF* const, float sample, tWDF* const outputPoint, uint8_t paramsChanged)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn void    tWDF_setValue               (tWDF* const, float value)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn void    tWDF_setSampleRate          (tWDF* const, float sample_rate)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn uint8_t tWDF_isLeaf                 (tWDF* const)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn float   tWDF_getPortResistance      (tWDF* const)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn float   tWDF_getReflectedWaveUp     (tWDF* const, float input)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn float   tWDF_getReflectedWaveDown   (tWDF* const, float input, float incident_wave)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn void    tWDF_setIncidentWave        (tWDF* const, float incident_wave, float input)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn float   tWDF_getVoltage             (tWDF* const)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn float   tWDF_getCurrent             (tWDF* const)
     @brief
     @param wdf A pointer to the relevant tWDF.
      
     @} */
    
    typedef enum WDFComponentType
    {
        SeriesAdaptor = 0,
        ParallelAdaptor,
        Resistor,
        Capacitor,
        Inductor,
        Inverter,
        ResistiveSource,
        IdealSource,
        Diode,
        DiodePair,
        RootNil,
        WDFComponentNil
    } WDFComponentType;
    
    typedef struct _tWDF _tWDF; // needed to allow tWDF pointers in struct
    typedef _tWDF* tWDF;
    struct _tWDF
    {
        
        tMempool mempool;
        WDFComponentType type;
        float port_resistance_up;
        float port_resistance_left;
        float port_resistance_right;
        float port_conductance_up;
        float port_conductance_left;
        float port_conductance_right;
        float incident_wave_up;
        float incident_wave_left;
        float incident_wave_right;
        float reflected_wave_up;
        float reflected_wave_left;
        float reflected_wave_right;
        float gamma_zero;
        float sample_rate;
        float value;
        tWDF* child_left;
        tWDF* child_right;
        float (*get_port_resistance)(tWDF* const);
        float (*get_reflected_wave_up)(tWDF* const, float);
        float (*get_reflected_wave_down)(tWDF* const, float, float);
        void (*set_incident_wave)(tWDF* const, float, float);
    };
    
    //WDF Linear Components
    void    tWDF_init                   (tWDF* const, WDFComponentType type, float value, tWDF* const rL, tWDF* const rR, LEAF* const leaf);
    void    tWDF_initToPool             (tWDF* const, WDFComponentType type, float value, tWDF* const rL, tWDF* const rR, tMempool* const);
    void    tWDF_free                   (tWDF* const);
    
    float   tWDF_tick                   (tWDF* const, float sample, tWDF* const outputPoint, uint8_t paramsChanged);
    
    void    tWDF_setValue               (tWDF* const, float value);
    void    tWDF_setSampleRate          (tWDF* const, float sample_rate);
    uint8_t tWDF_isLeaf                 (tWDF* const);
    
    float   tWDF_getPortResistance      (tWDF* const);
    float   tWDF_getReflectedWaveUp     (tWDF* const, float input); //for tree, only uses input for resistive source
    float   tWDF_getReflectedWaveDown   (tWDF* const, float input, float incident_wave); //for roots
    void    tWDF_setIncidentWave        (tWDF* const, float incident_wave, float input);
    
    float   tWDF_getVoltage             (tWDF* const);
    float   tWDF_getCurrent             (tWDF* const);
    
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif /* LEAF_INC_LEAF_ELECTRICAL_H_ */

