/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _JET_H_
#define _JET_H_

#include "TLorentzVector.h"
#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/Containers.h"

namespace TTHbb{

  class Jet : public Particle{
    
  public:
    Jet(float pt, float eta, float phi, float e);
    Jet();
    virtual ~Jet();

    virtual Jet* clone() const;
    virtual Jet* deepClone() const;
    
    //Jet& operator=(const Jet & q);

  private:

  };

  typedef Container<Jet> JetContainer;
}


#endif
