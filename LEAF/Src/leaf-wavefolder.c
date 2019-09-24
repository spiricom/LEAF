/*==============================================================================

    leaf-wavefolder.c
    Created: 30 Nov 2018 11:56:49am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-wavefolder.h"

#else

#include "../Inc/leaf-wavefolder.h"

#endif

void tLockhartWavefolder_init(tLockhartWavefolder* const w)
{
    w->Ln1 = 0.0;
    w->Fn1 = 0.0;
    w->xn1 = 0.0f;
}

double tLockhartWavefolderLambert(double x, double ln)
{
    double thresh, w, expw, p, r, s, err;
    // Error threshold
    thresh = 10e-6;
    // Initial guess (use previous value)
    w = ln;
    
    // Haley's method (Sec. 4.2 of the paper)
    for(int i=0; i<100; i+=1) {
        
        expw = exp(w);
        
        p = w*expw - x;
        r = (w+1.0)*expw;
        s = (w+2.0)/(2.0*(w+1.0));        err = (p/(r-(p*s)));
        
        if (fabs(err)<thresh) {
            break;
        }
        if (isnan(err))
        {
            break;
        }
        
        w = w - err;
    }
    return w;
}

float tLockhartWavefolder_tick(tLockhartWavefolder* const w, float samp)
{
    
    float out = 0.0f;
    // Constants
    double RL = 7.5e3;
    double R = 15e3;
    double VT = 26e-3;
    double Is = 10e-16;
    
    double a = 2.0*RL/R;
    double b = (R+2.0*RL)/(VT*R);
    double d = (RL*Is)/VT;
    
    // Antialiasing error threshold
    double thresh = 10e-10;
    
    // Compute Antiderivative
    int l = (samp > 0) - (samp < 0);
    double u = d*exp(l*b*samp);
    double Ln = tLockhartWavefolderLambert(u,w->Ln1);
    double Fn = (0.5*VT/b)*(Ln*(Ln + 2.0)) - 0.5*a*samp*samp;
    
    // Check for ill-conditioning
    if (fabs(samp-w->xn1)<thresh) {
        
        // Compute Averaged Wavefolder Output
        double xn = 0.5*(samp+w->xn1);
        u = d*exp(l*b*xn);
        Ln = tLockhartWavefolderLambert(u,w->Ln1);
        out = (float) (l*VT*Ln - a*xn);
        if (isnan(out))
        {
            ;
        }
        
    }
    else {
        
        // Apply AA Form
        out = (float) ((Fn-w->Fn1)/(samp-w->xn1));
        if (isnan(out))
        {
            ;
        }
    }
    
    // Update States
    w->Ln1 = Ln;
    w->Fn1 = Fn;
    w->xn1 = samp;
    
    return out;
}
