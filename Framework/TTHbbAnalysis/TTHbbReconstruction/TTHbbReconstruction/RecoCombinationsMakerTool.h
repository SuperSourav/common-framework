/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RecoCombinationsMakerTool_HH
#define RecoCombinationsMakerTool_HH

#include "TTHbbReconstruction/RecoCombinationsDecorator.h"

using namespace TTHbb;

class RecoCombinationsMakerTool{

public:

  RecoCombinationsMakerTool();
  virtual ~RecoCombinationsMakerTool();

  /// config
  bool m_allowBJetsForW; /// always not just in the case below
  bool m_allowBJetsForWAtLowNL; /// only if njets>=6 and nlight<2
  bool m_allowLightJetsAsB; /// only if nb>=4 otherwise always allowed
  bool m_forceTaggedJetsAsB; /// only if nb<4 otherwise use allowLightJetsAsB
  bool m_orderByWeightAtLargeNB; /// if nb>=5
  bool m_forceOneBJetsInHiggs; /// at least one b in higgs
  int m_allowedOrderedWeightForBCut;
  int m_allowedOrderedWeightForLightCut;
  bool m_useBTagForComb; 
  std::string m_btagOrderVariableName;

  std::string m_forceUseForLJetsName; /// external decoration of jets
  std::string m_forceUseForBJetsName; /// external decoration of jets

  /// public to configure it
  RecoCombinationsDecorator m_combinationsDecorator;
  //// ... add more config 


  ///// depending on framework
  bool m_isMC;  
  std::string m_btagIdName;
  // startegies for combinations
  
  bool buildStrategyTTHBoostedSemiLep(JetContainer* jets);
  bool buildStrategyTTHSemiLep(JetContainer* jets);
  bool buildStrategyTTHDiLep(JetContainer* jets);
  bool buildStrategyTTBarSemiLep(JetContainer* jets);
  bool buildStrategyTTBarDiLep(JetContainer* jets);

  /// composite containers
  CompositeContainer* createLepWContainer(LeptonContainer* leptons, NeutrinoContainer* neutrinos, std::string contname="");
  
  CompositeContainer* createBoostedHadWContainer(JetContainer* jets, std::string contname="");

  CompositeContainer* createHadWContainer(JetContainer* jets, std::string contname="");
  CompositeContainer* createHiggsContainer(JetContainer* jets, std::string contname="");

  TopCompositeContainer* createLepTopContainer(CompositeContainer* lepWContainer, JetContainer* jets, std::string contname="");
  CompositeContainer* createBoostedHadTopContainer(CompositeContainer* hadWContainer, JetContainer* jets, std::string contname="");

  TopCompositeContainer* createHadTopContainer(CompositeContainer* hadWContainer, JetContainer* jets, std::string contname="");

  CompositeContainer* createIncompLepTopContainer(std::shared_ptr<Lepton> lepton, JetContainer* jets, std::string contname="");
  CompositeContainer* createIncompleteHadTopContainer(JetContainer* jets, std::string contname="");

  /// ttH combinations
  CombinationContainer* createCombinations(TopCompositeContainer* lepTopContainer, TopCompositeContainer* hadTopContainer, CompositeContainer* higgsContainer, std::string contname="");
  CombinationContainer* createIncompleteCombinations(TopCompositeContainer* lepTopContainer, CompositeContainer* incompleteHadTopContainer, CompositeContainer* higgsContainer, std::string contname="");
  CombinationContainer* createCombinations(CompositeContainer* lepTop1Container, CompositeContainer* lepTop2Container, CompositeContainer* higgsContainer, double met_met, double met_phi, int eventNumber, std::string contname="");

  /// ttbar combinations
  CombinationContainer* createCombinations(TopCompositeContainer* lepTopContainer, TopCompositeContainer* hadTopContainer, JetContainer* jets, std::string contname="");
  CombinationContainer* createCombinations(CompositeContainer* lepTop1Container, CompositeContainer* lepTop2Container, JetContainer* jets, std::string contname="");

  protected:

  bool isBJet(Jet* jet);
  bool useAsLJet(Jet* jet);
  bool useAsBJet(Jet* jet);
  bool useAsBJetTTbar(Jet* jet);
  bool useAsLJetTTbar(Jet* jet);


  bool jetOverlap(Composite* comp1, Composite* comp2);
  bool jetOverlap(TopComposite* comp1, Composite* comp2);
  bool jetOverlap(Composite* comp1, TopComposite* comp2);
  bool jetOverlap(TopComposite* comp1, TopComposite* comp2);

  int m_nJets; 
  int m_nBJets;

  int m_neededJets;
  int m_neededB;
  int m_neededLight;
  int m_nUseBJets;
  int m_nUseLJets;


};

#endif
