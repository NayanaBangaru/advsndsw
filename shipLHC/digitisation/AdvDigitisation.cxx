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
#include "TNtuple.h"
#include "TObject.h"

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

std::map<std::string, std::vector<Int_t>> AdvDigitisation::digirunoutput(Int_t detID, const std::vector<AdvTargetPoint *> &V, std::vector<EnergyFluctUnit>& EnergyLossVector, std::vector<SurfaceSignal>& DiffusionSignal, AdvSignal& TotalSignal, AdvSignal& FEDResponseSignal)
{
    // Charge Division
    ChargeDivision chargedivision{};
    chargedivision.Divide(detID, V, EnergyLossVector);
     
    // //Charge Drift
    ChargeDrift chargedrift{};
    chargedrift.Drift(EnergyLossVector, DiffusionSignal);

    //Induced Charge on strips
    InducedCharge inducedcharge{};
    inducedcharge.IntegrateCharge(DiffusionSignal, TotalSignal);

    FrontendDriver frontenddriver{};
    frontenddriver.FEDResponse(TotalSignal, FEDResponseSignal);

    //Creating map of hit 
    std::map<std::string, std::vector<Int_t>> DigitisedHit; 
    std::vector<Double_t> Charge = FEDResponseSignal.getIntegratedSignal();
    std::vector<Int_t> Strips = FEDResponseSignal.getStrips();
    std::vector<Int_t> ADC(Charge.size()); 
    std::transform(Charge.begin(), Charge.end(), ADC.begin(), [](Double_t x) { 
        if (x>0){return (int)x;}
        else {return 0;}
        });

    DigitisedHit["Strips"] = Strips; 
    DigitisedHit["ADC"] = ADC;

    return DigitisedHit;
}

void AdvDigitisation::dEdx(const std::vector<AdvTargetPoint *> &V, std::vector<EnergyFluctUnit> EnergyLossVector, Int_t pc, Double_t mom)
{
    ofstream datafile;
    datafile.open("forplot.txt", std::ios_base::app);

    for (int i = 0; i < V.size(); i++)
    {
        if (V[i]->PdgCode() == pc && sqrt(pow(V[i]->GetPx(), 2) + pow(V[i]->GetPy(), 2) + pow(V[i]->GetPz(), 2)) < mom)
        {
            for (int j = 0; j < (EnergyLossVector[i].getEfluct()).size(); j++)
            {
                datafile << (EnergyLossVector[i].getEfluct())[j] / (EnergyLossVector[i].getsegLen()) << "\n";
            }
            
        }
    }
    datafile.close();
}

void AdvDigitisation::stripcharge(std::vector<Int_t> ADC)
{
    ofstream datafile;
    datafile.open("forplot.txt", std::ios_base::app);

    for (int i = 0; i < ADC.size(); i++)
    {
        datafile << ADC[i] << "\n";
    }

    datafile.close();
}

void AdvDigitisation::numberofstrips(const std::vector<AdvTargetPoint *> &V, std::vector<Int_t> Strips, Int_t pc, Double_t mom)
{
    ofstream datafile;
    datafile.open("forplot.txt", std::ios_base::app);
    for (int i = 0; i < V.size(); i++)
    {
        if (V[i]->PdgCode() == pc && sqrt(pow(V[i]->GetPx(), 2) + pow(V[i]->GetPy(), 2) + pow(V[i]->GetPz(), 2)) < mom)
        {
            datafile << Strips.size() << "\n";
            
        }
    }

    datafile.close();

}

void AdvDigitisation::clustercharge(std::vector<Int_t> ADC)
{
    ofstream datafile;
    datafile.open("forplot.txt", std::ios_base::app);
    Double_t sum = 0; 

    if (ADC.size()>0)
    {
    for (int i = 0; i < ADC.size(); i++)
    {
        sum = sum + ADC[i];
    }

    datafile << sum << endl; 
    }

    datafile.close();
    
}


void AdvDigitisation::eta(std::vector<Int_t> ADC, std::vector<Int_t> Strips)
{
    ofstream datafile;
    datafile.open("forplot.txt", std::ios_base::app);
    if (ADC.size()>0){
    Int_t index = distance(ADC.begin(), max_element(ADC.begin(), ADC.end()));
    Int_t strip_a = Strips[index];
    Double_t Q_a = ADC[index]; 
    Double_t Q_b1 ;
    Double_t Q_b2 ; 
    Double_t Q_b ; 
    Double_t etan; 
    if((std::find(Strips.begin(), Strips.end(), strip_a+1)!=Strips.end())) 
    {
        auto it = std::find(Strips.begin(), Strips.end(), strip_a +1);
        Q_b1 = ADC[it-Strips.begin()];
    } else {
        Q_b1 = 0; 
    }
    if((std::find(Strips.begin(), Strips.end(), strip_a-1)!=Strips.end())) 
    {
        auto it1 = std::find(Strips.begin(), Strips.end(), strip_a -1);
        Q_b2 = ADC[it1-Strips.begin()];

    } else {
        Q_b2 = 0; 
    }   
    if (Q_b2 > Q_b1)
    {
        Q_b = Q_b2;
        etan = Q_b / (Q_b + Q_a);

    }else{
        Q_b = Q_b1;
        etan = Q_a / (Q_b+Q_a);
    }

    datafile << etan << endl; 
    }

    datafile.close();

}

void AdvDigitisation::diffusionarea(std::vector<SurfaceSignal> DiffusionSignal)
{
    ofstream datafile;
    datafile.open("forplot.txt", std::ios_base::app);
    for (int i = 0; i < DiffusionSignal.size(); i++)
    {
        for (int j = 0; j < (DiffusionSignal[i].getDiffusionArea()).size(); j++ )
        {
        datafile << (DiffusionSignal[i].getDiffusionArea())[j] << "\n";
        }
    }
    datafile.close();
}
// void AdvDigitisation::write_to_root(const std::vector<AdvTargetPoint*>& V, std::vector<EnergyFluctUnit> EnergyLossVector, std::vector<SurfaceSignal> DiffusionSignal, AdvSignal ResponseSignal, AdvSignal FEDResponseSignal) {
    
//     if (gSystem->AccessPathName("digi.root"))
//     {
//         TFile ofile("digi.root", "RECREATE");
//         TTree tree("digis", "Tree with AdvTargetPoint data");

//         AdvTargetPoint* point = nullptr; // Pointer to the full object
//         EnergyFluctUnit* chargedivisionpoint; 
//         SurfaceSignal* chargedriftpoint;
//         AdvSignal* inducedchargepoint;
//         AdvSignal* fedresponsepoint; 


//         tree.Branch("AdvPoint", &point); // Store the pointer to the full object
//         tree.Branch("ChargeDivision", &chargedivisionpoint);
//         tree.Branch("ChargeDrift", &chargedriftpoint); 
//         tree.Branch("InducedCharge", &inducedchargepoint);
//         tree.Branch("FEDResponse", &fedresponsepoint);

//         // Fill the TTree with data
//         for (size_t i = 0; i < V.size(); ++i) {
//             point = V[i]; // Assign pointer to current AdvTargetPoint

//             chargedivisionpoint = &EnergyLossVector[i];
//             chargedriftpoint = &DiffusionSignal[i];
//             inducedchargepoint = &ResponseSignal; 
//             fedresponsepoint = &FEDResponseSignal;

//             // Fill the tree
//             tree.Fill();
//         }

//         // Write the TTree to the file and close it
//         tree.Write();
//         ofile.Close();

//     } else {
//         TFile *ofile;
//        ofile = TFile::Open("digi.root", "UPDATE");
//        TTree *tree = (TTree*) ofile->Get("digis"); 

//         AdvTargetPoint* point = nullptr; // Pointer to the full object
//         EnergyFluctUnit* chargedivisionpoint; 
//         SurfaceSignal* chargedriftpoint;
//         AdvSignal* inducedchargepoint;
//         AdvSignal* fedresponsepoint; 

//         tree->SetBranchAddress("AdvPoint", &point); // Store the pointer to the full object
//         tree->SetBranchAddress("ChargeDivision", &chargedivisionpoint);
//         tree->SetBranchAddress("ChargeDrift", &chargedriftpoint); 
//         tree->SetBranchAddress("InducedCharge", &inducedchargepoint);
//         tree->SetBranchAddress("FEDResponse", &fedresponsepoint);

//                 // Fill the TTree with data
//         for (size_t i = 0; i < V.size(); ++i) {
//             point = V[i]; // Assign pointer to current AdvTargetPoint

//             chargedivisionpoint = &EnergyLossVector[i];
//             chargedriftpoint = &DiffusionSignal[i];
//             inducedchargepoint = &ResponseSignal; 
//             fedresponsepoint = &FEDResponseSignal;

//             // Fill the tree
//             tree->Fill();
//         }

//         tree->Write(0,TObject::kWriteDelete,0);
//         ofile->Close();
//     }
// }
