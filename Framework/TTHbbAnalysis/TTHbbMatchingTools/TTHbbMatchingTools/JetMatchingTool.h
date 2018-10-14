/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetMatchingTool_HH
#define JetMatchingTool_HH

#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/TruthParticle.h"
#include "TTHbbObjects/Jet.h"

namespace TTHbb{


  class JetMatchingTool{

  public:

    JetMatchingTool();
    ~JetMatchingTool();

    inline void jetPtCut(float a){m_jetPtCut=a;}
    inline void jetEtaCut(float a){m_jetEtaCut=a;}
    inline void drCut(float a){m_drCut=a;}


    void matchHadronsToJets(TruthParticleContainer& hadrons, JetContainer& jets, std::string matchname); 

    //// match T to the closest V
    //// V can have multiple matchs but not T
    template <class T, class V>
    void simpleDeltaRMatch(std::vector<T>& uniq, std::vector<V>& multi,
			   float drcut, std::string matchname);

    /// match all V to T with DR<DRCUT, not only closest
    template <class T, class V>
    void inclusiveDeltaRMatch(std::vector<T>& matchTo, std::vector<V>& matchIn,
			      float drcut, std::string matchname);



  private:
    float m_jetPtCut;
    float m_jetEtaCut;
    float m_drCut;


  };


  template <class T, class V>
  void JetMatchingTool::simpleDeltaRMatch(std::vector<T>& uniq, std::vector<V>& multi, 
					  float drcut, std::string matchname){

    for(auto a : uniq){
      V holder;
      float drmin=999999;
      for(auto b : multi){
	float dr = b->p4().DeltaR(a->p4());
	if(dr<drmin){
	  drmin=dr;
	  holder=b;
	}
      }
      if(drmin<drcut){
	a->particleMatch(matchname).push_back(holder);
	holder->particleMatch(matchname).push_back(a);
      }
    }

  }


 template <class T, class V>
 void JetMatchingTool::inclusiveDeltaRMatch(std::vector<T>& matchTo, std::vector<V>& matchIn, 
					    float drcut, std::string matchname){

    for(auto a : matchTo){
      for(auto b : matchIn){
	float dr = b->p4().DeltaR(a->p4());
	if(dr>drcut) continue;
	a->particleMatch(matchname).push_back(b);
      }
    }

  }


}


#endif
