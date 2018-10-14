/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ONLINEMVAVARIABLES_H_
#define _ONLINEMVAVARIABLES_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

#include "MVAVariables/MVAVariables.h"
#include "TTHbbObjects/Event.h"

#include <string>
#include <map>
#include <utility>
#include <memory>

class OnlineMVAVariables : public top::EventSelectorBase{

 public:
  
  OnlineMVAVariables(std::string params, std::shared_ptr<top::TopConfig> config);
  ~OnlineMVAVariables();

  bool apply(const top::Event& event) const override;
  std::string name() const override;

  void calculateVariables(TTHbb::Event* evt) const;

 private:
  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;

  void initialise();
  void addIntVariable(std::string name, std::function<int(MVAVariables*)>, bool isBtag=false);
  void addFloatVariable(std::string name, std::function<float(MVAVariables*)>, bool isBtag=false);

  std::map<std::string, std::shared_ptr<MVAVariables> > mapOfMVAVars;
  std::map<std::string, std::shared_ptr<MVAVariables> > mapOfMVAVars_sort;
  std::vector<std::string> bTagWPs;
  std::vector<std::string> sortMethods;

  std::unordered_map<std::string, std::function<int(MVAVariables*)>> intDict;
  std::unordered_map<std::string, std::function<int(MVAVariables*)>> bTagIntDict;
  std::unordered_map<std::string, std::function<float(MVAVariables*)>> floatDict;
  std::unordered_map<std::string, std::function<float(MVAVariables*)>> bTagFloatDict;

};

#endif
