#include "AdvTargetHit.h"
#include "AdvTargetPoint.h"
#include "digitisation/AdvSignal.h"
#include "digitisation/AdvDigitisation.h"
#include "FairLogger.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TVector3.h"
#include "TStopwatch.h"
#include "TNtuple.h"

#include <TDatabasePDG.h>
#include <iomanip>
#include <typeinfo>
#include <iostream>
#include <string>
#include <map>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
AdvTargetHit::AdvTargetHit()
    : SndlhcHit()
{
    flag = true;
    for (Int_t i = 0; i < 16; i++) {
        fMasked[i] = kFALSE;
    }
}
// -----   Standard constructor   ------------------------------------------
AdvTargetHit::AdvTargetHit(Int_t detID)
    : SndlhcHit(detID)
{
    flag = true;
    for (Int_t i = 0; i < 16; i++) {
        fMasked[i] = kFALSE;
    }
}

// -----   constructor from AdvMuFilterPoint   ------------------------------------------
AdvTargetHit::AdvTargetHit(Int_t detID, const std::vector<AdvTargetPoint*>& V, TNtuple* dat, AdvSignal FEDResponseSignal)
    : SndlhcHit(detID)
{
    AdvDigitisation advdigi{};
    std::vector<EnergyFluctUnit> EnergyLossVector;
    std::vector<SurfaceSignal> DiffusionSignal; 
    AdvSignal TotalSignal;
    
    //AdvSignal FEDResponseSignal;
    fDigitisedHit = advdigi.digirunoutput(detID, V, EnergyLossVector, DiffusionSignal, TotalSignal, FEDResponseSignal);
    flag = true;

    std::cout << "yo " << V.size() << std::endl;
    std::cout << "yo1 " << EnergyLossVector.size() << std::endl;
    std::cout << "yo2 " << DiffusionSignal.size() << std::endl;
     

    //std::vector<Int_t> fedresponsepointstrips = fDigitisedHit.getStrips(); 
    //std::vector<Double_t> fedresponsepointcharge = fDigitisedHit.getIntegratedSignal();

    //int* fedresponsepointstripsarr = (fDigitisedHit.getStrips()).data();
    // Int_t pointsize = V.size();
    // std::vector<Float_t> pointmom(pointsize);
    // //std::vector<Double_t> pointentryx(pointsize);
    // for (int i = 0; i < pointsize; i++)
    // {
    //     pointmom[i]  = (sqrt(pow(V[i]->GetPx(), 2) + pow(V[i]->GetPy(), 2) + pow(V[i]->GetPz(), 2)));
    // } 
    // std::vector<Float_t> pointmom_float(pointmom.begin(), pointmom.end());  // Convert to Float_t
    // //dat->Fill(pointmom_float.data());

    //dat->Fill(pointmom_float.data(), pointmom_float.data(), pointmom_float.data());
    // dat->Fill(1, 1, 1);
    for (Int_t i = 0; i < 16; i++) {
        fMasked[i] = kFALSE;
    }
    LOG(DEBUG) << "signal created";
}

// -----   Public method Print   -------------------------------------------
void AdvTargetHit::Print() const
{
    LOG(INFO) << " AdvTargetHit: AdvMuFilter hit "
              << " in detector " << fDetectorID;
}
