#include "InducedCharge.h"
#include "SurfaceSignal.h"
#include "TVector3.h"
#include "TGraph.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace std; 

InducedCharge::InducedCharge() {}

void InducedCharge::VectoriseStrips(SurfaceSignal DiffusionSignal)
{   
    std::vector<Double_t> amplitude = DiffusionSignal.getAmplitude(); 


}