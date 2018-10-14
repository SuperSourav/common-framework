/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTaggingTruthTagging/BTaggingTruthTaggingTool.h"

//#include <AsgTools/MessageCheck.h>

#include <string>
#include <iomanip>
#include <vector>

//using CP::CorrectionCode;
//using CP::SystematicCode;

int main() {

  //  ANA_CHECK_SET_TYPE (StatusCode);

  xAOD::TStore store;

  BTaggingTruthTaggingTool * tool = new BTaggingTruthTaggingTool("BTagTest");

  StatusCode::enableFailure();
  CP::SystematicCode::enableFailure();
  CP::CorrectionCode::enableFailure();


  // properties not yet tested
  //  declareProperty( "MaxEta", m_maxEta = 2.5 );
  //  declareProperty( "MinPt", m_minPt = 20000 /*MeV*/);
  //  declareProperty( "MaxRangePt", m_maxRangePt = 1000000 /*MeV*/);
  //  declareProperty( "FlvTagCutDefinitionsFileName", m_CutFileName = "", "name of the files containing official cut definitions (uses PathResolver)");

  StatusCode  code = tool->setProperty("TaggerName",          "MV2c10");
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool TaggerName property" << std::endl;

  code = tool->setProperty("OperatingPoint", "FixedCutBEff_77");
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool OperatingPoint property" << std::endl;
  
  code = tool->setProperty("JetAuthor", "AntiKt4EMTopoJets");
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool JetAuthor property" << std::endl;
  
  code = tool->setProperty("ScaleFactorFileName", "xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-May31_v1.root");
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool ScaleFactorFileName property" << std::endl;
  
  code = tool->setProperty("IgnoreScaleFactors", false);
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool IgnoreScaleFactors property" << std::endl;
    
  code = tool->setProperty("UsePermutations", true);
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool UsePermutations property" << std::endl;
  
  code = tool->setProperty("UseQuantile", true);
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool UseQuantile property" << std::endl;  

  code = tool->setProperty("SystematicsStrategy", "Envelope");
  if (code != StatusCode::SUCCESS) std::cout << "error setting BTaggingTruthTaggingTool SystematicsStrategy property" << std::endl;  

  code = tool->initialize();
  if (code != StatusCode::SUCCESS) std::cout << "error" << std::endl;

  std::vector<double> pt = {44000., 66000., 77000.};
  std::vector<double> eta = {2.2, 1.6, 1.7};
  std::vector<int> flav = {0,4,5};
  std::vector<double> tagw = {0.3, 0.55, 0.99};
  
  std::vector<double> trf_weight_ex;
  std::vector<double> trf_weight_in;

  std::vector<std::vector<bool> > trf_chosen_perm_ex;
  std::vector<std::vector<bool> > trf_chosen_perm_in;;

  std::map<std::string,std::vector<double> > map_trf_weight_ex;
  std::map<std::string,std::vector<double> > map_trf_weight_in;

  std::cout << "SetSeed" << std::endl;

  code = tool->setSeed(100);
  if (code != StatusCode::SUCCESS) std::cout << "error" << std::endl;
  std::cout << "SetJets" << std::endl;
  code = tool->setJets(&pt,&eta,&flav,&tagw);
  if (code != StatusCode::SUCCESS) std::cout << "error" << std::endl;
  //code = tool->getTruthTagWei(2, trf_weight_ex, trf_weight_in);
  //  code = tool->getTruthTagWei(2, trf_weight_ex, trf_weight_in, "FT_EFF_Eigen_B_0__1down");
  std::cout << "getWei" << std::endl;
  code = tool->getTruthTagWei(2, map_trf_weight_ex, map_trf_weight_in);
  if (code != StatusCode::SUCCESS) std::cout << "error" << std::endl;
  std::cout << "Keys in maps" << std::endl;
  for(auto wei: map_trf_weight_ex) std::cout << wei.first << std::endl;
  std::cout << "Keys as in affectingSystematics " << std::endl;
  for(auto sys: tool->affectingSystematics()) std::cout << sys << std::endl;
  std::cout << std::endl;

  std::cout << "get DT jets" << std::endl;
  std::vector<bool> jet_DT;
  code = tool->getDirectTaggedJets(jet_DT);
  if (code != StatusCode::SUCCESS) std::cout << "error" << std::endl;
  double SF=  tool->getEvtSF(0);
  std::cout << "//______________________________________" << std::endl;
  std::cout << "SF DT " << SF << std::endl;
  for(auto is:jet_DT) std::cout << is << "  ";
  std::cout << std::endl;
  std::cout << "//______________________________________" << std::endl;




  //  for(auto wei: trf_weight_ex) std::cout << wei << std::endl;


  std::cout << "Permutations" << std::endl;
  code = tool->getTagPermutation(2, trf_chosen_perm_ex, trf_chosen_perm_in);
  if (code != StatusCode::SUCCESS) std::cout << "error" << std::endl;
  std::cout << "Size " << trf_chosen_perm_ex.size() << std::endl;


  std::cout << "Inclusive case" << std::endl;
  for(auto perm: trf_chosen_perm_in){
    for(auto is: perm) 
      std::cout << is << " " ;
    std::cout << std::endl;
  }
  std::cout << "Exclusive case" << std::endl;
  for(auto perm: trf_chosen_perm_ex){
    for(auto is: perm) 
      std::cout << is << " " ;
    std::cout << std::endl;
  }


  std::cout << std::endl;
  std::cout << "Tagged bins" << std::endl;
  std::vector<std::vector<int> > trf_bin_ex, trf_bin_in;
  code = tool->getQuantiles(trf_bin_ex, trf_bin_in);
  if (code != StatusCode::SUCCESS) std::cout << "error" << std::endl;


  std::cout << "Size " << trf_bin_ex.size() << std::endl;
  for(auto bins: trf_bin_ex){
    std::cout << "  Size " << bins.size() << std::endl;
    for(auto tb: bins){
      std::cout << "    ";
      std::cout << tb << " " ;
    }
    std::cout << std::endl;
  }

  bool retval = true;
  delete tool;

  return retval;
}
