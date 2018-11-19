/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _EVENT_H_
#define _EVENT_H_

#include "TTHbbObjects/BaseObject.h"
#include "TTHbbObjects/Jet.h"
#include "TTHbbObjects/Lepton.h"
#include "TTHbbObjects/LargeJet.h"
#include "TTHbbObjects/Neutrino.h"
#include "TTHbbObjects/EventData.h"
#include "TTHbbObjects/TruthParticle.h"
#include "TTHbbObjects/Bucket.h"

#include <vector>
#include <memory>

namespace TTHbb{

  class Combination;

  class Event : virtual public BaseObject{
    /**
     * @brief Event class for all the objects. Needs to be passed to the tools.
     */


  public:
    Event();
    virtual ~Event();

    /// Functions to add either new objects or a copy
    std::shared_ptr<Jet> addJet(float pt, float eta, float phi, float e, float mv2, float jvt);
    void addJet(std::shared_ptr<Jet> j);

    std::shared_ptr<Lepton> addLepton(float pt, float eta, float phi, float e);
    void addLepton(std::shared_ptr<Lepton> l);

    std::shared_ptr<LargeJet> addLargeJet(float pt, float eta, float phi, float e, int topTag, float D2, float tau32_wta);
    void addLargeJet(std::shared_ptr<LargeJet> j);

    void addElectron(std::shared_ptr<Lepton> l);
    void addMuon(std::shared_ptr<Lepton> l);

    std::shared_ptr<Particle> addCustomObject(std::string name); // Michele
    void addCustomObject(std::string name,std::shared_ptr<Particle> o); // Michele

    void addBucket(Bucket b); 

    inline void recoAvailable(bool a){m_reco=a;}
    inline bool recoAvailable(){return m_reco;}

    inline void truthAvailable(bool a){m_truth=a;}
    inline bool truthAvailable(){return m_truth;}

    void clearReco(); //// will also clear decorations
    void clearTruth();
    void clearDecorations();
    void clearAll();

    /// Containers of the objects in the event
    JetContainer m_jets;
    LeptonContainer m_leptons;
    LeptonContainer m_electrons;
    LeptonContainer m_muons;
    LargeJetContainer m_largeJets;
    std::unordered_map<std::string,ParticleContainer> m_customObj; // Michele
    float met_met;
    float met_phi;

    //Container for buckets of top in the event
    std::vector<Bucket> m_BucketCont;

    /// Containers for objects not created initially
    NeutrinoContainer m_neutrinos;

    std::shared_ptr<TTHbb::EventData> m_info;

    //// truth containers
    TruthParticleContainer m_partons;
    TruthParticleContainer m_HFHadrons;
    JetContainer           m_truthJets;

    /// make reco life easier in common code
    std::shared_ptr<Combination>& combinationVariable(std::string);

  private:

    BaseObjectMap<std::string, std::shared_ptr<Combination> > m_combinationDynVars;
    bool m_reco;
    bool m_truth;
  };

}

#endif
