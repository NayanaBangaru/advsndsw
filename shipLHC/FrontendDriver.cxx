#include "FrontendDriver.h"
#include "SiDigiParameters.h"

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;


FrontedDriver::FrontedDriver() {}

void FrontedDriver::ADCConversion(std::vector<AdvSignal> ResponseSignal)
{
    for (int k = 0; k < ResponseSignal.size(); k++)
    {
        std::vector<Double_t> NumberofElectrons = ResponseSignal[k].getIntegratedSignal();
        for (int i = 0; i < NumberofElectrons.size(); i++)
        {
            ADCcount.push_back(std::ceil(NumberofElectrons[i]/stripsensor::frontend::ElectronperADC));
        }

    }
    
}