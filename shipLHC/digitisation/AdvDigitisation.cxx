#include "AdvDigitisation.h"

#include "AdvSignal.h"
#include "AdvTargetPoint.h"
#include "ChargeDivision.h"
#include "ChargeDrift.h"
#include "Clustering.h"
#include "EnergyFluctUnit.h"
#include "FrontendDriver.h"
#include "InducedCharge.h"
#include "SiDigiParameters.h"
#include "SurfaceSignal.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TTree.h"
#include "TVector3.h"

#include <TSystem.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

// Running the digitisation

AdvDigitisation::AdvDigitisation() {}

std::vector<AdvSignal> AdvDigitisation::digirunoutput(Int_t detID, const std::vector<AdvTargetPoint *> &V)
{
    // Charge Division
    ChargeDivision chargedivision{};
    std::vector<EnergyFluctUnit> EnergyLossVector = chargedivision.Divide(detID, V);

    // Charge Drift
    ChargeDrift chargedrift{};
    std::vector<SurfaceSignal> DiffusionSignal = chargedrift.Drift(EnergyLossVector);

    // Induced Charge on strips
    InducedCharge inducedcharge{};
    std::vector<AdvSignal> ResponseSignal = inducedcharge.IntegrateCharge(DiffusionSignal);

    return ResponseSignal;
}
