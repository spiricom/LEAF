#include "atkdetect.h"

#include <stdlib.h>

/********Private function prototypes**********/
static void atkDetect_init(t_atkDetect *a, int blocksize, int atk, int rel);
static void atkDetect_envelope(t_atkDetect *a, t_float *in);

/********Constructor/Destructor***************/

t_atkDetect *atkDetect_new(int blocksize)
{
	t_atkDetect *a = NULL;
	a = (t_atkDetect*)malloc(sizeof(t_atkDetect));

	atkDetect_init(a, blocksize, DEFATTACK, DEFRELEASE);
	return (a);

}

t_atkDetect *atkDetect_new_expanded(int blocksize, int atk, int rel)
{
	t_atkDetect *a = NULL;
	a = (t_atkDetect*)malloc(sizeof(t_atkDetect));

	atkDetect_init(a, blocksize, atk, rel);
	return (a);

}

void atkDetect_free(t_atkDetect *a)
{
	free(a);
}

/*******Public Functions***********/


void atkDetect_set_blocksize(t_atkDetect *a, int size)
{

	if(!((size==64)|(size==128)|(size==256)|(size==512)|(size==1024)|(size==2048)))
		size = DEFBLOCKSIZE;
	a->blocksize = size;

	return;

}

void atkDetect_set_samplerate(t_atkDetect *a, int inRate)
{
	a->samplerate = inRate;

	//Reset atk and rel to recalculate coeff
	atkDetect_set_atk(a, a->atk);
	atkDetect_set_rel(a, a->rel);

	return;
}

void atkDetect_set_threshold(t_atkDetect *a, t_float thres)
{
	a->threshold = thres;
	return;
}

void atkDetect_set_atk(t_atkDetect *a, int inAtk)
{
	a->atk = inAtk;
	a->atk_coeff = pow(0.01, 1.0/(a->atk * a->samplerate * 0.001));

	return;
}

void atkDetect_set_rel(t_atkDetect *a, int inRel)
{
	a->rel = inRel;
	a->rel_coeff = pow(0.01, 1.0/(a->rel * a->samplerate * 0.001));

	return;
}


int atkDetect_detect(t_atkDetect *a, t_float *in)
{
	int result;

	atkDetect_envelope(a, in);

	if(a->env >= a->prevAmp*2) //2 times greater = 6dB increase
		result = 1;
	else
		result = 0;

	a->prevAmp = a->env;

	return result;
		
}

/*******Private Functions**********/

static void atkDetect_init(t_atkDetect *a, int blocksize, int atk, int rel)
{
	a->env = 0;
	a->blocksize = blocksize;
	a->threshold = DEFTHRESHOLD;
	a->samplerate = DEFSAMPLERATE;
	a->prevAmp = 0;

	a->env = 0;

	atkDetect_set_atk(a, atk);
	atkDetect_set_rel(a, rel);
}

static void atkDetect_envelope(t_atkDetect *a, t_float *in)
{
	int i = 0;
	t_float tmp;
	for(i = 0; i < a->blocksize; ++i){
		tmp = fastabs(in[i]);

		if(tmp > a->env)
			a->env = a->atk_coeff * (a->env - tmp) + tmp;
		else
			a->env = a->rel_coeff * (a->env - tmp) + tmp;
	}

}
