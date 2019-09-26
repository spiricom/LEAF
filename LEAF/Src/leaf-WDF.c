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
void tWDFresistor_setElectricalResistance(tWDFresistor* const r, float electrical_resistance)
{
	r->port_resistance = electrical_resistance;
	r->port_conductance = 1.0f / electrical_resistance;
	r->electrical_resistance = electrical_resistance;
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
	r->port_resistance = 1.0f / (sample_rate * 2.0f * electrical_capacitance); //based on trapezoidal discretization
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

void tWDFseriesAdaptor_setPortResistances(tWDFseriesAdaptor* const r)
{
	r->port_resistance_left = tWDFresistor_getPortResistance(r->rL);
	r->port_resistance_right = tWDFcapacitor_getPortResistance(r->rR);
	r->port_resistance_up = r->port_resistance_left + r->port_resistance_right;
	r->port_conductance_up  = 1.0f / r->port_resistance_up;
	r->port_conductance_left = 1.0f / r->port_resistance_left;
	r->port_conductance_right = 1.0f / r->port_resistance_right;
	r->gamma_zero = 1.0f / (r->port_resistance_right + r->port_resistance_left);
}

float tWDFseriesAdaptor_getPortResistance(tWDFseriesAdaptor* const r)
{
	return r->port_resistance_up;
}
void tWDFseriesAdaptor_setIncidentWave(tWDFseriesAdaptor* const r, float incident_wave)
{
	float gamma_left = r->port_resistance_left * r->gamma_zero;
	float gamma_right = r->port_resistance_right * r->gamma_zero;
	float left_wave = tWDFresistor_getReflectedWave(r->rL);
	float right_wave = tWDFcapacitor_getReflectedWave(r->rR);
	tWDFresistor_setIncidentWave(r->rL, (-1.0f * gamma_left * incident_wave) + ((1.0f - gamma_left) * left_wave) - (gamma_left * right_wave));
	tWDFcapacitor_setIncidentWave(r->rR, (-1.0f * gamma_right * incident_wave) + ((-1.0f * gamma_right) * left_wave) + ((1.0f - gamma_right) * right_wave));
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

//WDF

void tWDF_init(tWDF* const r, WDFComponentType type, float value, tWDF* const rL, tWDF* const rR, float sample_rate)
{
	r->type = type;
	r->incident_wave_up = 0.0f;
	r->incident_wave_left = 0.0f;
	r->incident_wave_right = 0.0f;
	r->reflected_wave_up = 0.0f;
	r->reflected_wave_left = 0.0f;
	r->reflected_wave_right = 0.0f;
	r->sample_rate = sample_rate;
	r->value = value;
	if (r->type == SeriesAdaptor)
	{
		r->child_left = rL;
		r->child_right = rR;
		r->port_resistance_left = tWDF_getPortResistance(rL);
		r->port_resistance_right = tWDF_getPortResistance(rR);
		r->port_resistance_up = r->port_resistance_left + r->port_resistance_right;
		r->port_conductance_up  = 1.0f / r->port_resistance_up;
		r->port_conductance_left = 1.0f / r->port_resistance_left;
		r->port_conductance_right = 1.0f / r->port_resistance_right;
		r->gamma_zero = 1.0f / (r->port_resistance_right + r->port_resistance_left);

		r->get_port_resistance = &get_port_resistance_for_series;
		r->get_reflected_wave = &get_reflected_wave_for_series;
		r->set_incident_wave = &set_incident_wave_for_series;
	}
	else if (r->type == ParallelAdaptor)
	{
		r->child_left = rL;
		r->child_right = rR;
		r->port_resistance_left = tWDF_getPortResistance(rL);
		r->port_resistance_right = tWDF_getPortResistance(rR);
		r->port_resistance_up = (r->port_resistance_left * r->port_resistance_right) / (r->port_resistance_left + r->port_resistance_right);
		r->port_conductance_up  = 1.0f / r->port_resistance_up;
		r->port_conductance_left = 1.0f / r->port_resistance_left;
		r->port_conductance_right = 1.0f / r->port_resistance_right;
		r->gamma_zero = 1.0f / (r->port_resistance_right + r->port_resistance_left);

		r->get_port_resistance = &get_port_resistance_for_parallel;
		r->get_reflected_wave = &get_reflected_wave_for_parallel;
		r->set_incident_wave = &set_incident_wave_for_parallel;
	}
	else if (r->type == Resistor)
	{
		r->port_resistance_up = r->value;
		r->port_conductance_up = 1.0f / r->value;

		r->get_port_resistance = &get_port_resistance_for_resistor;
		r->get_reflected_wave = &get_reflected_wave_for_resistor;
		r->set_incident_wave = &set_incident_wave_for_leaf;
	}
	else if (r->type == Capacitor)
	{
		r->port_resistance_up = 1.0f / (sample_rate * 2.0f * r->value); //based on trapezoidal discretization
		r->port_conductance_up = (1.0f / r->port_resistance_up);

		r->get_port_resistance = &get_port_resistance_for_capacitor;
		r->get_reflected_wave = &get_reflected_wave_for_capacitor;
		r->set_incident_wave = &set_incident_wave_for_leaf;
	}
	else if (r->type == ResistiveSource)
	{

	}
}

float tWDF_tick(tWDF* const r, float sample, uint8_t paramsChanged)
{
	//step 0 : update port resistances if something changed
	if (paramsChanged) tWDF_getPortResistance(r);

	//step 1 : set inputs to what they should be
	float input = sample;

	//step 2 : scan the waves up the tree
	float incident_wave = tWDF_getReflectedWave(r);

	//step 3 : do root scattering computation
	float reflected_wave = (2.0f * input) - incident_wave;

	//step 4 : propigate waves down the tree
	tWDF_setIncidentWave(r, reflected_wave);

	//step 5 : grab whatever voltages or currents we want as outputs
	return -1.0f * tWDF_getVoltage(r->child_right->child_right);
}

void tWDF_setValue(tWDF* const r, float value)
{
	r->value = value;
}

float tWDF_getPortResistance(tWDF* const r)
{
	return r->get_port_resistance(r);
}

void tWDF_setIncidentWave(tWDF* const r, float incident_wave)
{
	r->set_incident_wave(r, incident_wave);
}

float tWDF_getReflectedWave(tWDF* const r)
{
	return r->get_reflected_wave(r);
}

float tWDF_getVoltage(tWDF* const r)
{
	return ((r->incident_wave_up * 0.5f) + (r->reflected_wave_up * 0.5f));
}

float tWDF_getCurrent(tWDF* const r)
{
	return (((r->incident_wave_up * 0.5f) - (r->reflected_wave_up * 0.5f)) * r->port_conductance_up);
}

// static functions to be pointed to
static float get_port_resistance_for_resistor(tWDF* const r)
{
	r->port_resistance_up = r->value;
	r->port_conductance_up = 1.0f / r->value;

	return r->port_resistance_up;
}

static float get_port_resistance_for_capacitor(tWDF* const r)
{
	r->port_resistance_up = 1.0f / (r->sample_rate * 2.0f * r->value); //based on trapezoidal discretization
	r->port_conductance_up = (1.0f / r->port_resistance_up);

	return r->port_resistance_up;
}

static float get_port_resistance_for_series(tWDF* const r)
{
	r->port_resistance_left = tWDF_getPortResistance(r->child_left);
	r->port_resistance_right = tWDF_getPortResistance(r->child_right);
	r->port_resistance_up = r->port_resistance_left + r->port_resistance_right;
	r->port_conductance_up  = 1.0f / r->port_resistance_up;
	r->port_conductance_left = 1.0f / r->port_resistance_left;
	r->port_conductance_right = 1.0f / r->port_resistance_right;
	r->gamma_zero = 1.0f / (r->port_resistance_right + r->port_resistance_left);

	return r->port_resistance_up;
}

static float get_port_resistance_for_parallel(tWDF* const r)
{
	r->port_resistance_left = tWDF_getPortResistance(r->child_left);
	r->port_resistance_right = tWDF_getPortResistance(r->child_right);
	r->port_resistance_up = (r->port_resistance_left * r->port_resistance_right) / (r->port_resistance_left + r->port_resistance_right);
	r->port_conductance_up  = 1.0f / r->port_resistance_up;
	r->port_conductance_left = 1.0f / r->port_resistance_left;
	r->port_conductance_right = 1.0f / r->port_resistance_right;
	r->gamma_zero = 1.0f / (r->port_conductance_right + r->port_conductance_left);

	return r->port_resistance_up;
}

static void set_incident_wave_for_leaf(tWDF* const r, float incident_wave)
{
	r->incident_wave_up = incident_wave;
}

static void set_incident_wave_for_series(tWDF* const r, float incident_wave)
{
	float gamma_left = r->port_resistance_left * r->gamma_zero;
	float gamma_right = r->port_resistance_right * r->gamma_zero;
	float left_wave = tWDF_getReflectedWave(r->child_left);
	float right_wave = tWDF_getReflectedWave(r->child_right);
//    downPorts[0]->b = yl * ( downPorts[0]->a * ((1.0 / yl) - 1) - downPorts[1]->a - descendingWave );
//    downPorts[1]->b = yr * ( downPorts[1]->a * ((1.0 / yr) - 1) - downPorts[0]->a - descendingWave );
	tWDF_setIncidentWave(r->child_left, (-1.0f * gamma_left * incident_wave) + (gamma_right * left_wave) - (gamma_left * right_wave));
	tWDF_setIncidentWave(r->child_right, (-1.0f * gamma_right * incident_wave) + (gamma_left * right_wave) - (gamma_right * left_wave));
	// From rt-wdf
//	tWDF_setIncidentWave(r->child_left, gamma_left * (left_wave * ((1.0f / gamma_left) - 1.0f) - right_wave - incident_wave));
//	tWDF_setIncidentWave(r->child_right, gamma_right * (right_wave * ((1.0f / gamma_right) - 1.0f) - left_wave - incident_wave));

}

static void set_incident_wave_for_parallel(tWDF* const r, float incident_wave)
{
	float gamma_left = r->port_conductance_left * r->gamma_zero;
	float gamma_right = r->port_conductance_right * r->gamma_zero;
	float left_wave = tWDF_getReflectedWave(r->child_left);
	float right_wave = tWDF_getReflectedWave(r->child_right);
//    downPorts[0]->b = ( ( dl - 1 ) * downPorts[0]->a + dr * downPorts[1]->a + du * descendingWave );
//    downPorts[1]->b = ( dl * downPorts[0]->a + ( dr - 1 ) * downPorts[1]->a + du * descendingWave );
	tWDF_setIncidentWave(r->child_left, (gamma_left - 1.0f) * left_wave + gamma_right * right_wave + incident_wave);
	tWDF_setIncidentWave(r->child_right, gamma_left * left_wave + (gamma_right - 1.0f) * right_wave + incident_wave);
}

static float get_reflected_wave_for_resistor(tWDF* const r)
{
	r->reflected_wave_up = 0.0f;
	return r->reflected_wave_up;
}

static float get_reflected_wave_for_capacitor(tWDF* const r)
{
	r->reflected_wave_up = r->incident_wave_up;
	return r->reflected_wave_up;
}

static float get_reflected_wave_for_series(tWDF* const r)
{
	//-( downPorts[0]->a + downPorts[1]->a );
	return (-1.0f * (tWDF_getReflectedWave(r->child_left) + tWDF_getReflectedWave(r->child_right)));
}

static float get_reflected_wave_for_parallel(tWDF* const r)
{
	float gamma_left = r->port_conductance_left * r->gamma_zero;
	float gamma_right = r->port_conductance_right * r->gamma_zero;
	//return ( dl * downPorts[0]->a + dr * downPorts[1]->a );
	return (gamma_left * tWDF_getReflectedWave(r->child_left) + gamma_right * tWDF_getReflectedWave(r->child_right));
}
