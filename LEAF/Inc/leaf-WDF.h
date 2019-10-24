/*
 * leaf-WDF.h
 *
 *  Created on: Sep 25, 2019
 *      Author: jeffsnyder
 */

#ifndef LEAF_INC_LEAF_WDF_H_
#define LEAF_INC_LEAF_WDF_H_

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================

#include "leaf-global.h"
#include "leaf-math.h"

//==============================================================================

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

typedef struct _tWDF tWDF; // needed to allow tWDF pointers in struct
struct _tWDF
{
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
void tWDF_init(tWDF* const r, WDFComponentType type, float value, tWDF* const rL, tWDF* const rR);
void tWDF_free(tWDF* const r);
float tWDF_tick(tWDF* const r, float sample, tWDF* const outputPoint, uint8_t paramsChanged);

void tWDF_setValue(tWDF* const r, float value);
void tWDF_setSampleRate(tWDF* const r, float sample_rate);
uint8_t tWDF_isLeaf(tWDF* const r);

float tWDF_getPortResistance(tWDF* const r);
float tWDF_getReflectedWaveUp(tWDF* const r, float input); //for tree, only uses input for resistive source
float tWDF_getReflectedWaveDown(tWDF* const r, float input, float incident_wave); //for roots
void tWDF_setIncidentWave(tWDF* const r, float incident_wave, float input);

float tWDF_getVoltage(tWDF* const r);
float tWDF_getCurrent(tWDF* const r);
    

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif /* LEAF_INC_LEAF_WDF_H_ */
