/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHSemilepLikelihoodDiscriminant_H_
#define TTHSemilepLikelihoodDiscriminant_H_

#include "TTHbbReconstruction/LikelihoodDiscriminantTool.h"

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

class TTHSemilepLikelihoodDiscriminant:public top::EventSelectorBase {

 public:

  TTHSemilepLikelihoodDiscriminant(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTHSemilepLikelihoodDiscriminant();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:

  std::shared_ptr<top::TopConfig> m_config;

  
  std::string m_params;
  
  std::unordered_map<std::string,std::string> m_configMap;

  std::string m_btagOP;
  std::string m_trf;
  
  LikelihoodDiscriminantTool *m_discriminant;

  
  
};

#endif
