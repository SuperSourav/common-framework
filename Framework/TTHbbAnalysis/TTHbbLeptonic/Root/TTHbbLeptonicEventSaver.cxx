/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbLeptonicEventSaver.h"
#include "TopEvent/Event.h"
#include "TopEventSelectionTools/TreeManager.h"
#include "TopConfiguration/TopConfig.h"

#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/SelectionConfigurationData.h"

#include "TTHbbObjects/Event.h"


//for lepton truth-matching
#include "xAODEgamma/EgammaTruthxAODHelpers.h" // for xAOD::EgammaHelpers::getBkgElectronMother
#include "xAODTruth/xAODTruthHelpers.h" // for xAOD::TruthHelpers::getParticleTruthType(elPtr), getParticleTruthOrigin(elPtr), getTruthParticle(elPtr)
#include "xAODTruth/TruthParticle.h"

#include <TLorentzVector.h>
#include <TMath.h>
#include <cmath>
#include <algorithm>
#include "boost/algorithm/string.hpp"

TTHbbLeptonicEventSaver::TTHbbLeptonicEventSaver() :
  m_trigDecisionTool("Trig::TrigDecisionTool")
{
  // m_intVariableHolder.reset(new std::vector<int*>);

  m_truthEventVarRegistered=false;
  m_recoEventVarRegistered=false;

  branchFilters().push_back(std::bind(&getBranchStatus, std::placeholders::_1, std::placeholders::_2));
}

TTHbbLeptonicEventSaver::~TTHbbLeptonicEventSaver(){}

void TTHbbLeptonicEventSaver::initialize(std::shared_ptr<top::TopConfig> config, TFile* file, const std::vector<std::string>& extraBranches) {


  EventSaverFlatNtuple::initialize(config, file, extraBranches);
  m_config = config;

  // auto* const settings = top::ConfigurationSettings::get();
  registerRecoObjectVariables();
  registerEventDecoration(false);

  //// old style
  for (auto systematicTree : treeManagers()){

    bool isNominal = isNominalTree(systematicTree->name());
    registerTruthObjectVariables(systematicTree, isNominal);

    //// variables not in ttHbb::Event decoration
    //systematicTree->makeOutputVariable(m_nJets,          "nJets");
  }

}

//// register functions

void TTHbbLeptonicEventSaver::registerTruthObjectVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal){

  if (!m_config->isMC()) return;

  /// all in niminal for now
  if(isNominal){

    systematicTree->makeOutputVariable(m_truth_hadron_pt, "truth_hadron_pt");
    systematicTree->makeOutputVariable(m_truth_hadron_eta, "truth_hadron_eta");
    systematicTree->makeOutputVariable(m_truth_hadron_phi, "truth_hadron_phi");
    systematicTree->makeOutputVariable(m_truth_hadron_m, "truth_hadron_m");
    systematicTree->makeOutputVariable(m_truth_hadron_pdgid, "truth_hadron_pdgid");
    systematicTree->makeOutputVariable(m_truth_hadron_status, "truth_hadron_status");
    systematicTree->makeOutputVariable(m_truth_hadron_barcode, "truth_hadron_barcode");
    systematicTree->makeOutputVariable(m_truth_hadron_TopHadronOriginFlag, "truth_hadron_TopHadronOriginFlag");

    systematicTree->makeOutputVariable(m_truth_hadron_type, "truth_hadron_type");
    systematicTree->makeOutputVariable(m_truth_hadron_final, "truth_hadron_final");
    systematicTree->makeOutputVariable(m_truth_hadron_initial, "truth_hadron_initial");

    systematicTree->makeOutputVariable(m_truth_pt, "truth_pt");
    systematicTree->makeOutputVariable(m_truth_eta, "truth_eta");
    systematicTree->makeOutputVariable(m_truth_phi, "truth_phi");
    systematicTree->makeOutputVariable(m_truth_m, "truth_m");
    systematicTree->makeOutputVariable(m_truth_pdgid, "truth_pdgid");
    systematicTree->makeOutputVariable(m_truth_status, "truth_status");
    systematicTree->makeOutputVariable(m_truth_barcode, "truth_barcode");
    systematicTree->makeOutputVariable(m_truth_tthbb_info, "truth_tthbb_info");

    systematicTree->makeOutputVariable(m_truth_jet_pt, "truth_jet_pt");
    systematicTree->makeOutputVariable(m_truth_jet_eta, "truth_jet_eta");
    systematicTree->makeOutputVariable(m_truth_jet_phi, "truth_jet_phi");
    systematicTree->makeOutputVariable(m_truth_jet_m, "truth_jet_m");
    systematicTree->makeOutputVariable(m_truth_jet_flav, "truth_jet_flav");
    systematicTree->makeOutputVariable(m_truth_jet_id, "truth_jet_id");
    systematicTree->makeOutputVariable(m_truth_jet_count, "truth_jet_count");
  }
 
}

void TTHbbLeptonicEventSaver::registerRecoObjectHelper(const std::set<savevariable>& vars,
						       std::vector<outputVar>& charVariableNames,
						       std::vector<outputVar>& intVariableNames,
						       std::vector<outputVar>& floatVariableNames){

  for(auto var : vars){
    if(var.type == TTHbb::DecorationType::Char){
      charVariableNames.push_back(makeOutputVar(var.name,var.nominalOnly));
    }
    else if(var.type == TTHbb::DecorationType::Int){
      intVariableNames.push_back(makeOutputVar(var.name,var.nominalOnly));
    }
    else if(var.type == TTHbb::DecorationType::Float){
      floatVariableNames.push_back(makeOutputVar(var.name,var.nominalOnly));
    }
  }

}


void TTHbbLeptonicEventSaver::registerRecoObjectVariables(){

  /// can hardcode or get from ehs (recommended)
  auto* ehs = EventSaverHelper::get();
  const auto vars_jet = ehs->setOfJetVariables();
  const auto vars_elec = ehs->setOfElecVariables();
  const auto vars_muon = ehs->setOfMuonVariables();
  const auto vars_ljet = ehs->setOfLJetVariables();


  /// jet variables

  registerRecoObjectHelper(vars_jet, m_charJetVariableNames, m_intJetVariableNames, m_floatJetVariableNames);
  /// this is bad, do it in your tool
  ///m_floatJetVariableNames.push_back(makeOutputVar("theTomatos",false));

  registerBranchCharVariables(m_charJetVariableNames, m_charJetVariableHolder, "jet_");
  registerBranchIntVariables(m_intJetVariableNames, m_intJetVariableHolder, "jet_");
  registerBranchFloatVariables(m_floatJetVariableNames, m_floatJetVariableHolder, "jet_");

  /// elec variables

  registerRecoObjectHelper(vars_elec, m_charElecVariableNames, m_intElecVariableNames, m_floatElecVariableNames);
  registerBranchCharVariables(m_charElecVariableNames, m_charElecVariableHolder, "el_");
  registerBranchIntVariables(m_intElecVariableNames, m_intElecVariableHolder, "el_");
  registerBranchFloatVariables(m_floatElecVariableNames, m_floatElecVariableHolder, "el_");


  /// muon variables

  registerRecoObjectHelper(vars_muon, m_charMuonVariableNames, m_intMuonVariableNames, m_floatMuonVariableNames);
  registerBranchCharVariables(m_charMuonVariableNames, m_charMuonVariableHolder, "mu_");
  registerBranchIntVariables(m_intMuonVariableNames, m_intMuonVariableHolder, "mu_");
  registerBranchFloatVariables(m_floatMuonVariableNames, m_floatMuonVariableHolder, "mu_");

  /// ljet registerBranchCharVariables
  registerRecoObjectHelper(vars_ljet, m_charLJetVariableNames, m_intLJetVariableNames, m_floatLJetVariableNames);

  registerBranchCharVariables(m_charLJetVariableNames, m_charLJetVariableHolder, "ljet_");
  registerBranchIntVariables(m_intLJetVariableNames, m_intLJetVariableHolder, "ljet_");
  registerBranchFloatVariables(m_floatLJetVariableNames, m_floatLJetVariableHolder, "ljet_");
  //// very important: never clear/push_pack/copy/reset/... holders after you register them

}

void TTHbbLeptonicEventSaver::registerEventDecoration(bool isTruthParticle){

  ////this will happen only at the first event to define the list of decorated variables (make sure all variables are defined in all events, use dummy values when it is not)
  ////if the list of variables is already defined (e.g. read from text file at init) this list will be used and not all decorated variables


  if(!(m_floatVariableNames.size() + m_intVariableNames.size() + m_charVariableNames.size())){

    auto* ehs = EventSaverHelper::get();
    const auto vars = ehs->setOfVariables();

    for(auto var : vars){
      if(var.type == TTHbb::DecorationType::Char){
	m_charVariableNames.push_back(makeOutputVar(var.name,var.nominalOnly, var.truthTree));
      }
      else if(var.type == TTHbb::DecorationType::Int){
	m_intVariableNames.push_back(makeOutputVar(var.name,var.nominalOnly, var.truthTree));
      }
      else if(var.type == TTHbb::DecorationType::Float){
	m_floatVariableNames.push_back(makeOutputVar(var.name,var.nominalOnly, var.truthTree));
      }
    }
    m_charVariableHolder.reset(new std::vector<char>(m_charVariableNames.size(),-9));
    m_intVariableHolder.reset(new std::vector<int>(m_intVariableNames.size(),-9));
    m_floatVariableHolder.reset(new std::vector<float>(m_floatVariableNames.size(),-9));
    //// very important: never clear/push_pack/copy/reset/... m_intVariableHolder after this point

  }

  if(isTruthParticle && !m_truthEventVarRegistered){
    registerBranchCharVariables(particleLevelTreeManager() , true, true);
    registerBranchIntVariables(particleLevelTreeManager()  , true, true);
    registerBranchFloatVariables(particleLevelTreeManager(), true, true);
    m_truthEventVarRegistered=true;
  }
  
  if(!isTruthParticle && !m_recoEventVarRegistered){
    for(auto systematicTree : treeManagers()){
      bool isNominal = isNominalTree(systematicTree->name());
      registerBranchCharVariables(systematicTree, isNominal);
      registerBranchIntVariables(systematicTree, isNominal);
      registerBranchFloatVariables(systematicTree, isNominal);
    }
    m_recoEventVarRegistered=true;
  }

}


void TTHbbLeptonicEventSaver::registerBranchCharVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal, bool isTruth){
  for(unsigned int i=0; i<m_charVariableNames.size(); ++i){
    if(m_charVariableNames[i].nomOnly && !isNominal) continue;
    if(!m_charVariableNames[i].inTruth &&  isTruth ) continue;
    systematicTree->makeOutputVariable(m_charVariableHolder->at(i),m_charVariableNames[i].name);
  }
}

void TTHbbLeptonicEventSaver::registerBranchIntVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal, bool isTruth){
  for(unsigned int i=0; i<m_intVariableNames.size(); ++i){
    if(m_intVariableNames[i].nomOnly && !isNominal) continue;
    if(!m_intVariableNames[i].inTruth &&  isTruth ) continue;
    systematicTree->makeOutputVariable(m_intVariableHolder->at(i),m_intVariableNames[i].name);
  }
}

void TTHbbLeptonicEventSaver::registerBranchFloatVariables(std::shared_ptr<top::TreeManager> systematicTree, bool isNominal, bool isTruth){
  for(unsigned int i=0; i<m_floatVariableNames.size(); ++i){
    if(m_floatVariableNames[i].nomOnly && !isNominal) continue;
    if(!m_floatVariableNames[i].inTruth &&  isTruth ) continue;
    systematicTree->makeOutputVariable(m_floatVariableHolder->at(i),m_floatVariableNames[i].name);
  }
}


///// save functions

void TTHbbLeptonicEventSaver::saveEventVariables(std::shared_ptr<TTHbb::Event> tthevt){

  for(unsigned int i=0; i<m_charVariableNames.size(); ++i){
    m_charVariableHolder->at(i) = tthevt->charVariable(m_charVariableNames.at(i).name);
  }
  for(unsigned int i=0; i<m_intVariableNames.size(); ++i){
    m_intVariableHolder->at(i) = tthevt->intVariable(m_intVariableNames.at(i).name);
  }
  for(unsigned int i=0; i<m_floatVariableNames.size(); ++i){
    m_floatVariableHolder->at(i) = tthevt->floatVariable(m_floatVariableNames.at(i).name);
  }

}

void TTHbbLeptonicEventSaver::saveRecoObjectsVariables(std::shared_ptr<TTHbb::Event> tthevt){

  copyCharVectVariables(m_charJetVariableNames, m_charJetVariableHolder, tthevt->m_jets);
  copyIntVectVariables(m_intJetVariableNames, m_intJetVariableHolder, tthevt->m_jets);
  copyFloatVectVariables(m_floatJetVariableNames, m_floatJetVariableHolder, tthevt->m_jets);

  copyCharVectVariables(m_charElecVariableNames, m_charElecVariableHolder, tthevt->m_electrons);
  copyIntVectVariables(m_intElecVariableNames, m_intElecVariableHolder, tthevt->m_electrons);
  copyFloatVectVariables(m_floatElecVariableNames, m_floatElecVariableHolder, tthevt->m_electrons);

  copyCharVectVariables(m_charMuonVariableNames, m_charMuonVariableHolder, tthevt->m_muons);
  copyIntVectVariables(m_intMuonVariableNames, m_intMuonVariableHolder, tthevt->m_muons);
  copyFloatVectVariables(m_floatMuonVariableNames, m_floatMuonVariableHolder, tthevt->m_muons);

  copyCharVectVariables(m_charLJetVariableNames, m_charLJetVariableHolder, tthevt->m_largeJets);
  copyIntVectVariables(m_intLJetVariableNames, m_intLJetVariableHolder, tthevt->m_largeJets);
  copyFloatVectVariables(m_floatLJetVariableNames, m_floatLJetVariableHolder, tthevt->m_largeJets);

}

void TTHbbLeptonicEventSaver::saveTruthObjectsVariables(std::shared_ptr<TTHbb::Event> tthevt){

  if(!m_config->isMC()) return;

  fillHFHadrons(tthevt->m_HFHadrons);
  fillPartons(tthevt->m_partons);
  fillTruthJets(tthevt->m_truthJets);

}



void TTHbbLeptonicEventSaver::saveEvent(const top::Event& event){

  b_isNominal = (m_config->nominalHashValue() == event.m_hashValue );

  std::shared_ptr<TTHbb::Event> tthevt;
  if(event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
  }

  if (m_config->saveOnlySelectedEvents() && !event.m_saveEvent){
    if(tthevt)tthevt->clearReco();
    return;
  }
  if(!m_config->saveOnlySelectedEvents()){
    EventSaverFlatNtuple::saveEvent(event);
    if(tthevt)tthevt->clearReco();
    return;
  }

  if(!tthevt){
    std::cout << "TTHbbLeptonicEventSaver: TTHbbEventVariables (TTHbb::Event*) object not found" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  //// OK everything set
  //// clean, copy and save
  cleanEventSaver();


  saveEventVariables(tthevt);
  saveRecoObjectsVariables(tthevt);
  saveTruthObjectsVariables(tthevt);


  /// other variables not in TTHbb::Event decoration
  // if(event.m_info->isAvailable<int>("nJets"))
  //   m_nJets                  = event.m_info->auxdecor<int>("nJets");

  EventSaverFlatNtuple::saveEvent(event);
  tthevt->clearReco();

}

void TTHbbLeptonicEventSaver::saveParticleLevelEvent(const top::ParticleLevelEvent& plEvent) {

  if(!m_config->isMC()) return;
  if(!topConfig()->doTopParticleLevel()) return;

  if ( !particleLevelTreeManager() ){
    setupParticleLevelTreeManager( /*plEvent*/ );
  }

  std::shared_ptr<TTHbb::Event> tthevt;
  if(plEvent.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    tthevt = plEvent.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
  }

  if(!tthevt){
    std::cout << "TTHbbLeptonicEventSaver: TTHbbEventVariables (TTHbb::Event*) object not found" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  //// OK everything set
  //// clean, copy and save
  cleanEventSaver();

  saveEventVariables(tthevt);
  saveRecoObjectsVariables(tthevt);
  saveTruthObjectsVariables(tthevt);

  EventSaverFlatNtuple::saveParticleLevelEvent(plEvent);

}

void TTHbbLeptonicEventSaver::setupParticleLevelTreeManager(/*const top::ParticleLevelEvent& plEvent*/) {

  EventSaverFlatNtuple::setupParticleLevelTreeManager(/*plEvent*/);
  registerTruthObjectVariables(particleLevelTreeManager(), true);
  registerEventDecoration(true);

}

/// clean functions
    
void TTHbbLeptonicEventSaver::cleanEventSaver(){
  //m_nJets=-9;

  m_truth_hadron_pt.clear();
  m_truth_hadron_eta.clear();
  m_truth_hadron_phi.clear();
  m_truth_hadron_m.clear();
  m_truth_hadron_pdgid.clear();
  m_truth_hadron_status.clear();
  m_truth_hadron_barcode.clear();
  m_truth_hadron_TopHadronOriginFlag.clear();
  m_truth_hadron_type.clear();
  m_truth_hadron_final.clear();
  m_truth_hadron_initial.clear();
  //m_truth_hadron_indexmap.clear();

  m_truth_pt.clear();
  m_truth_eta.clear();
  m_truth_phi.clear();
  m_truth_m.clear();
  m_truth_pdgid.clear();
  m_truth_status.clear();
  m_truth_barcode.clear();
  m_truth_tthbb_info.clear();
  //m_truth_indexmap.clear();

  m_truth_jet_pt.clear();
  m_truth_jet_eta.clear();
  m_truth_jet_phi.clear();
  m_truth_jet_m.clear();
  m_truth_jet_flav.clear();
  m_truth_jet_id.clear();
  m_truth_jet_count.clear();

  for(unsigned int i=0; i<m_charVariableHolder->size(); ++i){
    m_charVariableHolder->at(i)=-9;
  }
  for(unsigned int i=0; i<m_intVariableHolder->size(); ++i){
    //*(m_intVariableHolder->at(i))=-9;
    m_intVariableHolder->at(i)=-9;
  }
  for(unsigned int i=0; i<m_floatVariableHolder->size(); ++i){
    m_floatVariableHolder->at(i)=-9;
  }

  clearVectVar(m_charJetVariableHolder);
  clearVectVar(m_intJetVariableHolder);
  clearVectVar(m_floatJetVariableHolder);

  clearVectVar(m_charElecVariableHolder);
  clearVectVar(m_intElecVariableHolder);
  clearVectVar(m_floatElecVariableHolder);

  clearVectVar(m_charMuonVariableHolder);
  clearVectVar(m_intMuonVariableHolder);
  clearVectVar(m_floatMuonVariableHolder);

  clearVectVar(m_charLJetVariableHolder);
  clearVectVar(m_intLJetVariableHolder);
  clearVectVar(m_floatLJetVariableHolder);

}

template <class T>
void TTHbbLeptonicEventSaver::clearVectVar(T& vars){
  for(unsigned int i=0; i<vars->size(); ++i){
    vars->at(i).clear();
  }
}

///// fill functions

void TTHbbLeptonicEventSaver::fillHFHadrons(TTHbb::TruthParticleContainer& parts){

  unsigned int npart = parts.size();
  m_truth_hadron_pt.resize(npart);
  m_truth_hadron_eta.resize(npart);
  m_truth_hadron_phi.resize(npart);
  m_truth_hadron_m.resize(npart);
  m_truth_hadron_pdgid.resize(npart);
  m_truth_hadron_status.resize(npart);
  m_truth_hadron_barcode.resize(npart);
  m_truth_hadron_TopHadronOriginFlag.resize(npart);
  m_truth_hadron_type.resize(npart);
  m_truth_hadron_final.resize(npart);
  m_truth_hadron_initial.resize(npart);

  for(unsigned int i=0; i<npart; ++i){
    auto part = parts[i];
    m_truth_hadron_pt[i]=part->p4().Pt();
    m_truth_hadron_eta[i]=part->p4().Eta();
    m_truth_hadron_phi[i]=part->p4().Phi();
    m_truth_hadron_m[i]=part->p4().M();
    m_truth_hadron_pdgid[i]=part->intVariable("pdgid");
    m_truth_hadron_status[i]=part->intVariable("status");
    m_truth_hadron_barcode[i]=part->intVariable("barcode");
    m_truth_hadron_TopHadronOriginFlag[i]=part->intVariable("TopHadronOriginFlag");
    m_truth_hadron_type[i]=part->charVariable("hadronType");
    m_truth_hadron_final[i]=part->charVariable("isInitial");
    m_truth_hadron_initial[i]=part->charVariable("isFinal");

    /// in case we want to dump indices
    ///  m_truth_hadron_indexmap[part->intVariable("barcode")]=i;

  }

}

void TTHbbLeptonicEventSaver::fillPartons(TTHbb::TruthParticleContainer& parts){

  unsigned int npart = parts.size();
  m_truth_pt.resize(npart);
  m_truth_eta.resize(npart);
  m_truth_phi.resize(npart);
  m_truth_m.resize(npart);
  m_truth_pdgid.resize(npart);
  m_truth_status.resize(npart);
  m_truth_barcode.resize(npart);
  m_truth_tthbb_info.resize(npart);

  for(unsigned int i=0; i<npart; ++i){
    auto part = parts[i];
    m_truth_pt[i]=part->p4().Pt();
    m_truth_eta[i]=part->p4().Eta();
    m_truth_phi[i]=part->p4().Phi();
    m_truth_m[i]=part->p4().M();
    m_truth_pdgid[i]=part->intVariable("pdgid");
    m_truth_status[i]=part->intVariable("status");
    m_truth_barcode[i]=part->intVariable("barcode");
    m_truth_tthbb_info[i]=part->historyInfo();

    /// in case we want to dump indices
    ///  m_truth_indexmap[part->intVariable("barcode")]=i;

  }
}

void TTHbbLeptonicEventSaver::fillTruthJets(TTHbb::JetContainer& jets){
  unsigned int njet = jets.size();
  m_truth_jet_pt.resize(njet);
  m_truth_jet_eta.resize(njet);
  m_truth_jet_phi.resize(njet);
  m_truth_jet_m.resize(njet);
  m_truth_jet_flav.resize(njet);
  m_truth_jet_id.resize(njet);
  m_truth_jet_count.resize(njet);

  for(unsigned int i=0; i<njet; ++i){
    auto jet = jets[i];
    m_truth_jet_pt[i]=jet->p4().Pt();
    m_truth_jet_eta[i]=jet->p4().Eta();
    m_truth_jet_phi[i]=jet->p4().Phi();
    m_truth_jet_m[i]=jet->p4().M();

    m_truth_jet_flav[i]=jet->intVariable("HFClassification_flav");
    m_truth_jet_id[i]=jet->intVariable("HFClassification_id");
    m_truth_jet_count[i]=jet->intVariable("HFClassification_count");
  }

}

bool TTHbbLeptonicEventSaver::isNominalTree(std::string name){

  bool isNominal = (m_config->systematicName(m_config->nominalHashValue()) == name);
  isNominal |= ((m_config->systematicName(m_config->nominalHashValue()) + "_Loose") == name);

  return isNominal;

}

outputVar TTHbbLeptonicEventSaver::makeOutputVar(std::string m_name, bool nom, bool truth){
  outputVar var;
  var.name = m_name;
  var.nomOnly = nom;
  var.inTruth = truth;
  return var;
}

///// reco objects helper functions

void TTHbbLeptonicEventSaver::registerBranchCharVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<char> > >& holders, std::string prefix){
  holders.reset(new std::vector<std::vector<char> >(names.size()));
  for (auto systematicTree : treeManagers()){
    for(unsigned int i=0; i<names.size(); ++i){
      if(!(names[i].nomOnly && !isNominalTree(systematicTree->name()))){
	systematicTree->makeOutputVariable(holders->at(i),prefix+names[i].name);
      }
    }
  }
}

void TTHbbLeptonicEventSaver::registerBranchIntVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<int> > >& holders, std::string prefix){
  holders.reset(new std::vector<std::vector<int> >(names.size()));
  for (auto systematicTree : treeManagers()){
    for(unsigned int i=0; i<names.size(); ++i){
      if(!(names[i].nomOnly && !isNominalTree(systematicTree->name()))){
	systematicTree->makeOutputVariable(holders->at(i),prefix+names[i].name);
      }
    }
  }
}

void TTHbbLeptonicEventSaver::registerBranchFloatVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<float> > >& holders, std::string prefix){
  holders.reset(new std::vector<std::vector<float> >(names.size()));
  for (auto systematicTree : treeManagers()){
    for(unsigned int i=0; i<names.size(); ++i){
      if(!(names[i].nomOnly && !isNominalTree(systematicTree->name()))){
	systematicTree->makeOutputVariable(holders->at(i),prefix+names[i].name);
      }
    }
  }
}

template <class T>
void TTHbbLeptonicEventSaver::copyCharVectVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<char> > >& holders, T& objects){

  for(unsigned int i=0; i<names.size(); ++i){
    for(auto obj : objects){
      holders->at(i).push_back(obj->charVariable(names[i].name));
    }
  }

}

template <class T>
void TTHbbLeptonicEventSaver::copyIntVectVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<int> > >& holders, T& objects){

  for(unsigned int i=0; i<names.size(); ++i){
    for(auto obj : objects){
      holders->at(i).push_back(obj->intVariable(names[i].name));
    }
  }

}

template <class T>
void TTHbbLeptonicEventSaver::copyFloatVectVariables(std::vector<outputVar>& names, std::unique_ptr<std::vector<std::vector<float> > >& holders, T& objects){

  for(unsigned int i=0; i<names.size(); ++i){
    for(auto obj : objects){
      holders->at(i).push_back(obj->floatVariable(names[i].name));
    }
  }

}


/** remove branches from output n-tuple using pattern matching */
int TTHbbLeptonicEventSaver::getBranchStatus(top::TreeManager const * treeManager, std::string const & variableName) {

  // Remove old btag weights
  if ( boost::algorithm::contains(variableName,"jet_ip3dsv1") || boost::algorithm::contains(variableName,"jet_mv2c00") ) 
    return 0;

  // Remove individual systematic weights from non-nominal trees
  if( boost::algorithm::contains(variableName, "weight_indiv_") 
      && ( boost::algorithm::contains(variableName, "_UP") || boost::algorithm::contains(variableName, "_DOWN") )
      && treeManager->name() != "nominal" && treeManager->name() != "nominal_Loose" )
    return 0;

  // Remove tau variables
  if( boost::algorithm::contains(variableName, "tau_") || boost::algorithm::contains(variableName, "weight_tau") )
    return 0;
  
  // Remove eigenvariations for all fixed cut/ dynamic cut btagging
  if( boost::algorithm::contains(variableName, "weight_bTagSF_MV2c10_60_") ||
      boost::algorithm::contains(variableName, "weight_bTagSF_MV2c10_70_") ||
      boost::algorithm::contains(variableName, "weight_bTagSF_MV2c10_77_") ||
      boost::algorithm::contains(variableName, "weight_bTagSF_MV2c10_85_") ||
      boost::algorithm::contains(variableName, "weight_bTagSF_DL1_60_")    ||
      boost::algorithm::contains(variableName, "weight_bTagSF_DL1_70_")    ||
      boost::algorithm::contains(variableName, "weight_bTagSF_DL1_77_")    ||
      boost::algorithm::contains(variableName, "weight_bTagSF_DL1_85_")    )
    return 0;



  return -1;
}

