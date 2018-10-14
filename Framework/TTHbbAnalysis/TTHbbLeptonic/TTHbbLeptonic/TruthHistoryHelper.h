/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthHistoryHelper_HH
#define TruthHistoryHelper_HH

#include "xAODTruth/TruthParticle.h"

#include <set>


class TruthHistoryHelper{

public:

  TruthHistoryHelper();
  ~TruthHistoryHelper();

  void avoidSherpaLoop(bool a){ m_avoidSherpaLoop=a; }
  void isPythia6(bool a){ m_isPythia6=a; }

  bool isLooping(const xAOD::TruthParticle* part, 
  		const xAOD::TruthParticle* child) const;
  bool doesLoop(const xAOD::TruthParticle* part, 
  	       std::set<const xAOD::TruthParticle*>& init) const;


  const xAOD::TruthParticle* getFinalState(const xAOD::TruthParticle* part) const;
  const xAOD::TruthParticle* getInitialState(const xAOD::TruthParticle* part) const;

  bool isHFHadronFinalState(const xAOD::TruthParticle* part, int type) const;
  bool isHFHadronInitialState(const xAOD::TruthParticle* part, int type) const;

  bool islepW(const xAOD::TruthParticle* particle) const;
  bool ishadW(const xAOD::TruthParticle* particle) const;

  bool isInitialHiggs(const xAOD::TruthParticle* particle) const;
  bool isFinalHiggs(const xAOD::TruthParticle* particle) const;
  bool isFinalVectorBoson(const xAOD::TruthParticle* part) const;
  bool isFinalTop(const xAOD::TruthParticle* particle) const;
  bool islepTop(const xAOD::TruthParticle* particle) const;
  bool isFinalBSMHiggs(const xAOD::TruthParticle* particle) const;
  bool isFinalChargedHiggs(const xAOD::TruthParticle* particle) const;

  bool isfromHiggs(const xAOD::TruthParticle* particle) const;
  bool isfromTop(const xAOD::TruthParticle* part) const;
  bool isfromTau(const xAOD::TruthParticle* part) const;
  bool isAssociatedWithChargedHiggs(const xAOD::TruthParticle* part) const;

private:

  bool m_avoidSherpaLoop;
  int m_isPythia6;

};

#endif
