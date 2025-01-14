#include "StripNoise.h"
#include "SiDigiParameters.h"
#include "AdvSignal.h"
#include "TRandom.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
using namespace std;

StripNoise::StripNoise() {}

AdvSignal StripNoise::AddGaussianNoise(AdvSignal Signal)
{
    std::vector<Int_t> Strips = Signal.getStrips();
    Int_t StripsSize = Strips.size();
    std::vector<Double_t> Amplitude = Signal.getIntegratedSignal();

    gRandom->SetSeed(0);
    TRandom* rndm = static_cast<TRandom*>(gRandom->Clone());
    for (int i = 0; i < StripsSize; i++)
    {
        Double_t x = rndm->Gaus(0, stripsensor::frontend::NoiseRMS);
        Amplitude[i] += x ; 
    }
    AdvSignal NoiseSignal(Strips, Amplitude); 
    return NoiseSignal; 
}

void TestingGaussianNoise()
{
    std::vector<Int_t> Strips = {100, 101, 102}; 
    std::vector<Double_t> IntegratedSignal = {200, 600, 50};
    
    AdvSignal TestSignal(Strips, IntegratedSignal); 

    StripNoise stripnoise; 
    stripnoise.AddGaussianNoise(TestSignal);
}