/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBB_DILEPTONSIMPLEPCREGIONTOOL_H_
#define _TTHBB_DILEPTONSIMPLEPCREGIONTOOL_H_

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "TH2F.h"

#include "TTHbbObjects/Jet.h"

namespace TTHbb {

  class DileptonSimplePCRegionTool {

  public:

    /// Constructor for tool
    /// @param filename Pass the path to the file containing all the region maps
    DileptonSimplePCRegionTool(std::string filename);
    ~DileptonSimplePCRegionTool();

    /// Function to return a vector of pairs which correspond to region name and region bin for the event
    /// @param jets JetCollection from TTHbb
    std::vector< std::pair<std::string, int> > getEventRegions(std::vector<std::shared_ptr<TTHbb::Jet> > & jets);

    /// Function to return the region bin for the event for a given region
    /// @param jets JetCollection from TTHbb
    /// @param region Region name
    int getEventRegionBin(std::vector<std::shared_ptr<TTHbb::Jet> > & jets, std::string region);

    /// Set the name of the PC jet variable name
    /// @param s String name, by default tagWeightBin
    void setPCVarName(std::string s){ m_pcvarname = s; };

    std::vector<std::string> getRegionList();

  private:
    std::map<std::string, TH2F> regionhists;
    std::string m_pcvarname;

    std::pair<int, int> getBin(std::vector<std::shared_ptr<TTHbb::Jet> > & jets);
    
  };

}

#endif
