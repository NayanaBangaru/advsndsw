#ifndef SHIPLHC_FRONTENDDRIVER_H_
#define SHIPLHC_FRONTENDDRIVER_H_

#include "AdvSignal.h"

#include <iostream>
#include <vector>

class FrontedDriver
{
  public:
    FrontedDriver();
    void ADCConversion(std::vector<AdvSignal> ResponseSignal);

};

#endif  