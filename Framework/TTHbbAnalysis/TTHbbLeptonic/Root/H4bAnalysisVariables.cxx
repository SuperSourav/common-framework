/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include "TTHbbLeptonic/H4bAnalysisVariables.h"
#include <cstdlib>
#include "TMVA/Types.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodBase.h"

//______________________________________________________________________________
H4bAnalysisVariables::H4bAnalysisVariables(std::string params,std::shared_ptr<top::TopConfig> config):
  m_event(0),
  m_config(config)
{
  m_params=params;
  m_debug = false;

  /// register the event decorations that you want to dump
 auto* esh = EventSaverHelper::get();
 //                           name,                    type,                       nominalOnly, origin
// esh->addVariableToBeSaved("nPrimaryVtx",              TTHbb::DecorationType::Int  , false,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("npvtx",                    TTHbb::DecorationType::Int  , false,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("vtxz",                     TTHbb::DecorationType::Float, false,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("lep_OS",                   TTHbb::DecorationType::Char,  false,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("mtw",                      TTHbb::DecorationType::Float,  false,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("mll",                      TTHbb::DecorationType::Float,  false,       "H4bAnalysisVariables");
// esh->addVariableToBeSaved("TopHeavyFlavorFilterFlag", TTHbb::DecorationType::Int  , false,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("prwHash",                  TTHbb::DecorationType::Int  , false,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_top_pt",            TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_top_eta",           TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_top_phi",           TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_top_e",             TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_antitop_pt",        TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_antitop_eta",       TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_antitop_phi",       TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_antitop_e",         TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_bsmHiggs_pt",       TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_bsmHiggs_eta",      TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_bsmHiggs_phi",      TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_bsmHiggs_e",        TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_Vboson_pt",         TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_Vboson_eta",        TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_Vboson_phi",        TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
 esh->addVariableToBeSaved("parton_Vboson_e",          TTHbb::DecorationType::Float, true,       "H4bAnalysisVariables");
}


//______________________________________________________________________________
H4bAnalysisVariables::~H4bAnalysisVariables(){
}


////______________________________________________________________________________
//void H4bAnalysisVariables::initialise(const top::Event& event) const{
//}


//______________________________________________________________________________
bool H4bAnalysisVariables::apply(const top::Event & event) const{

  m_event = &event;

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout << "DummyVariableTool: TTHbbEventVariables (TTHbb::Event*) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params
      << "> <systname: " << m_config->systematicName( event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
  }

  this->DecoratePrimaryVertex();
  this->DecorateEventInfo();
  this->DecorateTruthInfo();

  return true;
}


//______________________________________________________________________________
std::string H4bAnalysisVariables::name() const{
  return "H4BANALYSISVARS";
}


//______________________________________________________________________________
void H4bAnalysisVariables::DecoratePrimaryVertex() const
{

  //--- Get the TTHbb event pointer
  std::shared_ptr<TTHbb::Event> tthevt = m_event->m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  //--- Fill decorations
  tthevt->intVariable("nPrimaryVtx") = m_event->m_primaryVertices->size();

  const xAOD::VertexContainer *m_primvtx = m_event->m_primaryVertices;
  int npvtx = -1;
  float vtxz = -999.;
  for (const auto* const vtxPtr : *m_primvtx) {
    const xAOD::VxType::VertexType vtype = vtxPtr->vertexType();
    const int vmult = vtxPtr->trackParticleLinks().size();
    // count vertices of type 1 (primary) and 3 (pileup) with >4 tracks
    if ((vtype==1 || vtype==3) && vmult>4) {
      ++npvtx;
      // assuming there is only one primary vertex
      if (vtype==1) vtxz = vtxPtr->z();
    }
  }
  tthevt->intVariable("npvtx") = npvtx;
  tthevt->floatVariable("vtxz") = vtxz;
}


//______________________________________________________________________________
void H4bAnalysisVariables::DecorateEventInfo() const
{
  //--- Get the TTHbb event pointer
  std::shared_ptr<TTHbb::Event> tthevt = m_event->m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  //--- prw hash
  if (m_event->m_info->isAvailable<unsigned long long>("PileupWeight_Hash")){
    tthevt->intVariable("prwHash") = m_event->m_info->auxdata<unsigned long long>("PileupWeight_Hash");
  }

  //--- is opposite charge lepton
  tthevt->charVariable("lep_OS") = false;
  if (m_event->m_electrons.size()==2 && m_event->m_muons.size()==0){
    if (m_event->m_electrons.at(0)->charge()*m_event->m_electrons.at(1)->charge()<0)
      tthevt->charVariable("lep_OS") = true;
  }
  else if (m_event->m_electrons.size()==0 && m_event->m_muons.size()==2){
    if (m_event->m_muons.at(0)->charge()*m_event->m_muons.at(1)->charge()<0)
      tthevt->charVariable("lep_OS") = true;
  }
  else if (m_event->m_electrons.size()==1 && m_event->m_muons.size()==1){
    if (m_event->m_electrons.at(0)->charge()*m_event->m_muons.at(0)->charge()<0)
      tthevt->charVariable("lep_OS") = true;
  }

  //--- MTW and Mll for lepton
  tthevt->floatVariable("mtw") = -999.;
  tthevt->floatVariable("mll") = -999.;
  if (tthevt->m_leptons.size()>=1){
    TLorentzVector lep_TLV = tthevt->m_leptons.at(0)->p4();
    TVector2 met_vec(tthevt->met_met*TMath::Cos(tthevt->met_phi), tthevt->met_met*TMath::Sin(tthevt->met_phi));
    tthevt->floatVariable("mtw") = sqrt(2. * lep_TLV.Pt() * met_vec.Mod() * (1. - cos(lep_TLV.Phi() - met_vec.Phi())));
  }
  if (tthevt->m_leptons.size()>=2){
    TLorentzVector lep1_TLV = tthevt->m_leptons.at(0)->p4();
    TLorentzVector lep2_TLV = tthevt->m_leptons.at(1)->p4();
    tthevt->floatVariable("mll") = (lep1_TLV + lep2_TLV).M();
  }
}


//______________________________________________________________________________
void H4bAnalysisVariables::DecorateTruthInfo() const
{
  //--- Get the TTHbb event pointer
  std::shared_ptr<TTHbb::Event> tthevt = m_event->m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

    if (m_event->m_truth != nullptr) {
      TLorentzVector top_TLV;
      TLorentzVector antitop_TLV;
      TLorentzVector bsmHiggs_TLV;
      TLorentzVector Vboson_TLV;
      for(const xAOD::TruthParticle*  particle : *m_event->m_truth){
        //check for ttbar system 
        if(particle->pdgId() == 6 && (particle->status() == 1 || particle->status() == 3 || particle->status() == 11)){
          top_TLV.SetPtEtaPhiE(particle->pt(),particle->eta(),particle->phi(),particle->e());
        }
        else if(particle->pdgId() == -6 && (particle->status() == 1 || particle->status() == 3 || particle->status() == 11)){
          antitop_TLV.SetPtEtaPhiE(particle->pt(),particle->eta(),particle->phi(),particle->e());
        }
        else if(particle->pdgId() == 35 && (particle->status()==62 )){
          bsmHiggs_TLV.SetPtEtaPhiE(particle->pt(),particle->eta(),particle->phi(),particle->e());
        }
        else if((particle->pdgId() == 24 || particle->pdgId() == -24 || particle->pdgId() == 23) && (particle->status() == 22)){
          Vboson_TLV.SetPtEtaPhiE(particle->pt(),particle->eta(),particle->phi(),particle->e());
        }
      }
      tthevt->floatVariable("parton_top_pt")  = top_TLV.Pt();
      tthevt->floatVariable("parton_top_eta") = top_TLV.Eta();
      tthevt->floatVariable("parton_top_phi") = top_TLV.Phi();
      tthevt->floatVariable("parton_top_e")   = top_TLV.E();
      tthevt->floatVariable("parton_antitop_pt")  = top_TLV.Pt();
      tthevt->floatVariable("parton_antitop_eta") = top_TLV.Eta();
      tthevt->floatVariable("parton_antitop_phi") = top_TLV.Phi();
      tthevt->floatVariable("parton_antitop_e")   = top_TLV.E();
      tthevt->floatVariable("parton_bsmHiggs_pt")  = top_TLV.Pt();
      tthevt->floatVariable("parton_bsmHiggs_eta") = top_TLV.Eta();
      tthevt->floatVariable("parton_bsmHiggs_phi") = top_TLV.Phi();
      tthevt->floatVariable("parton_bsmHiggs_e")   = top_TLV.E();
      tthevt->floatVariable("parton_Vboson_pt")  = top_TLV.Pt();
      tthevt->floatVariable("parton_Vboson_eta") = top_TLV.Eta();
      tthevt->floatVariable("parton_Vboson_phi") = top_TLV.Phi();
      tthevt->floatVariable("parton_Vboson_e")   = top_TLV.E();
    }
  }
