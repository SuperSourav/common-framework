/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TTHBoostedSemilepReco_HH
#define TTHBoostedSemilepReco_HH

//#include "TopEventSelectionTools/EventSelectorBase.h"
//#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"

#include "TTHbbReconstruction/RecoCombinationsMakerTool.h"
//#include "TTHbbLeptonic/EventSaverHelper.h"

class TTHBoostedSemilepReco {

public:

  explicit TTHBoostedSemilepReco();
  virtual ~TTHBoostedSemilepReco();
  
  TTHbb::CombinationContainer* buildCombinations(TTHbb::JetContainer* jets, TTHbb::JetContainer *hadtopjets, TTHbb::LeptonContainer* leptons, TTHbb::NeutrinoContainer* neutrinos) const;

  std::string name() const;
  bool m_isMC;

  private:

  void initTool();

  RecoCombinationsMakerTool* m_recoCombinationTool;

  mutable int m_nJets;
  mutable int m_nbJets;
  mutable int m_leptonType;

  
  std::string m_regionName;
  int m_regionVal;

};

#endif
