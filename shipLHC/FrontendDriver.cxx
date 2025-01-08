#include "FrontendDriver.h"

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;


FrontedDriver::FrontedDriver() {}

void FrontedDriver::ADCConversion(std::vector<AdvSignal> ResponseSignal)
{
    for (int k = 0; k < ResponseSignal.size(); k++)
    {
        std::vector<Double_t> charge = ResponseSignal[k].getIntegratedSignal();
        cout << charge[0] << endl; 

    }
    
}