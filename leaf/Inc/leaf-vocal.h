/*
 * leaf-vocal.h
 *
 *  Created on: Oct 17, 2023
 *      Author: jeffsnyder
 */

#ifndef LEAF_VOCAL_H_INCLUDED
#define LEAF_VOCAL_H_INCLUDED

#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-filters.h"
#include "leaf-oscillators.h"

#define EPSILON 1.0e-38

#define MAX_TRANSIENTS 4



typedef struct glottis
{
	tMempool* mempool;
	float  freq;
    float  tenseness;
    float  Rd;
    float  waveform_length;
    float  time_in_waveform;

    float  alpha;
    float  E0;
    float  epsilon;
    float  shift;
    float  delta;
    float  Te;
    float  omega;
    float  Again;

    float  T;
} glottis;

void    glottis_init           (glottis**glo, LEAF* const leaf);
void    glottis_initToPool     (glottis**glo, tMempool** const mp);

float  glottis_compute        (glottis* glo);
void    glottis_setup_waveform (glottis* glo);



typedef struct transient
{
    int  position;
    float  time_alive;
    float  lifetime;
    float  strength;
    float  exponent;
    char is_free;
    unsigned int id;
    struct transient *next;
} transient;



typedef struct transient_pool
{
    transient* pool[MAX_TRANSIENTS];
    transient *root;
    int size;
    int next_free;
} transient_pool;


typedef struct tract
{
	tMempool* mempool;
	int n;
	float invN;
	int nMinusOne;
	int maxNumTractSections;
	    float*  diameter;
	    float*  rest_diameter;
	    float*  target_diameter;
	    float*  new_diameter;
	    float*  R;
	    float*  L;
	    float*  reflection;
	    float*  new_reflection;
	    float*  junction_outL;
	    float*  junction_outR;
	    float*  A;

	    float TnoiseGain;
	    float AnoiseGain;
	    int nose_length;


	    int nose_start;


	    int tip_start;
	    int blade_start;
	    int lip_start;

		float tongueUpperBound;
		float tongueLowerBound;

	    float*  noseL;
	    float*  noseR;
	    float*  nose_junc_outL;
	    float*  nose_junc_outR;
	    float*  nose_reflection;
	    float*  nose_diameter;
	    float*  noseA;

	    float  reflection_left;
	    float  reflection_right;
	    float  reflection_nose;

	    float  new_reflection_left;
	    float  new_reflection_right;
	    float  new_reflection_nose;

	    float  velum_target;

	    float  glottal_reflection;
	    float  lip_reflection;
	    int  last_obstruction;
	    float  fade;
	    float  movement_speed;
	    float  lip_output;
	    float  nose_output;
	    float  block_time;
	    float diameterScale;

	    tSVF* fricativeNoiseFilt[2];
	    tSVF* aspirationNoiseFilt;
	    tNoise* whiteNoise;

	    float turbuluencePointPosition[2];
	    float turbuluencePointDiameter[2];
	    transient_pool* tpool;

	    float  T;
} tract;



void    tract_init(tract** const t,  int numTractSections, int maxNumTractSections, LEAF* const leaf);
void    tract_initToPool(tract** const t,  int numTractSections, int maxNumTractSections, tMempool** const mp);

void    tract_calculate_reflections        (tract* t);
void    tract_newLength                    (tract* t, int newLength);
void    tract_reshape                      (tract* t);
void    tract_compute                      (tract* t, float  in, float  lambda);
void    tract_calculate_nose_reflections   (tract* t);
void    tract_addTurbulenceNoise           (tract* const t);
void    tract_addTurbulenceNoiseAtPosition (tract* const t, float turbulenceNoise, float position, float diameter);
int     append_transient                   (transient_pool* pool, int position);
void    remove_transient                   (transient_pool* pool, unsigned int id);
float  move_towards                       (float current, float target, float amt_up, float amt_down);




typedef struct tVoc
{
    tMempool* mempool;
    glottis*  glot; /*The Glottis*/
    tract*  tr; /*The Vocal Tract */
    int doubleCompute;
    //float*  buf;
    int counter;
    int sampleRate;
} tVoc;

void    tVoc_init(tVoc** const voc, int numTractSections, int maxNumTractSections, LEAF* const leaf);
void    tVoc_initToPool(tVoc** const voc, int numTractSections,int maxNumTractSections, tMempool** const mempool);
void    tVoc_free(tVoc** const voc);

float  tVoc_tick                           (tVoc* const voc);

void    tVoc_tractCompute                   (tVoc* const voc, float *in, float *out);
void    tVoc_setSampleRate                  (tVoc* const voc, float sr);
void    tVoc_setFreq                        (tVoc* const voc, float freq);
float* tVoc_get_tract_diameters            (tVoc* const voc);
float* tVoc_get_current_tract_diameters    (tVoc* const voc);
float* tVoc_get_tract_rest_diameters       (tVoc* const voc);
void    tVoc_set_tractLength                (tVoc* const voc, int newLength);
int     tVoc_get_tract_size                 (tVoc* const voc);
float* tVoc_get_nose_diameters             (tVoc* const voc);
int     tVoc_get_nose_size                  (tVoc* const voc);
void    tVoc_set_tongue_shape_and_touch     (tVoc* const voc, float tongue_index, float tongue_diameter, float touch_index, float touch_diameter);
void    tVoc_set_tongue_and_touch_diameters (tVoc* const voc, float tongue_index, float tongue_diameter, float touch_index, float touch_diameter, float *theDiameters);
void    tVoc_set_tenseness                  (tVoc* const voc, float breathiness);
float* tVoc_get_tenseness_ptr              (tVoc* const voc);
void    tVoc_set_velum                      (tVoc* const voc, float velum);
float* tVoc_get_velum_ptr                  (tVoc* const voc);
void    tVoc_setDoubleComputeFlag           (tVoc* const voc, int doubleCompute);
int     tVoc_get_counter                    (tVoc* const voc);
void    tVoc_rescaleDiameter                (tVoc* const voc, float scale);
void    tVoc_setTurbulenceNoiseGain         (tVoc* const voc, float gain);
void    tVoc_setAspirationNoiseGain         (tVoc* const voc, float gain);
void    tVoc_setAspirationNoiseFilterFreq   (tVoc* const voc, float Q);
void    tVoc_setAspirationNoiseFilterQ      (tVoc* const voc, float freq);

#endif /* INC_LEAF_VOCAL_H_ */
