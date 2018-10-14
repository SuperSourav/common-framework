/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbToolManager/OfflineTRFFakes.h"


namespace TTHbb{

  //___________________________________________________________________________
  OfflineTRFFakes::OfflineTRFFakes(std::string name)
    : ToolBase(false)
  {
    m_name = name;
  }


  //___________________________________________________________________________
  OfflineTRFFakes::~OfflineTRFFakes()
  {
  }


  //___________________________________________________________________________
  void OfflineTRFFakes::initialise()
  {
    //--- Get info from config file
    auto * config = TTHbb::GlobalConfiguration::get();
    m_btagWPs          = TTHbb::util::vectoriseString((*config)("MVAVariables.bTagWPs"));
    m_parametrizations = TTHbb::util::vectoriseString((*config)("TRFFakes.parametrization"));
    m_extrapRegions    = TTHbb::util::vectoriseString((*config)("TRFFakes.regionExtrap"));
    m_bTagHypotheses   = TTHbb::util::vectoriseString((*config)("TRFFakes.bTagHypothesis"));
    m_effmapPath       =                              (*config)("TRFFakes.DataPath");

    //--- Loop on parametrizations and extrapolation regions
    for (auto param : m_parametrizations){
      for (auto extrRegion : m_extrapRegions){
        std::string config_id = Form("r%s_p%s", param.c_str(), extrRegion.c_str());

        m_mapOfTRFFakes.insert(std::make_pair(config_id, std::make_shared<TRFFakesTool>()));

        (m_mapOfTRFFakes[config_id])->setBtaggingWPs(m_btagWPs);
        (m_mapOfTRFFakes[config_id])->setEffParam(param);
        (m_mapOfTRFFakes[config_id])->setExtrapolatingRegion(extrRegion);
        (m_mapOfTRFFakes[config_id])->setBTaggingHypotheses(m_bTagHypotheses);
        (m_mapOfTRFFakes[config_id])->setEffMap(m_effmapPath);

        (m_mapOfTRFFakes[config_id])->initialise();
      }
    }
  }


  //___________________________________________________________________________
  void OfflineTRFFakes::apply(TTHbb::Event* event)
  {
    //--- Loop over instances
    for (auto& trf : m_mapOfTRFFakes){
      trf.second->apply(event);
    }
  }


  //___________________________________________________________________________
  void OfflineTRFFakes::finalise()
  {
    for (auto& trf : m_mapOfTRFFakes){
      trf.second.reset();
    }
    m_mapOfTRFFakes.clear();
  }
}
