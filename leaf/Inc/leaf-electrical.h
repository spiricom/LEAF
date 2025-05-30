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
     
     @fn void    tWDF_init(tWDF** const, WDFComponentType type, Lfloat value, tWDF* const rL, tWDF* const rR, LEAF* const leaf)
     @brief Initialize a tWDF to the default mempool of a LEAF instance.
     @param wdf A pointer to the tWDF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWDF_initToPool(tWDF** const, WDFComponentType type, Lfloat value, tWDF* const rL, tWDF* const rR, * const)
     @brief Initialize a tWDF to a specified mempool.
     @param wdf A pointer to the tWDF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWDF_free(tWDF** const)
     @brief Free a tWDF from its mempool.
     @param wdf A pointer to the tWDF to free.
     
     @fn Lfloat   tWDF_tick                   (tWDF* const, Lfloat sample, tWDF* const outputPoint, uint8_t paramsChanged)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn void    tWDF_setValue               (tWDF* const, Lfloat value)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn void    tWDF_setSampleRate          (tWDF* const, Lfloat sample_rate)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn uint8_t tWDF_isLeaf                 (tWDF* const)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn Lfloat   tWDF_getPortResistance      (tWDF* const)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn Lfloat   tWDF_getReflectedWaveUp     (tWDF* const, Lfloat input)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn Lfloat   tWDF_getReflectedWaveDown   (tWDF* const, Lfloat input, Lfloat incident_wave)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn void    tWDF_setIncidentWave        (tWDF* const, Lfloat incident_wave, Lfloat input)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn Lfloat   tWDF_getVoltage             (tWDF* const)
     @brief
     @param wdf A pointer to the relevant tWDF.
     
     @fn Lfloat   tWDF_getCurrent             (tWDF* const)
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
    
    typedef struct tWDF tWDF; // needed to allow tWDF pointers in struct
    struct tWDF
    {

        tMempool* mempool;
        WDFComponentType type;
        Lfloat port_resistance_up;
        Lfloat port_resistance_left;
        Lfloat port_resistance_right;
        Lfloat port_conductance_up;
        Lfloat port_conductance_left;
        Lfloat port_conductance_right;
        Lfloat incident_wave_up;
        Lfloat incident_wave_left;
        Lfloat incident_wave_right;
        Lfloat reflected_wave_up;
        Lfloat reflected_wave_left;
        Lfloat reflected_wave_right;
        Lfloat gamma_zero;
        Lfloat sample_rate;
        Lfloat value;
        tWDF* child_left;
        tWDF* child_right;
        Lfloat (*get_port_resistance)(tWDF* const);
        Lfloat (*get_reflected_wave_up)(tWDF* const, Lfloat);
        Lfloat (*get_reflected_wave_down)(tWDF* const, Lfloat, Lfloat);
        void (*set_incident_wave)(tWDF* const, Lfloat, Lfloat);
    };
    
    //WDF Linear Components
    void    tWDF_init(tWDF** const, WDFComponentType type, Lfloat value, tWDF** const rL, tWDF** const rR, LEAF* const leaf);
    void    tWDF_initToPool(tWDF** const, WDFComponentType type, Lfloat value, tWDF** const rL, tWDF** const rR, tMempool** const);
    void    tWDF_free(tWDF** const);
    
    Lfloat  tWDF_tick                   (tWDF* const, Lfloat sample, tWDF* const outputPoint, uint8_t paramsChanged);
    
    void    tWDF_setValue               (tWDF* const, Lfloat value);
    void    tWDF_setSampleRate          (tWDF* const, Lfloat sample_rate);
    uint8_t tWDF_isLeaf                 (tWDF* const);
    Lfloat  tWDF_getPortResistance      (tWDF* const);
    Lfloat  tWDF_getReflectedWaveUp     (tWDF* const, Lfloat input); //for tree, only uses input for resistive source
    Lfloat  tWDF_getReflectedWaveDown   (tWDF* const, Lfloat input, Lfloat incident_wave); //for roots
    void    tWDF_setIncidentWave        (tWDF* const, Lfloat incident_wave, Lfloat input);
    Lfloat  tWDF_getVoltage             (tWDF* const);
    Lfloat  tWDF_getCurrent             (tWDF* const);
    
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif /* LEAF_INC_LEAF_ELECTRICAL_H_ */

