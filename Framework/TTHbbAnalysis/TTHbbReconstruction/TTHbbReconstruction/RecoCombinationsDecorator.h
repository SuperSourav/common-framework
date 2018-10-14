/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RecoCombinationsDecorator_HH
#define RecoCombinationsDecorator_HH


#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbObjects/Containers.h"

using namespace TTHbb;

class RecoCombinationsDecorator{

public: 

  RecoCombinationsDecorator();
  ~RecoCombinationsDecorator();

  /// split those functions in a better way
  void decorateHiggsCombination(Combination* comb);
  void decorateCombination(Combination* comb);
  void decorateInCompleteCombination(Combination* comb);
  void decorateTTbarCombination(Combination* comb);

  void decorateDilepCombination(Combination* comb);
  void decorateTTbarDilepCombination(Combination* comb);

  Composite* createIncompLepTop(std::shared_ptr<Lepton> lep,std::shared_ptr<Jet> jet, bool isMC);
  Composite* createLepW(std::shared_ptr<Lepton> lep,std::shared_ptr<Neutrino> nu, bool isMC);

  Composite* createBoostedHadW(std::shared_ptr<Jet> jet1, bool isMC);
  Composite* createHadW(std::shared_ptr<Jet> jet1,std::shared_ptr<Jet> jet2, bool isMC);
  Composite* createHiggs(std::shared_ptr<Jet> jet1,std::shared_ptr<Jet> jet2, bool isMC);
  TopComposite* createLepTop(std::shared_ptr<Jet> j, std::shared_ptr<Composite> w, bool isMC);
  TopComposite* createHadTop(std::shared_ptr<Jet> j, std::shared_ptr<Composite> w, bool isMC);
  Composite* createIncompleteHadTop(std::shared_ptr<Jet> jet1,std::shared_ptr<Jet> jet2, bool isMC);

  Combination* createCombination(std::shared_ptr<TopComposite> ltop, std::shared_ptr<TopComposite> htop, std::shared_ptr<Composite> higgs, bool isMC);
  Combination* createIncompleteCombination(std::shared_ptr<TopComposite> ltop, std::shared_ptr<Composite> htop, std::shared_ptr<Composite> higgs, bool isMC);
  Combination* createCombination(std::shared_ptr<TopComposite> ltop, std::shared_ptr<TopComposite> htop, bool isMC);


  Combination* createCombination(std::shared_ptr<Composite> ltop1, 
				 std::shared_ptr<Composite> ltop2, 
				 std::shared_ptr<Composite> higgs,
         double met_met, double met_phi,
				 int eventNumber, bool isMC);

  Combination* createCombination(std::shared_ptr<Composite> ltop1, 
				 std::shared_ptr<Composite> ltop2, 
         double met_met, double met_phi,
				 int eventNumber, bool isMC);


  void fillAdditionalJets(JetContainer* jets, Combination* comb);
  void decorateAdditionalJets(Combination* comb);

  std::vector<std::string> m_contBTagNames;

  float cos_theta_transverse(TLorentzVector top, TLorentzVector parent_t, TLorentzVector ttbar, TLorentzVector lep, float sign);
  float cos_theta_raxis(TLorentzVector top, TLorentzVector parent_t, TLorentzVector ttbar, TLorentzVector lep, float sign);
  float cos_theta_helicity(TLorentzVector top, TLorentzVector parent_t, TLorentzVector ttbar, TLorentzVector lep, float sign);

private:

  /// split those functions in a better way
  void decorateCommonCombination(Combination* comb);

};

#endif //// RecoCombinationsDecorator
