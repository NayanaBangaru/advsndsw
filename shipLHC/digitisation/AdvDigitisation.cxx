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
    
    // plotstrips(ADC); 

    //plotsaturationdetails(V, ADC, ResponseSignal);
    // plotclustercharge(ResponseSignal);
    DigitisedHit["Strips"] = Strips; 
    DigitisedHit["ADC"] = ADC; 


    write_to_root( std::vector<AdvTargetPoint *> &V);
    if (stripsensor::frontend::write_digi_to_text)
    {
        ofstream rawdatafile;
        rawdatafile.open("rawdata.txt", std::ios_base::app);

        ofstream chargedivisionfile;
        chargedivisionfile.open("chargedivision.txt", std::ios_base::app);

        ofstream chargedriftfile;
        chargedriftfile.open("chargedrift.txt", std::ios_base::app);

        ofstream inducedchargefile;
        inducedchargefile.open("inducedcharge.txt", std::ios_base::app);

        ofstream frontendfile;
        frontendfile.open("frontend.txt", std::ios_base::app);

        TVector3 local_entry;
        TVector3 local_exit;

        for (int i = 0; i < V.size(); i ++)
        {
            local_entry = getLocal(V[i] -> GetDetectorID(), V[i]->GetEntryPoint());
            local_exit = getLocal(V[i] -> GetDetectorID(), V[i]->GetExitPoint());
            rawdatafile << V.size() << "\t" << V[i]->GetEnergyLoss() << "\t" << V[i]->PdgCode() << "\t" << sqrt(pow(V[i]->GetPx(), 2) + pow(V[i]->GetPy(), 2) + pow(V[i]->GetPz(), 2)) << "\t" << V[i]->GetPx() << "\t" << V[i]->GetPy() << "\t" << V[i]->GetPz() << "\t" << (V[i]->GetEntryPoint()).X() << "\t" << (V[i]->GetEntryPoint()).Y() << "\t" << (V[i]->GetEntryPoint()).Z() << "\t" << local_entry.X() << "\t" << local_entry.Y() << "\t" << local_entry.Z() << "\t" << (V[i]->GetExitPoint()).X() << "\t" << (V[i]->GetExitPoint()).Y() << "\t" << (V[i]->GetExitPoint()).Z() << "\t" << local_exit.X() << "\t" << local_exit.Y() << "\t" << local_exit.Z() << "\t" << V[i]->GetLength() << "\t" << V[i]->GetEventID() << "\t" << V[i]->GetTrackID() << "\t" << V[i]->GetTime() << "\t" << V[i]->GetDetectorID() << endl; 

            std::vector<Double_t> EFluct;
            int EFluctSize;
            float segLen;
            std::vector<TVector3> DriftPos;
            std::vector<TVector3> glob_DriftPos;

            EFluct = EnergyLossVector[i].getEfluct();
            EFluctSize = EFluct.size();
            segLen = EnergyLossVector[i].getsegLen();
            DriftPos = EnergyLossVector[i].getDriftPos();
            glob_DriftPos = EnergyLossVector[i].getglobDriftPos();
            
            std::vector<TVector3> DiffPos;
            std::vector<Double_t> DiffArea;

            DiffArea = DiffusionSignal[i].getDiffusionArea();
            DiffPos = DiffusionSignal[i].getSurfacePos();

            std::vector<Int_t> Strips1; 
            std::vector<Double_t> IntegratedSignal;

            Strips1 = ResponseSignal.getStrips(); 
            IntegratedSignal = ResponseSignal.getIntegratedSignal();

            std::vector<Int_t> StripsFR = FEDResponseSignal.getStrips();
            std::vector<Double_t> ADCFR = FEDResponseSignal.getIntegratedSignal();
            
            
            for (int j = 0; j < EFluct.size(); j++)
            {
                chargedivisionfile << V.size() << "\t" << V[i]->GetEnergyLoss() << "\t" << V[i]->GetEventID() << "\t" << V[i]->GetTrackID() << "\t" << V[i]->GetTime() << "\t" << V[i]->GetDetectorID() << "\t" << EFluctSize << "\t" << EFluct[j] << "\t" << segLen << "\t" << DriftPos[j].X() << "\t" << DriftPos[j].Y() << "\t" << DriftPos[j].Z() << "\t" << glob_DriftPos[j].X() << "\t" << glob_DriftPos[j].Y() << "\t" << glob_DriftPos[j].Z() << endl;  

                chargedriftfile << V.size() << "\t" << V[i]->GetEnergyLoss() << "\t" << V[i]->GetEventID() << "\t" << V[i]->GetTrackID() << "\t" << V[i]->GetTime() << "\t" << V[i]->GetDetectorID() << "\t" << DiffArea[j] << "\t" << DiffPos[j].X() << "\t" << DiffPos[j].Y() << "\t" << DiffPos[j].Z() << endl; 
            }

            for (int l = 0; l < Strips1.size(); l++)
            {
                inducedchargefile << V.size() << "\t" << V[i]->GetEnergyLoss() << "\t" << V[i]->GetEventID() << "\t" << V[i]->GetTrackID() << "\t" << V[i]->GetTime() << "\t" << V[i]->GetDetectorID() << "\t" << Strips1[l] << "\t" << IntegratedSignal[l] << "\t" << Strips1.size() << endl; 
            }

            for (int m = 0; m < StripsFR.size(); m++)
            {
                frontendfile <<  V.size() << "\t" << V[i]->GetEnergyLoss() << "\t" << V[i]->GetEventID() << "\t" << V[i]->GetTrackID() << "\t" << V[i]->GetTime() << "\t" << V[i]->GetDetectorID() << "\t" << StripsFR.size() << "\t" << StripsFR[m] << "\t" << ADCFR[m] << endl; 
            }
            if (StripsFR.size() == 0)
            {
                frontendfile <<  V.size() << "\t" << V[i]->GetEnergyLoss() << "\t" << V[i]->GetEventID() << "\t" << V[i]->GetTrackID() << "\t" << V[i]->GetTime() << "\t" << V[i]->GetDetectorID() << "\t" << 1 << "\t" << 0 << "\t" << 0 << endl; 

            }
        }

        rawdatafile.close();
        chargedivisionfile.close();
        chargedriftfile.close();
        inducedchargefile.close();
        frontendfile.close();
    }
    
    return DigitisedHit;
}

TVector3 AdvDigitisation::getLocal(Int_t detID, TVector3 point)
{
    TVector3 local_point; 
    // Calculate the detector id as per the geofile, where strips are disrespected
    // int strip = (detID) % 1024;                // actual strip ID
    int geofile_detID = detID; // the det id number needed to read the geometry
    int station = geofile_detID >> 17;
    int plane = (geofile_detID >> 16) % 2;
    int row = (geofile_detID >> 13) % 8;
    int column = (geofile_detID >> 11) % 4;
    int sensor = geofile_detID;
    int sensor_module = advsnd::target::columns * row + 1 + column;

    TString path = TString::Format("/cave_1/"
                                   "Detector_0/"
                                   "volAdvTarget_1/"
                                   "TrackingStation_%d/"
                                   "TrackerPlane_%d/"
                                   "SensorModule_%d/"
                                   "SensorVolumeTarget_%d",
                                   station,
                                   plane,
                                   sensor_module,
                                   sensor);
    TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
    if (nav->CheckPath(path)) {
        nav->cd(path);
    } else {
        LOG(FATAL) << path;
    }
    // Get the corresponding node, which is a sensor made of strips
    TGeoNode *W = nav->GetCurrentNode();
    Double_t x = point.X();
    Double_t y = point.Y();
    Double_t z = point.Z();
    double global_pos[3] = {x, y, z};
    double local_pos[3];
    nav->MasterToLocal(global_pos, local_pos);

    local_point = local_pos; 
    return local_point; 
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


void AdvDigitisation::plotsaturationdetails(const std::vector<AdvTargetPoint *> &V, std::vector<Int_t> ADC, AdvSignal ResponseSignal)
{
    ofstream rawdatafile;
    rawdatafile.open("to_plot.txt", std::ios_base::app);

    ofstream rawdatafile2;
    rawdatafile2.open("to_plot2.txt", std::ios_base::app);
    for(int k = 0; k < ADC.size(); k++)
    {
        if (ADC[k] > 1022)
        {
            for (int l = 0; l < V.size(); l++)
            {
                rawdatafile << V.size() << "\t" << V[l]->PdgCode() << "\t" << sqrt(pow(V[l]->GetPx(), 2) + pow(V[l]->GetPy(), 2) + pow(V[l]->GetPz(), 2)) << "\t" << V[l]->GetEnergyLoss()*1e9/stripsensor::drift::perGeV << endl; 
            }

            for (int m = 0; m < (ResponseSignal.getIntegratedSignal()).size(); m++)
            {
                rawdatafile2 << ResponseSignal.getIntegratedSignal()[m] << endl ;
            }
        }
    }
    rawdatafile.close();
    rawdatafile2.close();
}

void AdvDigitisation::plotclustercharge(AdvSignal ResponseSignal)
{
    ofstream rawdatafile;
    rawdatafile.open("to_plot.txt", std::ios_base::app);

    for (int m = 0; m < (ResponseSignal.getIntegratedSignal()).size(); m++)
    {
        rawdatafile << ResponseSignal.getIntegratedSignal()[m] * 250 << endl ;
    }

    rawdatafile.close();
}

// void AdvDigitisation::plotefluct(const std::vector<AdvTargetPoint *> &V, std::vector<EnergyFluctUnit> EnergyLossVector)
// {
//     ofstream rawdatafile;
//     rawdatafile.open("to_plot.txt", std::ios_base::app);

//     for (int i = 0; i < EnergyLossVector.size(); i++)
//     {
//         for (int j = 0; j < (EnergyLossVector.getEfluct()).size(); j++)
//         {
//             rawdatafile <<  EnergyLossVector.getEfluct()[j] << endl ;
//         }
//     }

//     rawdatafile.close();
// }

void AdvDigitisation::write_to_root( std::vector<AdvTargetPoint *> &V)
{
    TFile ofile("test.root","UPDATE");

    TTree tree("tree", "testree");

    std::vector<Int_t> a; 
    std::vector<Int_t> b; 
    Int_t DetectorID; 

    tree.Branch("DetID", &DetectorID);

    for (int i = 0; i < V.size(); i ++)
    {
        DetectorID = int(V[i] -> GetDetectorID());
        tree.Fill();
    }
    tree.Write(); 
    ofile.Close();

}