/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBB_SEMILEPPCREGIONTOOL_H_
#define _TTHBB_SEMILEPPCREGIONTOOL_H_

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "TH2F.h"

#include "TTHbbObjects/Jet.h"

namespace TTHbb {

  class SemilepPCRegionTool {

  public:

    /// Constructor for tool
    SemilepPCRegionTool();
    ~SemilepPCRegionTool();

    /// Set the name of the PC jet variable name
    /// @param s String name, by default tagWeightBin
    void setPCVarName(std::string s){ m_pcvarname = s; };
    
    /// Get Region Numbers in the given option
    int SemilepPCRegionsNumber(TTHbb::JetContainer* jets, std::string option);
    int SemilepPCRegionsNumber(int nJ, int nB60, int nB70, int nB77, int nB85, std::string option);
    int SemilepPCRegionsNumber(int nJ, int PCBdiscr, std::string option);

    /// Return true/false whether this event is included in the region specified by name
    bool getSemilepPCRegionBit(std::string name, TTHbb::JetContainer* jets);
    bool getSemilepPCRegionBit(std::string name, int nB60, int nB70, int nB77, int nB85);
    bool getSemilepPCRegionBit(std::string name, int PCBdiscr);

    /// Useful functions for PCRegions discriminator
    int getBTagWPMapAddressX(int PCBdiscr);
    int getBTagWPMapAddressY(int PCBdiscr);
    int getNBTags(int PCBdiscr, int WP);

    /// Return discriminator for PCRegions:
    ///  1st leading mv2 jet PCB weight -> Thousands digit
    ///  2nd leading mv2 jet PCB weight -> Handreds digit
    ///  3rd leading mv2 jet PCB weight -> tens digit
    ///  4th leading mv2 jet PCB weight -> ones digit
    int SemilepPCRegionDiscri(int nB60, int nB70, int nB77, int nB85);
    int SemilepPCRegionDiscri(TTHbb::JetContainer* jets);

  private:
    std::string m_pcvarname;

  };

}

#endif
