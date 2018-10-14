/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetInCombinationDecorationTool_HH
#define JetInCombinationDecorationTool_HH

#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbObjects/Event.h"

class JetInCombinationDecorationTool{

 public:

  static void decorateJets(TTHbb::Combination* comb, std::string flag);
  static bool fillRecoVariables(TTHbb::RecoEvent* eventvar, std::string combname, std::string flag);
  static bool fillRecoVariables(TTHbb::Event* eventvar, std::string combname, std::string flag);


};

#endif
