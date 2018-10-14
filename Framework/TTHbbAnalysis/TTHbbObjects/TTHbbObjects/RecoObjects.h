/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _RECOOBJECTS_H_
#define _RECOOBJECTS_H_

#include "TTHbbObjects/BaseObject.h"
#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/Jet.h"
#include "TTHbbObjects/Lepton.h"
#include "TTHbbObjects/Neutrino.h"
#include "TTHbbObjects/Containers.h"
#include <memory>

namespace TTHbb{

  /// enum from semilepMVAreco from the common code
  enum JetPartonCand{
    jetReco_notDefined    = 0,
    jetReco_b1fromHiggs   = 1, // leading b
    jetReco_b2fromHiggs   = 2, // subleading b
    jetReco_bfromlepTop   = 3,
    jetReco_bfromhadTop   = 4, // or bfromincompleteTop
    jetReco_q1fromW       = 5, // leading q
    jetReco_q2fromW       = 6, // subleading q
    jetReco_qincompleteTop = 7,
    jetReco_bfromlepTop1 = 8,
    jetReco_bfromlepTop2 = 9,
   };

  enum LeptonCand{
    lepReco_notDefined    = 0,
    lepReco_lepfromlepTop1 = 1,
    lepReco_lepfromlepTop2 = 2,
   };

  enum NeutrinoApprox{
    nuReco_notDefined       = 0,
    nuReco_positiveSolution =  1,
    nuReco_negativeSolution =  2,
    nuReco_realonlySolution =  3,
  };

  class ObjectHelper{

  public:

    /// mainly to help with composite holding Particle
    static Particle* toParticle(BaseObject* a){return dynamic_cast<Particle*>(a);}
    static Lepton* toLepton(BaseObject* a){return dynamic_cast<Lepton*>(a);}
    static Neutrino* toNeutrino(BaseObject* a){return dynamic_cast<Neutrino*>(a);}
    static Jet* toJet(BaseObject* a){return dynamic_cast<Jet*>(a);}

  private:
    ObjectHelper();

  };


  class Composite: public Particle{

  public:

    Composite();
    virtual ~Composite();

    void clear();
    virtual Composite* clone() const;
    virtual Composite* deepClone() const;
    virtual void dump() const;

    std::shared_ptr<Particle> child1;
    std::shared_ptr<Particle> child2;
    bool isMatch;
    bool isOneMatch; 

  private:

  };


  class TopComposite: public Particle{

  public:

    TopComposite();
    virtual ~TopComposite();


    void clear();
    virtual TopComposite* clone() const;
    virtual TopComposite* deepClone() const;
    virtual void dump() const;

    std::shared_ptr<Composite> childw;
    std::shared_ptr<Particle> childb;
    bool isMatch;
    bool isWMatch;
    bool isBMatch;

  private:


  };


  class ParticleCluster: public Particle{


  public:

    ParticleCluster();
    virtual ~ParticleCluster();

    void clear();
    virtual ParticleCluster* clone() const;
    virtual ParticleCluster* deepClone() const;
    virtual void dump() const;

    std::vector<std::shared_ptr<Jet> > jets;
    std::shared_ptr<Lepton> lepton;
    std::shared_ptr<Neutrino> neutrino;
  


  private:

  };


  class Combination : public BaseObject{

  public:

    Combination();
    virtual ~Combination();


    void clear();
    Combination* clone() const; 
    Combination* deepClone() const; 
    void dump() const;


    std::shared_ptr<TopComposite> hadTop;
    std::shared_ptr<TopComposite> lepTop;
    std::shared_ptr<Composite> lepTop1; /// prepare for dilep
    std::shared_ptr<Composite> lepTop2; /// prepare for dilep
    std::shared_ptr<TopComposite> recolepTop1;
    std::shared_ptr<TopComposite> recolepTop2;
    std::shared_ptr<Composite> higgs;
    std::shared_ptr<Composite> incompleteHadTop; /// b and light from W, to be used in 5 jets
    std::vector<std::shared_ptr<Jet> > additionalJets;
    bool isMatch;
    int truthMatchPattern;


  private:

  };


  class RecoEvent : public BaseObject{

  public:

    RecoEvent();
    virtual ~RecoEvent();

    std::shared_ptr<Combination>& combinationVariable(std::string);

    void clear();

    int nJets;
    int nBJets;
    int leptonType;
    int nLeptons;
    int nAllLeptons;
    int nNeutrinos;

    //// move to use binary decoration for btag cuts; keep cut value commented for now
    // double m_btaggingCut; //// use this cut for all algos using this object; removed from config to avoid bugs
    std::string m_btagIdName;

  private:

    BaseObjectMap<std::string, std::shared_ptr<Combination> > m_combinationDynVars;


  };


  typedef Container<Composite> CompositeContainer;
  typedef Container<TopComposite> TopCompositeContainer;
  typedef Container<Combination> CombinationContainer;

}

#endif
