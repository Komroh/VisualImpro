#include "CoeffInterCorrel.hpp"
#include <iostream>

using namespace std;
extern "C"{
  vector<float> CoeffInterCorrel(const vector<float>& s1, const vector<float>& s2)
  {
      int tau;
      int sz = max(s1.size(), s2.size());
      vector<float> correl(sz);

      for(tau=0; tau<sz; tau++) {
          correl[tau] = 0.0;

          for (size_t i = 0; i < sz-tau; i++) {
              correl[tau] += s1[i] * s2[i + tau];
          }
      }
      return  correl;
  }
}
