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
AdvTargetHit::AdvTargetHit(Int_t detID, const std::vector<AdvTargetPoint*>& V, TNtuple* dat, std::vector<EnergyFluctUnit>* ChargeDivisionPoint, std::vector<SurfaceSignal>* ChargeDriftPoint, AdvSignal* InducedChargePoint, AdvSignal* FEDResponsePoint)
    : SndlhcHit(detID)
{
    AdvDigitisation advdigi{};
    std::vector<EnergyFluctUnit> EnergyLossVector;
    std::vector<SurfaceSignal> DiffusionSignal; 
    AdvSignal TotalSignal;
    AdvSignal FEDResponseSignal;
    fDigitisedHit = advdigi.digirunoutput(detID, V, EnergyLossVector, DiffusionSignal, TotalSignal, FEDResponseSignal);
    flag = true;

    *ChargeDivisionPoint = EnergyLossVector;
    *ChargeDriftPoint = DiffusionSignal;  
    *InducedChargePoint = TotalSignal; 
    *FEDResponsePoint = FEDResponseSignal;
    // for(int k =0; k < TotalSignal.getStrips().size(); k++)
    // {
    //     if (V.size() == 1)
    //     {
    //     std::cout << k << "\t" << TotalSignal.getStrips()[k] << "\t" << EnergyLossVector[0].getDriftPos()[0][0] << "\t" << EnergyLossVector[0].getDriftPos()[0][1] << "\t" << V[0]->GetStation() << "\t" << V[0]->GetPlane() << "\t" << V[0]->GetRow() << "\t" <<  V[0]->GetColumn() << "\t" << V[0]->GetSensor() << "\t" << V[0]->GetModule() << std::endl; 
    //     }
    // }

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
