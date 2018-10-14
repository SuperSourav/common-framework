/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTruthHelper/ParticleIDHelper.h"




bool ParticleIDHelper::isBHadron(int pdgId){
  if(pdgId>1e9) return false;  /// protect against some ions that could pass the test below
  int rest1(abs(pdgId)%1000);
  int rest2(abs(pdgId)%10000);
  if((rest2 >= 5000 && rest2 < 6000) || (rest1 >= 500 && rest1 < 600)) return true;
  return false;
}

bool ParticleIDHelper::isCHadron(int pdgId){
  if(pdgId>1e9) return false;  /// protect against some ions that could pass the test below
  int rest1(abs(pdgId)%1000);
  int rest2(abs(pdgId)%10000);
  if((rest2 >= 4000 && rest2 < 5000) || (rest1 >= 400 && rest1 < 500)) return true;
  return false;
}
