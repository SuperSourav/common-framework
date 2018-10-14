/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthMatchHelper_HH
#define TruthMatchHelper_HH

#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbTruthHelper/GeneralTruthHelper.h"

namespace TTHbbTruth{

  enum JetTruthMatchDefinition{JetMatchLeadingBHiggs=0,  
			       JetMatchSubLeadingBHiggs=1,
			       JetMatchBLepTop=2,
			       JetMatchBHadTop=3,
			       JetMatchLeadingJetW=4,
			       JetMatchSubLeadingJetW=5,

			       JetMatchLeadingChildFromLeadingA=6,
			       JetMatchSubleadingChildFromLeadingA=7,
			       JetMatchLeadingChildFromSubleadingA=8,
			       JetMatchSubleadingChildFromSubleadingA=9,

			       JetMatchAssociatedB=10,
			       JetMatchBChargedHiggs=11,
			       JetMatchBTop=12,
			       JetMatchBAntiTop=13,
			       JetMatchJetWplus=14,
			       JetMatchJetWminus=15,


			       DilepJetMatchBTop1=20,
			       DilepJetMatchBTop2=21,
			       DiLepMatchLepTop1=22,
			       DiLepMatchLepTop2=23,




  };

}

namespace TTHbb{

  using namespace TTHbbTruth;

  /// for now only static functions
  /// will transform to a full class if needed
  class JetTruthMatchHelper{

  public:
    static bool bitMatch(int mask, int bit);
    static bool bitMatchOnly(int mask,  int bit);

    static bool matchLeadingBFromHiggs(Particle* jet);
    static bool matchSubLeadingBFromHiggs(Particle* jet);
    static bool matchLeadingJetFromW(Particle* jet);
    static bool matchSubLeadingJetFromW(Particle* jet);
    static bool matchBFromHadTop(Particle* jet);
    static bool matchBFromLepTop(Particle* jet);

    static bool matchOnlyLeadingBFromHiggs(Particle* jet);
    static bool matchOnlySubLeadingBFromHiggs(Particle* jet);
    static bool matchOnlyLeadingJetFromW(Particle* jet);
    static bool matchOnlySubLeadingJetFromW(Particle* jet);
    static bool matchOnlyBFromHadTop(Particle* jet);
    static bool matchOnlyBFromLepTop(Particle* jet);

    static bool matchBFromHiggs(Particle* jet);
    static bool matchOnlyOneBFromHiggs(Particle* jet);
    static bool matchJetFromW(Particle* jet);
    static bool matchOnlyOneJetFromW(Particle* jet);

    static std::string getJetMatchName(Particle* jet);


  private:
    JetTruthMatchHelper(); /// for now only static methods so no one need an instance

  };

}

#endif
