/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TTHSemilepReco_HH
#define TTHSemilepReco_HH

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"

#include "TTHbbReconstruction/RecoCombinationsMakerTool.h"
#include "TTHbbReconstruction/RecoBDTCalculatorTool.h"
#include "TTHbbReconstruction/RecoMVAVariableTool.h"
#include "TTHbbLeptonic/EventSaverHelper.h"

class TTHSemilepReco : public top::EventSelectorBase{

public:

  explicit TTHSemilepReco(std::string, std::shared_ptr<top::TopConfig> config);
  virtual ~TTHSemilepReco();

  bool apply(const top::Event& event) const override;
  std::string name() const override;

  //// ... add more config 

  private:


  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;
  std::string m_xmlPath;

  void initTool();
  void initReaders();
  void initVariablesToCopy();
  void defaultValues(TTHbb::Event* tthevt) const;
  void registerSaverVariables();
  void registerSaverVariables(std::vector<std::string> vect, std::string flag, 
			      TTHbb::DecorationType type, std::string description);

  TTHbb::CombinationContainer* buildCombinations(TTHbb::JetContainer* jets,
						 TTHbb::LeptonContainer* leptons,
						 TTHbb::NeutrinoContainer* neutrinos) const;


  void computeBDT(TTHbb::CombinationContainer* combinations, TTHbb::Event* eventvar,
		  unsigned long long eventNumber) const;
  void decorateNeutrinoSolution(TTHbb::Event* tthevt, std::string combname) const;

  void getBDTVars(std::vector<std::string> &floatVar, 
		  std::vector<std::string> &floatVarH);

  bool passRegion(TTHbb::Event* eventvar) const;

  std::unordered_map<std::string,std::string> m_configMap;
  std::string m_recoStrategy;
  std::string m_btagOP;
  std::string m_trf;
  std::string m_flag;
  std::string m_region;
  std::string m_xmlFlag;
  std::string m_varOption;

  RecoCombinationsMakerTool* m_recoCombinationTool;
  RecoBDTCalculatorTool* m_recoBDTTool;
  RecoMVAVariableTool* m_recoMVATool;

  mutable int m_nJets;
  mutable int m_nbJets;
  mutable int m_leptonType;

  std::vector<std::string> m_copyIntVariables;
  std::vector<std::string> m_copyIntVariablesH;
  std::vector<std::string> m_copyFloatVariables;
  std::vector<std::string> m_copyFloatVariablesH;

  //// temp, waiting for common region definition
  int m_njets_cuts;
  int m_nmaxjets_cuts;
  int m_nbjets_cuts;
  int m_nmaxbjets_cuts;

  std::string m_regionName;
  int m_regionVal;

};

#endif
