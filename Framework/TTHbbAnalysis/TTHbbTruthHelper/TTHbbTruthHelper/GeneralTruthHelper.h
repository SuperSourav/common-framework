/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeneralTruthHelper_HH
#define GeneralTruthHelper_HH


#include <iostream>

namespace TTHbbTruth{

  /// need long long here
  enum TTHbbTruthParticleLabel{

    higgs=0,
    hadTop=1,
    lepTop=2,
    leadingTop=3,
    subleadingTop=4,
    vectorBoson=5, /// not from top or Higgs
    BSMHiggs=6,
    ChargedHiggs=7,
    bFromCollide=8,
    childFromChargedHiggs=9,

    //// particles from Higgs and Top
    childFromHiggs=10,
    leadingChildFromHiggs=11,
    subleadingChildFromHiggs=12,
    childFromLepTop=13,
    childFromHadTop=14,
    childFromLeadingTop=15,
    childFromSubleadingTop=16,
    directChildFromTop=17,
    leadingAFromHiggs=18,
    subleadingAFromHiggs=19,

    childFromTop=28,
    childFromAntiTop=29,

    //// particules from Z/W/A from Higgs/Top
    leadingChildFromWTop=20,
    subleadingChildFromWTop=21,
    childFromWHiggs=22,
    childFromZHiggs=23,
    leadingChildFromLeadingA=24,
    subleadingChildFromLeadingA=25,
    leadingChildFromSubleadingA=26,
    subleadingChildFromSubleadingA=27,

    /// particles from vector bosons (not from top/higgs decay)
    childFromVectorBoson=30,

    /// state of particle
    finalState=50,
    initialState=51,
   
  };

  enum TTHbbTruthHiggsDecay{

    Hbb=0,
    HWW=1,
    HZZ=2,
    Hgaga=3,
    Htt=4,
    Hgg=5,
    Hcc=6,
    Hmumu=7,
    Hother=8

  };

  bool isHiggs(int pdg, long long info);
  bool isFromHiggs(long long info);
  bool isLeadingFromHiggs(long long info);
  bool isSubleadingFromHiggs(long long info);
  bool isFromWFromHiggs(long long info);
  bool isFromZFromHiggs(long long info);

  bool isHadTop(int pdg, long long info);
  bool isLepTop(int pdg, long long info);
  bool isLeadingTop(int pdg, long long info);
  bool isSubLeadingTop(int pdg, long long info);
  bool isFromHadTop(long long info);  /// takes into account t->w->xx
  bool isFromLepTop(long long info);
  bool isFromLeadingTop(long long info);
  bool isFromSubLeadingTop(long long info);
  bool isFromTop(long long info);
  bool isFromAntiTop(long long info);
  bool isDirectlyFromTop(long long info);
  bool isLeadingFromWTop(long long info);
  bool isSubleadingFromWTop(long long info);
  
  bool isVectorBoson(int pdg, long long info); /// not from top/H decay
  bool isFromVectorBoson(long long info); /// not from top/H decay

  bool isBSMHiggs(long long info);
  bool isChargedHiggs(long long info);
  bool isFromChargedHiggs(long long info);
  bool isAssociatedBwithChargedHiggs(long long info);
  bool isLeadingAFromHiggs(long long info);
  bool isSubleadingAFromHiggs(long long info);
  bool isLeadingChildFromLeadingA(long long info);
  bool isSubleadingChildFromLeadingA(long long info);
  bool isLeadingChildFromSubleadingA(long long info);
  bool isSubleadingChildFromSubleadingA(long long info);

  bool isInitialState(long long info);
  bool isFinalState(long long info);
  


}






#endif
