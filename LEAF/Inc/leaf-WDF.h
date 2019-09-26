/*
 * leaf-WDF.h
 *
 *  Created on: Sep 25, 2019
 *      Author: jeffsnyder
 */

#ifndef LEAF_INC_LEAF_WDF_H_
#define LEAF_INC_LEAF_WDF_H_


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
void tWDFresistor_setIncidentWave(tWDFresistor* const r, float incident_wave);
float tWDFresistor_getVoltage(tWDFresistor* const r);
float tWDFresistor_getCurrent(tWDFresistor* const r);


#endif /* LEAF_INC_LEAF_WDF_H_ */
