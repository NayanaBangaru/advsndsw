#include "InducedCharge.h"
#include "SurfaceSignal.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TRandom.h"
#include "SiSensor.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std; 

InducedCharge::InducedCharge() {}

void InducedCharge::IntegrateCharge(vector<Int_t> detID, SurfaceSignal DiffusionSignal)
{   
    std::vector<Double_t> amplitude = DiffusionSignal.getAmplitude(); 
    std::vector<TVector3> surfacepos = DiffusionSignal.getSurfacePos();
    std::vector<Double_t> diffusionarea = DiffusionSignal.getDiffusionArea();

    std::vector<Int_t> AffectedStrips; 
    std::vector<Double_t> ChargeDeposited; 

    Int_t N ;
    Double_t x_start; 
    Double_t x_end; 
    Double_t z_start; 
    Double_t z_end;  
    Double_t integratedcharge; 
    ofstream myfile;
            

    for(int i = 0; i < surfacepos.size(); i++)
    {
        AffectedStrips = GetStrips(surfacepos[i], diffusionarea[i]);

        for(int j = 0; j < AffectedStrips.size(); j++)
        {
            x_start = (AffectedStrips[j] - (advsnd::strips / 2))*(advsnd::sensor_width / advsnd::strips) - (strip_pitch / 2); // check calculation
            x_end = (AffectedStrips[j] - (advsnd::strips / 2))*(advsnd::sensor_width / advsnd::strips) + (strip_pitch / 2); // check calculation 
            z_start = (x_start - surfacepos[i].X()) / diffusionarea[i];
            z_end = abs(x_end - surfacepos[i].X()) / diffusionarea[i];
            integratedcharge = (erf((z_end) / TMath::Sqrt2()) / 2) - (erf((z_start) / TMath::Sqrt2()) / 2);
            ChargeDeposited.push_back(integratedcharge);
            
            myfile.open("test.txt", std::ios_base::app);
            myfile << AffectedStrips.size() << "\t" << AffectedStrips[j] << "\t" << surfacepos[i].X() << "\t" << x_start << "\t" << x_end << "\t" << z_start << "\t" << z_end << "\t" << AffectedStrips[j] << "\t" << ChargeDeposited[j] << endl;
            
        }
    }
    myfile.close();
}

std::vector<Int_t> InducedCharge::GetStrips(TVector3 point, Double_t area)
{
    std::vector<Int_t> affectedstrips;

    int fromstrip = floor(((point.X()-(NSigma*area)) / (advsnd::sensor_width / advsnd::strips)) + (advsnd::strips / 2)); // check calculation 
    fromstrip = max(0, fromstrip);
    fromstrip = min(advsnd::strips - 1, fromstrip); 

    int tostrip = floor(((point.X()+(NSigma*area)) / (advsnd::sensor_width / advsnd::strips)) + (advsnd::strips / 2));
    tostrip = max(0, tostrip);
    tostrip = min(advsnd::strips - 1, tostrip);

    Int_t N; 
    N = tostrip - fromstrip; 

    for (int i = 0 ; i <= N ; i++)
    {
        affectedstrips.push_back(fromstrip + i);
    }
    return affectedstrips; 
}