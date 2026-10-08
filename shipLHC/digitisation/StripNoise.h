#ifndef SHIPLHC_STRIPNOISE_H_
#define SHIPLHC_STRIPNOISE_H_

#include "StripNoise.h"
#include "AdvSignal.h"

#include <iostream>
#include <vector>

class StripNoise
{
  public:
    StripNoise();

    static std::map<int, int> noise_map;
     
    AdvSignal AddGaussianNoise(AdvSignal Signal);
    AdvSignal AddGaussianTailNoise(AdvSignal Signal);
    AdvSignal AddCMNoise(AdvSignal Signal);
    double generate_gaussian_tail(const double a, const double sigma); 


    void CreateNoiseProfile(); 
    void CreatePedestalProfile();

    void TestingNoise();

};

#endif  