/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
#include <vector>
#include <map>
#include "TVector3.h"
#include "TLorentzVector.h"
#include "MVAVariables/PairedSystem.h"
#include "TTHbbObjects/Event.h"


#ifndef MVAVARIABLES_H_
#define MVAVARIABLES_H_

enum class variable{ MaxPt, MinPt, MaxdEta, MindEta, MaxdPhi, MindPhi, MaxdR, MindR, MaxM, MinM, Avg,
    HiggsMass, Wmass };
enum class pairing{ jj, bj, bb, bu, uu, jl, bl, ul };

enum class shapefunc{Aplanarity, Aplanority, Sphericity, Spherocity, SphericityT, Planarity,
    Variable_C, Variable_D, Circularity, PlanarFlow };
enum class tensor{ATensor, TTensor, OTensor};
enum class collection{all, jets, bjets, ujets};

static const float higgsMass = 125000.;
static const float Wmass = 80400.;

class MVAVariables{
  /**
   * @brief A class which works as a calculator for a large number of variables
   * useful in MVA techniques.
   * The bulk of the calculations are all done behind the scene and the user
   * is provided with simple accessor functions for the variables.
   * It has been set up to only perform the heavy calculations on the first
   * request of a variable of that sort.
   * It requires that the user initialises the tool with either a TTHbb::Event
   * or the individual vectors of collections, all using the TTHbb EDM.
   */
 public:
  
  MVAVariables();
  ~MVAVariables();

  /// set the btaggingWP, assuming the string is "isbtagged_wp"
  void setBtagging(std::string wp);
  /// full decoration string
  void setBtagString(std::string wp);

  /// initialise the event with the TTHbb::Event object
  void initialiseEvent(TTHbb::Event* event);
  /// initialise the event with collections
  void initialiseEvent(std::vector< std::shared_ptr<TTHbb::Particle> >& jets,
		       std::vector< std::shared_ptr<TTHbb::Particle> >& bjets,
		       std::vector< std::shared_ptr<TTHbb::Particle> >& ujets,
		       std::vector< std::shared_ptr<TTHbb::Particle> >& leps,
		       std::vector< std::shared_ptr<TTHbb::Particle> >& all,
		       float metmet, float metphi);
  
  /// clear all the vectors to make sure nothing is duplicated on a new entry
  void cleanVectors();

  /// Access function for the pairing which satisfies the given variable
  PairedSystem getEntry(pairing p, variable v);
  /// Access function for the variable s for the collection c
  float getShape(const collection& c, const shapefunc& s);

  /// Get the dilepton system
  PairedSystem getDilepton();

  /// Get the triplet from collection c that satisfies variable b
  PairedSystem getTriplet(collection c, variable b);
  /// Get the quadruplet from collection c that satisfies variable b
  PairedSystem getQuadruplet(collection c, variable b);
  
  /// Function to get the ith jet from collection c, sorted by pT. i starts at 1.
  std::shared_ptr<TTHbb::Particle> getPtOrderedJet(collection c, unsigned int i);
  std::shared_ptr<TTHbb::Particle> getPtOrderedLepton(unsigned int i);

  /// Get a copy of all the jets, sorted by mv2
  std::vector< std::shared_ptr<TTHbb::Particle> > getMV2OrderedJetContainer();

  //variable functions

  /// Get number of jets
  int nJets(){ return jetVecs.size(); }
  /// Get number of jets above pT threshold (in GeV)
  int nJetsAbovePt(float Pt);
  //int nTrackJets(){ return m_event->m_trackJets.size(); }
  //int nTrackJetsAbovePt(float Pt);
  /// Get number of btags for the WP specified in this implementation
  int nbTag();
  float HT(collection c);
  float Meff(collection c);
  float Centrality(collection c);
  float MCT();
  float HhadT_frac_met();
  float HhadT_frac_lep();

  /// Get the number of candidate pairs from pairing p within mass M (in GeV) of the Higgs mass
  int nHiggsCandidatesMassWindow(pairing p, int M);
  /// Get the number of candidate pairs from pairing p within mass M (in GeV) of the W mass
  int nWCandidatesMassWindow(pairing p, int M);

  //Fox wolfram functions
  /// Get the value of the Fox Wolfram moment for collection c
  float FirstFoxWolframMoment(collection c) { return FoxWolfram(0, false, c); }
  float SecondFoxWolframMoment(collection c){ return FoxWolfram(1, false, c); }
  float ThirdFoxWolframMoment(collection c) { return FoxWolfram(2, false, c); }
  float FourthFoxWolframMoment(collection c){ return FoxWolfram(3, false, c); }
  float FifthFoxWolframMoment(collection c) { return FoxWolfram(4, false, c); }
  
  /// Get the value of the Transverse Fox Wolfram moment for collection c
  float FirstFoxWolframTransverseMoment(collection c) { return FoxWolfram(0, true, c); }
  float SecondFoxWolframTransverseMoment(collection c){ return FoxWolfram(1, true, c); }
  float ThirdFoxWolframTransverseMoment(collection c) { return FoxWolfram(2, true, c); }
  float FourthFoxWolframTransverseMoment(collection c){ return FoxWolfram(3, true, c); }
  float FifthFoxWolframTransverseMoment(collection c) { return FoxWolfram(4, true, c); }
  
  // Thrust
  /// Get the valuefor the thrust of collection c
  float getThrust(collection c){ return setThrust(c).first; }
  /// Get the Axis of the thrust of collection c
  TVector3 getThrustAxis(collection c){ return setThrust(c).second; }
  
  // paired variables
  float PtofPair(pairing p, variable v)      { return getEntry(p,v).Pt(); }
  float MassofPair(pairing p, variable v)    { return getEntry(p,v).M();  }
  float PtSumofPair(pairing p, variable v)   { return getEntry(p,v).SumPt();  }
  float deltaRofPair(pairing p, variable v)  { return getEntry(p,v).DeltaR(); }
  float deltaPhiofPair(pairing p, variable v){ return getEntry(p,v).DeltaPhi(); }
  float deltaEtaofPair(pairing p, variable v){ return getEntry(p,v).DeltaEta(); }

  // dilepton accessor
  float DileptonPt()   { return getDilepton().Pt(); }
  float DileptonMass() { return getDilepton().M();  }
  float DileptonSumPt(){ return getDilepton().SumPt();  }
  float DileptondR()   { return getDilepton().DeltaR(); }
  float DileptondPhi() { return getDilepton().DeltaPhi(); }
  float DileptondEta() { return getDilepton().DeltaEta(); }

  // triplet accessors
  float MassofJetTriplet(variable v){ return getTriplet(collection::jets,v).M(); }
  float MassofBJetTriplet(variable v){ return getTriplet(collection::bjets,v).M(); }
  float PtofJetTriplet(variable v){return getTriplet(collection::jets,v).Pt(); }

  // quadruplet accessors
  float MassofJetQuadruplet(variable v){ return getQuadruplet(collection::jets, v).M(); }
  float MassofBJetQuadruplet(variable v){ return getQuadruplet(collection::bjets, v).M(); }

  // Shape functions from E-M tensor
  float Aplanarity(const collection&c) { return getShape(c, shapefunc::Aplanarity);  }
  float Aplanority(const collection&c) { return getShape(c, shapefunc::Aplanority);  }
  float Sphericity(const collection&c) { return getShape(c, shapefunc::Sphericity);  }
  float Spherocity(const collection&c) { return getShape(c, shapefunc::Spherocity);  }
  float SphericityT(const collection&c){ return getShape(c, shapefunc::SphericityT); }
  float Planarity(const collection&c)  { return getShape(c, shapefunc::Planarity);   }
  float Variable_C(const collection&c) { return getShape(c, shapefunc::Variable_C);  }
  float Variable_D(const collection&c) { return getShape(c, shapefunc::Variable_D);  }
  float Circularity(const collection&c){ return getShape(c, shapefunc::Circularity); }
  float PlanarFlow(const collection&c) { return getShape(c, shapefunc::PlanarFlow);  }
  
  //dPhi of an object with the MET
  float deltaPhiMet(std::shared_ptr<TLorentzVector> t){ return TVector2::Phi_mpi_pi(t->Phi() - met_phi); };

 protected:

  /// Set the pairs and tensors for current event
  void SetPairs();
  void DefinePairs();
  void EvaluatePairs();

  void SetTensors();
  void DefineTensorVariables();
  void EvaluateTensors();

  //event
  //std::shared_ptr<TTHbb::Event> m_event;
  //TTHbb::Event* m_event;

  /// name of decoration for jets to say whether btagged
  std::string m_btagCheckString;

  /// bools for whether calculated for this event
  /// only calculate when asked, speed up code
  bool b_pairsCalculated;
  bool b_tensorsCalculated;

  /// Collections

  std::vector< std::shared_ptr<TTHbb::Particle> > jetVecs;
  std::vector< std::shared_ptr<TTHbb::Particle> > bjetVecs;
  std::vector< std::shared_ptr<TTHbb::Particle> > ujetVecs;
  std::vector< std::shared_ptr<TTHbb::Particle> > lepVecs;
  std::vector< std::shared_ptr<TTHbb::Particle> > allVecs;

  /// Triplet collections
  std::map<collection,std::vector<PairedSystem>> tripVecs;
  /// Quadruplet collections
  std::map<collection,std::vector<PairedSystem>> quadVecs;

  /// MET
  float met_met;
  float met_phi;
  
  ///Heavy work functions
  ///Tensor eignevalues
  std::vector< std::vector<float> > CalculateEigenvals(std::vector< std::shared_ptr<TTHbb::Particle> >& Collection);
  
  ///Fox Wolfram
  float FoxWolfram(int order, bool transv, std::vector< std::shared_ptr<TTHbb::Particle> >& particles);
  float FoxWolfram(int order, bool tranvs, collection c);
  
  ///Thrust
  std::pair<float,TVector3> setThrust(std::vector< std::shared_ptr<TTHbb::Particle> >& particles);
  std::pair<float,TVector3> setThrust(collection c);

  /// Make the triplet vectors for a given collection
  void makeTriplets(collection c);
  /// Make the quadruplet vectors for a given collection
  void makeQuadruplets(collection c);

  /// Maps and typedefs for making pair and tensor variables easier to handle

  ///pairs
  typedef std::pair<variable, std::function< PairedSystem(std::vector<PairedSystem>)> > vpair;
  typedef std::pair<pairing, std::vector<PairedSystem> > pvector;
  typedef std::pair<pairing, variable> entry;
  
  std::map<entry, PairedSystem> mapping;
  std::map<pairing, std::vector<PairedSystem> > vectors;
  std::map<vpair::first_type, vpair::second_type> functions;

  ///matrix
  typedef std::map<tensor, std::vector<float> > eigenvalues;
  typedef std::pair<tensor, std::vector<float> > eigenpair;
  typedef std::pair<shapefunc, std::function<float(eigenvalues)> > spair;
  typedef std::pair<collection, eigenvalues> cmap;
  typedef std::pair<collection, shapefunc> shape; 

  std::map<shape, float> eventshape;
  std::map<collection, eigenvalues> collections;
  std::map<spair::first_type, spair::second_type> shapefunctions;
};

#endif

//-------------------------------------//
