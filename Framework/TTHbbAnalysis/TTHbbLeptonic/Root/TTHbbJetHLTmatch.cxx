/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbJetHLTmatch.h"

#include "TLorentzVector.h"
#include "TopEvent/EventTools.h"
#include "TTHbbLeptonic/EventSaverHelper.h"


TTHbbJetHLTmatch::TTHbbJetHLTmatch(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
{
  m_params=params;
  
  auto* esh = EventSaverHelper::get();


  //==== TRIGGER TOOLS INITIALIZATION ====
  //Initialize and configure trigger tools
  if(asg::ToolStore::contains<TrigConf::xAODConfigTool>("xAODConfigTool")) {
    m_TrigConfigTool = asg::ToolStore::get<TrigConf::xAODConfigTool>("xAODConfigTool");
  } else {
    auto TrigConfigTool = new TrigConf::xAODConfigTool("xAODConfigTool"); 
    top::check(TrigConfigTool->initialize(),"Failed to initialize TrigConfTool");
    m_TrigConfigTool = TrigConfigTool;
  }

  if(asg::ToolStore::contains<Trig::TrigDecisionTool>("TrigDecisionTool")) {
    m_TrigDecisionTool = asg::ToolStore::get<Trig::TrigDecisionTool>("TrigDecisionTool");
  } else {
    ToolHandle< TrigConf::ITrigConfigTool > trigConfigHandle( m_TrigConfigTool );
    auto TrigDecisionTool = new Trig::TrigDecisionTool("TrigDecisionTool");
    top::check(TrigDecisionTool->setProperty( "ConfigTool", trigConfigHandle ),"Failed to set Property for TrigDecisionTool");
    top::check(TrigDecisionTool->setProperty( "TrigDecisionKey", "xTrigDecision" ),"Failed to set Property for TrigDecisionTool");
    top::check(TrigDecisionTool->setProperty( "AcceptMultipleInstance", "1" ),"Failed to set Property for TrigDecisionTool");
    top::check(TrigDecisionTool->initialize(),"Failed to initialize TrigDecisionTool");
    m_TrigDecisionTool = TrigDecisionTool;
  }

  //set list of triggers
  m_SplitJetTriggerList = {
    "HLT_j35_boffperf_split",
    "HLT_j175_bmv2c2040_split",
    "HLT_j225_bmv2c2060_split",
    "HLT_j275_bmv2c2070_split",
    "HLT_j300_bmv2c2077_split",
    "HLT_j360_bmv2c2085_split",
    "HLT_2j35_bmv2c2060_split_2j35_L14J15.0ETA25",
    "HLT_2j45_bmv2c2077_split_3j45",
    "HLT_2j45_bmv2c2077_split_3j45_L14J15.0ETA25",
    "HLT_2j55_bmv2c2060_split_ht300_L14J15",
    "HLT_2j70_bmv2c2060_split_j70",
    "HLT_2j75_bmv2c2070_split_j75",
    "HLT_j100_2j55_bmv2c2060_split",
    "HLT_j150_bmv2c2060_split_j50_bmv2c2060_split",
    "HLT_j175_bmv2c2060_split_j50_bmv2c2050_split",
    "HLT_j150_bmv2c2077_split_j50_bmv2c2077_split",
    "HLT_2j35_bmv2c2070_split_2j35_L14J15",
    "HLT_2j65_bmv2c2070_split_j65",
    "HLT_2j55_bmv2c2070_split_ht300_L14J15",
    "HLT_j100_2j55_bmv2c2077_split",
    "HLT_2j35_bmv2c2050_split_2j35_L14J15",
    "HLT_2j70_bmv2c2070_split_j70",
    "HLT_j65_bmv2c2070_split_3j65_L14J15",
    "HLT_j55_bmv2c2060_split_ht500_L14J15",
    "HLT_j75_bmv2c2070_split_3j75_L14J15.0ETA25",
    "HLT_mu26_ivarmedium_2j35_boffperf_split",
    "HLT_e26_lhtight_nod0_ivarloose_2j35_boffperf_split",
    "HLT_e28_lhtight_nod0_ivarloose_2j35_boffperf_split",
    "HLT_2mu14_2j35_boffperf_split",
  };



  // Add online mv2 variable
  esh->addJetVariableToBeSaved("onlinemv2",TTHbb::DecorationType::Float,false,"TTHbbJetHLTmatch");


}


TTHbbJetHLTmatch::~TTHbbJetHLTmatch(){

}


bool TTHbbJetHLTmatch::apply(const top::Event & event) const{


  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params 
	      << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");


  //evaluate online mv2 var
  jethltmatch(tthevt->m_jets);
  
  return true;

}


void TTHbbJetHLTmatch::jethltmatch(const TTHbb::JetContainer& Jets) const{

  //define trigjet variables
  std::vector<float> m_HLT_SplitJet_pt;
  std::vector<float> m_HLT_SplitJet_et;
  std::vector<float> m_HLT_SplitJet_eta;
  std::vector<float> m_HLT_SplitJet_phi;
  std::vector<float> m_HLT_SplitJet_m;
  std::vector<float> m_HLT_SplitJet_mv2c00;
  std::vector<float> m_HLT_SplitJet_mv2c10;
  std::vector<float> m_HLT_SplitJet_mv2c20;
  std::vector<float> m_jet_online_mv2c20;


  ///////////////TRIGGER NAVIGATION///////////////

  std::string m_TriggerChain;

  //SplitJets
  for (unsigned int isplittrig = 0; isplittrig < m_SplitJetTriggerList.size(); isplittrig++) {
    m_TriggerChain = m_SplitJetTriggerList[isplittrig];
    auto cg = m_TrigDecisionTool->getChainGroup(m_TriggerChain);
    if (cg->isPassed()) break;
  }

  std::vector<TLorentzVector> SplitJetVectors;

  Trig::FeatureContainer fcsplit = m_TrigDecisionTool->features(m_TriggerChain);
  Trig::FeatureContainer::combination_const_iterator comb_split   (fcsplit.getCombinations().begin());
  Trig::FeatureContainer::combination_const_iterator combEnd_split(fcsplit.getCombinations().end());

  for( ; comb_split!=combEnd_split ; ++comb_split) {
    std::vector< Trig::Feature<xAOD::JetContainer> >  jetCollections  = comb_split->containerFeature<xAOD::JetContainer>("SplitJet");
    std::vector< Trig::Feature<xAOD::BTaggingContainer> > bjetCollections = comb_split->containerFeature<xAOD::BTaggingContainer>("HLTBjetFex");

    if(jetCollections.size() != bjetCollections.size()){
      std::cout << "ERROR Problem in container size SplitJet  -->  jets: "<< jetCollections.size() << " bjets: "<< bjetCollections.size() <<"\n";
      continue;
    }

    for ( unsigned ifeat=0 ; ifeat<jetCollections.size() ; ifeat++ ) {
      const xAOD::Jet* hlt_jet = getTrigObject<xAOD::Jet, xAOD::JetContainer>(jetCollections.at(ifeat));
      if(!hlt_jet) continue;
      const xAOD::BTagging* hlt_btag = getTrigObject<xAOD::BTagging, xAOD::BTaggingContainer>(bjetCollections.at(ifeat));
      if(!hlt_btag) continue;

      TLorentzVector vtemp; vtemp.SetPtEtaPhiM(hlt_jet->pt(), hlt_jet->eta(), hlt_jet->phi(), hlt_jet->m());
      bool notsaved = true;
      for (auto vsaved : SplitJetVectors) if (vsaved.DeltaR(vtemp) == 0) notsaved = false;

      if (notsaved) {

	double MV2c00_mvx = -999, MV2c10_mvx = -999, MV2c20_mvx = -999;
	hlt_btag->MVx_discriminant("MV2c00" ,MV2c00_mvx);
	hlt_btag->MVx_discriminant("MV2c10" ,MV2c10_mvx);
	hlt_btag->MVx_discriminant("MV2c20" ,MV2c20_mvx);

	m_HLT_SplitJet_pt.push_back(hlt_jet->pt());
	m_HLT_SplitJet_eta.push_back(hlt_jet->eta());
	m_HLT_SplitJet_phi.push_back(hlt_jet->phi());
	m_HLT_SplitJet_m.push_back(hlt_jet->m());
	m_HLT_SplitJet_et.push_back(vtemp.Et());
	m_HLT_SplitJet_mv2c00.push_back(MV2c00_mvx);
	m_HLT_SplitJet_mv2c10.push_back(MV2c10_mvx);
	m_HLT_SplitJet_mv2c20.push_back(MV2c20_mvx);

	SplitJetVectors.push_back(vtemp);
      }

    }
  }

  //jet matching
  std::vector<uint> matchedsplitjets;
   
  for(auto jet : Jets){

    TLorentzVector vjet = jet->p4();
    float online_mv2 = -2;
    for (uint itrigjet = 0; itrigjet < m_HLT_SplitJet_pt.size(); itrigjet++) {
      bool isalreadymatched = false;
      for (auto imtj : matchedsplitjets) if (itrigjet == imtj) isalreadymatched = true;
      if (isalreadymatched) continue;
      TLorentzVector vtrigjet;
      vtrigjet.SetPtEtaPhiM(m_HLT_SplitJet_pt.at(itrigjet), m_HLT_SplitJet_eta.at(itrigjet), m_HLT_SplitJet_phi.at(itrigjet), m_HLT_SplitJet_m.at(itrigjet));
      if (vjet.DeltaR(vtrigjet) < 0.2) {
	online_mv2=m_HLT_SplitJet_mv2c20.at(itrigjet);
	matchedsplitjets.push_back(itrigjet);
	break;
      }
    }

    jet->floatVariable("onlinemv2")=online_mv2;


  }// end jet loop


}


std::string TTHbbJetHLTmatch::name() const{
  return "JETHLTMATCH";
}



