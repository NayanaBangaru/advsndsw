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
    AdvSignal AddGaussianNoise(AdvSignal Signal);

    void TestingGaussianNoise();

};

#endif  