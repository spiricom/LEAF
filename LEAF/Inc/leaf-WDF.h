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

#include "leaf-globals.h"
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
    ComponentNil
} WDFComponentType;

typedef enum WDFRootType
{
    IdealSource = 0,
    Diode,
    DiodePair,
    RootNil
} WDFRootType;

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
    tWDF* outpoint;
    float (*get_port_resistance)(tWDF* const);
    float (*get_reflected_wave)(tWDF* const);
    void (*set_incident_wave)(tWDF* const, float);
};

typedef struct _tWDFNonlinear tWDFNonlinear;
struct _tWDFNonlinear
{
    WDFRootType type;
    tWDF* child;
    float (*calculate_reflected_wave)(tWDFNonlinear* const, float, float);
};

//WDF Linear Components
void tWDF_init(tWDF* const r, WDFComponentType type, float value, tWDF* const rL, tWDF* const rR);
float tWDF_tick(tWDF* const r, tWDFNonlinear* const n, float sample, uint8_t paramsChanged);

void tWDF_setValue(tWDF* const r, float value);
void tWDF_setSampleRate(tWDF* const r, float sample_rate);
void tWDF_setOutputPoint(tWDF* const r, tWDF* const outpoint);
uint8_t tWDF_isLeaf(tWDF* const r);

tWDF* tWDF_findOutputPoint(tWDF* const r);

float tWDF_getPortResistance(tWDF* const r);
float tWDF_getReflectedWave(tWDF* const r);
void tWDF_setIncidentWave(tWDF* const r, float incident_wave);

float tWDF_getVoltage(tWDF* const r);
float tWDF_getCurrent(tWDF* const r);

static float get_port_resistance_for_resistor(tWDF* const r);
static float get_port_resistance_for_capacitor(tWDF* const r);
static float get_port_resistance_for_inductor(tWDF* const r);
static float get_port_resistance_for_resistive(tWDF* const r);
static float get_port_resistance_for_inverter(tWDF* const r);
static float get_port_resistance_for_series(tWDF* const r);
static float get_port_resistance_for_parallel(tWDF* const r);

static void set_incident_wave_for_leaf(tWDF* const r, float incident_wave);
static void set_incident_wave_for_leaf_inverted(tWDF* const r, float incident_wave);
static void set_incident_wave_for_inverter(tWDF* const r, float incident_wave);
static void set_incident_wave_for_series(tWDF* const r, float incident_wave);
static void set_incident_wave_for_parallel(tWDF* const r, float incident_wave);

static float get_reflected_wave_for_resistor(tWDF* const r);
static float get_reflected_wave_for_capacitor(tWDF* const r);
static float get_reflected_wave_for_resistive(tWDF* const r);
static float get_reflected_wave_for_inverter(tWDF* const r);
static float get_reflected_wave_for_series(tWDF* const r);
static float get_reflected_wave_for_parallel(tWDF* const r);
    
//WDF Nonlinear Roots
void tWDFNonlinear_init(tWDFNonlinear* const n, WDFRootType type, tWDF* const child);
float tWDFNonlinear_calculateReflectedWave(tWDFNonlinear*  const n, float input, float incident_wave);

static float calculate_reflected_wave_for_ideal(tWDFNonlinear* const n, float input, float incident_wave);
static float calculate_reflected_wave_for_diode(tWDFNonlinear* const n, float input, float incident_wave);
    
    
    
    
    
    
    
    
    
    
    
    
//WDF resistor
typedef struct _tWDFresistor
{
	float port_resistance;
	float port_conductance;
	float electrical_resistance;
	float incident_wave;
	float reflected_wave;
} tWDFresistor;

void tWDFresistor_init(tWDFresistor* const r, float electrical_resistance);
float tWDFresistor_getPortResistance(tWDFresistor* const r);
void tWDFresistor_setElectricalResistance(tWDFresistor* const r, float electrical_resistance);
void tWDFresistor_setIncidentWave(tWDFresistor* const r, float incident_wave);
float tWDFresistor_getReflectedWave(tWDFresistor* const r);
float tWDFresistor_getVoltage(tWDFresistor* const r);
float tWDFresistor_getCurrent(tWDFresistor* const r);



//---
//WDF resistive source
typedef struct _tWDFresistiveSource
{
	float port_resistance;
	float port_conductance;
	float electrical_resistance;
	float incident_wave;
	float reflected_wave;
	float source_voltage;
} tWDFresistiveSource;

void tWDFresistiveSource_init(tWDFresistiveSource* const r, float electrical_resistance, float source_voltage);
float tWDFresistiveSource_getPortResistance(tWDFresistiveSource* const r);
void tWDFresistiveSource_setIncidentWave(tWDFresistiveSource* const r, float incident_wave);
float tWDFresistiveSource_getReflectedWave(tWDFresistiveSource* const r);
float tWDFresistiveSource_getVoltage(tWDFresistiveSource* const r);
float tWDFresistiveSource_getCurrent(tWDFresistiveSource* const r);
void tWDFresistiveSource_setSourceVoltage(tWDFresistiveSource* const r, float source_voltage);


//---
//WDF capacitor
typedef struct _tWDFcapacitor
{
	float port_resistance;
	float port_conductance;
	float electrical_capacitance;
	float incident_wave;
	float reflected_wave;
	float sample_rate;
	float memory;
} tWDFcapacitor;

void tWDFcapacitor_init(tWDFcapacitor* const r, float electrical_capacitance, float sample_rate);
float tWDFcapacitor_getPortResistance(tWDFcapacitor* const r);
void tWDFcapacitor_setIncidentWave(tWDFcapacitor* const r, float incident_wave);
float tWDFcapacitor_getReflectedWave(tWDFcapacitor* const r);
float tWDFcapacitor_getVoltage(tWDFcapacitor* const r);
float tWDFcapacitor_getCurrent(tWDFcapacitor* const r);

//---
//WDF series adaptor
typedef struct _tWDFseriesAdaptor
{
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
	tWDFresistor* rL;
	tWDFcapacitor* rR;
} tWDFseriesAdaptor;

void tWDFseriesAdaptor_init(tWDFseriesAdaptor* const r, tWDFresistor* const rL, tWDFcapacitor* const rR);
float tWDFseriesAdaptor_getPortResistance(tWDFseriesAdaptor* const r);
void tWDFseriesAdaptor_setPortResistances(tWDFseriesAdaptor* const r);
void tWDFseriesAdaptor_setIncidentWave(tWDFseriesAdaptor* const r, float incident_wave);
float tWDFseriesAdaptor_getReflectedWave(tWDFseriesAdaptor* const r);
float tWDFseriesAdaptor_getVoltage(tWDFseriesAdaptor* const r);
float tWDFseriesAdaptor_getCurrent(tWDFseriesAdaptor* const r);

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif /* LEAF_INC_LEAF_WDF_H_ */
