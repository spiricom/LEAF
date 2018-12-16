/*
  ==============================================================================

    leaf-wavefolder.cpp
    Created: 30 Nov 2018 11:56:49am
    Author:  airship

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-wavefolder.h"

#else

#include "../Inc/leaf-wavefolder.h"

#endif

void tLockhartWavefolderInit(tLockhartWavefolder* const w)
{
    w->Ln1 = 0.0;
    w->Fn1 = 0.0;
    w->xn1 = 0.0;
}

double tLockhartWavefolderLambert(double x, double ln)
{
    double w = ln;
    double expw, p, r, s, err, q;
    
    for (int i = 0; i < 1000; i++)
    {
        //err = x * (logf(x) - logf(w));
        
        /*
         r = logf(x / w) - w;
         q = 2.0f * (1.0f+w) * (1.0f + w + 0.666666666f*r);
         err = (r / (1.0f+w)) * ((q - r) / (q - 2*r));
         */
        expw = exp(w);
        
        p = (w * expw) - x;
        r = (w + 1.0) * expw;
        s = (w + 2.0) / (2.0 * (w + 1.0));
        err = (p/(r-(p*s)));
        
        if (fabs(err) < THRESH) break;
        
        w -= err;
        //w = w * (1 + err);
        //w = w - err;
    }
    
    return w;
}

float tLockhartWavefolderTick(tLockhartWavefolder* const w, float samp)
{
    int l;
    double u, Ln, Fn, xn;
    float o;
    
    samp=(double)samp;
    // Compute Antiderivative
    if (samp > 0) l = 1;
    else if (samp < 0) l = -1;
    else l = 0;
    
    u = LOCKHART_D * exp(l*LOCKHART_B*samp);
    Ln = tLockhartWavefolderLambert(u, w->Ln1);
    Fn = (0.5 * VT_DIV_B) * (Ln*(Ln + 2.0)) - 0.5*LOCKHART_A*samp*samp;
    
    // Check for ill-conditioning
    if (fabs(samp - w->xn1) < ILL_THRESH)
    {
        // Compute Averaged Wavefolder Output
        xn = 0.5*(samp + w->xn1);
        u = LOCKHART_D*exp(l*LOCKHART_B*xn);
        Ln = tLockhartWavefolderLambert(u, w->Ln1);
        o = (float)((l*LOCKHART_VT*Ln) - (LOCKHART_A*xn));
    }
    else
    {
        // Apply AA Form
        o = (float)((Fn - w->Fn1)/(samp - w->xn1));
    }
    
    /*
     // Check for ill-conditioning
     if (abs(in1-xn1)<thresh) {
     
     // Compute Averaged Wavefolder Output
     xn = 0.5*(in1+xn1);
     u = d*pow(e,l*b*xn);
     Ln = Lambert_W(u,Ln1);
     out1 = l*VT*Ln - a*xn;
     
     }
     else {
     
     // Apply AA Form
     out1 = (Fn-Fn1)/(in1-xn1);
     
     }
     */
    
    // Update States
    w->Ln1 = Ln;
    w->Fn1 = Fn;
    w->xn1 = samp;
    
    return o;
}
