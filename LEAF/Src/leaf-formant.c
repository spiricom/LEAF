/*==============================================================================

    leaf-formant.c
    Created: 30 Nov 2018 11:03:30am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-formant.h"
#else

#include "../Inc/leaf-formant.h"

#endif


void tFormantShifter_init(tFormantShifter* const fs)
{
    fs->ford = FORD;
    fs->falph = powf(0.001f, 80.0f / (leaf.sampleRate));
    fs->flamb = -(0.8517f*sqrt(atanf(0.06583f*leaf.sampleRate))-0.1916f);
    fs->fhp = 0.0f;
    fs->flp = 0.0f;
    fs->flpa = powf(0.001f, 10.0f / (leaf.sampleRate));
    fs->fmute = 1.0f;
    fs->fmutealph = powf(0.001f, 1.0f / (leaf.sampleRate));
    fs->cbi = 0;
}

void tFormantShifter_free(tFormantShifter* const fs)
{
    leaf_free(fs);
}


float tFormantShifter_tick(tFormantShifter* fs, float in, float fwarp)
{
    return tFormantShifter_add(fs, tFormantShifter_remove(fs, in), fwarp);
}

float tFormantShifter_remove(tFormantShifter* fs, float in)
{
    float fa, fb, fc, foma, falph, ford, flpa, flamb, tf, fk, tf2, f0resp, f1resp, frlamb;
    int outindex = 0;
    int ti4;
    ford = fs->ford;
    falph = fs->falph;
    foma = (1.0f - falph);
    flpa = fs->flpa;
    flamb = fs->flamb;
    
    tf = in;
    ti4 = fs->cbi;
    
    fa = tf - fs->fhp;
    fs->fhp = tf;
    fb = fa;
    for(int i = 0; i < ford; i++)
    {
        fs->fsig[i] = fa*fa*foma + fs->fsig[i]*falph;
        fc = (fb - fs->fc[i])*flamb + fs->fb[i];
        fs->fc[i] = fc;
        fs->fb[i] = fb;
        fk = fa*fc*foma + fs->fk[i]*falph;
        fs->fk[i] = fk;
        tf = fk/(fs->fsig[i] + 0.000001f);
        tf = tf*foma + fs->fsmooth[i]*falph;
        fs->fsmooth[i] = tf;
        fs->fbuff[i][ti4] = tf;
        fb = fc - tf*fa;
        fa = fa - tf*fc;
    }
    fs->cbi++;
    if(fs->cbi >= FORMANT_BUFFER_SIZE)
    {
        fs->cbi = 0;
    }
    
    return fa;
}

float tFormantShifter_add(tFormantShifter* fs, float in, float fwarp)
{
    float fa, fb, fc, foma, falph, ford, flpa, flamb, tf, fk, tf2, f0resp, f1resp, frlamb;
    int outindex = 0;
    int ti4;
    ford = fs->ford;
    falph = fs->falph;
    foma = (1.0f - falph);
    flpa = fs->flpa;
    flamb = fs->flamb;
    tf = exp2(fwarp/2.0f) * (1+flamb)/(1-flamb);
    frlamb = (tf-1)/(tf+1);
    ti4 = fs->cbi;
    
    tf2 = in;
    fa = 0;
    fb = fa;
    for (int i=0; i<ford; i++)
    {
        fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
        tf = fs->fbuff[i][ti4];
        fb = fc - tf*fa;
        fs->ftvec[i] = tf*fc;
        fa = fa - fs->ftvec[i];
    }
    tf = -fa;
    for (int i=ford-1; i>=0; i--)
    {
        tf = tf + fs->ftvec[i];
    }
    f0resp = tf;
    
    //  second time: compute 1-response
    fa = 1;
    fb = fa;
    for (int i=0; i<ford; i++)
    {
        fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
        tf = fs->fbuff[i][ti4];
        fb = fc - tf*fa;
        fs->ftvec[i] = tf*fc;
        fa = fa - fs->ftvec[i];
    }
    tf = -fa;
    for (int i=ford-1; i>=0; i--)
    {
        tf = tf + fs->ftvec[i];
    }
    f1resp = tf;
    
    //  now solve equations for output, based on 0-response and 1-response
    tf = (float)2*tf2;
    tf2 = tf;
    tf = ((float)1 - f1resp + f0resp);
    if (tf!=0)
    {
        tf2 = (tf2 + f0resp) / tf;
    }
    else
    {
        tf2 = 0;
    }
    
    //  third time: update delay registers
    fa = tf2;
    fb = fa;
    for (int i=0; i<ford; i++)
    {
        fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
        fs->frc[i] = fc;
        fs->frb[i] = fb;
        tf = fs->fbuff[i][ti4];
        fb = fc - tf*fa;
        fa = fa - tf*fc;
    }
    tf = tf2;
    tf = tf + flpa * fs->flp;  // lowpass post-emphasis filter
    fs->flp = tf;
    
    // Bring up the gain slowly when formant correction goes from disabled
    // to enabled, while things stabilize.
    if (fs->fmute>0.5)
    {
        tf = tf*(fs->fmute - 0.5)*2;
    }
    else
    {
        tf = 0;
    }
    tf2 = fs->fmutealph;
    fs->fmute = (1-tf2) + tf2*fs->fmute;
    // now tf is signal output
    // ...and we're done messing with formants
    
    return tf;
}
