/*
 * voc.c
 *
 *  Created on: Oct 17, 2023
 *      Author: jeffsnyder
 */


//this vocal synthesis model is copied from soundpipe, written by Paul Batcheler. He put it in the public domain, so this particular part of leaf is also public domain, since I added very little.


#if _WIN32 || _WIN64
#include "..\Inc\leaf-math.h"
#include "..\Inc\leaf-vocal.h"
#include "..\leaf.h"

#else
#include "../Inc/leaf-math.h"
#include "../Inc/leaf-vocal.h"
#include "../leaf.h"

#ifdef ARM_MATH_CM7
#include "arm_math.h"
#endif


#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#ifndef MIN
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#endif

#ifndef MAX
#define MAX(A,B) ((A) > (B) ? (A) : (B))
#endif



volatile uint32_t ERRcatch = 0;

static inline int v_isnan(float f)
{
    union { float f; uint32_t x; } u = { f };
    return (u.x << 1) > 0xff000000u;
}

void glottis_setup_waveform(glottis* const glot)
{
    Lfloat Rd;
    Lfloat Ra;
    Lfloat Rk;
    Lfloat Rg;

    Lfloat Ta;
    Lfloat Tp;
    Lfloat Te;

    Lfloat epsilon;
    Lfloat shift;
    Lfloat delta;
    Lfloat rhs_integral;

    Lfloat lower_integral;
    Lfloat upper_integral;

    Lfloat omega;
    Lfloat s;
    Lfloat y;
    Lfloat z;

    Lfloat alpha;
    Lfloat E0;


    glot->Rd = 3.0f * (1.0f - glot->tenseness);
    glot->waveform_length = 1.0f / glot->freq;

    Rd = glot->Rd;
    if(Rd < 0.5f) Rd = 0.5f;
    if(Rd > 2.7f) Rd = 2.7f;


    Ra = -0.01f + 0.048f*Rd;
    Rk = 0.224f + 0.118f*Rd;
    Rg = (Rk*0.25f)*(0.5f + 1.2f*Rk)/(0.11f*Rd-Ra*(0.5f+1.2f*Rk));


    Ta = Ra;
    Tp = (Lfloat)1.0f / (2.0f*Rg);
    Te = Tp + Tp*Rk;



    epsilon = (Lfloat)1.0f / Ta;
    shift = fasterexpf(-epsilon * (1.0f - Te));
    delta = 1.0f - shift;



    rhs_integral = (Lfloat)(1.0f/epsilon) * (shift-1.0f) + (1.0f-Te)*shift;
    rhs_integral = rhs_integral / delta;
    lower_integral = - (Te - Tp) / 2.0f + rhs_integral;
    upper_integral = -lower_integral;


    omega = M_PI / Tp;
    s = fast_sinf2(omega * Te);

    y = -M_PI * s * upper_integral / (Tp*2.0f);
    z = my_faster_logf(y);
    alpha = z / (Tp/2.0f - Te);
    E0 = -1.0f / (s * fastExp4(alpha*Te));



    glot->alpha = alpha;
    glot->E0 = E0;
    glot->epsilon = epsilon;
    glot->shift = shift;
    glot->delta = delta;
    glot->Te = Te;
    glot->omega = omega;
}
void glottis_init(glottis** const glo, LEAF* const leaf)
{
	glottis_initToPool(glo, &leaf->mempool);
}

void glottis_initToPool(glottis** const glo, tMempool** const mp)
{

	tMempool* m = *mp;
	glottis* glot = *glo = (glottis*) mpool_calloc(sizeof(glottis), m);
	glot->mempool = m;
	LEAF* leaf = glot->mempool->leaf;
	glot->mempool = m;
	glot->freq = 140.0f; /* 140Hz frequency by default */
    glot->tenseness = 0.6f; /* value between 0 and 1 */
    glot->T = 1.0f/leaf->sampleRate; /* big T */
    glot->time_in_waveform = 0;
    glottis_setup_waveform(glot);
}

void glottis_free(glottis** const glo)
{
	glottis* glot = *glo;
	mpool_free((char*)glot, glot->mempool);
}


Lfloat glottis_compute(glottis* const glot)
{
	LEAF* leaf = glot->mempool->leaf;
	Lfloat out;
    Lfloat aspiration;
    Lfloat noise;
    Lfloat t;
    Lfloat intensity;

    out = 0.0f;
    intensity = 1.0f;
    glot->time_in_waveform += glot->T;

    if(glot->time_in_waveform > glot->waveform_length) {
        glot->time_in_waveform -= glot->waveform_length;
        glottis_setup_waveform(glot);

    }

    t = (glot->time_in_waveform * glot->freq);//multiplying by freq same as divide by length since length is 1/f

    if(t > glot->Te) {
        out = (-fastExp4(-glot->epsilon * (t-glot->Te)) + glot->shift) / glot->delta;
    } else {
#ifdef ARM_MATH_CM7
        out = glot->E0 * fastExp4(glot->alpha * t) * arm_sin_f32(glot->omega * t);
#else
        out = glot->E0 * fastExp4(glot->alpha * t) * sinf(glot->omega * t);
#endif

    }

    noise = (2.0f * leaf->random()) - 1.0f;

#ifdef ARM_MATH_CM7
    Lfloat sqr = 0.0f;
    arm_sqrt_f32(glot->tenseness, &sqr);
    aspiration = intensity * (1.0f - sqr) * 0.3f * noise;
#else
        aspiration = intensity * (1.0f - sqrtf(glot->tenseness)) * 0.3f * noise;
#endif

    aspiration *= 0.2f;

    out += aspiration;

    return out;
}

void tract_init(tract** const t,  int numTractSections, int maxNumTractSections, LEAF* const leaf)
{
	tract_initToPool(t, numTractSections, maxNumTractSections, &leaf->mempool);
}

void tract_initToPool(tract** const t,  int numTractSections, int maxNumTractSections, tMempool** const mp)
{
	tMempool* m = *mp;
	tract* tr = *t = (tract*) mpool_calloc(sizeof(tract), m);
	tr->mempool = m;
	LEAF* leaf = m->leaf;

	int i;
    Lfloat diameter, d; /* needed to set up diameter arrays */
    Lfloat n = numTractSections;
    tr->maxNumTractSections = maxNumTractSections;
    tr->n = n; //44
    tr->invN = 1.0f / n;
    tr->nose_length = n*0.636363636363636f; //28
    tr->nose_start = (n - tr->nose_length) + 1; //17

    tr->reflection_left = 0.0f;
    tr->reflection_right = 0.0f;
    tr->reflection_nose = 0.0f;
    tr->new_reflection_left = 0.0f;
    tr->new_reflection_right= 0.0f;
    tr->new_reflection_nose = 0.0f;
    tr->velum_target = 0.01f;
    tr->glottal_reflection = 0.75f;
    tr->lip_reflection = -0.85f;
    tr->last_obstruction = -1;
    tr->movement_speed = 200.0f;
    tr->lip_output = 0.0f;
    tr->nose_output = 0.0f;
    tr->tip_start = n*0.727272727272727f;
    tr->blade_start = n*0.227272727272727f;
    tr->lip_start = n*0.886363636363636f;

    tr->TnoiseGain = 1.0f;
    tr->AnoiseGain = 1.0f;
    tr->diameterScale = 1.0f;
	tr->tongueUpperBound = (tr->tip_start-(n*0.068181818181818f));
	tr->tongueLowerBound = (tr->blade_start+(n*0.045454545454545f));

	tr->turbuluencePointPosition[0] = n* 0.45f;
	tr->turbuluencePointPosition[1] = n* 0.5f;
	tr->turbuluencePointDiameter[0] = 1.5f;
	tr->turbuluencePointDiameter[1] = 1.5f;

    Lfloat invNoseLength = 1.0f / tr->nose_length;

    tr->diameter = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->rest_diameter = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->target_diameter = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->new_diameter = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->L = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->R = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->reflection = (Lfloat*) mpool_calloc(sizeof(Lfloat) * (maxNumTractSections + 1), m);
    tr->new_reflection = (Lfloat*) mpool_calloc(sizeof(Lfloat) * (maxNumTractSections + 1), m);
    tr->junction_outL = (Lfloat*) mpool_calloc(sizeof(Lfloat) * (maxNumTractSections + 1), m);
    tr->junction_outR = (Lfloat*) mpool_calloc(sizeof(Lfloat) * (maxNumTractSections + 1), m);
    tr->A = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->noseL = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->noseR = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->nose_junc_outL = (Lfloat*) mpool_calloc(sizeof(Lfloat) * (maxNumTractSections + 1), m);
    tr->nose_junc_outR = (Lfloat*) mpool_calloc(sizeof(Lfloat) * (maxNumTractSections + 1), m);
    tr->nose_diameter = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->nose_reflection = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);
    tr->noseA = (Lfloat*) mpool_calloc(sizeof(Lfloat) * maxNumTractSections, m);

    for(i = 0; i < tr->n; i++) {
        diameter = 0;
        if(i < (int)(((7.0f*tr->invN) * tr->n) - 0.5f)) { //was 7
            diameter = 0.6f;
        } else if( i < (int)((12.0f*tr->invN) * (Lfloat)tr->n)) { //was 12
            diameter = 1.1f;
        } else {
            diameter = 1.5f;
        }

        tr->diameter[i] =
            tr->rest_diameter[i] =
            tr->target_diameter[i] =
            tr->new_diameter[i] = diameter;

    }

	for(i = 0; i < tr->nose_length; i++) {
		d = 2.0f * ((Lfloat)i * invNoseLength);
		if(d < 1.0f) {
			diameter = 0.4f + 1.6f * d;
		} else {
			diameter = 0.5f + 1.5f*(2.0f-d);
		}
		diameter = MIN(diameter, 1.9f);
		tr->nose_diameter[i] = diameter;
	}

	tSVF_initToPool(&tr->fricativeNoiseFilt[0], SVFTypeBandpass, 1010.0f, 0.7f, &m);
	tSVF_initToPool(&tr->fricativeNoiseFilt[1], SVFTypeBandpass, 990.0f, 0.7f, &m);
	tSVF_initToPool(&tr->aspirationNoiseFilt, SVFTypeBandpass, 500.0f, 0.7f, &m);
	tNoise_initToPool(&tr->whiteNoise, WhiteNoise, &m);

    tract_calculate_reflections(tr);
    tract_calculate_nose_reflections(tr);
    tr->nose_diameter[0] = tr->velum_target;

    tr->block_time = 64.0f / leaf->sampleRate;
    tr->T = 1.0f / leaf->sampleRate;

    tr->tpool = (transient_pool*) mpool_alloc(sizeof(transient_pool), m);
    tr->tpool->size = 0;
    tr->tpool->next_free = 0;
    for(i = 0; i < MAX_TRANSIENTS; i++) {
    	tr->tpool->pool[i] = (transient*) mpool_alloc(sizeof(transient), m);
        tr->tpool->pool[i]->is_free = 1;
        tr->tpool->pool[i]->id = i;
        tr->tpool->pool[i]->position = 0;
        tr->tpool->pool[i]->time_alive = 0;
        tr->tpool->pool[i]->strength = 0;
        tr->tpool->pool[i]->exponent = 0;
    }
}

void tract_free(tract** const t)
{
	tract* tr = *t;
	mpool_free((char*)tr, tr->mempool);
}


void tract_calculate_reflections(tract* const tr)
{
	int i;
    Lfloat  sum;

    for(i = 0; i < tr->n; i++)
    {
    	Lfloat scaledDiameter = tr->diameter[i] * tr->diameterScale;
        tr->A[i] = scaledDiameter * scaledDiameter;
        /* Calculate area from diameter squared*/
    }

    for(i = 1; i < tr->n; i++)
    {
        tr->reflection[i] = tr->new_reflection[i];

        Lfloat divisorTest = (tr->A[i - 1] + tr->A[i]);

        if(tr->A[i] <= 0.0000001f)
        {
            tr->new_reflection[i] = 0.99f; /* to prevent bad behavior if 0 */
        }
        else
        {
        	if ((divisorTest >= 0.0000001f)|| (divisorTest <= -0.0000001f))
        	{
        		tr->new_reflection[i] = (tr->A[i - 1] - tr->A[i]) / divisorTest;
        	}
        }
    }

    tr->reflection_left = tr->new_reflection_left;
    tr->reflection_right = tr->new_reflection_right;
    tr->reflection_nose = tr->new_reflection_nose;

    sum = tr->A[tr->nose_start] + tr->A[tr->nose_start + 1] + tr->noseA[0];
    if ((sum  <= 0.001f) && (sum >= -0.001f))
    {
    	sum = 0.001f;
    }
    Lfloat invSum = 1.0f / sum;
    tr->new_reflection_left = (Lfloat)(2.0f * tr->A[tr->nose_start] - sum) * invSum;
    tr->new_reflection_right = (Lfloat)(2.0f * tr->A[tr->nose_start + 1] - sum) * invSum;
    tr->new_reflection_nose = (Lfloat)(2.0f * tr->noseA[0] - sum) * invSum;
}

void tract_newLength(tract* const tr, int numTractSections)
{
	int i;
	Lfloat diameter, d;
    Lfloat n;
    if (numTractSections < tr->maxNumTractSections)
    {
    	n = numTractSections;
    }
    else
    {
    	n = tr->maxNumTractSections;
    }

    tr->n = n; //44
    tr->invN = 1.0f / n;
    tr->nose_length = n*0.636363636363636f; //28
    tr->nose_start = (n - tr->nose_length) + 1; //17

    tr->reflection_left = 0.0f;
    tr->reflection_right = 0.0f;
    tr->reflection_nose = 0.0f;
    tr->new_reflection_left = 0.0f;
    tr->new_reflection_right= 0.0f;
    tr->new_reflection_nose = 0.0f;
    tr->velum_target = 0.01f;
    tr->glottal_reflection = 0.75f;
    tr->lip_reflection = -0.85f;
    tr->last_obstruction = -1;
    tr->movement_speed = 200.0f;
    tr->lip_output = 0.0f;
    tr->nose_output = 0.0f;
    tr->tip_start = n*0.727272727272727f;
    tr->blade_start = n*0.227272727272727f;
    tr->lip_start = n*0.886363636363636f;

	tr->tongueUpperBound = (tr->tip_start-(n*0.068181818181818f));
	tr->tongueLowerBound = (tr->blade_start+(n*0.045454545454545f));

	tr->turbuluencePointPosition[0] = n* 0.45f;
	tr->turbuluencePointPosition[1] = n* 0.5f;
	tr->turbuluencePointDiameter[0] = 1.5f;
	tr->turbuluencePointDiameter[1] = 1.5f;

    tr->invN = 1.0f / tr->n;

    for(i = 0; i < tr->n; i++) {
        diameter = 0;
        if(i < (int)(((7.0f*tr->invN) * tr->n) - 0.5f)) { //was 7
            diameter = 0.6f;
        } else if( i < (int)((12.0f*tr->invN) * (Lfloat)tr->n)) { //was 12
            diameter = 1.1f;
        } else {
            diameter = 1.5f;
        }

        tr->diameter[i] =
            tr->rest_diameter[i] =
            tr->target_diameter[i] =
            tr->new_diameter[i] = diameter;
        tr->reflection[i] = 0.0f;
        tr->new_reflection[i] = 0.0f;
        tr->R[i] = 0.0f;
        tr->L[i] = 0.0f;
        tr->junction_outR[i] = 0.0f;
        tr->junction_outL[i] = 0.0f;
    }

    if (tr->nose_length < 0.01f)
    {
    	tr->nose_length = 0.01f;
    }
    Lfloat invNoseLength = 1.0f / tr->nose_length;
	for(i = 0; i < tr->nose_length; i++) {
		d = 2.0f * ((Lfloat)i * invNoseLength);
		if(d < 1.0f) {
			diameter = 0.4f + 1.6f * d;
		} else {
			diameter = 0.5f + 1.5f*(2.0f-d);
		}
		diameter = MIN(diameter, 1.9f);
		tr->nose_diameter[i] = diameter;
        tr->nose_reflection[i] = 0.0f;
        tr->noseR[i] = 0.0f;
        tr->noseL[i] = 0.0f;
        tr->nose_junc_outR[i] = 0.0f;
        tr->nose_junc_outL[i] = 0.0f;
	}

    tr->new_reflection_left = 0.0f;
    tr->reflection_left = 0.0f;

    tr->new_reflection_right = 0.0f;
    tr->reflection_right = 0.0f;

    tr->new_reflection_nose = 0.0f;
    tr->reflection_nose = 0.0f;

    tr->lip_output = 0.0f;
    tr->nose_output = 0.0f;

}

void tract_reshape(tract* const tr)
{
	Lfloat amount;
    Lfloat slow_return;
    Lfloat diameter;
    Lfloat target_diameter;
    int i;
    //int current_obstruction;

    //current_obstruction = -1;


    amount = tr->block_time * tr->movement_speed;

    for(i = 0; i < tr->n; i++) {
        slow_return = 0.0f;
        diameter = tr->diameter[i];
        target_diameter = tr->target_diameter[i];

        //if(diameter < 0.001f) current_obstruction = i;

        if(i < tr->nose_start) slow_return = 0.6f;
        else if(i >= tr->tip_start) slow_return = 1.0f;
        else {
        	Lfloat tempDiv= (tr->tip_start - tr->nose_start);

            slow_return =
                0.6f+0.4f*(i - tr->nose_start)/tempDiv;
        }

        tr->diameter[i] = move_towards(diameter, target_diameter,
                slow_return * amount, 0.1f * amount);

    }

/*
    if(tr->last_obstruction > -1 && current_obstruction == -1 &&
            tr->noseA[0] < 0.05f) {
        append_transient(tr->tpool, tr->last_obstruction);
    }
    tr->last_obstruction = current_obstruction;

*/
    tr->nose_diameter[0] = move_towards(tr->nose_diameter[0], tr->velum_target,
            amount * 0.25f, amount * 0.1f);
    tr->noseA[0] = tr->nose_diameter[0] * tr->nose_diameter[0];
}

void tract_addTurbulenceNoise(tract* const tr)
{
	for (int i = 0; i < 2; i++)
	{
		Lfloat turbulenceNoise = tr->TnoiseGain * tSVF_tick(tr->fricativeNoiseFilt[i], tNoise_tick(tr->whiteNoise) * 0.20f);
		tract_addTurbulenceNoiseAtPosition(tr, turbulenceNoise, tr->turbuluencePointPosition[i], tr->turbuluencePointDiameter[i]);
	}
}

void tract_addTurbulenceNoiseAtPosition(tract* const tr, Lfloat turbulenceNoise, Lfloat position, Lfloat diameter)
{
	int i = (int)floorf(position);
	Lfloat delta = position - i;
	//Lfloat mapped = LEAF_map(diameter, 0.38f, 0.245f, 0.0f, 1.0f);
	Lfloat thinness0 = LEAF_clip(0.0f, 8.0f * (0.09f - diameter),  1.0f);
	Lfloat openness = LEAF_clip(0.0f, 30.0f * (diameter), 1.0f);
	Lfloat noise0 = turbulenceNoise * (1.0f - delta) * thinness0 * openness * 0.5f;
	Lfloat noise1 = turbulenceNoise * delta * thinness0 * openness * 0.5f;

	if ((i + 1) < tr->n) {
		tr->R[i + 1] += noise0;
		tr->L[i + 1] += noise0;
	}
	if ((i + 2) < tr->n) {
		tr->R[i + 2] += noise1;
		tr->L[i + 2] += noise1;
	}
 }


void tract_compute(tract* const tr, Lfloat  in, Lfloat  lambda)
{
	Lfloat  r, w;
    int i;
    //Lfloat  amp;
    //int current_size;
   // _transient_pool *thepool;
    //_transient *n;

    Lfloat oneMinusLambda = 1.0f - lambda;

   // thepool = tr->tpool;
	//current_size = thepool->size;
	/*
	n = thepool->root;
	for(i = 0; i < current_size; i++) {
		amp = n->strength * fastPowf(2.0f, -1.0f * n->exponent * n->time_alive);
		tr->L[n->position] += amp * 0.5f;
		tr->R[n->position] += amp * 0.5f;
		n->time_alive += tr->T * 0.5f;
		if(n->time_alive > n->lifetime) {
			 remove_transient(thepool, n->id);
		}
		n = n->next;
	}
                  */

    Lfloat UVnoise = tNoise_tick(tr->whiteNoise);
    UVnoise = tSVF_tick(tr->aspirationNoiseFilt,UVnoise);

    in = fast_tanh5((UVnoise * tr->AnoiseGain) + (in * (1.0f - tr->AnoiseGain)));

	tract_addTurbulenceNoise(tr);
    tr->junction_outR[0] = tr->L[0] * tr->glottal_reflection + in;
    tr->junction_outL[tr->n] = tr->R[tr->n - 1] * tr->lip_reflection;

    for(i = 1; i < tr->n; i++) {
        r = tr->reflection[i] * oneMinusLambda + tr->new_reflection[i] * lambda;
        w = r * (tr->R[i - 1] + tr->L[i]);
        tr->junction_outR[i] = tr->R[i - 1] - w;
        tr->junction_outL[i] = tr->L[i] + w;
    }


    i = tr->nose_start;
    r = tr->new_reflection_left * oneMinusLambda + tr->reflection_left*lambda;
    tr->junction_outL[i] = r*tr->R[i-1] + (1.0f+r)*(tr->noseL[0]+tr->L[i]);
    r = tr->new_reflection_right * oneMinusLambda + tr->reflection_right * lambda;
    tr->junction_outR[i] = r*tr->L[i] + (1.0f+r)*(tr->R[i-1]+tr->noseL[0]);
    r = tr->new_reflection_nose * oneMinusLambda + tr->reflection_nose * lambda;
    tr->nose_junc_outR[0] = r * tr->noseL[0]+(1.0f+r)*(tr->L[i]+tr->R[i-1]);


    for(i = 0; i < tr->n; i++) {
        tr->R[i] = (tr->junction_outR[i]*0.999f);
        tr->L[i] = (tr->junction_outL[i + 1]*0.999f);
        //tr->R[i] = LEAF_clip(-1.5f, tr->R[i], 1.5f);



    }
    tr->L[0] = LEAF_clip(-1.5f, tr->L[0], 1.5f);
    tr->L[tr->n - 1] = LEAF_clip(-1.5f, tr->L[tr->n - 1], 1.5f);
    tr->lip_output = tr->R[tr->n - 1];


    tr->nose_junc_outL[tr->nose_length] =
        tr->noseR[tr->nose_length-1] * tr->lip_reflection;

    for(i = 1; i < tr->nose_length; i++) {
        w = tr->nose_reflection[i] * (tr->noseR[i-1] + tr->noseL[i]);
        tr->nose_junc_outR[i] = tr->noseR[i - 1] - w;
        tr->nose_junc_outL[i] = tr->noseL[i] + w;
    }


    for(i = 0; i < tr->nose_length; i++) {
        tr->noseR[i] = tr->nose_junc_outR[i];
        tr->noseL[i] = tr->nose_junc_outL[i + 1];
    }
    tr->nose_output = tr->noseR[tr->nose_length - 1];

}

void tract_calculate_nose_reflections(tract* const tr)
{
	int i;

    for(i = 0; i < tr->nose_length; i++) {
        tr->noseA[i] = tr->nose_diameter[i] * tr->nose_diameter[i];
    }

    for(i = 1; i < tr->nose_length; i++) {
    	Lfloat tempDiv = (tr->noseA[i-1] + tr->noseA[i]);
    	if ((tempDiv <= 0.001f) &&  (tempDiv >= -0.001f))
    	{
    		tempDiv = 0.01f;
    	}
        tr->nose_reflection[i] = (tr->noseA[i - 1] - tr->noseA[i]) / tempDiv;
    }
}



int append_transient(transient_pool* const pool, int position)
{
	transient* t;
	int i;
    int free_id;

    free_id = pool->next_free;
    if(pool->size == MAX_TRANSIENTS) return 0;

    if(free_id == -1) {
        for(i = 0; i < MAX_TRANSIENTS; i++) {
            if(pool->pool[i]->is_free) {
                free_id = i;
                break;
            }
        }
    }

    if(free_id == -1) return 0;

    t = pool->pool[free_id];
    t->next = pool->root;
    pool->root = t;
    pool->size++;
    t->is_free = 0;
    t->time_alive = 0.0f;
    t->lifetime = 0.2f;
    t->strength = 0.3f;
    t->exponent = 200.0f;
    t->position = position;
    pool->next_free = -1;
    return 0;
}


void remove_transient(transient_pool* const pool, unsigned int id)
{
	int i;
    transient *n;

    pool->next_free = id;
    n = pool->root;
    if(id == n->id) {
        pool->root = n->next;
        pool->size--;
        return;
    }

    for(i = 0; i < pool->size; i++) {
        if(n->next->id == id) {
            pool->size--;
            n->next->is_free = 1;
            n->next = n->next->next;
            break;
        }
        n = n->next;
    }
}



Lfloat move_towards(Lfloat current, Lfloat target,
        Lfloat amt_up, Lfloat amt_down)
{
    Lfloat tmp;
    if(current < target) {
        tmp = current + amt_up;
        return MIN(tmp, target);
    } else {
        tmp = current - amt_down;
        return MAX(tmp, target);
    }
    return 0.0f;
}



void    tVoc_init(tVoc** const voc, int numTractSections, int maxNumTractSections, LEAF* const leaf)
{
	tVoc_initToPool   (voc, numTractSections, maxNumTractSections, &leaf->mempool);
}

void    tVoc_initToPool(tVoc** const voc, int numTractSections, int maxNumTractSections, tMempool** const mp)
{
	tMempool* m = *mp;
	tVoc* v = *voc = (tVoc*) mpool_alloc(sizeof(tVoc), m);
	v->mempool = m;
	glottis_initToPool(&v->glot, &m); /* initialize glottis */
	tract_initToPool(&v->tr, numTractSections, maxNumTractSections, &m); /* initialize vocal tract */
	v->counter = 0;
}
void    tVoc_free(tVoc** const voc)
{
	tVoc* v = *voc;
	glottis_free(&v->glot);
	tract_free(&v->tr);
	//mpool_free((char*)v->buf, v->mempool);
	mpool_free((char*)v, v->mempool);
}

Lfloat   tVoc_tick         (tVoc* const v)
{
	Lfloat vocal_output, glot;
	Lfloat lambda1,lambda2;

	if(v->counter == 0) {
		tract_reshape(v->tr);
		tract_calculate_reflections(v->tr);
	}

	//Lfloat finalOutput = v->buf[v->counter];
	vocal_output = 0.0f;
	lambda1 = ((Lfloat) v->counter) * 0.015625f;// /64
	lambda2 = (Lfloat) (v->counter + 0.5f) * 0.015625f; //   /64
	glot = glottis_compute(v->glot);

	tract_compute(v->tr, glot, lambda1);
	vocal_output += v->tr->lip_output + v->tr->nose_output;

	if (v->doubleCompute)
	{
		tract_compute(v->tr, glot, lambda2);
		vocal_output += v->tr->lip_output + v->tr->nose_output;
		vocal_output *= 0.125f;
	}
	else
	{
		vocal_output *= 0.25f;
	}

	v->counter = (v->counter + 1) & 63;
	return vocal_output;
}

void    tVoc_tractCompute     (tVoc* const v, Lfloat *in, Lfloat *out)
{
	Lfloat vocal_output;
    Lfloat lambda1, lambda2;

    if(v->counter == 0) {
        tract_reshape(v->tr);
        tract_calculate_reflections(v->tr);
    }

    vocal_output = 0.0f;
    lambda1 = (Lfloat) v->counter * 0.015625f; // /64
    lambda2 = (Lfloat) (v->counter + 0.5f) * 0.015625f;// /64

    tract_compute(v->tr, *in, lambda1);
    vocal_output += v->tr->lip_output + v->tr->nose_output;
    tract_compute(v->tr, *in, lambda2);
    vocal_output += v->tr->lip_output + v->tr->nose_output;


    *out = vocal_output * 0.125;
    v->counter = (v->counter + 1) & 64;
}
void    tVoc_setDoubleComputeFlag(tVoc* const v, int doubleCompute)
{
	v->doubleCompute = doubleCompute;
}
void    tVoc_setSampleRate(tVoc* const v, Lfloat sr)
{
	v->sampleRate = sr;
}

void    tVoc_setFreq      (tVoc* const v, Lfloat freq)
{
	v->glot->freq = freq;
}

Lfloat * tVoc_get_frequency_ptr(tVoc* const v)
{
	return &v->glot->freq;
}

Lfloat* tVoc_get_tract_diameters(tVoc* const v)
{
	return v->tr->target_diameter;
}

Lfloat* tVoc_get_tract_rest_diameters(tVoc* const v)
{
	return v->tr->rest_diameter;
}

Lfloat* tVoc_get_current_tract_diameters(tVoc* const v)
{
	return v->tr->diameter;
}

int tVoc_get_tract_size(tVoc* const v)
{
	return v->tr->n;
}


Lfloat* tVoc_get_nose_diameters(tVoc* const v)
{
	return v->tr->nose_diameter;
}

int tVoc_get_nose_size(tVoc* const v)
{
	return v->tr->nose_length;
}
//diameter and index are 0-1.0f
void tVoc_set_tongue_shape_and_touch(tVoc* const v, Lfloat tongue_index, Lfloat tongue_diameter, Lfloat touch_index, Lfloat touch_diameter)
{
	Lfloat *diameters;
	diameters = tVoc_get_tract_diameters(v);
	tongue_diameter *= 1.45f;
	tongue_diameter += 2.05f;
	tongue_index = (tongue_index * (v->tr->tongueUpperBound - v->tr->tongueLowerBound)) + v->tr->tongueLowerBound;

	if (touch_diameter<0.0f) touch_diameter = 0.0f;
	touch_diameter *= 1.6f;
	touch_index = ((v->tr->n - 3.0f)* touch_index) + 2.0f;

	tVoc_set_tongue_and_touch_diameters(v, tongue_index, tongue_diameter, touch_index, touch_diameter, diameters); //10 39, 32
	v->tr->turbuluencePointPosition[0] = tongue_index;
	v->tr->turbuluencePointDiameter[0] = tongue_diameter;
	v->tr->turbuluencePointPosition[1] = touch_index;
	v->tr->turbuluencePointDiameter[1] = touch_diameter;
}
void tVoc_set_tractLength(tVoc* const v, int newLength)
{
	tract_newLength(v->tr, newLength);
}

void tVoc_set_tenseness(tVoc* const v, Lfloat tenseness)
{
	v->glot->tenseness = tenseness;
}
Lfloat * tVoc_get_tenseness_ptr(tVoc* const v)
{
	return &v->glot->tenseness;
}
void tVoc_set_velum(tVoc* const v, Lfloat velum)
{
	v->tr->velum_target = velum;
}

void tVoc_setTurbulenceNoiseGain(tVoc* const v, Lfloat gain)
{
	v->tr->TnoiseGain = gain;
}

void tVoc_setAspirationNoiseGain(tVoc* const v, Lfloat gain)
{
	v->tr->AnoiseGain = gain;
}

Lfloat * tVoc_get_velum_ptr(tVoc* const v)
{
	return &v->tr->velum_target;
}

//0-1
void tVoc_setAspirationNoiseFilterFreq(tVoc* const v, Lfloat freq)
{
	tSVF_setFreqFast(v->tr->aspirationNoiseFilt,freq*30.0f + 60.0f);
}

void tVoc_setAspirationNoiseFilterQ(tVoc* const v, Lfloat Q)
{
	tSVF_setQ(v->tr->aspirationNoiseFilt,Q*0.49f + 0.5f);
}


//diameter and index are 0-1.0f
void tVoc_set_tongue_and_touch_diameters(tVoc* const v, Lfloat tongue_index, Lfloat tongue_diameter, Lfloat touch_index, Lfloat touch_diameter, Lfloat *theDiameters)
{
	int i;
	Lfloat t;
	Lfloat curve;
	//do tongue position
	Lfloat alpha = 1.0f / (Lfloat)(v->tr->tip_start - v->tr->blade_start);

	for (i = 0; i < v->tr->n; i++)
	{
		theDiameters[i] = v->tr->rest_diameter[i];
	}
	for(i = v->tr->blade_start; i < v->tr->lip_start; i++)
	{
		t = 1.1f * M_PI * (Lfloat)(tongue_index - i) * alpha;
		Lfloat fixedTongueDiameter = 2.0f + ((tongue_diameter - 2.0f) * 0.66666666666667f);
#ifdef ARM_MATH_CM7
		curve = (1.5f - fixedTongueDiameter + 1.7f) * arm_cos_f32(t);
#else
		curve = (1.5f - fixedTongueDiameter + 1.7f) * cosf(t);
#endif
		if(i == v->tr->blade_start - 2 || i == v->tr->lip_start - 1)
		{
			curve *= 0.8f;
		}
		if(i == v->tr->blade_start || i == v->tr->lip_start - 2)
		{
			curve *= 0.94f;
		}
		theDiameters[i] = 1.5f - curve;
	}

	//now do additional constrictions (touch position)
	/*
	Lfloat width=2.0f;

	Lfloat twentyfivewidth = 25.0f * v->tr->invN;
	Lfloat tenwidth = 10.0f * v->tr->invN;
	Lfloat fivewidth = 5.0f * v->tr->invN;
	Lfloat normalizedTipStart = v->tr->tip_start*v->tr->invN;
	if (touch_index< (twentyfivewidth)) width = tenwidth; //25 in original code
	else if (touch_index>=normalizedTipStart) width= fivewidth;

	else width = tenwidth-((fivewidth*(touch_index-twentyfivewidth))/(normalizedTipStart-twentyfivewidth));

	Lfloat invWidth = 1.0f / width;
	*/

	Lfloat width=2.0f;
		Lfloat tenwidth = v->tr->n*0.227272727272727f;
		Lfloat fivewidth = v->tr->n*0.113636363636364f;
		Lfloat twentyfivewidth = v->tr->n *0.568181818181818f;
		if (touch_index< (twentyfivewidth)) width = tenwidth; //25 in original code
		else if (touch_index>=v->tr->tip_start) width= fivewidth;
		else width = tenwidth-fivewidth*(touch_index-twentyfivewidth)/(v->tr->tip_start-twentyfivewidth);
		if ((width <= 0.01f) && (width >= -0.01f))
		{
			width = 1.0f;
		}
		Lfloat invWidth = 1.0f / width;
	if ((touch_index < v->tr->n) && (touch_diameter < 3.0f))
	{
		int intIndex = roundf(touch_index);
		for (int i= -ceilf(width)-1.0f; i<width+1.0f; i++)
		{
			if (intIndex+i<0 || intIndex+i>=v->tr->n) continue;
			Lfloat relpos = (intIndex+i) - touch_index;
			relpos = fabsf(relpos)-0.5f;
			Lfloat shrink;
			if (relpos <= 0.0f) shrink = 0.0f;
			else if (relpos > width) shrink = 1.0f;
#ifdef ARM_MATH_CM7
			else shrink = 0.5f*(1.0f-arm_cos_f32(M_PI * relpos * invWidth));
#else
			else shrink = 0.5f*(1.0f-cosf(M_PI * relpos * invWidth));
#endif
			if (touch_diameter < theDiameters[intIndex+i])
			{
				theDiameters[intIndex+i] = touch_diameter + (theDiameters[intIndex+i]-touch_diameter)*shrink;
			}
		}
	}

}


int tVoc_get_counter(tVoc* const v)
{
	return v->counter;
}

void tVoc_rescaleDiameter(tVoc* const v, Lfloat scale)
{
	v->tr->diameterScale = scale;
}



