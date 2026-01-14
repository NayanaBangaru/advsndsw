#include "TROOT.h"

#include "sndFilterProcessBase.h"


bool isMC = true;
if (gROOT->GetListOfGlobals()->FindObject("rawConv")) isMC = false;


std::vector< snd::analysis_core::baseProcess * > pipeline;
