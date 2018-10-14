/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRUTHPARTICLE_H_
#define _TRUTHPARTICLE_H_

#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/Containers.h"


namespace TTHbb{

  /// extend it with parents and children if needed

  class TruthParticle : public Particle{

  public:
    TruthParticle(double pt, double eta, double phi, double e);
    TruthParticle();
    virtual ~TruthParticle();

    void setHistoryInfoBit(int bit) { long long one=1; m_historyInfo |= one<<bit; }
    void historyInfo(long long info){ m_historyInfo=info; }
    long long historyInfo(){ return m_historyInfo; }

    virtual TruthParticle* clone() const;
    virtual TruthParticle* deepClone() const;

  private:
    long long m_historyInfo;

  };

  typedef Container<TruthParticle> TruthParticleContainer;


}

#endif
