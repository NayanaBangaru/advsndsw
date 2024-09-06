#ifndef SHIPLHC_SIDIGIPARAMETERS_H_
#define SHIPLHC_SIDIGIPARAMETERS_H_

/* Header-only to implement common code for CMS TOB */
#include "ShipUnit.h"

namespace stripsensor {

const bool peakmode = 0;
const std::string APVpeakpulse = "data/APVShapePeak_default.txt";
const std::string APVdecopulse = "data/APVShapeDeco_default.txt";
const Double_t saturation_charge_limit = 48e-16;

namespace drift{
    const bool cms_approximation = 0; // 
}

namespace frontend{
    const Int_t muxgain_register = 2; // 5 possible values with middle sized register giving gain of 1 mA/mip with +/- 20% 
    const Double_t reference_current = 0.128; // in mA, above which is the gain 
    // const bool peakmode = 0; // 0 for peak mode operation of the APV, 1 for deconvolution mode 
    const Double_t gain = 0.1; // in uA. Need to check which value to use
}
}   // namespace advsnd

#endif   // SHIPLHC_SISENSOR_H_


