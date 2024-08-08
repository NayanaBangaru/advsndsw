#include <iostream>

class EnergyFluctUnit 
{
    public:

    EnergyFluctUnit(std::vector<Double_t> Efluct, float segLen) : Efluct_(Efluct), segLen_(segLen) {}

    std::vector<Double_t> getEfluct() const {return Efluct_;}
    float getsegLen() const {return segLen_;}

    void setEfluct(std::vector<Double_t> value) {Efluct_ = value;}
    void setsegLen(float value) {segLen_ = value;}

    private:

    std::vector<Double_t> Efluct_;
    float segLen_;
};