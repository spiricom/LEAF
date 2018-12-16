#ifndef atkdetect_h
#define atkdetect_h

/*************************************

atkdetect lib written for DSP2G project
Original author: Kenny Carlsen (kcarlsen@umich.edu)

Source: http://www.musicdsp.org/showArchiveComment.php?ArchiveID=136

Implements a simple attack detector using envelope detection
Designed to be fed into the solad library used in DPS2G for
the purpose of reducing latency while time domain pitch shifting.

Function atkDetect_new() is a constructor.  It returns a pointer which is
needed in all other external calls to lib atkdetect functions

Function atkDetect_free() is a destructor.  This function should be called
to deallocate all memory used by atkDetect instance.

Function atkDetect_set_blocksize() changes the analysis block size.  Valid sizes are 128,
256, 512, 1024, and 2048.  Default size is 1024.

Function atkDetect_set_samplerate() changes the sample rate of the attack detector and thus
the attack and release coefficients of the envelope detector. Default rate is 44100

Function atkDetect_set_threshold() sets the RMS level a frame must exceed (over the previous frame)
to be detected as an attack (example: frame 2 is an 'attack' if RMS(frame2) > RMS(frame1)*2)
Default value is 6dB RMS (or a doubling of the amplitude)

Functions atkDetect_set_atk/rel() set the attack/release values in msec and calculate the atk/rel coefficients
for the envelope detector.  Default value is 10 msec.

Function atkDetect_detect() evaluates the RMS level of the input signal and determines if an attack
exists in the given frame

****************************************
version 0.2 June 2017

***************************************/

#include <string.h>
#include <math.h>

#define DEFSAMPLERATE 44100
#define DEFBLOCKSIZE 1024
#define DEFTHRESHOLD 6
#define DEFATTACK	10
#define DEFRELEASE	10

#define t_float float


typedef struct t_atkDetect
{
	t_float env;

	//Attack & Release times in msec
	int atk;
	int rel;

	//Attack & Release coefficients based on times
	t_float atk_coeff;
	t_float rel_coeff;

	int blocksize;
	int samplerate;

	//RMS amplitude of previous block - used to decide if attack is present
	t_float prevAmp;

	t_float threshold;
}t_atkDetect;

t_atkDetect *atkDetect_new(int blocksize);

t_atkDetect *atkDetect_new_expanded(int blocksize, int atk, int rel);

void atkDetect_free(t_atkDetect *a);

// set expected input blocksize
void atkDetect_set_blocksize(t_atkDetect *a, int size);

// change atkDetector sample rate
void atkDetect_set_samplerate(t_atkDetect *a, int inRate);

// set attack time and coeff
void atkDetect_set_atk(t_atkDetect *a, int inAtk);

// set release time and coeff
void atkDetect_set_rel(t_atkDetect *a, int inRel);

// set level above which values are identified as attacks
void atkDetect_set_threshold(t_atkDetect *a, t_float thres);

// find largest transient in input block, return index of attack
int atkDetect_detect(t_atkDetect *a, t_float *in);


#endif
