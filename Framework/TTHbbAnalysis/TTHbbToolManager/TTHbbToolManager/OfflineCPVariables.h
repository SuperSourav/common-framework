/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _OFFLINECPVARIABLES_H_
#define _OFFLINECPVARIABLES_H_

#include "TTHbbToolManager/ToolBase.h"

#include <memory>

namespace TTHbb{

  class OfflineCPVariables : public ToolBase{
    /**
     * Implementation of CPVariables which is accessible whilst running in all tools
     * to keep down the number of calculations
     */
  public:
    OfflineCPVariables(std::string name);
    ~OfflineCPVariables();

    void initialise();

    void apply(TTHbb::Event* event);

    void finalise();
    float cos_theta_1_wrt_123(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3);
    float cos_theta_3_wrt_23(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3);
    float cos_theta_4_wrt_3(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3, TLorentzVector v4);
    float cos_theta_4_wrt_3seq(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3, TLorentzVector v4);
    float b4(TLorentzVector v1, TLorentzVector v2);
  private:
    std::vector<std::string> recobdts;
    std::string varFile;
    bool isdilepton;
  };


}
#endif
