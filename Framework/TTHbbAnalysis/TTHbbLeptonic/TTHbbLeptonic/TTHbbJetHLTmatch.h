/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHbbJetHLTmatch_H_
#define TTHbbJetHLTmatch_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


class TTHbbJetHLTmatch:public top::EventSelectorBase {

 public:

  TTHbbJetHLTmatch(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTHbbJetHLTmatch();

  bool apply(const top::Event & event) const override;
  std::string name() const override;

 private:

  std::shared_ptr<top::TopConfig> m_config;

  void jethltmatch(const TTHbb::JetContainer& Jets) const;

  std::string m_params;

  //---Trigger tools---
  TrigConf::xAODConfigTool* m_TrigConfigTool;
  Trig::TrigDecisionTool* m_TrigDecisionTool;
  //trigger list
  std::vector<std::string> m_SplitJetTriggerList;

  //helper function for trigger navigation
  template<class Object, class Collection>
    const Object* getTrigObject(Trig::Feature<Collection>& feature) const{

    const Collection* trigCol = feature.cptr();
    if ( !trigCol ) {
      std::cout << "ERROR: No Trig Collection pointer" << std::endl;
      return 0;
    }
    if(trigCol->size() != 1){
      std::cout << "ERROR Trig Collection size " << trigCol->size() << std::endl;
      return 0;
    }
    return trigCol->at(0);
  }


};

#endif
