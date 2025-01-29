#ifndef SHIPLHC_ENERGYFLUCTUNIT_H_
#define SHIPLHC_ENERGYFLUCTUNIT_H_

#include "TVector3.h"

#include <iostream>

class EnergyFluctUnit
{
  public:
      EnergyFluctUnit()
        : fEfluct(), fSegLen(), fDriftPos(), fglob_DriftPos()
    {
    }

    // EnergyFluctUnit::~EnergyFluctUnit() {}

    EnergyFluctUnit(std::vector<Double_t> Efluct, float segLen, std::vector<TVector3> DriftPos, std::vector<TVector3> glob_DriftPos)
        : fEfluct(Efluct)
        , fSegLen(segLen)
        , fDriftPos(DriftPos)
        , fglob_DriftPos(glob_DriftPos)
    {
    }

    std::vector<Double_t> getEfluct() const { return fEfluct; }
    float getsegLen() const { return fSegLen; }
    std::vector<TVector3> getDriftPos() const { return fDriftPos; }
    std::vector<TVector3> getglobDriftPos() const { return fglob_DriftPos; }
    

    void setEfluct(std::vector<Double_t> value) { fEfluct = value; }
    void setsegLen(float value) { fSegLen = value; }
    void setDriftPos(std::vector<TVector3> value) { fDriftPos = value; }
    void setglobDriftPos(std::vector<TVector3> value) { fglob_DriftPos = value; }

  private:
    std::vector<Double_t> fEfluct;
    float fSegLen;
    std::vector<TVector3> fDriftPos;
    std::vector<TVector3> fglob_DriftPos;
};

#endif
