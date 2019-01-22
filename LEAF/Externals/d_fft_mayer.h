/*
 * d_fft_mayer.h
 *
 * Created: 5/4/2017 10:33:59 PM
 *  Author: Kenny
 */ 


#ifndef D_FFT_MAYER_H_
#define D_FFT_MAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef float t_sample;       /* a float type at most the same size */

#define REAL t_sample

void mayer_fht(REAL *fz, int n);
void mayer_fft(int n, REAL *real, REAL *imag);
void mayer_ifft(int n, REAL *real, REAL *imag);
void mayer_realfft(int n, REAL *real);
void mayer_realifft(int n, REAL *real);


#ifdef __cplusplus
}
#endif

#endif /* D_FFT_MAYER_H_ */
