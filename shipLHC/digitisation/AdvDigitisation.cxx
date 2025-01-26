#include "AdvDigitisation.h"
#include "AdvTargetPoint.h"
#include "ChargeDivision.h"
#include "ChargeDrift.h"
#include "InducedCharge.h"
#include "FrontendDriver.h"
#include "StripNoise.h"
#include "EnergyFluctUnit.h"
#include "SurfaceSignal.h"
#include "AdvSignal.h"
#include "SiDigiParameters.h"

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "TStopwatch.h"

#include <TSystem.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
using namespace std;

// Running the digitisation 

/* To be included :
    Saturation of FED dynamic range depending on mode 
    Add noise
    Add FED modes */

AdvDigitisation::AdvDigitisation() {}

std::map<std::string, std::vector<Int_t>> AdvDigitisation::digirunoutput(Int_t detID, const std::vector<AdvTargetPoint *> &V)
{
    // Charge Division
    ChargeDivision chargedivision{};
    std::vector<EnergyFluctUnit> EnergyLossVector = chargedivision.Divide(detID, V);

    //Charge Drift
    ChargeDrift chargedrift{};
    std::vector<SurfaceSignal> DiffusionSignal = chargedrift.Drift(EnergyLossVector);

    //Induced Charge on strips
    InducedCharge inducedcharge{};
    AdvSignal ResponseSignal = inducedcharge.IntegrateCharge(DiffusionSignal);

    // Int_t num = 0;
    // for (int i = 0; i < V.size(); i ++)
    // {
    //     //cout << V[i]->GetEnergyLoss() << "\t" << V[i]->PdgCode() << "\t" << sqrt(pow(V[i]->GetPx(), 2) + pow(V[i]->GetPy(), 2) + pow(V[i]->GetPz(), 2)) << "\t" << ResponseSignal.getStrips()[k] << "\t" << ResponseSignal.getIntegratedSignal()[k] << endl ;
    //     if ((V[i]->PdgCode() == 13) && (sqrt(pow(V[i]->GetPx(), 2) + pow(V[i]->GetPy(), 2) + pow(V[i]->GetPz(), 2)) > 80))
    //     {
    //         // cout << sqrt(pow(V[i]->GetPx(), 2) + pow(V[i]->GetPy(), 2) + pow(V[i]->GetPz(), 2)) << endl; 
    //         num = 1; 
    //     }
    
    // }

    //FED Response 
    FrontendDriver frontenddriver{};
    AdvSignal FEDResponseSignal = frontenddriver.FEDResponse(ResponseSignal);

    //Creating map of hit 
    std::map<std::string, std::vector<Int_t>> DigitisedHit; 
    std::vector<Double_t> Charge = FEDResponseSignal.getIntegratedSignal();
    std::vector<Int_t> Strips = FEDResponseSignal.getStrips();
    std::vector<Int_t> ADC(Charge.size()); 
    std::transform(Charge.begin(), Charge.end(), ADC.begin(), [](Double_t x) { 
        if (x>0){return (int)x;}
        else {return 0;}
        });
    // if (num == 1)
    // {
    //     plotclustersize(Strips);
    //     cout << V.size() << "\t" << sqrt(pow(V[0]->GetPx(), 2) + pow(V[0]->GetPy(), 2) + pow(V[0]->GetPz(), 2)) << endl; 
    // }
    
    plotstrips(ADC); 

    DigitisedHit["Strips"] = Strips; 
    DigitisedHit["ADC"] = ADC; 
    
    return DigitisedHit;
}

void AdvDigitisation::plotclustersize(std::vector<Int_t> Strips)
{
    ofstream rawdatafile;
    rawdatafile.open("to_plot.txt", std::ios_base::app);
    rawdatafile << Strips.size() << endl; 
    rawdatafile.close();
}

void AdvDigitisation::plotZSevent()
{
    ofstream rawdatafile;
    rawdatafile.open("to_plot.txt", std::ios_base::app);
    std::vector<Int_t> Strips = {665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 675, 676, 677};
    std::vector<Double_t> Charge = {36, 75, 83, 82, 82, 89, 88, 88, 82, 90, 89, 74, 7};
    AdvSignal testsignal(Strips, Charge);
    
    FrontendDriver frontenddriver{};
    AdvSignal testresponse = frontenddriver.FEDResponse(testsignal);

    for (int k = 0; k < (testresponse.getStrips()).size(); k++)
    {
        rawdatafile << testresponse.getStrips()[k] << "\t" << testresponse.getIntegratedSignal()[k] << endl; 
    }
    
    rawdatafile.close();
}

void AdvDigitisation::plotstrips(std::vector<Int_t> Charge)
{
    ofstream rawdatafile;
    rawdatafile.open("to_plot.txt", std::ios_base::app);
    for (int i = 0; i < Charge.size(); i++)
    {
        rawdatafile << Charge[i] << endl; 
    }
    
    rawdatafile.close();
}


