#ifndef SHIPLHC_ADVDIGITISATION_H_
#define SHIPLHC_ADVDIGITISATION_H_

#include "AdvTargetPoint.h"
#include "EnergyFluctUnit.h"
#include "SurfaceSignal.h"
#include "AdvSignal.h"
#include "TVector3.h"
#include "TGeoNavigator.h"

#include "FairGeoBuilder.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "FairGeoTransform.h"
#include "FairGeoVolume.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "ShipDetectorList.h"
#include "ShipStack.h"
#include "ShipUnit.h"
#include "SiSensor.h"
#include "TGeoArb8.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoGlobalMagField.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoSphere.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"
#include "TGeoUniformMagField.h"
#include "TParticle.h"
#include "TString.h"   // for TString
#include "TVector3.h"
#include "TVirtualMC.h"

#include <iostream>
#include <vector>
using namespace std;



class AdvDigitisation
{
  public:
    AdvDigitisation();
    std::map<std::string, std::vector<Int_t>> digirunoutput(Int_t detID, const std::vector<AdvTargetPoint*>& V, std::vector<EnergyFluctUnit>& EnergyLossVector, std::vector<SurfaceSignal>& DiffusionSignal, AdvSignal& TotalSignal, AdvSignal& FEDResponseSignal);    
    TVector3 getLocal(Int_t detID, TVector3 global_pos);

    void write_to_root(const std::vector<AdvTargetPoint*>& V, std::vector<EnergyFluctUnit> EnergyLossVector, std::vector<SurfaceSignal> DiffusionSignal, AdvSignal ResponseSignal, AdvSignal FEDResponseSignal);
    void dEdx(const std::vector<AdvTargetPoint *> &V, std::vector<EnergyFluctUnit> EnergyLossVector, Int_t pc, Double_t mom);
    void stripcharge(std::vector<Int_t> ADC);
    void numberofstrips(const std::vector<AdvTargetPoint *> &V, std::vector<Int_t> Strips, Int_t pc, Double_t mom);
    void clustercharge(std::vector<Int_t> ADC);
    void eta(std::vector<Int_t> ADC, std::vector<Int_t> Strips);
    void diffusionarea(std::vector<SurfaceSignal> DiffusionSignal);

};
#endif
