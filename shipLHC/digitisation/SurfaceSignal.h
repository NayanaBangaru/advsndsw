#ifndef SHIPLHC_SURFACESIGNAL_H_
#define SHIPLHC_SURFACESIGNAL_H_

#include "TVector3.h"

#include <iostream>

class SurfaceSignal
{
  public:
    SurfaceSignal()
        : fDiffusionArea(), fSurfacePos(), fAmplitude()
    {
    }

    SurfaceSignal(std::vector<Double_t> DiffusionArea, std::vector<TVector3> SurfacePos, std::vector<Double_t> Amplitude)
        : fDiffusionArea(DiffusionArea)
        , fSurfacePos(SurfacePos)
        , fAmplitude(Amplitude)
    {
    }

    std::vector<Double_t> getDiffusionArea() const { return fDiffusionArea; }
    std::vector<TVector3> getSurfacePos() const { return fSurfacePos; }    
    std::vector<Double_t> getAmplitude() const { return fAmplitude; }    

    void setDiffusionArea(std::vector<Double_t> value) { fDiffusionArea = value; }
    void setSurfacePos(std::vector<TVector3> value) { fSurfacePos = value; }
    void setAmplitude(std::vector<Double_t> value) { fAmplitude = value; }

  private:
    std::vector<Double_t> fDiffusionArea;
    std::vector<TVector3> fSurfacePos;
    std::vector<Double_t> fAmplitude;
};

#endif
