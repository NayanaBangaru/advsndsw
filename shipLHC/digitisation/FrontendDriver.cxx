#include "FrontendDriver.h"
#include "StripNoise.h"
#include "InducedCharge.h"
#include "SiDigiParameters.h"
#include "AdvSignal.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
using namespace std;


FrontendDriver::FrontendDriver() {}

void FrontendDriver::FEDResponse(AdvSignal& Signal, AdvSignal& FEDResponseSignal)
{
    AdvSignal ADCResponse = ADCConversion(Signal); 
 
    std::vector<AdvSignal> temp_FEDResponseSignal;

    StripNoise stripnoise{}; 
    InducedCharge inducedcharge{}; 
    
    if (stripsensor::frontend::ZSModeOption && stripsensor::frontend::NoiseOption)
    {
        FEDResponseSignal = stripnoise.AddGaussianTailNoise(ADCResponse);  
        temp_FEDResponseSignal.push_back(FEDResponseSignal);
        FEDResponseSignal = inducedcharge.Combine(temp_FEDResponseSignal); 
        FEDResponseSignal = ZeroSuppressionAlgorithms(FEDResponseSignal);
    }
    if (!stripsensor::frontend::ZSModeOption)
    {
        FEDResponseSignal = stripnoise.AddGaussianNoise(ADCResponse); 
        FEDResponseSignal = stripnoise.AddCMNoise(FEDResponseSignal); 
        temp_FEDResponseSignal.push_back(FEDResponseSignal);
        FEDResponseSignal = inducedcharge.Combine(temp_FEDResponseSignal); 
        FEDResponseSignal = ZeroSuppressionAlgorithms(FEDResponseSignal);
        FEDResponseSignal = stripnoise.AddPedestals(FEDResponseSignal);
    }
    FEDResponseSignal = SaturateRange(FEDResponseSignal);
}

AdvSignal FrontendDriver::ADCConversion(AdvSignal ResponseSignal)
{
        std::vector<Double_t> NumberofElectrons = ResponseSignal.getIntegratedSignal();
        for (int i = 0; i < NumberofElectrons.size(); i++)
        {
            ADCcount.push_back(std::ceil(NumberofElectrons[i]/stripsensor::frontend::ElectronperADC));
        }

        AdvSignal ADCResponse(ResponseSignal.getStrips(), ADCcount);

        return ADCResponse; 
}

AdvSignal FrontendDriver::SaturateRange(AdvSignal Signal)
{
    std::vector<Double_t> Charge = Signal.getIntegratedSignal();
    for (int i = 0; i < Charge.size(); i++)
    {
        if (stripsensor::frontend::ZSModeOption)
        {
            if (Charge[i] > 1022)
            {
                Charge[i] = 255; 
            }
            if (Charge[i] > 253)
            {
                Charge[i] = 254; 
            }
            if (Charge[i] < 0)
            {
                Charge[i] = 0; 
            }

        } else {
            if (Charge[i] > 1023)
            {
                Charge[i] = 1023; 
            }
            if (Charge[i] < 0)
            {
                Charge[i] = 0; 
            }
        }
    }
    AdvSignal SaturatedSignal(Signal.getStrips(), Charge);
    return SaturatedSignal; 
}

AdvSignal FrontendDriver::ZeroSuppressionAlgorithms(AdvSignal Signal)
{
    std::vector<Double_t> Amplitude = Signal.getIntegratedSignal(); 
    std::vector<Int_t> Strips = Signal.getStrips();

    std::vector<Int_t> ClusterStrips; 
    std::vector<Double_t> ClusterAmplitudes; 

    std::vector<Int_t> temp_ClusterStrips; 
    std::vector<Double_t> temp_ClusterAmplitudes;

    Int_t mode = stripsensor::frontend::ZeroSuppressionMode; 

    switch (mode) {
        case 1:
            for (int i = 0; i < Amplitude.size(); i++)
            {
                if (Amplitude[i] > stripsensor::frontend::ZeroSuppressionMode1T*stripsensor::frontend::StripNoise)
                {
                    ClusterStrips.push_back(Strips[i]);
                    ClusterAmplitudes.push_back(Amplitude[i]);
                }
            } 
            break; 
        case 2:
            for (int i = 0; i < Amplitude.size(); i++)
            {
                if (Amplitude[i] > 2*stripsensor::frontend::StripNoise)
                {
                    temp_ClusterStrips.push_back(Strips[i]); 
                    temp_ClusterAmplitudes.push_back(Amplitude[i]); 
                }
            } 
            for (int j = 0; j < temp_ClusterStrips.size(); j++)
            {
                if((std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j]+1)==temp_ClusterStrips.end()) && (std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j]-1)==temp_ClusterStrips.end())) 
                {
                    if (temp_ClusterAmplitudes[j] > 5*stripsensor::frontend::StripNoise)
                    {
                        ClusterStrips.push_back(temp_ClusterStrips[j]); ClusterAmplitudes.push_back(temp_ClusterAmplitudes[j]);
                    }
                } else {
                    ClusterStrips.push_back(temp_ClusterStrips[j]); ClusterAmplitudes.push_back(temp_ClusterAmplitudes[j]);
                }
            }
            break; 
        case 3:
            for (int i = 0; i < Amplitude.size(); i++)
            {
                if (Amplitude[i] > 3*stripsensor::frontend::StripNoise)
                {
                    ClusterStrips.push_back(Strips[i]); 
                    ClusterAmplitudes.push_back(Amplitude[i]); 
                }
            } 
            ClusterStrips = temp_ClusterStrips; 
            ClusterAmplitudes = temp_ClusterAmplitudes; 

            for (int j = 0; j < temp_ClusterStrips.size(); j++)
            {
                if((std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j]+1)!=temp_ClusterStrips.end()) && (std::find(ClusterStrips.begin(), ClusterStrips.end(), temp_ClusterStrips[j]+1)==ClusterStrips.end())) 
                {
                    auto it = std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j]+1 );
                    if ((temp_ClusterAmplitudes[it - temp_ClusterStrips.begin()])>0)
                    {
                        ClusterStrips.push_back(temp_ClusterStrips[it - temp_ClusterStrips.begin()]);
                        ClusterAmplitudes.push_back(temp_ClusterAmplitudes[it - temp_ClusterStrips.begin()]);
                    }

                } 
                if((std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j]-1)!=temp_ClusterStrips.end()) && (std::find(ClusterStrips.begin(), ClusterStrips.end(), temp_ClusterStrips[j]-1)==ClusterStrips.end()))
                {
                    auto itlower = std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j]-1);
                    if ((temp_ClusterAmplitudes[itlower - temp_ClusterStrips.begin()])>0)
                    {
                        ClusterStrips.push_back(temp_ClusterStrips[itlower - temp_ClusterStrips.begin()]);
                        ClusterAmplitudes.push_back(temp_ClusterAmplitudes[itlower - temp_ClusterStrips.begin()]);
                    }

                } 

            }
            break; 
        case 4:
            Double_t SumCharge = 0;
            Double_t SumNoise = 0;

            for (int i = 0; i < Amplitude.size(); i++)
            {
                if (Amplitude[i] > 3*stripsensor::frontend::StripNoise)
                {
                    //cout << "Amp : " <<  Amplitude[i] << endl; 
                    temp_ClusterStrips.push_back(Strips[i]); 
                    temp_ClusterAmplitudes.push_back(Amplitude[i]); 
                    ClusterStrips.push_back(Strips[i]); 
                    ClusterAmplitudes.push_back(Amplitude[i]); 
                }
            } 
            Int_t neighbhour = 0 ; 
            Int_t neighbhourlower = 0; 
            Int_t j = 0; 
            std::vector<Int_t> Clusters ; 
            std::vector<Int_t> SingleClusters;
            while (j < temp_ClusterStrips.size())
            {
                neighbhour = 1; 
                neighbhourlower = 1;
                while ((std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j] + neighbhour) != temp_ClusterStrips.end()) ||
            (std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j] - neighbhourlower) != temp_ClusterStrips.end()))
                {
                    if (std::find(ClusterStrips.begin(), ClusterStrips.end(), temp_ClusterStrips[j]) == ClusterStrips.end())
                    {
                        ClusterStrips.push_back(temp_ClusterStrips[j]);
                        ClusterAmplitudes.push_back(temp_ClusterAmplitudes[j]);   
                    }

                    // Check for +1 neighbor
                    auto it_forward = std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j] + neighbhour);
                    if (it_forward != temp_ClusterStrips.end())
                    {
                        int index_forward = it_forward - temp_ClusterStrips.begin();
                        if (std::find(ClusterStrips.begin(), ClusterStrips.end(), temp_ClusterStrips[index_forward]) == ClusterStrips.end() &&
                            temp_ClusterAmplitudes[index_forward] > 2 * stripsensor::frontend::StripNoise)
                        {
                            ClusterStrips.push_back(temp_ClusterStrips[index_forward]);
                            ClusterAmplitudes.push_back(temp_ClusterAmplitudes[index_forward]);
                        }
                        neighbhour += 1; 
                    }

                    // Check for -1 neighbor
                    auto it_backward = std::find(temp_ClusterStrips.begin(), temp_ClusterStrips.end(), temp_ClusterStrips[j] - neighbhourlower);
                    if (it_backward != temp_ClusterStrips.end())
                    {
                        int index_backward = it_backward - temp_ClusterStrips.begin();
                        if (std::find(ClusterStrips.begin(), ClusterStrips.end(), temp_ClusterStrips[index_backward]) == ClusterStrips.end() &&
                            temp_ClusterAmplitudes[index_backward] > 2 * stripsensor::frontend::StripNoise)
                        {
                            ClusterStrips.push_back(temp_ClusterStrips[index_backward]);
                            ClusterAmplitudes.push_back(temp_ClusterAmplitudes[index_backward]);
                        }
                        neighbhourlower += 1; 
                    }
                }



                if (neighbhour > 1)
                {
                    Clusters.push_back(j); 
                    Clusters.push_back(j+neighbhour);
                }else{
                    SingleClusters.push_back(j);
                }
                j = j + neighbhour; 
            }

            for (int l = 0; l < Clusters.size(); l+=2)
            {
                for (int m = Clusters[l]; m < Clusters[l+1]; m++)
                {
                    SumCharge += temp_ClusterAmplitudes[m]; 
                    SumNoise += pow(stripsensor::frontend::StripNoise, 2);
                } 
                if (SumCharge < SumNoise)
                {
                        
                    for (int m = Clusters[l]; m < Clusters[l+1]; m++)
                    {
                        ClusterStrips.erase(find(ClusterStrips.begin(), ClusterStrips.end(), temp_ClusterStrips[m]));
                        ClusterAmplitudes.erase(find(ClusterAmplitudes.begin(), ClusterAmplitudes.end(), temp_ClusterAmplitudes[m]));
                    } 
                }
                SumCharge = 0 ; 
                SumNoise = 0 ; 
            }

            for (int l = 0; l < SingleClusters.size(); ++l)
            {
                    int pos = SingleClusters[l];
                    SumCharge = temp_ClusterAmplitudes[pos]; 
                    SumNoise = pow(stripsensor::frontend::StripNoise, 2);
                if (SumCharge < 5*SumNoise)
                {
                        
                        ClusterStrips.erase(find(ClusterStrips.begin(), ClusterStrips.end(), temp_ClusterStrips[pos]));
                        ClusterAmplitudes.erase(find(ClusterAmplitudes.begin(), ClusterAmplitudes.end(), temp_ClusterAmplitudes[pos]));
                }
                SumCharge = 0 ; 
                SumNoise = 0 ; 
            }
            break; 
    }
    AdvSignal ClusterSignal(ClusterStrips, ClusterAmplitudes); 
    return ClusterSignal; 
}

void Testing()
{
    ofstream rawdatafile;
    rawdatafile.open("to_plot.txt", std::ios_base::app);
    std::vector<Int_t> Strips = {665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 675, 676, 677};
    std::vector<Double_t> Charge = {36, 75, 83, 82, 82, 89, 88, 88, 82, 90, 89, 74, 7};
    AdvSignal testsignal(Strips, Charge);
    
    FrontendDriver frontenddriver{};
    AdvSignal testresponse;
    frontenddriver.FEDResponse(testsignal, testresponse);
    std::vector<Double_t> Charge1 = testresponse.getIntegratedSignal();
    //std::vector<Int_t> Strips = FEDResponseSignal.getStrips();
    std::vector<Int_t> ADC(Charge.size()); 

    std::transform(Charge1.begin(), Charge1.end(), ADC.begin(), [](Double_t x) { 
        if (x>0){return (int)x;}
        else {return 0;}
        });

    for (int k = 0; k < (testresponse.getStrips()).size(); k++)
    {
        rawdatafile << testresponse.getStrips()[k] << "\t" << ADC[k] << endl; 
        cout << testresponse.getStrips()[k] << "\t" << ADC[k] << endl; 
    }
    
    rawdatafile.close();
}