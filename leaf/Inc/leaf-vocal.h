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
	tMempool mempool;
	Lfloat  freq;
    Lfloat  tenseness;
    Lfloat  Rd;
    Lfloat  waveform_length;
    Lfloat  time_in_waveform;

    Lfloat  alpha;
    Lfloat  E0;
    Lfloat  epsilon;
    Lfloat  shift;
    Lfloat  delta;
    Lfloat  Te;
    Lfloat  omega;
    Lfloat  Again;

    Lfloat  T;
} glottis;

void    glottis_init           (glottis**glo, LEAF* const leaf);
void    glottis_initToPool     (glottis**glo, tMempool* const mp);

Lfloat  glottis_compute        (glottis* glo);
void    glottis_setup_waveform (glottis* glo);



typedef struct _transient
{
    int  position;
    Lfloat  time_alive;
    Lfloat  lifetime;
    Lfloat  strength;
    Lfloat  exponent;
    char is_free;
    unsigned int id;
    struct _transient *next;
} _transient;

typedef _transient* transient;

typedef struct _transient_pool
{
    transient pool[MAX_TRANSIENTS];
    _transient *root;
    int size;
    int next_free;
} _transient_pool;

typedef _transient_pool* transient_pool;


typedef struct _tract
{
	tMempool mempool;
	int n;
	Lfloat invN;
	int nMinusOne;
	int maxNumTractSections;
	    Lfloat*  diameter;
	    Lfloat*  rest_diameter;
	    Lfloat*  target_diameter;
	    Lfloat*  new_diameter;
	    Lfloat*  R;
	    Lfloat*  L;
	    Lfloat*  reflection;
	    Lfloat*  new_reflection;
	    Lfloat*  junction_outL;
	    Lfloat*  junction_outR;
	    Lfloat*  A;

	    Lfloat TnoiseGain;
	    Lfloat AnoiseGain;
	    int nose_length;


	    int nose_start;


	    int tip_start;
	    int blade_start;
	    int lip_start;

		Lfloat tongueUpperBound;
		Lfloat tongueLowerBound;

	    Lfloat*  noseL;
	    Lfloat*  noseR;
	    Lfloat*  nose_junc_outL;
	    Lfloat*  nose_junc_outR;
	    Lfloat*  nose_reflection;
	    Lfloat*  nose_diameter;
	    Lfloat*  noseA;

	    Lfloat  reflection_left;
	    Lfloat  reflection_right;
	    Lfloat  reflection_nose;

	    Lfloat  new_reflection_left;
	    Lfloat  new_reflection_right;
	    Lfloat  new_reflection_nose;

	    Lfloat  velum_target;

	    Lfloat  glottal_reflection;
	    Lfloat  lip_reflection;
	    int  last_obstruction;
	    Lfloat  fade;
	    Lfloat  movement_speed;
	    Lfloat  lip_output;
	    Lfloat  nose_output;
	    Lfloat  block_time;
	    Lfloat diameterScale;

	    tSVF fricativeNoiseFilt[2];
	    tSVF aspirationNoiseFilt;
	    tNoise whiteNoise;

	    Lfloat turbuluencePointPosition[2];
	    Lfloat turbuluencePointDiameter[2];
	    transient_pool tpool;

	    Lfloat  T;
} _tract;

typedef _tract* tract;

void    tract_init(tract* const t,  int numTractSections, int maxNumTractSections, LEAF* const leaf);
void    tract_initToPool(tract* const t,  int numTractSections, int maxNumTractSections, tMempool* const mp);

void    tract_calculate_reflections        (tract t);
void    tract_newLength                    (tract t, int newLength);
void    tract_reshape                      (tract t);
void    tract_compute                      (tract t, Lfloat  in, Lfloat  lambda);
void    tract_calculate_nose_reflections   (tract t);
void    tract_addTurbulenceNoise           (tract const t);
void    tract_addTurbulenceNoiseAtPosition (tract const t, Lfloat turbulenceNoise, Lfloat position, Lfloat diameter);
int     append_transient                   (transient_pool pool, int position);
void    remove_transient                   (transient_pool pool, unsigned int id);
Lfloat  move_towards                       (Lfloat current, Lfloat target, Lfloat amt_up, Lfloat amt_down);




typedef struct _tVoc
{
    tMempool mempool;
    glottis  glot; /*The Glottis*/
    tract  tr; /*The Vocal Tract */
    int doubleCompute;
    //Lfloat*  buf;
    int counter;
    int sampleRate;
} _tVoc;

typedef _tVoc* tVoc;

void    tVoc_init                           (tVoc* const voc, int numTractSections, int maxNumTractSections, LEAF* const leaf);
void    tVoc_initToPool                     (tVoc* const voc, int numTractSections,int maxNumTractSections, tMempool* const mempool);
void    tVoc_free                           (tVoc* const voc);

Lfloat  tVoc_tick                           (tVoc const voc);

void    tVoc_tractCompute                   (tVoc const voc, Lfloat *in, Lfloat *out);
void    tVoc_setSampleRate                  (tVoc const voc, Lfloat sr);
void    tVoc_setFreq                        (tVoc const voc, Lfloat freq);
Lfloat* tVoc_get_tract_diameters            (tVoc const voc);
Lfloat* tVoc_get_current_tract_diameters    (tVoc const voc);
Lfloat* tVoc_get_tract_rest_diameters       (tVoc const voc);
void    tVoc_set_tractLength                (tVoc const voc, int newLength);
int     tVoc_get_tract_size                 (tVoc const voc);
Lfloat* tVoc_get_nose_diameters             (tVoc const voc);
int     tVoc_get_nose_size                  (tVoc const voc);
void    tVoc_set_tongue_shape_and_touch     (tVoc const voc, Lfloat tongue_index, Lfloat tongue_diameter, Lfloat touch_index, Lfloat touch_diameter);
void    tVoc_set_tongue_and_touch_diameters (tVoc const voc, Lfloat tongue_index, Lfloat tongue_diameter, Lfloat touch_index, Lfloat touch_diameter, Lfloat *theDiameters);
void    tVoc_set_tenseness                  (tVoc const voc, Lfloat breathiness);
Lfloat* tVoc_get_tenseness_ptr              (tVoc const voc);
void    tVoc_set_velum                      (tVoc const voc, Lfloat velum);
Lfloat* tVoc_get_velum_ptr                  (tVoc const voc);
void    tVoc_setDoubleComputeFlag           (tVoc const voc, int doubleCompute);
int     tVoc_get_counter                    (tVoc const voc);
void    tVoc_rescaleDiameter                (tVoc const voc, Lfloat scale);
void    tVoc_setTurbulenceNoiseGain         (tVoc const voc, Lfloat gain);
void    tVoc_setAspirationNoiseGain         (tVoc const voc, Lfloat gain);
void    tVoc_setAspirationNoiseFilterFreq   (tVoc const voc, Lfloat Q);
void    tVoc_setAspirationNoiseFilterQ      (tVoc const voc, Lfloat freq);

#endif /* INC_LEAF_VOCAL_H_ */
