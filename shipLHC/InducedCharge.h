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
    void IntegrateCharge(std::vector<Int_t> detID, SurfaceSignal DiffusionSignal);
    std::vector<Int_t> GetStrips(TVector3 point, Double_t area);
  
  protected:
    Int_t NSigma = 3; 
    Double_t strip_width = 30e-4; //in cm  
    Double_t strip_pitch = 120e-4; //in cm 

};

#endif  