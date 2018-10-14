/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthxAODPartHelper_HH
#define TruthxAODPartHelper_HH

#include <memory>
#include "TTHbbObjects/TruthParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"

class TruthxAODPartHelper{

public:

  static std::shared_ptr<TTHbb::TruthParticle> createTruthPart(const xAOD::TruthParticle* part);
  static void printdeepParticle(const xAOD::TruthParticle* part, int deep);
  static void printParticle(const xAOD::TruthParticle* part);
  static void printVertex(const xAOD::TruthVertex* vertex);


};


#endif
