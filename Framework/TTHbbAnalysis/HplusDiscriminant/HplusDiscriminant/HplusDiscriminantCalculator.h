/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HPLUSDISCRIMINANTCALCULATOR_H
#define HPLUSDISCRIMINANTCALCULATOR_H
#include "HplusDiscriminant/HplusDiscriminantData.h"
#include "HplusDiscriminant/HplusDiscriminantTemplateHolder.h"
#include <vector>
#include <map>
#include <string>
#include "TLorentzVector.h"

namespace TTHbb {

class HplusDiscriminantCalculator {

public:
  HplusDiscriminantCalculator(std::vector<int> masspoints);
  ~HplusDiscriminantCalculator(){};
  void InitializeWeights(HplusDiscriminantData* data);
  static std::vector<TLorentzVector> NuSolution_Rescale( TLorentzVector& l, TLorentzVector& nu );
  static std::pair<double,double>Calculate_lambda_delta( TLorentzVector& lepton, TLorentzVector& neutrino );
  void CalculateDiscriminant(HplusDiscriminantData* data);
  double GetDiscriminant(int mass);
  double GetDiscriminantAPM(int mass);
  double GetDiscriminantMJ(int mass);
  double GetLikelihood(int mass); //for mass<0 ttbar likelihood is returned
  double GetLikelihoodAPM(int mass); //for mass<0 ttbar likelihood is returned
  double GetLikelihoodMJ(int mass); //for mass<0 ttbar likelihood is returned
  double GetLikelihoodDkin(int mass); //for mass<0 ttbar likelihood is returned
  double GetLikelihoodDkinAPM(int mass); //for mass<0 ttbar likelihood is returned
  double GetLikelihoodDkinMJ(int mass); //for mass<0 ttbar likelihood is returned
  double GetLikelihoodJWP(int mass); //for mass<0 ttbar likelihood is returned
  
  static constexpr long double vop[8]       = {-1., 0.1758475, 0.645925, 0.8244273, 0.934906,0.9769329,0.9977155,1.};
  static constexpr long double eff_vop_b[8] = {1., 0.85, 0.77, 0.7, 0.6, 0.5,0.3,0.};
  static constexpr long double eff_vop_c[8] = {1., 0.3846, 0.3226, 0.1610,0.0822,0.0290,0.0094,0.0007};
  static constexpr long double eff_vop_l[8] = {1., 0.02982,0.00744,0.00262,0.00065,0.00018,0.00002};
  static constexpr double WMass = 80.385;
 private:
    std::vector<int> m_masspoints;
    std::vector<std::string> m_channels;
    std::vector<std::vector<HplusDiscriminantTemplateHolder> > m_templates_Hplus_APM; //first index mass, second index channel
    std::vector<std::vector<HplusDiscriminantTemplateHolder> > m_templates_Hplus_MJ;
    std::vector<HplusDiscriminantTemplateHolder> m_templates_WbWb_APM; // index is channel 0=5j3b, 1=5j4b, 2=6j3b, 3=6j4b
    std::vector<HplusDiscriminantTemplateHolder> m_templates_WbWb_MJ;

    std::vector<long double>   m_btag_weight_prob;
    std::vector<long double>   m_ctag_weight_prob;
    std::vector<long double>   m_ltag_weight_prob;

    std::vector<TLorentzVector>   m_NuSolutions;

    std::map<int,long double> m_HplustbprobJWP;      
    long double               m_WbWbprobJWP;
    std::map<int,long double> m_HplustbprobDkin;
    std::map<int,long double> m_HplustbprobDkin_APM;
    std::map<int,long double> m_HplustbprobDkin_MJ;
    long double               m_WbWbprobDkin;
    long double               m_WbWbprobDkin_MJ;
    long double               m_WbWbprobDkin_APM;
};
}

#endif // HPLUSDISCRIMINANTCALCULATOR_H
