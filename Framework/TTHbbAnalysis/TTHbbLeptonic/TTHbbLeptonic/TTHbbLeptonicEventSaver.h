/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHBBLEPTONICEVENTSAVER_H_
#define TTHBBLEPTONICEVENTSAVER_H_

#include "TopAnalysis/EventSaverFlatNtuple.h"
#include "TopParticleLevel/ParticleLevelEvent.h"
#include "TTHbbLeptonic/EventSaverHelper.h"

// for trigger prescales
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TTHbbObjects/Event.h"

class TreeManager;

struct outputVar{
  std::string name;
  bool nomOnly;
  bool inTruth;
};

class TTHbbLeptonicEventSaver : public top::EventSaverFlatNtuple {
 public:

  TTHbbLeptonicEventSaver();

  ~TTHbbLeptonicEventSaver();

  using top::EventSaverFlatNtuple::initialize;
  void initialize(std::shared_ptr<top::TopConfig> config, TFile* file, const std::vector<std::string>& extraBranches);

  void saveEvent(const top::Event& event);

  void cleanEventSaver();

  void setupParticleLevelTreeManager(/*const top::ParticleLevelEvent& plEvent*/);

  void saveParticleLevelEvent(const top::ParticleLevelEvent& plEvent);

 private:

  std::shared_ptr<top::TopConfig> m_config;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;// for trigger prescales

  bool m_truthEventVarRegistered;
  bool m_recoEventVarRegistered;

  // int m_nJets;
  bool b_isNominal;

  // for trigger prescales
  std::unordered_map<std::string, float> m_triggerPrescales;

  // static function that prevents the unwanted creation of branches in any eventsaver using the tree manager
  static int getBranchStatus(top::TreeManager const *, std::string const & variableName);

  bool isNominalTree(std::string name);

  ///build an outputVar
  outputVar makeOutputVar(std::string m_name, bool nom=false, bool truth=false);

  /// event dynamic variables
  std::vector<outputVar> m_charVariableNames;  
  std::unique_ptr<std::vector<char> > m_charVariableHolder;

  std::vector<outputVar> m_intVariableNames; 
  std::unique_ptr<std::vector<int> > m_intVariableHolder;
  // std::set<std::string> m_intOldVariableNames; 
  // std::unique_ptr<std::vector<int*> > m_intVariableHolder;


  std::vector<outputVar> m_floatVariableNames;  
  std::unique_ptr<std::vector<float> > m_floatVariableHolder;

  void saveEventVariables(std::shared_ptr<TTHbb::Event> tthevt);
  void saveRecoObjectsVariables(std::shared_ptr<TTHbb::Event> tthevt);
  void saveTruthObjectsVariables(std::shared_ptr<TTHbb::Event> tthevt);


  void registerRecoObjectHelper(const std::set<savevariable>& vars,
				std::vector<outputVar>& charVariableNames,
				std::vector<outputVar>& intVariableNames,
				std::vector<outputVar>& floatVariableNames);

  void registerRecoObjectVariables();
  void registerTruthObjectVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal);

  void registerEventDecoration(bool isTruthParticle);
  void registerBranchCharVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal, bool isTruth=false);
  void registerBranchIntVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal, bool isTruth=false);
  void registerBranchFloatVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal, bool isTruth=false);

  void registerBranchCharVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<char> > >& holders, std::string);
  void registerBranchIntVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<int> > >& holders, std::string);
  void registerBranchFloatVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<float> > >& holders, std::string);

  template <class T>
  void copyCharVectVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<char> > >& holders, T& objects);

  template <class T>
  void copyIntVectVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<int> > >& holders, T& objects);

  template <class T>
  void copyFloatVectVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<float> > >& holders, T& objects);

  template <class T>
  void clearVectVar(T& vars);

  ///// jet decorations /// not dynamic // but can be changed easily // remember most variables are dumped by AT
  std::vector<outputVar> m_charJetVariableNames;  
  std::unique_ptr<std::vector<std::vector<char> > > m_charJetVariableHolder;

  std::vector<outputVar> m_intJetVariableNames;  
  std::unique_ptr<std::vector<std::vector<int> > > m_intJetVariableHolder;

  std::vector<outputVar> m_floatJetVariableNames;  
  std::unique_ptr<std::vector<std::vector<float> > > m_floatJetVariableHolder;

  ///// elec decorations /// not dynamic
  std::vector<outputVar> m_charElecVariableNames;  
  std::unique_ptr<std::vector<std::vector<char> > > m_charElecVariableHolder;

  std::vector<outputVar> m_intElecVariableNames;  
  std::unique_ptr<std::vector<std::vector<int> > > m_intElecVariableHolder;

  std::vector<outputVar> m_floatElecVariableNames;  
  std::unique_ptr<std::vector<std::vector<float> > > m_floatElecVariableHolder;

  ///// muon decorations /// not dynamic
  std::vector<outputVar> m_charMuonVariableNames;  
  std::unique_ptr<std::vector<std::vector<char> > > m_charMuonVariableHolder;

  std::vector<outputVar> m_intMuonVariableNames;  
  std::unique_ptr<std::vector<std::vector<int> > > m_intMuonVariableHolder;

  std::vector<outputVar> m_floatMuonVariableNames;  
  std::unique_ptr<std::vector<std::vector<float> > > m_floatMuonVariableHolder;

  ///// ljet decorations /// not dynamic
  std::vector<outputVar> m_charLJetVariableNames;  
  std::unique_ptr<std::vector<std::vector<char> > > m_charLJetVariableHolder;

  std::vector<outputVar> m_intLJetVariableNames;  
  std::unique_ptr<std::vector<std::vector<int> > > m_intLJetVariableHolder;

  std::vector<outputVar> m_floatLJetVariableNames;  
  std::unique_ptr<std::vector<std::vector<float> > > m_floatLJetVariableHolder;

  //// truth objects, not dynamic
  void fillHFHadrons(TTHbb::TruthParticleContainer& parts);
  void fillPartons(TTHbb::TruthParticleContainer& parts);
  void fillTruthJets(TTHbb::JetContainer& jets);

  std::vector<float> m_truth_hadron_pt;
  std::vector<float> m_truth_hadron_eta;
  std::vector<float> m_truth_hadron_phi;
  std::vector<float> m_truth_hadron_m;
  std::vector<int> m_truth_hadron_pdgid;
  std::vector<int> m_truth_hadron_status;
  std::vector<int> m_truth_hadron_barcode;
  std::vector<int> m_truth_hadron_TopHadronOriginFlag;
  std::vector<char> m_truth_hadron_type;
  std::vector<char> m_truth_hadron_final;
  std::vector<char> m_truth_hadron_initial;
  //std::unordered_map<int, int> m_truth_hadron_indexmap;

  std::vector<float> m_truth_pt;
  std::vector<float> m_truth_eta;
  std::vector<float> m_truth_phi;
  std::vector<float> m_truth_m;
  std::vector<int> m_truth_pdgid;
  std::vector<int> m_truth_status;
  std::vector<int> m_truth_barcode;
  std::vector<long long> m_truth_tthbb_info;
  //std::unordered_map<int, int> m_truth_indexmap;

  std::vector<float> m_truth_jet_pt;
  std::vector<float> m_truth_jet_eta;
  std::vector<float> m_truth_jet_phi;
  std::vector<float> m_truth_jet_m;

  std::vector<int> m_truth_jet_flav;
  std::vector<int> m_truth_jet_id;
  std::vector<int> m_truth_jet_count;


  ClassDef(TTHbbLeptonicEventSaver, 0);
};

#endif
