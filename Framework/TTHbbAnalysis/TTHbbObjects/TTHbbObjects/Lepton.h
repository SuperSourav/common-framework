/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LEPTON_H_
#define _LEPTON_H_

#include "TLorentzVector.h"
#include "TTHbbObjects/Particle.h"
#include "TTHbbObjects/Containers.h"

namespace TTHbb{

  /// Lepton type enum
  enum class leptonType{
    electron, muon, unknown
      };

  class Lepton : public Particle{
    /**
     * @brief the Lepton object class
     */
    
  public:
    Lepton(float pt, float eta, float phi, float e);
    Lepton();
    virtual ~Lepton();

    virtual Lepton* clone() const;
    virtual Lepton* deepClone() const;

    /// Accessor for lepton type (electron or muon)
    leptonType type(){ return m_type; };
    /// Setting for lepton type
    void type(leptonType a){ m_type = a; };

    //Lepton& operator=(const Lepton& q);

  private:
    leptonType m_type;

  };

  typedef Container<Lepton> LeptonContainer;

}

#endif
