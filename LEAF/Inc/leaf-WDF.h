/*
 * leaf-WDF.h
 *
 *  Created on: Sep 25, 2019
 *      Author: jeffsnyder
 */

#ifndef LEAF_INC_LEAF_WDF_H_
#define LEAF_INC_LEAF_WDF_H_



//---
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



#endif /* LEAF_INC_LEAF_WDF_H_ */
