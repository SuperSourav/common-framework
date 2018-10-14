/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TTHSemilepClassBDT_HH
#define TTHSemilepClassBDT_HH

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbLeptonic/TMVAClassificationInterface.h"

class TTHSemilepClassBDT : public top::EventSelectorBase, public TMVAClassificationInterface{

public:

  explicit TTHSemilepClassBDT(std::string, std::shared_ptr<top::TopConfig> config);
  virtual ~TTHSemilepClassBDT();

  bool apply(const top::Event& event) const override;
  std::string name() const override;

  //// ... add more config 

  private:


  std::shared_ptr<top::TopConfig> m_config;

  virtual bool passRegion(TTHbb::Event* eventvar) const;
  virtual void getBDTVars(std::vector<std::string> &floatVar);
  void getBDTVarsKinICHEP(std::vector<std::string> &BDTVar, std::string& btagopVar);
  void getBDTVarsKin(std::vector<std::string> &BDTVar, std::string& btagopVar);
  void getBDTVarsRecoICHEP(std::vector<std::string> &BDTVar);
  void getBDTVarsReco(std::vector<std::string> &BDTVar);
  void getBDTVarsBTag(std::vector<std::string> &BDTVar);
  void getBDTVarsBTag1(std::vector<std::string> &BDTVar);
  void getBDTVarsReduced(std::vector<std::string> &BDTVar);
  void getBDTVarsReducedInclusive(std::vector<std::string> &BDTVar, bool addjet5);

  std::string m_recoFlag;

  mutable int m_nJets;
  mutable int m_nbJets;
  mutable int m_leptonType;

  //// temp, waiting for common region definition
  int m_njets_cuts;
  int m_nmaxjets_cuts;
  int m_nbjets_cuts;
  int m_nmaxbjets_cuts;
  std::string m_regionName;
  int m_regionVal;

};

#endif
