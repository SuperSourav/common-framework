/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHbbRegionDefinition_H_
#define TTHbbRegionDefinition_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbClassification/SemilepPCRegionTool.h"
#include "TTHbbClassification/DileptonSimplePCRegionTool.h"

class TTHbbRegionDefinition:public top::EventSelectorBase {

 public:

  TTHbbRegionDefinition(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTHbbRegionDefinition();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;

  std::string name() const override;

 private:
  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;

  void decorateSemilepRegions(std::shared_ptr<TTHbb::Event> tthevt) const;
  //void decorateDilepRegions(std::shared_ptr<TTHbb::Event> tthevt) const;

  mutable TTHbb::SemilepPCRegionTool m_semilepPCRegionTool;
  mutable TTHbb::DileptonSimplePCRegionTool * m_dilepPCRegionTool;

  std::vector<std::string> m_semilepOptions;
  std::string m_dilepFilePath;

};

#endif
