/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "TTHbbObjects/BaseObject.h"
#include <TLorentzVector.h>
#include <memory>

namespace TTHbb{

  //// store type, checking type faster than dynamic_cast
  //// to be extended
  enum class PhysObjectType{BasicParticleType=0,
      LeptonType=1,
      JetType=2,
      NeutrinoType=3,
      LargeJetType=4,
      CompositeType=5,
      TopCompositeType=6,	
      ParticleClusterType=7,
      BasicTruthParticleType=8,
      NPhysObjectType=9,
      CustomObjectType=10}; // Michele

  class Particle : virtual public BaseObject, public TLorentzVector{
    /**
     * @brief A common base class for all particles
     * Contains common functions and properties to all particle objects
     */
    
  public:
    Particle();
    virtual ~Particle();

    virtual Particle* clone() const;
    virtual Particle* deepClone() const;
    virtual void dump() const;

    void objectType(PhysObjectType type){ m_objectType=type; } // Michele
    PhysObjectType objectType(){ return m_objectType; }

    /// not to break things to be removed
    void pdgid(int a){ intVariable("pdgid")=a; }
    int pdgid(){ return intVariable("pdgid"); }

    void truthMatch(int a){ intVariable("truthmatch")=a; }
    int  truthMatch() { return intVariable("truthmatch"); }

    void charge(int a){ floatVariable("charge")=a;  }
    float charge(){ return floatVariable("charge"); }

    TLorentzVector p4() const{ return *this; }
    void p4(const TLorentzVector& a){ SetPtEtaPhiM(a.Pt(),a.Eta(),a.Phi(),a.M()); }

    bool isJet(){return m_objectType == PhysObjectType::JetType;}

    std::vector<std::weak_ptr<Particle> >& particleMatch(std::string);
    bool checkParticleMatch(std::string);

  protected:

    PhysObjectType m_objectType;
    int m_truthMatch;

    BaseObjectMap<std::string, std::vector<std::weak_ptr<Particle> > > m_particleMatchs;

  };

  typedef Container<Particle> ParticleContainer; // Michele

}

#endif

