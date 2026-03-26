#include "InducedCharge.h"
#include "SurfaceSignal.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TRandom.h"
#include "SiSensor.h"
#include "AdvSignal.h"
#include "SiDigiParameters.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>
#include <filesystem>
#include <random>
#include <map>

// Class for calculating the induced charge on the strips 

InducedCharge::InducedCharge() {}

void InducedCharge::IntegrateCharge(std::vector<SurfaceSignal>& DiffusionSignal, AdvSignal& TotalSignal)
{    
    std::vector<AdvSignal> ResponseSignal; 
    for (int k = 0; k < DiffusionSignal.size(); k++)
    {
        std::vector<Double_t> amplitude = DiffusionSignal[k].getAmplitude(); 
        std::vector<TVector3> surfacepos = DiffusionSignal[k].getSurfacePos();
        std::vector<Double_t> diffusionarea = DiffusionSignal[k].getDiffusionArea();

        std::vector<Int_t> AffectedStrips;
        std::vector<Int_t> temp_AffectedStrips; 
        std::vector<Int_t> UniqueAffectedStrips;  
        std::vector<Double_t> ChargeDeposited; 
        std::vector<Double_t> TotalChargeDeposited;

        Double_t x_start; 
        Double_t x_end; 
        Double_t z_start; 
        Double_t z_end;  
        Double_t integratedcharge; 

        Double_t e = 1.6e-19; 

        for (int i = 0; i < surfacepos.size(); i++)
        {

        AffectedStrips = GetStrips(surfacepos[i], diffusionarea[i]);   

            // Calculating the charge deposited by using the error function with the z position of the concerned strip 

            for(int j = 0; j < AffectedStrips.size(); j++)
            {
                x_start = (AffectedStrips[j] * (advsnd::sensor_width/advsnd::strips)) - (advsnd::sensor_width/2);
                x_end = ((AffectedStrips[j]+1) * (advsnd::sensor_width/advsnd::strips)) - (advsnd::sensor_width/2);
                
                z_start = (x_start - surfacepos[i].X()) / diffusionarea[i];
                z_end = (x_end - surfacepos[i].X()) / diffusionarea[i];
                integratedcharge = abs((erf((z_end) / TMath::Sqrt2()) / 2) - (erf((z_start) / TMath::Sqrt2()) / 2));
                temp_AffectedStrips.push_back(AffectedStrips[j]);
                ChargeDeposited.push_back(integratedcharge*amplitude[i]);   
            }

        }


        Double_t z = accumulate(amplitude.begin(), amplitude.end(), 0.0);
        // Getting the unique strips 

        std::vector<AdvSignal> temp_Signalvector;
        AdvSignal temp_signal(temp_AffectedStrips, ChargeDeposited); 
        temp_Signalvector.push_back(temp_signal);
        AdvSignal TotalSignalUnscale = Combine(temp_Signalvector);

        TotalChargeDeposited = TotalSignalUnscale.getIntegratedSignal();
        UniqueAffectedStrips = TotalSignalUnscale.getStrips();

        Double_t r = accumulate(TotalChargeDeposited.begin(), TotalChargeDeposited.end(), 0.0);
        Double_t rescale_ratio = z/r;
        for (int m = 0; m < UniqueAffectedStrips.size(); m++)
        {
            //the total number of electrons
            TotalChargeDeposited[m] = std::ceil((TotalChargeDeposited[m]) * rescale_ratio) ;
        }
        // Add coupling to neighbour strips  

        if (stripsensor::inducedcharge::Coupling)
        {
            AdvSignal CoupledChargeDeposit = Coupling(TotalChargeDeposited, UniqueAffectedStrips);
            ResponseSignal.push_back(CoupledChargeDeposit); 

        } else {
            AdvSignal PulseSignal(UniqueAffectedStrips, TotalChargeDeposited); 
            ResponseSignal.push_back(PulseSignal);
            }         

    }
    TotalSignal = Combine(ResponseSignal);
    
}

AdvSignal InducedCharge::Combine(std::vector<AdvSignal> Signal)
{

    std::map<Int_t, Double_t> ChargeMap; 

    std::vector<Int_t> CombinedStrips; 
    std::vector<Double_t> CombinedCharge;

    for (int n = 0; n < Signal.size(); ++n)
    {
        CombinedStrips = Signal[n].getStrips(); 
        CombinedCharge = Signal[n].getIntegratedSignal(); 

        for (int l = 0; l < CombinedStrips.size(); l++)
        {
            ChargeMap[CombinedStrips[l]] += CombinedCharge[l]; 
        }
    }

    std::vector<Int_t> UniqueAffectedStrips ;  
    std::vector<Double_t> SummedCharge ;

    for (const auto& entry : ChargeMap)
    {
        UniqueAffectedStrips.push_back(entry.first);
        SummedCharge.push_back(entry.second); 
    }

    return AdvSignal(UniqueAffectedStrips, SummedCharge);

}

std::vector<Int_t> InducedCharge::GetStrips(TVector3 point, Double_t area)
{
    std::vector<Int_t> affectedstrips;
    int fromstrip = floor(((point.X() - (stripsensor::inducedcharge::NSigma*area)) + (advsnd::sensor_width/2))/(advsnd::sensor_width/advsnd::strips));
    fromstrip = std::max(0, fromstrip);
    fromstrip = std::min(advsnd::strips - 1, fromstrip); 

    int tostrip = floor(((point.X() + (stripsensor::inducedcharge::NSigma*area)) + (advsnd::sensor_width/2))/(advsnd::sensor_width/advsnd::strips));
    tostrip = std::max(0, tostrip);
    tostrip = std::min(advsnd::strips - 1, tostrip);

    Int_t N; 
    N = tostrip - fromstrip; 

    for (int i = 0 ; i <= N ; i++)
    {
        affectedstrips.push_back(fromstrip + i);
    }

    return affectedstrips; 
}

std::vector<std::vector<Double_t>> InducedCharge::GetPulseShape(std::string PulseFileName, std::vector<Double_t> ChargeDeposited)
{
    // For pulse response for time correction, not used currently

    std::vector<double> PulseValues;

    std::ifstream inputFile(PulseFileName);

    if (!inputFile.is_open()) {
        std::cout << "Error opening the file!" << std::endl;
    }
    std::string line;
    std::string res_find = "resolution:";
    float res;
    std::string s;

    while (getline(inputFile, line)) {
        if ((!line.empty()) && (line.substr(0, 1) != "#")) {
            std::stringstream ss(line);
            if (line.find(res_find) != std::string::npos) {
                res = stof(line.substr(line.find(res_find) + res_find.length()));   // implement check for
                                                                                    // resolution
            } else {
                std::string value;
                while (getline(ss, value, ' ')) {
                    PulseValues.push_back(stod(value));
                }
            }
        }
    }
    const auto max_value = max_element(PulseValues.begin(), PulseValues.end());
    if (abs(*max_value - 1) > std::numeric_limits<Double_t>::epsilon()) {
        throw std::invalid_argument("Maximum value of pulse shape not 1.");
    }

    std::vector<std::vector<Double_t>> PulseResponse; 
    std::vector<Double_t> temp_response ;
    Double_t response_value; 

    Int_t sampling_step = stripsensor::sampling / res; 

    for(int i = 0; i < ChargeDeposited.size(); i++)
    {
        Double_t amplitude_max = ChargeDeposited[i]; 

        std::default_random_engine generator;
        std::normal_distribution<double> dist(stripsensor::noise_mean, stripsensor::noise_std_dev);
        response_value = ((stripsensor::baseline + (amplitude_max * stripsensor::amplificaton_factor)) > stripsensor::rail ? stripsensor::rail : (stripsensor::baseline + (amplitude_max * stripsensor::amplificaton_factor)));
        temp_response.push_back(response_value + dist(generator));
        PulseResponse.push_back(temp_response);
    } 
   
    return PulseResponse; 
    // get the vector of beginning to max of the pulse
    // time response not included!
}

AdvSignal InducedCharge::Coupling(std::vector<Double_t> TotalCharge, std::vector<Int_t> AffectedStrips)
{

    // Using the coupling constants, charge sharing between strips and their neighbours
    
    Int_t couplingsize = stripsensor::inducedcharge::CouplingConstants.size();
    std::vector<Double_t> CoupledCharge; 
    for (int k = 1; k < couplingsize; k++){
        AffectedStrips.emplace(AffectedStrips.begin(), AffectedStrips[k-1]- k); 
        AffectedStrips.push_back(AffectedStrips[AffectedStrips.size()-k] + k);
        TotalCharge.emplace(TotalCharge.begin(), 0); 
        TotalCharge.push_back(0);

    }
    
    CoupledCharge = TotalCharge; 
    for(int n = 0; n < TotalCharge.size(); n++)
    {
        CoupledCharge[n] = TotalCharge[n]*stripsensor::inducedcharge::CouplingConstants[0];
    }

    for (int i = 0; i < TotalCharge.size(); i++)
    {
        if (TotalCharge[i] == 0)
            continue; 
        for (int j = 1; j < couplingsize; j++)
        {
            CoupledCharge[i-j] += TotalCharge[i]*stripsensor::inducedcharge::CouplingConstants[j]*0.5;
            CoupledCharge[i+j] += TotalCharge[i]*stripsensor::inducedcharge::CouplingConstants[j]*0.5;
        }
        

    }
    AdvSignal Coupledresult(AffectedStrips, CoupledCharge);
    return Coupledresult; 
}