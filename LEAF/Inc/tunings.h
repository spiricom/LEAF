/*
 * tunings.h
 *
 *  Created on: Dec 18, 2019
 *      Author: josnyder
 */

#ifndef TUNINGS_H_
#define TUNINGS_H_

#define NUM_TUNINGS 66

extern float centsDeviation[12];
extern float tuningPresets[NUM_TUNINGS][12];
extern uint32_t currentTuning;
extern uint8_t keyCenter;
extern char tuningNames[NUM_TUNINGS][6];


#endif /* TUNINGS_H_ */
