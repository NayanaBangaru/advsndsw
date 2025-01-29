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

    DigitisedHit["Strips"] = Strips; 
    DigitisedHit["ADC"] = ADC; 

    write_to_root(V, EnergyLossVector, DiffusionSignal, ResponseSignal, FEDResponseSignal);
    
    return DigitisedHit;
}

// void AdvDigitisation::write_to_root(const std::vector<AdvTargetPoint*>& V, std::vector<EnergyFluctUnit> EnergyLossVector, std::vector<SurfaceSignal> DiffusionSignal, AdvSignal ResponseSignal, AdvSignal FEDResponseSignal) {
    
//     if (gSystem->AccessPathName("digi.root"))
//     {
//         TFile ofile("digi.root", "RECREATE");
//         TTree tree("digis", "Tree with AdvTargetPoint data");

//         AdvTargetPoint* point = nullptr; // Pointer to the full object
//         EnergyFluctUnit chargedivisionpoint; 
//         SurfaceSignal chargedriftpoint;
//         AdvSignal inducedchargepoint;
//         AdvSignal fedresponsepoint; 


//         tree.Branch("AdvPoint", &point); // Store the pointer to the full object
//         tree.Branch("ChargeDivision", &chargedivisionpoint);
//         tree.Branch("ChargeDrift", &chargedriftpoint); 
//         tree.Branch("InducedCharge", &inducedchargepoint);
//         tree.Branch("FEDResponse", &fedresponsepoint);

//         // Fill the TTree with data
//         for (size_t i = 0; i < V.size(); ++i) {
//             point = V[i]; // Assign pointer to current AdvTargetPoint

//             chargedivisionpoint = EnergyLossVector[i];
//             chargedriftpoint = DiffusionSignal[i];
//             inducedchargepoint = ResponseSignal; 
//             fedresponsepoint = FEDResponseSignal;

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

void AdvDigitisation::write_to_root(const std::vector<AdvTargetPoint*>& V, 
                                    const std::vector<EnergyFluctUnit>& EnergyLossVector, 
                                    const std::vector<SurfaceSignal>& DiffusionSignal, 
                                    const AdvSignal& ResponseSignal, 
                                    const AdvSignal& FEDResponseSignal) {

    static TFile* ofile = nullptr;
    static TTree* tree = nullptr;

    if (!ofile) {
        ofile = TFile::Open("digi.root", "UPDATE");
        tree = (TTree*)ofile->Get("digis");

        if (!tree) {
            ofile->Close();  // Close if corrupted
            ofile = new TFile("digi.root", "RECREATE");
            tree = new TTree("digis", "Tree with AdvTargetPoint data");
        }

        tree->SetAutoFlush(50000);
        ofile->SetCompressionLevel(1);
    }

    AdvTargetPoint* point = nullptr;
    EnergyFluctUnit chargedivisionpoint;
    SurfaceSignal chargedriftpoint;
    AdvSignal inducedchargepoint;
    AdvSignal fedresponsepoint;

    tree->Branch("AdvPoint", &point, 32000, 0);
    tree->Branch("ChargeDivision", &chargedivisionpoint, 32000, 0);
    tree->Branch("ChargeDrift", &chargedriftpoint, 32000, 0);
    tree->Branch("InducedCharge", &inducedchargepoint, 32000, 0);
    tree->Branch("FEDResponse", &fedresponsepoint, 32000, 0);

    for (size_t i = 0; i < V.size(); ++i) {
        point = V[i];
        chargedivisionpoint = EnergyLossVector[i];
        chargedriftpoint = DiffusionSignal[i];
        inducedchargepoint = ResponseSignal;
        fedresponsepoint = FEDResponseSignal;

        tree->Fill();
    }

    tree->AutoSave("FlushBaskets");  // Save only new data

    // Keep the file open to avoid unnecessary overhead
}