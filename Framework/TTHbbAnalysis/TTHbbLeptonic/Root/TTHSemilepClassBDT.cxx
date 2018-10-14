/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHSemilepClassBDT.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHSemilepClassBDT::TTHSemilepClassBDT(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {
 
   m_params=params;


   m_nJets=0;
   m_nbJets=0;
   m_leptonType=0;

   m_xmlPath+="/BDTweights_semilep/classification";

   /// keep this before init tool
   m_configMap["RecoFlag"]="";
   initTool();
   //// keep this after init tool
   m_recoFlag=m_configMap["RecoFlag"];

   std::cout << "TTHbbSemilepClass with options:"<< std::endl;
   for(auto a : m_configMap){
     std::cout << " "<< a.first<<":"<<a.second;
   }
   std::cout << std::endl;


   m_njets_cuts=-1;
   m_nmaxjets_cuts=-1;
   m_nbjets_cuts=-1;
   m_nmaxbjets_cuts=-1;
   m_regionName="";
   m_regionVal=-1;

   if(m_region.find("=")!=std::string::npos){
     TTHbb::util::decodeRegionName(m_region, m_regionName, m_regionVal);
   }
   else{
     TTHbb::util::decodeRegionName(m_region, m_njets_cuts, m_nmaxjets_cuts, m_nbjets_cuts, m_nmaxbjets_cuts);
   }

   std::string xml = m_xmlPath+"/TMVAClassification_BDT"+m_xmlFlag;
   initReaders(xml);
   registerSaverVariables();

}

TTHSemilepClassBDT::~TTHSemilepClassBDT(){
}


bool TTHSemilepClassBDT::apply(const top::Event & event) const{


  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
   std::cout <<name() <<": TTHbbEventVariables (std::shared_ptr<TTHbb::Event>) object not found" << std::endl;
   std::cout << "-----> more info: <params: " << m_params 
	     << "> <systname: " << m_config->systematicName(event.m_hashValue) << ">" << std::endl;
   std::cout << "------> aborting :-( " << std::endl;
   abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt = event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");


  TTHbb::JetContainer& jets = tthevt->m_jets;
  TTHbb::LeptonContainer& leptons = tthevt->m_leptons;
  // TTHbb::NeutrinoContainer& neutrinos = tthevt->m_neutrinos;

  m_nJets=jets.size();
  m_nbJets=tthevt->intVariable("nBTags_"+m_btagOP+m_trf);
  m_leptonType=tthevt->intVariable("leptonType");


  defaultValues(tthevt.get());
 
  if(leptons.size() != 1){
    return true;
  }
  if(m_nJets<5){ /// will never do it in 4j
    return true;
  }

  if(!passRegion(tthevt.get())) return true;

  m_classBDTCalculatorTool.computeBDT(tthevt.get(), m_nJets, m_nbJets, m_leptonType, tthevt->m_info->eventNumber);

  return true;

}

void TTHSemilepClassBDT::getBDTVars(std::vector<std::string> &BDTVar){

  BDTVar.clear();

  std::string recos = m_strategy;
  std::transform(recos.begin(), recos.end(), recos.begin(), ::tolower);
  bool ichep = recos.find("ichep") != std::string::npos;
  bool addreco = recos.find("reco") != std::string::npos;
  bool b4l4 = recos.find("b4l4") != std::string::npos;
  bool btagonly = recos.find("btagonly") != std::string::npos;
  bool btagonly1 = recos.find("btagonly1") != std::string::npos;
  //bool mva70 = recos.find("mva70") != std::string::npos;
  bool withLL = recos.find("withll") != std::string::npos;
  bool withBTag = recos.find("withbtag") != std::string::npos;

  bool reduced = recos.find("reduced") != std::string::npos;
  bool inclusive = recos.find("inclusive") != std::string::npos;
  bool nojet5 = recos.find("nojet5") != std::string::npos;

  if(reduced){

    getBDTVarsReduced(BDTVar);
    if(inclusive){
      getBDTVarsReducedInclusive(BDTVar, !nojet5);
    }

    return;
  }

  std::string btagopVar=m_btagOP;
  if(b4l4)btagopVar="Sort4";

  if(btagonly){
    if(btagonly1){
      getBDTVarsBTag1(BDTVar);
    }
    else{
      getBDTVarsBTag(BDTVar);
    }
    return;
  }

  if(ichep){
    getBDTVarsKinICHEP(BDTVar, btagopVar);
    if(addreco){
      getBDTVarsRecoICHEP(BDTVar);
    }
  }
  else{
    getBDTVarsKin(BDTVar, btagopVar);
    if(addreco){
      getBDTVarsReco(BDTVar);
    }
    if(withLL){
      BDTVar.push_back("LHD_Discriminant");
    }
    if(withBTag){
      /// quick hack with all the possibilities we have
      TString xmlflag = m_xmlFlag.c_str();
      if(!xmlflag.Contains("H501") && !xmlflag.Contains("E601") 
	 && !xmlflag.Contains("C501") && !xmlflag.Contains("C601") 
	 && !xmlflag.Contains("E602") & !xmlflag.Contains("E604")){
	 BDTVar.push_back("jet_mv2_order_3_tagWeightBin/I");
      }
      if(!xmlflag.Contains("H501") && !xmlflag.Contains("E601") && !xmlflag.Contains("E604") ){
	BDTVar.push_back("jet_mv2_order_4_tagWeightBin/I");
      }
      if(!xmlflag.Contains("E604") ){
	BDTVar.push_back("jet_mv2_order_5_tagWeightBin/I");
      }
      if(!xmlflag.Contains("H501") && !xmlflag.Contains("E601") ){
	BDTVar.push_back(m_recoFlag+"_best_bbHiggs_tagWeightBin_sum/I");
      }
    }
  }

}

/// can change and read from text file
void TTHSemilepClassBDT::getBDTVarsKinICHEP(std::vector<std::string> &BDTVar, std::string& btagopVar){

  /// should remove when xmls with new names are updated

  if( "6je4bi" == m_varOption || "7ji4bi" == m_varOption || "6ji4bi" == m_varOption){

    BDTVar.push_back("Centrality_all:Centrality"   );
    BDTVar.push_back("Mbb_MindR_"+btagopVar+m_trf+":Mbb_MindR"    );
    BDTVar.push_back("dRbb_MaxPt_"+btagopVar+m_trf+":dRbb_MaxPt"   );
    BDTVar.push_back("pT_jet5"      );
    BDTVar.push_back("H1_all"       );
    BDTVar.push_back("dRbb_avg_"+btagopVar+m_trf+":dRbb_avg"     );
    BDTVar.push_back("nHiggsbb30_"+btagopVar+m_trf+":NHiggs_30/I");
    BDTVar.push_back("dEtajj_MaxdEta");
    BDTVar.push_back("Aplanarity_jets:Aplan_jets");

  }

  if("6ji3be" == m_varOption ){

    BDTVar.push_back("Centrality_all:Centrality"   );
    BDTVar.push_back("Mbb_MindR_"+btagopVar+m_trf+":Mbb_MindR"    );
    BDTVar.push_back("pT_jet5"      );
    BDTVar.push_back("H1_all"       );
    BDTVar.push_back("dRbb_avg_"+btagopVar+m_trf+":dRbb_avg"     );
    BDTVar.push_back("Mbj_MaxPt_"+btagopVar+m_trf+":Mbj_MaxPt"    );
    BDTVar.push_back("nJets_Pt40:Njet_pt40/I"    );
    BDTVar.push_back("HT_jets:HhadT_jets"    );
    BDTVar.push_back("dEtajj_MaxdEta"    );
    BDTVar.push_back("Aplanarity_jets:Aplan_jets"    );

  }

  if("5je4bi" == m_varOption ){

    BDTVar.push_back("Centrality_all:Centrality"   );
    BDTVar.push_back("H1_all"       );
    BDTVar.push_back("dRbb_avg_"+btagopVar+m_trf+":dRbb_avg"     );
    BDTVar.push_back("dRlepbb_MindR_"+btagopVar+m_trf+":dRlepbb_MindR");
    BDTVar.push_back("HT_jets:HhadT_jets"  );
    BDTVar.push_back("Mjj_MindR"   );
    BDTVar.push_back("pT_jet5"   );
    BDTVar.push_back("nHiggsbb30_"+btagopVar+m_trf+":NHiggs_30/I");
    BDTVar.push_back("dEtajj_MaxdEta"  );
    BDTVar.push_back("Aplanarity_jets:Aplan_jets"   );


  }

}


void TTHSemilepClassBDT::getBDTVarsKin(std::vector<std::string> &BDTVar, std::string& btagopVar){

  if( "6je4bi" == m_varOption || "7ji4bi" == m_varOption || "6ji4bi" == m_varOption){

    BDTVar.push_back("Centrality_all"   );
    BDTVar.push_back("Mbb_MindR_"+btagopVar+m_trf+":Mbb_MindR_"+btagopVar    );
    BDTVar.push_back("dRbb_MaxPt_"+btagopVar+m_trf+":dRbb_MaxPt_"+btagopVar   );
    BDTVar.push_back("pT_jet5"      );
    BDTVar.push_back("H1_all"       );
    BDTVar.push_back("dRbb_avg_"+btagopVar+m_trf+":dRbb_avg_"+btagopVar     );
    BDTVar.push_back("nHiggsbb30_"+btagopVar+m_trf+":nHiggsbb30_"+btagopVar+"/I");
    BDTVar.push_back("dEtajj_MaxdEta");
    BDTVar.push_back("Aplanarity_jets");

  }

  if("5je4bi" == m_varOption ){

    BDTVar.push_back("Centrality_all");
    BDTVar.push_back("H1_all");
    BDTVar.push_back("dRbb_avg_"+btagopVar+m_trf+":dRbb_avg_"+btagopVar     );
    BDTVar.push_back("dRlepbb_MindR_"+btagopVar+m_trf+":dRlepbb_MindR_"+btagopVar);
    BDTVar.push_back("HT_jets");
    BDTVar.push_back("Mjj_MindR");
    BDTVar.push_back("pT_jet5");
    BDTVar.push_back("nHiggsbb30_"+btagopVar+m_trf+":nHiggsbb30_"+btagopVar+"/I");
    BDTVar.push_back("dEtajj_MaxdEta");
    BDTVar.push_back("Aplanarity_jets");

  }

}

void TTHSemilepClassBDT::getBDTVarsRecoICHEP(std::vector<std::string> &BDTVar){


  if( "6je4bi" == m_varOption || "7ji4bi" == m_varOption || "6ji4bi" == m_varOption){

    BDTVar.push_back(m_recoFlag+"_best_Higgs_mass:BDTOutput_basic_best_semilepMVAreco_Higgs_mass");
    BDTVar.push_back(m_recoFlag+"_best_bbHiggs_dR:BDTOutput_basic_best_semilepMVAreco_bbHiggs_dR");
    BDTVar.push_back(m_recoFlag+"_best_Higgsleptop_dR:BDTOutput_basic_best_semilepMVAreco_Higgsleptop_dR");   
    BDTVar.push_back(m_recoFlag+"_best_Higgsbleptop_mass:BDTOutput_basic_best_semilepMVAreco_Higgsbleptop_mass");
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsttbar_dR:BDTOutput_withH_basic_best_semilepMVAreco_Higgsttbar_dR" );    
    BDTVar.push_back(m_recoFlag+"_withH_best_"+m_recoFlag+"_withH:BDTOutput_withH_basic_best_BDTOutput_withH_basic");

  }

  if("6ji3be" == m_varOption ){

    BDTVar.push_back(m_recoFlag+"_best_Higgs_mass:BDTOutput_basic_best_semilepMVAreco_Higgs_mass");
    BDTVar.push_back(m_recoFlag+"_best_bbHiggs_dR:BDTOutput_basic_best_semilepMVAreco_bbHiggs_dR");
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsbhadtop_dR:BDTOutput_withH_basic_best_semilepMVAreco_Higgsbhadtop_dR");
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsttbar_dR:BDTOutput_withH_basic_best_semilepMVAreco_Higgsttbar_dR"  );   
    BDTVar.push_back(m_recoFlag+"_withH_best_"+m_recoFlag+"_withH:BDTOutput_withH_basic_best_BDTOutput_withH_basic");

  }

  if("5je4bi" == m_varOption ){

    BDTVar.push_back(m_recoFlag+"_best_Higgs_mass:BDTOutput_basic_best_semilepMVAreco_Higgs_mass");
    BDTVar.push_back(m_recoFlag+"_best_bbHiggs_dR:BDTOutput_basic_best_semilepMVAreco_bbHiggs_dR");
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsbhadtop_dR:BDTOutput_withH_basic_best_semilepMVAreco_Higgsbhadtop_dR"  );
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsttbar_dR:BDTOutput_withH_basic_best_semilepMVAreco_Higgsttbar_dR"    );
    BDTVar.push_back(m_recoFlag+"_withH_best_"+m_recoFlag+"_withH:BDTOutput_withH_basic_best_BDTOutput_withH_basic");

  }

}

void TTHSemilepClassBDT::getBDTVarsReco(std::vector<std::string> &BDTVar){


  if( "6je4bi" == m_varOption || "7ji4bi" == m_varOption || "6ji4bi" == m_varOption){

    BDTVar.push_back(m_recoFlag+"_best_Higgs_mass");
    BDTVar.push_back(m_recoFlag+"_best_bbHiggs_dR");
    BDTVar.push_back(m_recoFlag+"_best_Higgsleptop_dR");   
    BDTVar.push_back(m_recoFlag+"_best_Higgsbleptop_mass");
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsttbar_dR" );    
    BDTVar.push_back(m_recoFlag+"_withH_best_"+m_recoFlag+"_withH");

  }


  if("5je4bi" == m_varOption ){

    BDTVar.push_back(m_recoFlag+"_best_Higgs_mass" ); 
    BDTVar.push_back(m_recoFlag+"_best_bbHiggs_dR");
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsbhadtop_dR");
    BDTVar.push_back(m_recoFlag+"_withH_best_Higgsttbar_dR");
    BDTVar.push_back(m_recoFlag+"_withH_best_TTHReco_withH");  


  }

}


void TTHSemilepClassBDT::getBDTVarsBTag(std::vector<std::string> &BDTVar){


  BDTVar.push_back(m_recoFlag+"_best_b1Higgs_tagWeightBin/I");
  BDTVar.push_back(m_recoFlag+"_best_b2Higgs_tagWeightBin/I");
  BDTVar.push_back(m_recoFlag+"_best_bbHiggs_tagWeightBin_sum/I");
  BDTVar.push_back(m_recoFlag+"_best_bbTop_bbHiggs_tagWeightBin_diff/I");
  BDTVar.push_back(m_recoFlag+"_withH_best_bhadTop_tagWeightBin/I");

  BDTVar.push_back("jet_pt_order_4_tagWeightBin/I");
  BDTVar.push_back("jet_pt_order_5_tagWeightBin/I");
  BDTVar.push_back("jet_pt_order_6_tagWeightBin/I");

  BDTVar.push_back("jet_mv2_order_2_tagWeightBin/I");

  BDTVar.push_back("jet_eta_order_1_tagWeightBin/I");
  BDTVar.push_back("jet_eta_order_2_tagWeightBin/I");
  BDTVar.push_back("jet_eta_order_3_tagWeightBin/I");
  BDTVar.push_back("jet_eta_order_4_tagWeightBin/I");
  BDTVar.push_back("jet_eta_order_5_tagWeightBin/I");
  BDTVar.push_back("jet_eta_order_6_tagWeightBin/I");

}

void TTHSemilepClassBDT::getBDTVarsBTag1(std::vector<std::string> &BDTVar){

  BDTVar.push_back(m_recoFlag+"_best_b1Higgs_tagWeightBin/I");
  BDTVar.push_back(m_recoFlag+"_best_b2Higgs_tagWeightBin/I");
  BDTVar.push_back(m_recoFlag+"_best_bbHiggs_tagWeightBin_sum/I");
  BDTVar.push_back(m_recoFlag+"_best_bbTop_bbHiggs_tagWeightBin_diff/I");
  BDTVar.push_back(m_recoFlag+"_withH_best_bhadTop_tagWeightBin/I");

  BDTVar.push_back("jet_pt_order_4_tagWeightBin/I");
  BDTVar.push_back("jet_pt_order_5_tagWeightBin/I");
  BDTVar.push_back("jet_pt_order_6_tagWeightBin/I");

  BDTVar.push_back("jet_mv2_order_2_tagWeightBin/I");
  BDTVar.push_back("jet_mv2_order_3_tagWeightBin/I");
  BDTVar.push_back("jet_mv2_order_4_tagWeightBin/I");
  BDTVar.push_back("jet_mv2_order_5_tagWeightBin/I");

  BDTVar.push_back("jet_eta_order_4_tagWeightBin/I");
  BDTVar.push_back("jet_eta_order_5_tagWeightBin/I");
  BDTVar.push_back("jet_eta_order_6_tagWeightBin/I");
  BDTVar.push_back("nBTags_60/I");
   

}

void TTHSemilepClassBDT::getBDTVarsReduced(std::vector<std::string> &BDTVar){

  if( "6ji4bi" == m_varOption){
    BDTVar.push_back("LHD_Discriminant");
    BDTVar.push_back("TTHReco_withH_best_TTHReco_withH");
    BDTVar.push_back("dEtajj_MaxdEta");
    BDTVar.push_back("Mbb_MindR_Sort4");
    BDTVar.push_back("dRbb_avg_Sort4");
    BDTVar.push_back("dRbb_MaxPt_Sort4");
    BDTVar.push_back("Aplanarity_bjets_Sort4"); 
  }

  if("5je4bi" == m_varOption ){
    BDTVar.push_back("LHD_Discriminant");
    BDTVar.push_back("TTHReco_withH_best_TTHReco_withH");
    BDTVar.push_back("Mjj_MindR");
    BDTVar.push_back("dEtajj_MaxdEta");
    BDTVar.push_back("Mbb_MindR_Sort4");
    BDTVar.push_back("dRbb_avg_Sort4");
    BDTVar.push_back("dRlepbb_MindR_Sort4");
    BDTVar.push_back("Aplanarity_jets");
    BDTVar.push_back("H1_all");
    BDTVar.push_back("nHiggsbb30_Sort4/I");
    BDTVar.push_back("TTHReco_best_Higgs_mass");
  }

}

void TTHSemilepClassBDT::getBDTVarsReducedInclusive(std::vector<std::string> &BDTVar, bool addjet5){

  if( "6ji4bi" == m_varOption){
    BDTVar.push_back("TTHReco_withH_best_lepTophadTop_dR");
    if(addjet5)BDTVar.push_back("pT_jet5");
    BDTVar.push_back("Aplanarity_jets");
    BDTVar.push_back("TTHReco_best_Higgshadtop_dR");
    BDTVar.push_back("jet_mv2_order_3_tagWeightBin/I");
    BDTVar.push_back("jet_mv2_order_4_tagWeightBin/I");
    BDTVar.push_back("jet_mv2_order_5_tagWeightBin/I");
    BDTVar.push_back("TTHReco_best_bbHiggs_tagWeightBin_sum/I");

  }

  if("5je4bi" == m_varOption ){
    BDTVar.push_back("jet_mv2_order_3_tagWeightBin/I");
    BDTVar.push_back("jet_mv2_order_4_tagWeightBin/I");
    BDTVar.push_back("jet_mv2_order_5_tagWeightBin/I");
    BDTVar.push_back("TTHReco_best_bbHiggs_tagWeightBin_sum/I");
  }

}


bool TTHSemilepClassBDT::passRegion(TTHbb::Event* evt) const{

  if(m_regionName!=""){
    if(!evt->checkIntVariable(m_regionName)){
      std::cout << "ERROR: TTHSemilepReco::passRegion region not found " << m_regionName << std::endl;
      return false;
    }
    if(evt->intVariable(m_regionName)==m_regionVal)return true;
    return false;
  }

  if(m_njets_cuts<=m_nmaxjets_cuts){
    if(m_nJets<m_njets_cuts && m_njets_cuts>=0) return false;
    if(m_nJets>m_nmaxjets_cuts && m_nmaxjets_cuts>=0) return false;
  }

  if(m_nbjets_cuts<=m_nmaxbjets_cuts){
    if(m_nbJets<m_nbjets_cuts && m_nbjets_cuts>=0) return false;
    if(m_nbJets>m_nmaxbjets_cuts && m_nmaxbjets_cuts>=0) return false;
  }

  return true;

}



std::string TTHSemilepClassBDT::name() const{

  return "TTHSEMILEPCLASSBDT";

}



