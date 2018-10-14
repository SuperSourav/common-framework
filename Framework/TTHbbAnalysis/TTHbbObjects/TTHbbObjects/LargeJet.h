/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LARGEJET_H_
#define _LARGEJET_H_

#include "TLorentzVector.h"
#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/Containers.h"

namespace TTHbb{

  class LargeJet : public Particle{
    
  public:
    LargeJet(float pt, float eta, float phi, float e, int topTag, float D2, float tau32_wta);
    LargeJet(float pt, float eta, float phi, float e);
    LargeJet();
    virtual ~LargeJet();

    virtual LargeJet* clone() const;
    virtual LargeJet* deepClone() const;

  private:

  };

  typedef Container<LargeJet> LargeJetContainer;
}


#endif
