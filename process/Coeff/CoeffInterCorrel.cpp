#include "CoeffInterCorrel.hpp"
#include <iostream>
#include <fftw3.h>
#include <complex>
#include <algorithm>

using namespace std;
fftw_plan plan;
fftw_plan iplan;

// IFFT
void
ifft_init (complex<double>* in, complex<double>* out, int size)
{
    iplan = fftw_plan_dft_1d (size, reinterpret_cast<fftw_complex *>(in), reinterpret_cast<fftw_complex *>(out), FFTW_BACKWARD, FFTW_ESTIMATE);
}

void
ifft_exit (void)
{
    fftw_destroy_plan (iplan);
}

void
ifft_process (void)
{
    fftw_execute (iplan);
}

// FFT
void
fft_init (complex<double>* in, complex<double>* out, int size)
{
    plan = fftw_plan_dft_1d (size, reinterpret_cast<fftw_complex *>(in), reinterpret_cast<fftw_complex *>(out), FFTW_FORWARD, FFTW_ESTIMATE);
}

void
fft_exit (void)
{
    fftw_destroy_plan (plan);
}

void
fft_process (void)
{
    fftw_execute (plan);
}

extern "C"{
  vector<float> CoeffInterCorrel(const vector<float>& s1, const vector<float>& s2)
  {

  int size = s1.size();
/*float min = 1.0;
  float max = 0.0;
  vector<float> s1Norm = s1;
  vector<float> s2Norm = s2;
  min = *min_element(s1.begin(), s1.end());
  max = *max_element(s1.begin(), s1.end());
  for(int i = 0; i< size; i++)
  {
    if ((max-min) < 0.0000001)
    {
      s1Norm[i] = 0.0;
      s2Norm[i] = 0.0;
    }

  else
    {

      s1Norm[i] = 2*((s1[i] - min) / (max - min))-1;
      s2Norm[i] = 2*((s2[i] - min) / (max - min))-1;
    }

    if(s2[i] < 0.0)
      s2Norm[i] = 0.0;
  }*/


  /*  int tau;
      int sz = max(s1.size(), s2.size());
      vector<float> correl(sz);

      for(tau=0; tau<sz; tau++) {
          correl[tau] = 0.0;

          for (size_t i = 0; i < sz; i++) {
              correl[tau] += s1[i] * s2[i - tau];
          }
      }
      return  correl;*/

  vector<float> correl(size);
  complex<double>* in_s1, *in_s2, *out_s1, *out_s2;
  complex<double>* c;
  complex<double>* correlComplex;
  c = (complex<double>*) fftw_malloc(sizeof(complex<double>)*size*2);
  correlComplex = (complex<double>*) fftw_malloc(sizeof(complex<double>)*size*2);
  in_s1 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*size*2);
  out_s1 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*size*2);
  in_s2 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*size*2);
  out_s2 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*size*2);

  for(int i = 0; i< size*2 ; i++)
  {
    in_s1[i] = in_s2[i] = out_s1[i] = out_s2[i] = c[i] = correlComplex[i] = 0;
  }

  for(int i = 0; i<size ;i++)
  {
      if(i<size) {
          in_s1[i].real(s1[i]);
          in_s2[i].real(s2[i]);
      }

  }


  fft_init(in_s1, out_s1, size*2);
  //ifft_init(out_s1, in_s1);


//Compute the FFT of s1
  fft_process();

  fft_init(in_s2,out_s2, size*2);

//Compute the FFT of s2
  fft_process();

//    Compute correlation
  for(int i = 0; i<size*2;i++)
  {

     c[i] = (out_s1[i]*conj(out_s2[i]))/(1.0*size*2) ;

  }
  ifft_init(c, correlComplex, size*2);
  ifft_process();

//    Take real part of the correlation to return
  for(int i = 0; i<size;i++) {
      correl[i] = real(correlComplex[i]);
      cout << correl[i] << endl;
  }

  fft_exit();
  ifft_exit();
  fftw_free(in_s1); fftw_free(in_s2); fftw_free(out_s1); fftw_free(out_s2);
  fftw_free(c); fftw_free(correlComplex);
  return correl;
  }
}
