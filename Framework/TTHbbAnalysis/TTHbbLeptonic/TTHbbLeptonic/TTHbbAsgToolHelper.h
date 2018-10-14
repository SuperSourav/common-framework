/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHbbAsgToolHelper_H_
#define TTHbbAsgToolHelper_H_

#include "AsgTools/AsgTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include <string>

/// just because I could not directly inherit from both AsgTool EventSelectorBase (conflict in name() method).

class TTHbbAsgToolHelper: public asg::AsgTool{

 public:

  TTHbbAsgToolHelper(std::string name);
  ~TTHbbAsgToolHelper();

  const xAOD::TruthParticleContainer* getTruthParticleContainer(std::string name) const;
  const xAOD::JetContainer* getJetContainer(std::string name) const;

 private:


};

#endif
