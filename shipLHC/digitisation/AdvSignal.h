#ifndef SHIPLHC_ADVSIGNAL_H_
#define SHIPLHC_ADVSIGNAL_H_

#include "TVector3.h"

#include <iostream>

class AdvSignal
{
  public:
    AdvSignal()
        : fStrips(), fIntegratedSignal()
    {
    }

    AdvSignal(std::vector<Int_t> Strips, std::vector<Double_t> IntegratedSignal)
        : fStrips(Strips)
        , fIntegratedSignal(IntegratedSignal)
    {
    }

    std::vector<Int_t> getStrips() const { return fStrips; }
    std::vector<Double_t> getIntegratedSignal() const { return fIntegratedSignal; }  

    void setStrips(std::vector<Int_t> value) { fStrips = value; }
    void setIntegratedSignal(std::vector<Double_t> value) { fIntegratedSignal = value; }

  private:
    std::vector<Int_t> fStrips;
    std::vector<Double_t> fIntegratedSignal;
};

#endif // SHIPLHC_ADVSIGNAL_H_
