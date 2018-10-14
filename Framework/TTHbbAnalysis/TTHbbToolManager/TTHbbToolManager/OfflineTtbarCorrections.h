/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBB_OFFLINETTBARCORRECTIONS_H_
#define _TTHBB_OFFLINETTBARCORRECTIONS_H_

#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbToolManager/ToolBase.h"
#include "TTHbbTtbarCorrections/FractionReweighting.h"
#include "TTHbbTtbarCorrections/ttbbNormReweighting.h"
#include "TTHbbTtbarCorrections/ttbbShapeReweighting.h"
#include "TTHbbTtbarCorrections/ttcReweighting.h"
#include "NNLOReweighter/NNLOReweighter.h"

#include <iostream>
#include <vector>

#include "TLorentzVector.h"
#include "TMath.h"

namespace TTHbb {

  class OfflineTtbarCorrections : public ToolBase{

  public:
    OfflineTtbarCorrections(std::string name);
    ~OfflineTtbarCorrections();

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();
    
  private:
    FractionReweighting *m_FracRW=0;
    ttbbNormReweighting *m_NormRW=0;
    ttbbShapeReweighting *m_ShapeRW=0;
    ttcReweighting *m_ttcRW=0;
    NNLOReweighter *m_NNLOReweighter=0;
    bool m_doNNLOrw;
    unsigned int sampleID=0;
    int m_HF_SimpleClassification=-9;
    int m_HF_Classification=-9;
    std::vector<std::string> m_NormRWTargets;
    std::vector<std::string> m_NormRWNames;
    std::vector<std::string> m_shapeRWTargets;
    std::vector<std::string> m_shapeRWVarNames;
    bool m_DoFracRw;
    bool m_DoNormRw;
    bool m_DoShapeRw;
    bool m_DottccRw;
  };

}

#endif 
