/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTruthHelper/TruthMatchHelper.h"


namespace TTHbb{

  bool JetTruthMatchHelper::bitMatch(int mask, int bit){
    return mask & (1<<bit);
  }
  bool JetTruthMatchHelper::bitMatchOnly(int mask,  int bit){
    return (mask&63) == (1<<bit);
  }


  bool JetTruthMatchHelper::matchLeadingBFromHiggs(Particle* jet){
    return bitMatch(jet->truthMatch(), JetMatchLeadingBHiggs);
  }
  bool JetTruthMatchHelper::matchSubLeadingBFromHiggs(Particle* jet){
    return bitMatch(jet->truthMatch(), JetMatchSubLeadingBHiggs);
  }
  bool JetTruthMatchHelper::matchLeadingJetFromW(Particle* jet){
    return bitMatch(jet->truthMatch(), JetMatchLeadingJetW);
  }
  bool JetTruthMatchHelper::matchSubLeadingJetFromW(Particle* jet){
    return bitMatch(jet->truthMatch(), JetMatchSubLeadingJetW);
  }
  bool JetTruthMatchHelper::matchBFromHadTop(Particle* jet){
    return bitMatch(jet->truthMatch(), JetMatchBHadTop);
  }
  bool JetTruthMatchHelper::matchBFromLepTop(Particle* jet){
    return bitMatch(jet->truthMatch(), JetMatchBLepTop);
  }


  bool JetTruthMatchHelper::matchOnlyLeadingBFromHiggs(Particle* jet){
    return bitMatchOnly(jet->truthMatch(), JetMatchLeadingBHiggs);
  }
  bool JetTruthMatchHelper::matchOnlySubLeadingBFromHiggs(Particle* jet){
    return bitMatchOnly(jet->truthMatch(), JetMatchSubLeadingBHiggs);
  }
  bool JetTruthMatchHelper::matchOnlyLeadingJetFromW(Particle* jet){
    return bitMatchOnly(jet->truthMatch(), JetMatchLeadingJetW);
  }
  bool JetTruthMatchHelper::matchOnlySubLeadingJetFromW(Particle* jet){
    return bitMatchOnly(jet->truthMatch(), JetMatchSubLeadingJetW);
  }
  bool JetTruthMatchHelper::matchOnlyBFromHadTop(Particle* jet){
    return bitMatchOnly(jet->truthMatch(), JetMatchBHadTop);
  }
  bool JetTruthMatchHelper::matchOnlyBFromLepTop(Particle* jet){
    return bitMatchOnly(jet->truthMatch(), JetMatchBLepTop);
  }


  bool JetTruthMatchHelper::matchBFromHiggs(Particle* jet){
    return matchLeadingBFromHiggs(jet) || matchSubLeadingBFromHiggs(jet);
  }
  bool JetTruthMatchHelper::matchOnlyOneBFromHiggs(Particle* jet){
    return matchOnlyLeadingBFromHiggs(jet) || matchOnlySubLeadingBFromHiggs(jet);
  }
  bool JetTruthMatchHelper::matchJetFromW(Particle* jet){
    return matchLeadingJetFromW(jet) || matchSubLeadingJetFromW(jet);
  }
  bool JetTruthMatchHelper::matchOnlyOneJetFromW(Particle* jet){
    return matchOnlyLeadingJetFromW(jet) || matchOnlySubLeadingJetFromW(jet);
  }

  std::string JetTruthMatchHelper::getJetMatchName(Particle* jet){

    std::string smatch="";

    if(matchBFromHiggs(jet))smatch+="bHiggs";
    if(matchBFromLepTop(jet))smatch+="bLepTop";
    if(matchBFromHadTop(jet))smatch+="bHadTop";
    if(matchJetFromW(jet))smatch+="jetW";

    return smatch;

  }

}
