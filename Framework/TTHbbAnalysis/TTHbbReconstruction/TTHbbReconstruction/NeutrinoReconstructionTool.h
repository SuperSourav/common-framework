/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NeutrinoReconstructionTool_HH
#define NeutrinoReconstructionTool_HH

#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbObjects/Containers.h"

class NeutrinoMETRefit;

namespace TTHbb{

  enum class NeutrinoRecoType {
    ORIGINAL,
    METREFIT
   };

  class NeutrinoReconstructionTool{

  public:

    explicit NeutrinoReconstructionTool(NeutrinoRecoType type=NeutrinoRecoType::ORIGINAL);
    virtual ~NeutrinoReconstructionTool();

    NeutrinoContainer* compute(float met_met, float met_phi, LeptonContainer* sleptons);


  private:
    NeutrinoRecoType m_recType;

  };

}

#endif
