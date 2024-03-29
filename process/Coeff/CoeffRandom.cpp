/**
 *  \file    CoeffRandom.cpp
 *  \author  Alexandre CASANOVA--FRANGER, Gauthier LARMARQUE, Paul SIMORRE,
 *            Lucas VIVAS
 *  \date    March 2018
 *
 *  \brief Code containing the random correlation function.
 *
 *  This is a correlation function used to return a random correlation value.
 *  The volume and the coloration matrix will be affected by this value.
 *
 */

#include "CoeffRandom.hpp"

extern "C"{

float CoeffRandom(const vector<float>& s1, const vector<float>& s2){
  float coeff;
  coeff = rand() /(float)(RAND_MAX);
  if(coeff + std::abs(s1[0]-s2[0])<=1)
    coeff += std::abs(s1[0]-s2[0]);
  return coeff;
}

}
