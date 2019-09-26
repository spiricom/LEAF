/*
 * leaf-WDF.c
 *
 *  Created on: Sep 25, 2019
 *      Author: jeffsnyder
 */


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
float tWDFresistor_getVoltage(tWDFresistor* const r)
{
	return ((r->incident_wave * 0.5f) + (r->reflected_wave * 0.5f));
}
float tWDFresistor_getCurrent(tWDFresistor* const r)
{
	return (((r->incident_wave * 0.5f) - (r->reflected_wave * 0.5f)) / r->port_conductance);
}
