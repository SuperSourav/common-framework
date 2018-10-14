/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PartonTruthFinder_HH
#define PartonTruthFinder_HH


#include "xAODTruth/TruthParticleContainer.h"
#include "TTHbbLeptonic/TruthHistoryHelper.h"
#include "TTHbbTruthHelper/GeneralTruthHelper.h"

#include <set>

class PartonTruthFinder{

public:

  PartonTruthFinder();
  ~PartonTruthFinder();

  inline void setTruthHistoryHelper(TruthHistoryHelper* a){ m_truthHistoryHelper=a; }

  void fillPartons(const xAOD::TruthParticleContainer* truthcontainer, 
		   std::set<const xAOD::TruthParticle*>& partons) const;
  
private:

  TruthHistoryHelper* m_truthHistoryHelper;

  void setTruthPartDecoration(const xAOD::TruthParticle* part, 
			      TTHbbTruth::TTHbbTruthParticleLabel bit) const;

  void sortPt(std::vector<const xAOD::TruthParticle*>& parts) const;

  int fillHiggsDaughters(const xAOD::TruthParticle* part, 
			 std::set<const xAOD::TruthParticle*>& parts) const;

  void fillTopTruth(const std::vector<const xAOD::TruthParticle*>& sortedTops,
		    std::set<const xAOD::TruthParticle*>& parts) const;

  void fillVectorBosonsTruth(const xAOD::TruthParticle* part, 
			     std::set<const xAOD::TruthParticle*>& parts) const;

  void fillBSMHiggsMatching(const xAOD::TruthParticle* part, 
			    std::set<const xAOD::TruthParticle*>& parts) const;

  void fillChargedHiggsMatching(const xAOD::TruthParticle* part, 
				std::set<const xAOD::TruthParticle*>& parts) const;

  void fillAssociatedBwithChargedHiggs(const xAOD::TruthParticle* part, 
				       std::set<const xAOD::TruthParticle*>& parts) const;


};

#endif
