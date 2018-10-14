/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbEDMConverter.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

#include "xAODTruth/xAODTruthHelpers.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TopEvent/EventTools.h"
#include "TTHbbLeptonic/EventSaverHelper.h"

#include "MCFakes/MCFakesCalculator.h"


TTHbbEDMConverter::TTHbbEDMConverter(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config),
  m_muonsel("CP::MuonSelectionTool")
{
  m_params=params;
  
  auto* esh = EventSaverHelper::get();

  esh->addVariableToBeSaved("leptonType", TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
  esh->addVariableToBeSaved("nJets", TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
  esh->addVariableToBeSaved("nElectrons", TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
  esh->addVariableToBeSaved("nMuons", TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
  esh->addVariableToBeSaved("nPrimaryVtx", TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
  esh->addVariableToBeSaved("nHFJets", TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");

  if(m_config->useTaus()){
    esh->addVariableToBeSaved("nTaus", TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
  }
  
  // Our chosen b-tagger
  m_chosenBTagger = "MV2c10";

  // Calculating this variable      
  esh->addJetVariableToBeSaved("tagWeightBin",TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");

  for(std::string wp : m_config->bTagWP_available()){
    std::string swp = TTHbb::util::shortBtagWP(wp);
    if (wp.find("Continuous") != std::string::npos) continue;
    esh->addVariableToBeSaved("nBTags_"+swp, TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
    if (wp.find(m_chosenBTagger) != std::string::npos){
      std::string numericalWP = TTHbb::util::onlyBtagWP(wp);
      esh->addVariableToBeSaved("nBTags_"+numericalWP, TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");
    }
  }

  //esh->addJetVariableToBeSaved("truthPartonLabel",TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");

  esh->addElecVariableToBeSaved("LHMedium",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("LHTight",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("isoGradient",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("isoGradientLoose",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("isoLoose",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("isoLooseTrackOnly",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("isoFixedCutTight",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("isoFixedCutTightTrackOnly",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("isoFixedCutLoose",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");

  esh->addElecVariableToBeSaved("isPrompt",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("true_pdg",TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");

  esh->addElecVariableToBeSaved("true_pt",TTHbb::DecorationType::Float,true,"TTHbbEDMConverter");
  esh->addElecVariableToBeSaved("true_eta",TTHbb::DecorationType::Float,true,"TTHbbEDMConverter");

  esh->addMuonVariableToBeSaved("Tight",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("Medium",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("isoGradient",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("isoGradientLoose",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("isoLoose",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("isoLooseTrackOnly",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("isoFixedCutTightTrackOnly",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("isoFixedCutLoose",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("isPrompt",TTHbb::DecorationType::Char,false,"TTHbbEDMConverter");

  esh->addMuonVariableToBeSaved("true_pdg",TTHbb::DecorationType::Int,false,"TTHbbEDMConverter");

  esh->addMuonVariableToBeSaved("true_pt",TTHbb::DecorationType::Float,true,"TTHbbEDMConverter");
  esh->addMuonVariableToBeSaved("true_eta",TTHbb::DecorationType::Float,true,"TTHbbEDMConverter");


}

TTHbbEDMConverter::~TTHbbEDMConverter(){

}

bool TTHbbEDMConverter::apply(const top::Event & event) const{


  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
 }

 std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

 //// do not refill is already filled
 //// can happen if an event pass multiple selection
 //// will be cleared in EventSaver for each syst so we fill again

 if(tthevt->recoAvailable()) return true;

   
  for(std::string wp : m_config->bTagWP_available()){
    std::string swp = TTHbb::util::shortBtagWP(wp);
    if (wp.find("Continuous") == std::string::npos){
      m_nBTagsOP[swp]=0;
      if(wp.find(m_chosenBTagger) != std::string::npos){
	m_nBTagsOP[TTHbb::util::onlyBtagWP(wp)] = 0;
      }
    }
  }

  tthevt->met_met = event.m_met->met();
  tthevt->met_phi = event.m_met->phi();

  fillJets(event.m_jets, tthevt->m_jets);

  
  m_jetOrderingTool.decorateEventBtagSort(tthevt.get(), tthevt->m_jets, "mv2", "tagWeightBin");
  m_jetOrderingTool.decorateEventPseudoBtagSort(tthevt.get(), tthevt->m_jets, "tagWeightBin");
  m_jetOrderingTool.decorateAbsEtaSortedVar(tthevt.get(), tthevt->m_jets, "tagWeightBin", TTHbb::DecorationType::Int);
  /// last sort pt like this it is pt sorted at the end
  m_jetOrderingTool.decoratePtSortedVar(tthevt.get(), tthevt->m_jets,  "tagWeightBin", TTHbb::DecorationType::Int);
  m_jetOrderingTool.decoratePtSortedVar(tthevt.get(), tthevt->m_jets,  "mv2", TTHbb::DecorationType::Float);

  fillElectrons(event.m_electrons, tthevt->m_electrons);
  fillMuons(event.m_muons, tthevt->m_muons);
  tthevt->m_leptons.reserve(tthevt->m_muons.size()+tthevt->m_electrons.size());
  
  for(auto a : tthevt->m_muons) tthevt->m_leptons.push_back(a);
  for(auto a : tthevt->m_electrons) tthevt->m_leptons.push_back(a);
  int leptonType=13*tthevt->m_muons.size() + 11*tthevt->m_electrons.size();
  tthevt->intVariable("leptonType")=leptonType;

  for(auto a : m_nBTagsOP){
    tthevt->intVariable("nBTags_"+a.first)=a.second;
  }

  tthevt->intVariable("nJets")       = event.m_jets.size();
  tthevt->intVariable("nPrimaryVtx") = event.m_primaryVertices->size();
  tthevt->intVariable("nElectrons")  = event.m_electrons.size();
  tthevt->intVariable("nMuons")      = event.m_muons.size();

  int nHFjets=0;
  for(auto a : tthevt->m_jets){
    int label = a->intVariable("truthflav");
    if(label == 4 ||label == 5) ++nHFjets;
  }
  tthevt->intVariable("nHFJets")      = nHFjets;

  if(m_config->useTaus()){
    tthevt->intVariable("nTaus")       = event.m_tauJets.size();
  }

  tthevt->recoAvailable(true);
  //// fill all other objects/info here
  
  return true;

}

void TTHbbEDMConverter::fillJets(const xAOD::JetContainer& orig, TTHbb::JetContainer& current) const{
  
  for(auto jet : orig){
    auto tthjet = std::make_shared<TTHbb::Jet>(jet->pt(), jet->eta(), jet->phi(), jet->e());
    tthjet->id = current.size();
    current.push_back(tthjet);
    /// decorate all needed jet variables here
    int truthPartonLabel = -99;
    if(jet->isAvailable<int>("PartonTruthLabelID")){
      jet->getAttribute("PartonTruthLabelID", truthPartonLabel);
    }
    tthjet->intVariable("truthPartonLabel") = truthPartonLabel;
    int label = -9;
    if(jet->isAvailable<int>("HadronConeExclTruthLabelID")){
      jet->getAttribute("HadronConeExclTruthLabelID", label);
    }
    tthjet->intVariable("truthflav") = label;
    int extendedlabel = -9;
    if(jet->isAvailable<int>("HadronConeExclExtendedTruthLabelID")){
      jet->getAttribute("HadronConeExclExtendedTruthLabelID", extendedlabel);
    }
    tthjet->intVariable("truthflavExtended") = extendedlabel;
    double mv2 =-999;
    jet->btagging()->MVx_discriminant("MV2c10", mv2);
    tthjet->floatVariable("mv2")=mv2;

    // Continuous is currently not available, but we want to decorate the tag bin so do it before the loop below
    // This will only decorate the jets if it is not available in AnalysisTop
    // jet->auxdecor<int>("tagWeightBin")  = calculateJetTagWeightBin(mv2);
    // tthjet->intVariable("tagWeightBin") = calculateJetTagWeightBin(mv2);


    for(std::string wp : m_config -> bTagWP_available()){
      if(wp.find("Continuous") != std::string::npos){
	tthjet->intVariable("tagWeightBin_"+wp) = jet->auxdataConst<int>("tagWeightBin_"+wp);
	// To stop things breaking, let us also decorate tagWeightBin from continuous MV2c10
	if(wp.find(m_chosenBTagger) != std::string::npos){
	  tthjet->intVariable("tagWeightBin") = jet->auxdataConst<int>("tagWeightBin_"+wp);
	}
      }      
      else{
	std::string swp = TTHbb::util::shortBtagWP(wp);
	std::string jetTag = ("isbtagged_"+swp);
	tthjet->charVariable(jetTag) = jet->auxdataConst<char>("isbtagged_"+wp);
	if(tthjet->charVariable(jetTag)){
	  ++m_nBTagsOP[swp];
	  if(wp.find(m_chosenBTagger) != std::string::npos){
	    tthjet->charVariable("isbtagged_"+TTHbb::util::onlyBtagWP(wp)) = jet->auxdataConst<char>("isbtagged_"+wp);
	    ++m_nBTagsOP[TTHbb::util::onlyBtagWP(wp)];
	  }
	}
      }
    }
  }

}


void TTHbbEDMConverter::fillElectrons(const xAOD::ElectronContainer& orig, TTHbb::LeptonContainer& current) const{
  
  current.reserve(orig.size());
  for(auto lepton : orig){
    auto tthlepton = std::make_shared<TTHbb::Lepton>(lepton->pt(), lepton->eta(), lepton->phi(), lepton->e());
    tthlepton->id = current.size();
    current.push_back(tthlepton);
    /// decorate all needed lepton variables here
    tthlepton->type(TTHbb::leptonType::electron);

    /// dumped by AT
    int true_type = 0;
    int true_origin = 0;
    int true_typebkg = 0;
    int true_originbkg = 0;
    static SG::AuxElement::Accessor<int> typeel("truthType");
    static SG::AuxElement::Accessor<int> origel("truthOrigin");
    static SG::AuxElement::Accessor<int> typebkgel("bkgTruthType");
    static SG::AuxElement::Accessor<int> origbkgel("bkgTruthOrigin");
    if(typeel.isAvailable(*lepton)) true_type = typeel(*lepton);
    if(origel.isAvailable(*lepton)) true_origin = origel(*lepton);
    if(typebkgel.isAvailable(*lepton)) true_typebkg = typebkgel(*lepton);
    if(origbkgel.isAvailable(*lepton)) true_originbkg = origbkgel(*lepton);

    tthlepton->intVariable("true_type")=true_type;
    tthlepton->intVariable("true_origin")=true_origin;
    tthlepton->intVariable("true_typebkg")=true_typebkg;
    tthlepton->intVariable("true_originbkg")=true_originbkg;

    bool isPrompt = !TTHbb::MCFakesCalculator::isFakeElectron(true_type, true_origin, true_originbkg);
    //bool isBrem = isBremElec(true_type, true_origin, true_originbkg);

    tthlepton->charVariable("isPrompt") = isPrompt;// || isBrem;

    char isTight=0;
    if (lepton->isAvailable<char>("passPreORSelection")){
      isTight = lepton->auxdataConst<char>("passPreORSelection");
    }
    tthlepton->charVariable("isTight") = isTight;

    tthlepton->floatVariable("charge")=lepton->charge();


    /// not dummped by AT
    int true_pdg = 0;
    float true_pt = 0;
    float true_eta = 0;
    const xAOD::TruthParticle* etrue = xAOD::TruthHelpers::getTruthParticle(*lepton);
    if (etrue!=nullptr) {
      true_pdg = etrue->pdgId();
      true_pt = etrue->pt();
      true_eta = etrue->eta();
    }

    tthlepton->intVariable("true_pdg")=true_pdg;
    tthlepton->floatVariable("true_pt")=true_pt;
    tthlepton->floatVariable("true_eta")=true_eta;


    char LHMedium = 0;          
    char LHTight = 0;           
    char isoGradient = 0;       
    char isoGradientLoose = 0;  
    char isoLoose = 0;          
    char isoLooseTrackOnly = 0; 
    char isoFixedCutTight = 0;
    char isoFixedCutTightTrackOnly = 0;
    char isoFixedCutLoose = 0;


    /// faster to use Accessor
    try { LHMedium = lepton->auxdataConst<int>("DFCommonElectronsLHMedium"); }
    catch(std::exception& e) { LHMedium = lepton->auxdataConst<char>("DFCommonElectronsLHMedium"); }
    try { LHTight = lepton->auxdataConst<int>("DFCommonElectronsLHTight"); }
    catch(std::exception& e) { LHTight = lepton->auxdataConst<char>("DFCommonElectronsLHTight"); }
    isoGradient      = lepton->auxdataConst<char>("AnalysisTop_Isol_Gradient");
    isoGradientLoose  = lepton->auxdataConst<char>("AnalysisTop_Isol_GradientLoose");
    isoLoose      = lepton->auxdataConst<char>("AnalysisTop_Isol_Loose");
    isoLooseTrackOnly = lepton->auxdataConst<char>("AnalysisTop_Isol_LooseTrackOnly");
    isoFixedCutTight = lepton->auxdataConst<char>("AnalysisTop_Isol_FixedCutTight");
    isoFixedCutTightTrackOnly = lepton->auxdataConst<char>("AnalysisTop_Isol_FixedCutTightTrackOnly");
    isoFixedCutLoose = lepton->auxdataConst<char>("AnalysisTop_Isol_FixedCutLoose");

    tthlepton->charVariable("LHMedium") = LHMedium;
    tthlepton->charVariable("LHTight") = LHTight;
    tthlepton->charVariable("isoGradient") = isoGradient;
    tthlepton->charVariable("isoGradientLoose") = isoGradientLoose;
    tthlepton->charVariable("isoLoose") = isoLoose;
    tthlepton->charVariable("isoLooseTrackOnly") = isoLooseTrackOnly;
    tthlepton->charVariable("isoFixedCutTight") = isoFixedCutTight;
    tthlepton->charVariable("isoFixedCutTightTrackOnly") = isoFixedCutTightTrackOnly;
    tthlepton->charVariable("isoFixedCutLoose") = isoFixedCutLoose;

  }

// el_cl_eta
// el_d0sig
// el_delta_z0_sintheta
// el_isTight
// el_ptvarcone20
// el_topoetcone20
// el_trigMatch_HLT_e120_lhloose
// el_trigMatch_HLT_e140_lhloose_nod0
// el_trigMatch_HLT_e24_lhmedium_L1EM20VH
// el_trigMatch_HLT_e26_lhtight_nod0_ivarloose
// el_trigMatch_HLT_e60_lhmedium
// el_trigMatch_HLT_e60_lhmedium_nod0



}

void TTHbbEDMConverter::fillMuons(const xAOD::MuonContainer& orig, TTHbb::LeptonContainer& current) const{
  
  current.reserve(orig.size());
  for(auto lepton : orig){
    auto tthlepton = std::make_shared<TTHbb::Lepton>(lepton->pt(), lepton->eta(), lepton->phi(), lepton->e());
    tthlepton->id = current.size();
    current.push_back(tthlepton);
    /// decorate all needed lepton variables here
    tthlepton->type(TTHbb::leptonType::muon);

    /// dummped by AT

    static SG::AuxElement::Accessor<int> acc_mctt("truthType");
    static SG::AuxElement::Accessor<int> acc_mcto("truthOrigin");
    static SG::AuxElement::Accessor<int> typebkgel("bkgTruthType");
    static SG::AuxElement::Accessor<int> origbkgel("bkgTruthOrigin");
    int true_type=0;
    int true_origin=0;
    int true_typebkg = 0;
    int true_originbkg = 0;
 
    const xAOD::TrackParticle* mutrack = lepton->primaryTrackParticle();
    if (mutrack!=nullptr) {
      if (acc_mctt.isAvailable(*mutrack)) true_type = acc_mctt(*mutrack);
      if (acc_mcto.isAvailable(*mutrack)) true_origin = acc_mcto(*mutrack);
      if(typebkgel.isAvailable(*mutrack)) true_typebkg = typebkgel(*mutrack);
      if(origbkgel.isAvailable(*mutrack)) true_originbkg = origbkgel(*mutrack);
    }
  
    tthlepton->intVariable("true_type") = true_type;
    tthlepton->intVariable("true_origin") = true_origin;
    tthlepton->intVariable("true_typebkg")=true_typebkg;
    tthlepton->intVariable("true_originbkg")=true_originbkg;

    bool isPrompt = !TTHbb::MCFakesCalculator::isFakeMuon(true_type, true_origin, true_originbkg);

    tthlepton->charVariable("isPrompt") = isPrompt;


    char isTight=0;
    if (lepton->isAvailable<char>("passPreORSelection")){
      isTight = lepton->auxdataConst<char>("passPreORSelection");
    }
    tthlepton->charVariable("isTight") = isTight;

    tthlepton->floatVariable("charge") = lepton->charge();

    /// not dummped by AT
    int true_pt = 0;
    float true_eta = 0;
    int true_pdg = 0;

    if(mutrack!=nullptr && mutrack->isAvailable< ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
      // isAvailable sometimes returns true even when the auxdata call will fail
      // protect this with a try/catch
      try {
	const xAOD::TruthParticle* truthmu = * mutrack->auxdata< ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
	if (truthmu!=nullptr) {
	  true_pt = truthmu->pt();
	  true_eta = truthmu->eta();
	  true_pdg = truthmu->pdgId();
	}
      } catch (...) {}

    }

    tthlepton->intVariable("true_pdg") = true_pdg;
    tthlepton->floatVariable("true_pt") = true_pt;
    tthlepton->floatVariable("true_eta") = true_eta;

    char Tight             = 0;
    char Medium            = 0;
    char isoGradient       = 0;
    char isoGradientLoose  = 0;
    char isoLoose          = 0;
    char isoLooseTrackOnly = 0;
    char isoFixedCutTightTrackOnly = 0;
    char isoFixedCutLoose  = 0;

    /// move to accessor (faster)
    xAOD::Muon::Quality quality = m_muonsel->getQuality( *lepton );
    Tight      = quality <= xAOD::Muon::Tight ? 1 : 0;
    Medium      = quality <= xAOD::Muon::Medium ? 1 : 0;
    isoGradient      = lepton->auxdataConst<char>("AnalysisTop_Isol_Gradient");
    isoGradientLoose  = lepton->auxdataConst<char>("AnalysisTop_Isol_GradientLoose");
    isoLoose      = lepton->auxdataConst<char>("AnalysisTop_Isol_Loose");
    isoLooseTrackOnly = lepton->auxdataConst<char>("AnalysisTop_Isol_LooseTrackOnly");
    isoFixedCutTightTrackOnly = lepton->auxdataConst<char>("AnalysisTop_Isol_FixedCutTightTrackOnly");
    isoFixedCutLoose = lepton->auxdataConst<char>("AnalysisTop_Isol_FixedCutLoose");

    tthlepton->charVariable("Tight") = Tight;
    tthlepton->charVariable("Medium") = Medium;
    tthlepton->charVariable("isoGradient") = isoGradient;
    tthlepton->charVariable("isoGradientLoose") = isoGradientLoose;
    tthlepton->charVariable("isoLoose") = isoLoose;
    tthlepton->charVariable("isoLooseTrackOnly") = isoLooseTrackOnly;
    tthlepton->charVariable("isoFixedCutTightTrackOnly") = isoFixedCutTightTrackOnly;
    tthlepton->charVariable("isoFixedCutLoose") = isoFixedCutLoose;

  }
    // mu_d0sig
    // mu_delta_z0_sintheta
    // mu_ptvarcone30
    // mu_topoetcone20
    // mu_trigMatch_HLT_mu20_iloose_L1MU15
    // mu_trigMatch_HLT_mu26_ivarmedium
    // mu_trigMatch_HLT_mu50

}

int TTHbbEDMConverter::calculateJetTagWeightBin(double tagweight) const{
  // Hard-coded cuts for 100%, 85%, 77%, 70%, 60%
  // Bin number             1,   2,   3,   4,   5
  // See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BTaggingCalibrationDataInterface#Example_for_continuous_tagging
  std::vector<float> cutvalues = {-999, 0.11, 0.64, 0.83, 0.94};
  int tagBin = 1; // Default for untagged
  for(unsigned int iBin = 0; iBin < cutvalues.size(); iBin++){
    if(tagweight >= cutvalues.at(iBin)){
      tagBin = iBin+1; // Add one to match the b-tag convention 
    }
    else{
      break;
    }
  }
  return tagBin;
}


std::string TTHbbEDMConverter::name() const{
  return "TTHBBEDM";
}



