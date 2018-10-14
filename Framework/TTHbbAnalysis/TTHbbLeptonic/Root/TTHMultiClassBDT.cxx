/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHMultiClassBDT.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TLorentzVector.h"
#include <sstream>
#include <algorithm>

TTHMultiClassBDT::TTHMultiClassBDT(std::string params,std::shared_ptr<top::TopConfig> config):
  m_config(config)
 {

   m_params=params;


   m_nJets=0;
   m_nbJets=0;
   m_leptonType=0;

   m_xmlPath+="/BDTweights_MultiClass";

   /// keep this before init tool
   m_configMap["RecoFlag"]="";
   initTool();
   //// keep this after init tool
   m_recoFlag=m_configMap["RecoFlag"];

   std::cout << "TTHbbMultiClass with options:"<< std::endl;
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

   std::string xml = m_xmlPath+"/MultiClassBDT_"+m_xmlFlag;
   getMultiClassNames(multiclass_names);
   initReaders(xml);
   registerSaverVariables();
}

TTHMultiClassBDT::~TTHMultiClassBDT(){
}


bool TTHMultiClassBDT::apply(const top::Event & event) const{


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

  m_classBDTCalculatorTool.computeBDTMultiClass(tthevt.get(), m_nJets, m_nbJets, m_leptonType, tthevt->m_info->eventNumber);

  return true;

}

void TTHMultiClassBDT::getBDTVars(std::vector<std::string> &BDTVar){
  BDTVar.clear();
  getMultiClassVariables(BDTVar);

  return;
}


void TTHMultiClassBDT::getMultiClassVariables(std::vector<std::string> &BDTVar){
  // m_varOption flag can be used to add different configurations
  if( m_varOption == "5je3bi85_01pt"){
    BDTVar.push_back("jet_pt_order_1_tagWeightBin/I");
    BDTVar.push_back("jet_pt_order_2_tagWeightBin/I");
    BDTVar.push_back("jet_pt_order_3_tagWeightBin/I");
    BDTVar.push_back("jet_pt_order_4_tagWeightBin/I");
    BDTVar.push_back("jet_pt_order_5_tagWeightBin/I");
    BDTVar.push_back("H1_all/F");
    BDTVar.push_back("Aplanarity_jets:Aplan_jets/F");
    BDTVar.push_back("Centrality_all/F");
  } else if( m_varOption == "6ji3bi85_01mv"){
    BDTVar.push_back("nJets/I");
    BDTVar.push_back("jet_tagWeightBin_order_2_tagWeightBin/I");
    BDTVar.push_back("jet_tagWeightBin_order_3_tagWeightBin/I");
    BDTVar.push_back("jet_tagWeightBin_order_4_tagWeightBin/I");
    BDTVar.push_back("jet_tagWeightBin_order_5_tagWeightBin/I");
    BDTVar.push_back("H1_all/F");
    BDTVar.push_back("Aplanarity_jets:Aplan_jets/F");
    BDTVar.push_back("Centrality_all/F");
  }
}


bool TTHMultiClassBDT::passRegion(TTHbb::Event* evt) const{

  if(m_regionName!=""){
    if(!evt->checkIntVariable(m_regionName)){
      std::cout << "ERROR: TTHMultiClassBDT::passRegion region not found " << m_regionName << std::endl;
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

void TTHMultiClassBDT::initReaders(std::string xml){

  EventSampling OddSampling;
  EventSampling EvenSampling;

  OddSampling.modulo=2;
  OddSampling.rest.push_back(1);
  OddSampling.patternName="modulo_2_rest_0"; /// the training is Even

  EvenSampling.modulo=2;
  EvenSampling.rest.push_back(0);
  EvenSampling.patternName="modulo_2_rest_1";/// the training is Odd

  std::vector<std::string> classVar;
  getBDTVars(classVar);

  TMVAConfiguration classConfig;
  classConfig.samplings.push_back(OddSampling);
  classConfig.samplings.push_back(EvenSampling);

  classConfig.name=m_flag;
  classConfig.setCalibFilePattern(xml);
  classConfig.setMultiClassName(multiclass_names);
  classConfig.setVariables(classVar);
  classConfig.bookReader();

  RegionInterface::RegionDefinition region;
  //// can add region cuts into RegionDefinition
  region.tmvaConfiguration=classConfig;

  m_classBDTCalculatorTool.addRegion(region);
}


void TTHMultiClassBDT::registerSaverVariables(){
  auto* esh = EventSaverHelper::get();
  for (auto name : multiclass_names){
    esh->addVariableToBeSaved(name, TTHbb::DecorationType::Float, false, "TMVAClassificationInterface" + name);
  }
}

void TTHMultiClassBDT::getMultiClassNames(std::vector<std::string> &multi_names){
  char *myString = new char [m_flag.size() + 1];
  std::copy(m_flag.begin(), m_flag.end(), myString);
  myString[m_flag.size()] = '\0';
  char *p = strtok(myString, ";");
  std::vector<std::string> names;
  std::string prefix = p;
  while (p) {
    names.push_back((prefix + "_" + p).c_str());
    p = strtok(NULL, ";");
  }
  names.erase (names.begin()); //delete first entry of vector
  multi_names = names;
  delete[] myString;
  delete p;
}

void TTHMultiClassBDT::defaultValues(TTHbb::Event* tthevt) const{
  for (auto name : multiclass_names){
    if(!tthevt->checkFloatVariable(name)){
      tthevt->floatVariable(name)=-9;
    }
  }
}

std::string TTHMultiClassBDT::name() const{
  return "TTHMULTICLASSBDT";
}
