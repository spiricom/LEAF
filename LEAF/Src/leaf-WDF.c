/*
 * leaf-WDF.c
 *
 *  Created on: Sep 25, 2019
 *      Author: jeffsnyder
 */


#include "../Inc/leaf-WDF.h"

//WDF resistor
void tWDFresistor_init(tWDFresistor* const r, float electrical_resistance)
{
	r->port_resistance = electrical_resistance;
	r->port_conductance = 1.0f / electrical_resistance;
	r->electrical_resistance = electrical_resistance;
	r->incident_wave = 0.0f;
	r->reflected_wave = 0.0f;
}
float tWDFresistor_getPortResistance(tWDFresistor* const r)
{
	return r->port_resistance;
}
void tWDFresistor_setIncidentWave(tWDFresistor* const r, float incident_wave)
{
	r->incident_wave = incident_wave;
}
float tWDFresistor_getReflectedWave(tWDFresistor* const r)
{
	r->reflected_wave = 0.0f;
	return r->reflected_wave;
}
float tWDFresistor_getVoltage(tWDFresistor* const r)
{
	return ((r->incident_wave * 0.5f) + (r->reflected_wave * 0.5f));
}
float tWDFresistor_getCurrent(tWDFresistor* const r)
{
	return (((r->incident_wave * 0.5f) - (r->reflected_wave * 0.5f)) * r->port_conductance);
}



///----WDF resistive source
void tWDFresistiveSource_init(tWDFresistiveSource* const r, float electrical_resistance, float source_voltage)
{
	r->port_resistance = electrical_resistance;
	r->port_conductance = 1.0f / electrical_resistance;
	r->electrical_resistance = electrical_resistance;
	r->incident_wave = 0.0f;
	r->reflected_wave = 0.0f;
	r->source_voltage = source_voltage;
}
float tWDFresistiveSource_getPortResistance(tWDFresistiveSource* const r)
{
	return r->port_resistance;
}
void tWDFresistiveSource_setIncidentWave(tWDFresistiveSource* const r, float incident_wave)
{
	r->incident_wave = incident_wave;
}
float tWDFresistiveSource_getReflectedWave(tWDFresistiveSource* const r)
{
	r->reflected_wave = r->source_voltage;
	return r->reflected_wave;
}
float tWDFresistiveSource_getVoltage(tWDFresistiveSource* const r)
{
	return ((r->incident_wave * 0.5f) + (r->reflected_wave * 0.5f));
}
float tWDFresistiveSource_getCurrent(tWDFresistiveSource* const r)
{
	return (((r->incident_wave * 0.5f) - (r->reflected_wave * 0.5f)) * r->port_conductance);
}
void tWDFresistiveSource_setSourceVoltage(tWDFresistiveSource* const r, float source_voltage)
{
	r->source_voltage = source_voltage;
}


//WDF capacitor

void tWDFcapacitor_init(tWDFcapacitor* const r, float electrical_capacitance, float sample_rate)
{
	r->port_resistance = 1.0f / (sample_rate * electrical_capacitance); //based on trapezoidal discretization
	r->port_conductance = (1.0f / r->port_resistance);
	r->electrical_capacitance = electrical_capacitance;
	r->incident_wave = 0.0f;
	r->reflected_wave = 0.0f;
	r->sample_rate = sample_rate;
	r->memory = 0.0f;
}
float tWDFcapacitor_getPortResistance(tWDFcapacitor* const r)
{
	return r->port_resistance;
}
void tWDFcapacitor_setIncidentWave(tWDFcapacitor* const r, float incident_wave)
{
	r->incident_wave = incident_wave;
	r->memory = r->incident_wave;
}
float tWDFcapacitor_getReflectedWave(tWDFcapacitor* const r)
{
	r->reflected_wave = r->memory;
	return r->reflected_wave;
}
float tWDFcapacitor_getVoltage(tWDFcapacitor* const r)
{
	return ((r->incident_wave * 0.5f) + (r->reflected_wave * 0.5f));
}
float tWDFcapacitor_getCurrent(tWDFcapacitor* const r)
{
	return (((r->incident_wave * 0.5f) - (r->reflected_wave * 0.5f)) * r->port_conductance);
}


// WDF series
void tWDFseriesAdaptor_init(tWDFseriesAdaptor* const r, tWDFresistor* const rL, tWDFcapacitor* const rR)
{
	r->rL = rL;
	r->rR = rR;
	r->port_resistance_left = tWDFresistor_getPortResistance(rL);
	r->port_resistance_right = tWDFcapacitor_getPortResistance(rR);
	r->port_resistance_up = r->port_resistance_left + r->port_resistance_right;
	r->port_conductance_up  = 1.0f / r->port_resistance_up;
	r->port_conductance_left = 1.0f / r->port_resistance_left;
	r->port_conductance_right = 1.0f / r->port_resistance_right;
	r->incident_wave_up = 0.0f;
	r->incident_wave_left = 0.0f;
	r->incident_wave_right = 0.0f;
	r->reflected_wave_up = 0.0f;
	r->reflected_wave_left = 0.0f;
	r->reflected_wave_right = 0.0f;

	r->gamma_zero = 1.0f / (r->port_resistance_right + r->port_resistance_left);
}
float tWDFseriesAdaptor_getPortResistance(tWDFseriesAdaptor* const r)
{
	return r->port_resistance_up;
}
void tWDFseriesAdaptor_setIncidentWaves(tWDFseriesAdaptor* const r, float incident_wave)
{
	float gamma_left = r->port_resistance_left * r->gamma_zero;
	float gamma_right = r->port_resistance_right * r->gamma_zero;
	tWDFresistor_setIncidentWave(r->rL, (-1.0f * gamma_left * incident_wave) + ((1.0f - gamma_left) * tWDFresistor_getReflectedWave(r->rL)) - (gamma_left * tWDFcapacitor_getReflectedWave(r->rR)));
	tWDFcapacitor_setIncidentWave(r->rR, (-1.0f * gamma_right * incident_wave) + ((-1.0f * gamma_right) * tWDFresistor_getReflectedWave(r->rL)) + (1.0f - gamma_right * tWDFcapacitor_getReflectedWave(r->rR)));
}

float tWDFseriesAdaptor_getReflectedWave(tWDFseriesAdaptor* const r)
{
	 return (-1.0f * (tWDFresistor_getReflectedWave(r->rL) + tWDFcapacitor_getReflectedWave(r->rR)));
}
float tWDFseriesAdaptor_getVoltage(tWDFseriesAdaptor* const r)
{
	return ((r->incident_wave_up * 0.5f) + (r->reflected_wave_up * 0.5f));
}
float tWDFseriesAdaptor_getCurrent(tWDFseriesAdaptor* const r)
{
	return (((r->incident_wave_up * 0.5f) - (r->reflected_wave_up * 0.5f)) * r->port_conductance_up);
}
