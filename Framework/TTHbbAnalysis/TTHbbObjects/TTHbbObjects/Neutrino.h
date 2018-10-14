/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NEUTRINO_H_
#define _NEUTRINO_H_

#include "TLorentzVector.h"
#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/Containers.h"

namespace TTHbb{

  class Neutrino : public Particle{
    
  public:
    Neutrino(float pt, float eta, float phi, float e);
    Neutrino();
    virtual ~Neutrino();

    virtual Neutrino* clone() const;
    virtual Neutrino* deepClone() const;

    //Neutrino& operator=(const Neutrino& q);

  private:

  };

  typedef Container<Neutrino> NeutrinoContainer;
}

#endif
