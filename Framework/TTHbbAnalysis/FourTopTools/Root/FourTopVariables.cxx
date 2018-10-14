/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <cstdlib>
#include "FourTopTools/FourTopVariables.h"
#include "TopEvent/EventTools.h"

//______________________________________________________________________________
FourTopVariables::FourTopVariables(std::string params, std::shared_ptr<top::TopConfig> config):
  m_event(0),
  m_config(config)
{
  m_params=params;
  m_debug = false;

  if ( asg::ToolStore::contains<TTHbbAsgToolHelper>("TTHbbAsgHelper") ) {
    m_asgHelper = asg::ToolStore::get<TTHbbAsgToolHelper>("TTHbbAsgHelper");
  }
  else {
    m_asgHelper = new TTHbbAsgToolHelper("TTHbbAsgHelper");
    top::check( m_asgHelper->initialize() , "Failed to initialize TTHbbAsgHelper" );
  }

  /// register the event decorations that you want to dump
  auto* esh = EventSaverHelper::get();

  // esh->addVariableToBeSaved(name, type, nominalOnly, origin)

  if(m_config->isMC()){
    esh->addJetVariableToBeSaved("firstghost_pdgId", TTHbb::DecorationType::Int, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("firstghost_barcode", TTHbb::DecorationType::Int, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("firstghost_status", TTHbb::DecorationType::Int, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("firstghost_e", TTHbb::DecorationType::Float, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("firstghost_pt", TTHbb::DecorationType::Float, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("firstghost_eta", TTHbb::DecorationType::Float, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("firstghost_phi", TTHbb::DecorationType::Float, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("parentghost_pdgId", TTHbb::DecorationType::Int, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("parentghost_barcode", TTHbb::DecorationType::Int, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("parentghost_status", TTHbb::DecorationType::Int, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("parentghost_e", TTHbb::DecorationType::Float, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("parentghost_pt", TTHbb::DecorationType::Float, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("parentghost_eta", TTHbb::DecorationType::Float, true, "FourTopVariables");
    esh->addJetVariableToBeSaved("parentghost_phi", TTHbb::DecorationType::Float, true, "FourTopVariables");
  }

}


//______________________________________________________________________________
FourTopVariables::~FourTopVariables(){
}


////______________________________________________________________________________
//void FourTopVariables::initialise(const top::Event& event) const{
//}


//______________________________________________________________________________
bool FourTopVariables::apply(const top::Event & event) const{

  m_event = &event;

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout << "FourTopVariables: TTHbbEventVariables (TTHbb::Event*) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params
      << "> <systname: " << m_config->systematicName( event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
  }

  if(m_config->isMC())
    this->DecorateJetsWithGhost();

  return true;
}


//______________________________________________________________________________
std::string FourTopVariables::name() const{
  return "FOURTOPSANALYSISVARS";
}


// Find the associated xAO::Jet based on the TTHbb:Jet pt ______________________
const xAOD::Jet* FourTopVariables::FindxAODJets(std::shared_ptr<TTHbb::Jet>& jet,
						       const xAOD::JetContainer& xAODJets) const
{
  for(auto xjet : xAODJets)
    if( (float)jet->p4().Pt() == xjet->pt() ) return xjet;

  return 0;
}

// Find the first parent of a given truth particle _____________________________
const xAOD::TruthParticle* FourTopVariables::getParent(const xAOD::TruthParticle* part) const
{
  for(unsigned int i = 0; i<part->nParents(); ++i){
    const xAOD::TruthParticle* parent = part->parent(i);
    if(!parent) continue;
    if(parent->pdgId() != part->pdgId()) return parent;
    else return getParent(parent);
  }
  return part;
}

// Needed to rank the associated xAOD:Jet ______________________________________
bool highestPt(const xAOD::TruthParticle* i, const xAOD::TruthParticle* j)
{
  bool iChecked = i->absPdgId()!=6 && (i->status()==2 || i->status()==51 || i->status()==52
				       || i->status()==71 || i->status()==72 );
  bool jChecked = j->absPdgId()!=6 && (j->status()==2 || j->status()==51 || j->status()==52
				       || j->status()==71 || j->status()==72 );
  return  iChecked*i->pt() < jChecked*j->pt() ;
}

//______________________________________________________________________________
void FourTopVariables::DecorateJetsWithGhost() const
{

  //--- Get the TTHbb event pointer
  std::shared_ptr<TTHbb::Event> tthevt =
    m_event->m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  int ij=0;
  //--- Loop over jets
  for(auto jet : tthevt->m_jets){
    ij++;

    //---- Find the corresponding xAOD jet
    const xAOD::Jet* xjet = FindxAODJets(jet, m_event->m_jets);
    if(!xjet) {
      std::cout << "FourTopVariables  WARNING   xAOD Jet NOT FOUND" << std::endl;
      continue;
    }

    //----- Get the highest-pt associated ghost-matched truth particles and its parent
    std::vector<const xAOD::TruthParticle*> ghostP =
      xjet->getAssociatedObjects<xAOD::TruthParticle>("GhostPartons");
    std::vector<const xAOD::TruthParticle*> ghostTau =
      xjet->getAssociatedObjects<xAOD::TruthParticle>("GhostTausFinal");
    ghostP.insert( ghostP.end(), ghostTau.begin(), ghostTau.end() );
    if(!ghostP.size()){
      std::cout << "FourTopVariables  WARNING   Ghost particles NOT FOUND" << std::endl;
      continue;
    }
    const xAOD::TruthParticle* firstGhost = *std::max_element(ghostP.begin(), ghostP.end(), highestPt);
    const xAOD::TruthParticle* parent = getParent(firstGhost);

    //----- Fill decorations
    jet->intVariable("firstghost_pdgId") = firstGhost->pdgId();
    jet->intVariable("firstghost_barcode") = firstGhost->barcode();
    jet->intVariable("firstghost_status") = firstGhost->status();
    jet->floatVariable("firstghost_e") = firstGhost->e();
    jet->floatVariable("firstghost_pt") = firstGhost->pt();
    jet->floatVariable("firstghost_eta") = firstGhost->eta();
    jet->floatVariable("firstghost_phi") = firstGhost->phi();
    jet->intVariable("parentghost_pdgId") = parent->pdgId();
    jet->intVariable("parentghost_barcode") = parent->barcode();
    jet->intVariable("parentghost_status") = parent->status();
    jet->floatVariable("parentghost_e") = parent->e();
    jet->floatVariable("parentghost_pt") = parent->pt();
    jet->floatVariable("parentghost_eta") = parent->eta();
    jet->floatVariable("parentghost_phi") = parent->phi();
  }

}
