/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbLargeJetEDMConverter.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

#include "xAODTruth/xAODTruthHelpers.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TopEvent/EventTools.h"
#include "TTHbbLeptonic/EventSaverHelper.h"


TTHbbLargeJetEDMConverter::TTHbbLargeJetEDMConverter(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config),
  m_rc(nullptr)
{
  m_params=params;

  if ( asg::ToolStore::contains<TTHbbAsgToolHelper>("TTHbbAsgHelper") ) {
    m_asgHelper = asg::ToolStore::get<TTHbbAsgToolHelper>("TTHbbAsgHelper");
  } 
  else {
    m_asgHelper = new TTHbbAsgToolHelper("TTHbbAsgHelper");
    top::check( m_asgHelper->initialize() , "Failed to initialize TTHbbAsgHelper" );
  }

  auto* esh = EventSaverHelper::get();

  if (m_config->useLargeRJets()) {    
    esh->addLJetVariableToBeSaved("C2", TTHbb::DecorationType::Float, false, "TTHbbLargeJetEDMConverter");
    esh->addLJetVariableToBeSaved("D2", TTHbb::DecorationType::Float, false, "TTHbbLargeJetEDMConverter");
    // sd12 is saved from EventSaverFlatNtuple.cxx from TopAnalysis
    esh->addLJetVariableToBeSaved("sd23", TTHbb::DecorationType::Float, false, "TTHbbLargeJetEDMConverter");
    esh->addLJetVariableToBeSaved("tau21_wta", TTHbb::DecorationType::Float, false, "TTHbbLargeJetEDMConverter");
    esh->addLJetVariableToBeSaved("tau32_wta", TTHbb::DecorationType::Float, false, "TTHbbLargeJetEDMConverter");
    esh->addLJetVariableToBeSaved("topTag", TTHbb::DecorationType::Char, false, "TTHbbLargeJetEDMConverter");
    esh->addLJetVariableToBeSaved("topTag_loose", TTHbb::DecorationType::Char, false, "TTHbbLargeJetEDMConverter");
  }

}

TTHbbLargeJetEDMConverter::~TTHbbLargeJetEDMConverter(){

}

bool TTHbbLargeJetEDMConverter::apply(const top::Event & event) const{


  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
 	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  if (!tthevt->recoAvailable()) {
    std::cout <<name() <<": Error, you must call TTHBBEDM before TTHBBLARGEJETEDM." << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  if (m_config->useLargeRJets()) {    
    fillLargeJets(event.m_largeJets, *tthevt);
  }
  if (m_config->useRCJets()) {
    // For now, these are only filled into the EDM, but not stored into the ntuple from here.
    // Writing to the ntuple is done by the EventSaverFlatNtuple.
    // This here is just so we can access RC jets from dual-use tools (L1/2).
    fillReclusteredJets(event, *tthevt);
  }
  
  return true;
}


void TTHbbLargeJetEDMConverter::fillReclusteredJets(const top::Event& event, TTHbb::Event& tthevt) const {
  // Initialize the RC tool here and not in the constructor,                                                                                                                                             
  // because execute() fails otherwise.  This is because the                                                                                                                                             
  // hash values used for different systematics are not set yet.
  // We are remaking this repeatedly and ASG tool is tell us that
  if ( asg::ToolStore::contains<RCJetMC15>( "RCJetMC15" ) ){
    m_rc = asg::ToolStore::get<RCJetMC15>( "RCJetMC15");
  }
  else{
    RCJetMC15* rc = new RCJetMC15( "RCJetMC15" );
    top::check(m_rc->setProperty( "config" , m_config ) , "Failed to set config property of RCJetMC15");
    top::check(m_rc->initialize(),"Failed to initialize RCJetMC15");
    m_rc = rc;
  }


  // Execute the re-clustering code
  // - make jet container of small-r jets in the event, put it in TStore, do re-clustering
  top::check(m_rc->execute(event),"Failed to execute RCJetMC15 container");

  // Get the name of the container of re-clustered jets in TStore
  auto rcJetContainer = m_rc->rcjetContainerName(event.m_hashValue,event.m_isLoose);

  // -- Retrieve the re-clustered jets from TStore & save good re-clustered jets -- //
  const xAOD::JetContainer* rc_jets(nullptr);
  rc_jets = m_asgHelper->getJetContainer(rcJetContainer);

  // re-clustered jet substructure
  static SG::AuxElement::ConstAccessor<float> RCSplit12("Split12");
  static SG::AuxElement::ConstAccessor<float> RCSplit23("Split23");

  // JSS from clusters
  static SG::AuxElement::ConstAccessor<float> d12("d12_clstr");
  static SG::AuxElement::ConstAccessor<float> d23("d23_clstr");
  static SG::AuxElement::ConstAccessor<float> tau21("Tau21_clstr");
  static SG::AuxElement::ConstAccessor<float> tau32("Tau32_clstr");
  static SG::AuxElement::ConstAccessor<float> tau1("Tau1_clstr");
  static SG::AuxElement::ConstAccessor<float> tau2("Tau2_clstr");
  static SG::AuxElement::ConstAccessor<float> tau3("Tau3_clstr");
  static SG::AuxElement::ConstAccessor<float> D2("D2_clstr");
  static SG::AuxElement::ConstAccessor<float> ECF1("ECF1_clstr");
  static SG::AuxElement::ConstAccessor<float> ECF2("ECF2_clstr");
  static SG::AuxElement::ConstAccessor<float> ECF3("ECF3_clstr");
  static SG::AuxElement::ConstAccessor<float> Qw("Qw_clstr");

  static SG::AuxElement::ConstAccessor<float> RRCpt("RRCJet_pt");
  static SG::AuxElement::ConstAccessor<float> RRCeta("RRCJet_eta");
  static SG::AuxElement::ConstAccessor<float> RRCphi("RRCJet_phi");
  static SG::AuxElement::ConstAccessor<float> RRCe("RRCJet_e");

  unsigned int i = 0;
  for (const xAOD::Jet* rc_jet: *rc_jets) {
    if (!m_rc->passSelection(*rc_jet))
      continue;

    auto rcjetEDM = tthevt.addCustomObject("rcjet");
    rcjetEDM->SetPtEtaPhiE(rc_jet->pt(), rc_jet->eta(), rc_jet->phi(), rc_jet->e());
    rcjetEDM->intVariable("index") = i;
    rcjetEDM->floatVariable("d12") = (RCSplit12.isAvailable(*rc_jet)) ? RCSplit12(*rc_jet) : -999.;
    rcjetEDM->floatVariable("d23") = (RCSplit23.isAvailable(*rc_jet)) ? RCSplit23(*rc_jet) : -999.;

    // cluster based JSS
    rcjetEDM->floatVariable("d12_clstr")   = (d12.isAvailable(*rc_jet))   ? d12(*rc_jet)   : -999.;
    rcjetEDM->floatVariable("d23_clstr")   = (d23.isAvailable(*rc_jet))   ? d23(*rc_jet)   : -999.;
    rcjetEDM->floatVariable("tau21_clstr") = (tau21.isAvailable(*rc_jet)) ? tau21(*rc_jet) : -999.;
    rcjetEDM->floatVariable("tau32_clstr") = (tau32.isAvailable(*rc_jet)) ? tau32(*rc_jet) : -999.;
    rcjetEDM->floatVariable("tau1_clstr")  = (tau1.isAvailable(*rc_jet))  ? tau1(*rc_jet)  : -999.;
    rcjetEDM->floatVariable("tau2_clstr")  = (tau2.isAvailable(*rc_jet))  ? tau2(*rc_jet)  : -999.;
    rcjetEDM->floatVariable("tau3_clstr")  = (tau3.isAvailable(*rc_jet))  ? tau3(*rc_jet)  : -999.;
    rcjetEDM->floatVariable("D2_clstr")    = (D2.isAvailable(*rc_jet))    ? D2(*rc_jet)    : -999.;
    rcjetEDM->floatVariable("ECF1_clstr")  = (ECF1.isAvailable(*rc_jet))  ? ECF1(*rc_jet)  : -999.;
    rcjetEDM->floatVariable("ECF2_clstr")  = (ECF2.isAvailable(*rc_jet))  ? ECF2(*rc_jet)  : -999.;
    rcjetEDM->floatVariable("ECF3_clstr")  = (ECF3.isAvailable(*rc_jet))  ? ECF3(*rc_jet)  : -999.;
    rcjetEDM->floatVariable("Qw_clstr")    = (Qw.isAvailable(*rc_jet))    ? Qw(*rc_jet)    : -999.;    
    rcjetEDM->floatVariable("rrcjet_pt")   = (RRCpt.isAvailable(*rc_jet)) ? RRCpt(*rc_jet)  : -999.;
    rcjetEDM->floatVariable("rrcjet_eta")  = (RRCeta.isAvailable(*rc_jet))? RRCeta(*rc_jet) : -999.;
    rcjetEDM->floatVariable("rrcjet_phi")  = (RRCphi.isAvailable(*rc_jet))? RRCphi(*rc_jet) : -999.;
    rcjetEDM->floatVariable("rrcjet_e")    = (RRCe.isAvailable(*rc_jet))  ? RRCe(*rc_jet)   : -999.;
    
    const xAOD::Jet* subjet(nullptr);
    for(auto rc_jet_subjet : rc_jet->getConstituents()){
      subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());

      TTHbb::Jet *matchedjet{nullptr};
      for (auto smalljet: tthevt.m_jets) {
        if (fabs(smalljet->Eta() - subjet->eta()) < 1e-6 && fabs(smalljet->Phi() - subjet->phi()) < 1e-6) {
          matchedjet = smalljet.get();
          break;
        }
      }
      if (matchedjet) {
        matchedjet->intVariable("rcjet_index") = i;
      } else {
        // this should not happen!
        std::cerr << "TTHbbLargeJetEDMConverter::apply: matching small jet for RC subjet NOT found" << std::endl;
      }
      
    } // end for-loop over subjets


    
    ++i;
  } // end for-loop over re-clustered jets
}


void TTHbbLargeJetEDMConverter::fillLargeJets(const xAOD::JetContainer& orig, TTHbb::Event& event) const {
  for(auto jet : orig){
    auto ljet = std::make_shared<TTHbb::LargeJet>(jet->pt(), jet->eta(), jet->phi(), jet->e());
  
    float Split23 = 0;
    float tau21_wta = 0;
    float tau32_wta = 0;
    float D2 = 0;
    float C2 = 0;
    char isTopTagged_50;
    char isTopTagged_80;

    jet->getAttribute("Split23", Split23);
    jet->getAttribute("tau21_wta", tau21_wta);
    jet->getAttribute("tau32_wta", tau32_wta);
    jet->getAttribute("D2", D2);
    jet->getAttribute("C2", C2);
    jet->getAttribute("isTopTagged_50", isTopTagged_50);
    jet->getAttribute("isTopTagged_80", isTopTagged_80);

    ljet->floatVariable("C2") = C2;
    ljet->floatVariable("D2") = D2;
    ljet->floatVariable("sd23") = Split23;
    ljet->floatVariable("tau21_wta") = tau21_wta;
    ljet->floatVariable("tau32_wta") = tau32_wta;
    ljet->charVariable("topTag") = isTopTagged_50;
    ljet->charVariable("topTag_loose") = isTopTagged_80;
    event.addLargeJet(ljet);
  }
}

std::string TTHbbLargeJetEDMConverter::name() const{
  return "TTHBBLARGEJETEDM";
}



