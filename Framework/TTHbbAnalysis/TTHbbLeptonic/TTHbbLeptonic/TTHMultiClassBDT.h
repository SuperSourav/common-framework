/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TTHMultiClassBDT_HH
#define TTHMultiClassBDT_HH

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbLeptonic/TMVAClassificationInterface.h"

class TTHMultiClassBDT : public top::EventSelectorBase, public TMVAClassificationInterface{

public:

  explicit TTHMultiClassBDT(std::string, std::shared_ptr<top::TopConfig> config);
  virtual ~TTHMultiClassBDT();

  bool apply(const top::Event& event) const override;
  std::string name() const override;

  //// ... add more config

  private:


  std::shared_ptr<top::TopConfig> m_config;

  virtual bool passRegion(TTHbb::Event* eventvar) const;
  virtual void getBDTVars(std::vector<std::string> &floatVar);
  virtual void initReaders(std::string xml);
  virtual void registerSaverVariables();
  virtual void defaultValues(TTHbb::Event* tthevt) const;
  void getMultiClassVariables(std::vector<std::string> &BDTVar);
  void getMultiClassNames(std::vector<std::string> &multi_names);

  std::string m_recoFlag;
  std::vector<std::string> multiclass_names;

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
