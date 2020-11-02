#include "CoeffInterCorrel.hpp"
#include <stdio.h>
#include <iostream>
#include <fftw3.h>
#include <complex>
#include <complex.h>
#include <algorithm>
#include <fstream>
using namespace std;
fftw_plan plan1;
fftw_plan plan2;
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
/*void
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
}*/

extern "C"{
  vector<float> CoeffInterCorrel(const vector<float>& s1, const vector<float>& s2)
  {

  int size = s1.size();

  //Normalisation
  float min ;
  float max ;
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
    if(s2[i] >1.0)
      s2Norm[i] = 1.0;
  }


  /*
      // Inter-correlation in time space
      int tau;
      int sz = max(s1.size(), s2.size());
      vector<float> correl(sz);

      for(tau=0; tau<sz; tau++) {
          correl[tau] = 0.0;

          for (size_t i = 0; i < sz; i++) {
              correl[tau] += s1[i] * s2[i - tau];
          }
      }
      return  correl;*/

  vector<float> correl(size*2-1);
  complex<double>* in_s1, *in_s2, *out_s1, *out_s2;
  complex<double>* c;
  complex<double>* correlComplex;
  c = (complex<double>*) fftw_malloc(sizeof(complex<double>)*(size*2-1));
  correlComplex = (complex<double>*) fftw_malloc(sizeof(complex<double>)*(size*2-1));
  in_s1 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*(size*2-1));
  out_s1 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*(size*2-1));
  in_s2 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*(size*2-1));
  out_s2 = (complex<double>*) fftw_malloc(sizeof(complex<double>)*(size*2-1));

  for(int i = 0; i< size*2-1; i++)
  {
    in_s1[i] = in_s2[i] = out_s1[i] = out_s2[i] = c[i] = correlComplex[i] = 0;
  }

  for(int i = 0; i<size-1; i++)
  {
          in_s1[i].real(s1Norm[i]);
          in_s2[size+i-1].real(s2Norm[i]);
  }

  // fft_init(in_s1, out_s1, size*2);
plan1 = fftw_plan_dft_1d (size*2-1 , reinterpret_cast<fftw_complex *>(in_s1), reinterpret_cast<fftw_complex *>(out_s1), FFTW_FORWARD, FFTW_ESTIMATE);
plan2 = fftw_plan_dft_1d (size*2-1 , reinterpret_cast<fftw_complex *>(in_s2), reinterpret_cast<fftw_complex *>(out_s2), FFTW_FORWARD, FFTW_ESTIMATE);
// Compute the FFT of s1
  // fft_process();
  fftw_execute(plan1);
  //fft_init(in_s2,out_s2, size*2);
  fftw_execute(plan2);
// Compute the FFT of s2
  // fft_process();

// Compute correlation
  for(int i = 0; i< size*2-1;i++)
  {
     c[i] = out_s1[i]*conj(out_s2[i]);
  }
  ifft_init(c, correlComplex, size*2-1);
  ifft_process();
// Take real part of the correlation to return
  for(int i = 0; i < size*2-1; i++) {
      correl[i] = real(correlComplex[i])*(1.0/(2*size-1));
  }


  std::ofstream myfilestream;
  myfilestream.open("plot.txt");

  if (!myfilestream.is_open())
  {
      cout << "File openning error " << endl;
  }

  for(int i = 0 ; i < size*2-1 ; i++)
  {
    myfilestream << correl[i] << endl;
  }


  myfilestream.close();
  // fft_exit();
  fftw_destroy_plan(plan1);
  fftw_destroy_plan(plan2);
  ifft_exit();
  fftw_free(in_s1); fftw_free(in_s2); fftw_free(out_s1); fftw_free(out_s2);
  fftw_free(c); fftw_free(correlComplex);
  return correl;
  }
}
