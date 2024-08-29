#ifndef SHIPLHC_INDUCEDCHARGE_H_
#define SHIPLHC_INDUCEDCHARGE_H_

#include "EnergyFluctUnit.h"
#include "SurfaceSignal.h"

#include <iostream>
#include <vector>

class InducedCharge
{
  public:
    InducedCharge();
    void VectoriseStrips(SurfaceSignal DiffusionSignal);

};

#endif  