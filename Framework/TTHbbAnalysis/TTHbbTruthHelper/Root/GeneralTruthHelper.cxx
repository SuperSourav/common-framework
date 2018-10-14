/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTruthHelper/GeneralTruthHelper.h"
#include "TTHbbTruthHelper/ParticleIDHelper.h"


bool TTHbbTruth::isHiggs(int pdg, long long info){
  return ParticleIDHelper::isH(pdg) && (info & (1<<TTHbbTruth::higgs) );
}
bool TTHbbTruth::isFromHiggs(long long info){
  return info & (1<<TTHbbTruth::childFromHiggs);
}
bool TTHbbTruth::isLeadingFromHiggs(long long info){
  return info & (1<<TTHbbTruth::leadingChildFromHiggs);
}
bool TTHbbTruth::isSubleadingFromHiggs(long long info){
  return info & (1<<TTHbbTruth::subleadingChildFromHiggs);
}
bool TTHbbTruth::isFromWFromHiggs(long long info){
  return info & (1<<TTHbbTruth::childFromWHiggs);
}
bool TTHbbTruth::isFromZFromHiggs(long long info){
  return info & (1<<TTHbbTruth::childFromZHiggs);
}
bool TTHbbTruth::isHadTop(int pdg, long long info){
  return ParticleIDHelper::isTop(pdg) && (info & (1<<TTHbbTruth::hadTop));
}
bool TTHbbTruth::isLepTop(int pdg, long long info){
  return ParticleIDHelper::isTop(pdg)  && (info & (1<<TTHbbTruth::lepTop));
}
bool TTHbbTruth::isLeadingTop(int pdg, long long info){
  return ParticleIDHelper::isTop(pdg)  && (info & (1<<TTHbbTruth::leadingTop));
}
bool TTHbbTruth::isSubLeadingTop(int pdg, long long info){
  return ParticleIDHelper::isTop(pdg)  && (info & (1<<TTHbbTruth::subleadingTop));
}
bool TTHbbTruth::isFromHadTop(long long info){
  return info & (1<<TTHbbTruth::childFromHadTop);
} 
bool TTHbbTruth::isFromLepTop(long long info){
  return info & (1<<TTHbbTruth::childFromLepTop);
}
bool TTHbbTruth::isFromLeadingTop(long long info){
  return info & (1<<TTHbbTruth::childFromLeadingTop);
}
bool TTHbbTruth::isFromSubLeadingTop(long long info){
  return info & (1<<TTHbbTruth::childFromSubleadingTop);
}
bool TTHbbTruth::isDirectlyFromTop(long long info){
  return info & (1<<TTHbbTruth::directChildFromTop);
}
bool TTHbbTruth::isLeadingFromWTop(long long info){
  return info & (1<<TTHbbTruth::leadingChildFromWTop);
}
bool TTHbbTruth::isSubleadingFromWTop(long long info){
  return info & (1<<TTHbbTruth::subleadingChildFromWTop);
}

bool TTHbbTruth::isFromTop(long long info){
  return info & (1<<TTHbbTruth::childFromTop);
}
  
bool TTHbbTruth::isFromAntiTop(long long info){
  return info & (1<<TTHbbTruth::childFromAntiTop);
}
  
bool TTHbbTruth::isVectorBoson(int pdg, long long info){
  return ParticleIDHelper::isV(pdg) && (info & (1<<TTHbbTruth::vectorBoson));
} 
bool TTHbbTruth::isFromVectorBoson(long long info){
  return info & (1<<TTHbbTruth::childFromVectorBoson);
} 

bool TTHbbTruth::isBSMHiggs(long long info){
  return info & (1<<TTHbbTruth::BSMHiggs);
}
bool TTHbbTruth::isChargedHiggs(long long info){
  return info & (1<<TTHbbTruth::ChargedHiggs);
}
bool TTHbbTruth::isFromChargedHiggs(long long info){
  return info & (1<<TTHbbTruth::childFromChargedHiggs);
}
bool TTHbbTruth::isAssociatedBwithChargedHiggs(long long info){
  return info & (1<<TTHbbTruth::bFromCollide);
}

bool TTHbbTruth::isLeadingAFromHiggs(long long info){
  return info & (1<<TTHbbTruth::leadingAFromHiggs);
}
bool TTHbbTruth::isSubleadingAFromHiggs(long long info){
  return info & (1<<TTHbbTruth::subleadingAFromHiggs);
}
bool TTHbbTruth::isLeadingChildFromLeadingA(long long info){
  return info & (1<<TTHbbTruth::leadingChildFromLeadingA);
}
bool TTHbbTruth::isSubleadingChildFromLeadingA(long long info){
  return info & (1<<TTHbbTruth::subleadingChildFromLeadingA);
}
bool TTHbbTruth::isLeadingChildFromSubleadingA(long long info){
  return info & (1<<TTHbbTruth::leadingChildFromSubleadingA);
}
bool TTHbbTruth::isSubleadingChildFromSubleadingA(long long info){
  return info & (1<<TTHbbTruth::subleadingChildFromSubleadingA);
}

bool TTHbbTruth::isInitialState(long long info){
  long long one=1;
  return info & (one<<TTHbbTruth::initialState);
}
bool TTHbbTruth::isFinalState(long long info){
  long long one=1;
  return info & (one<<TTHbbTruth::finalState);
}
  



