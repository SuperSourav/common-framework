/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbTRFCalc/TTHbbTRFCalc.h"

TTHbbTRFCalc::TTHbbTRFCalc(std::map<std::string, std::string> m_trf_config)
    :  //
      m_OP("FixedCutBEff_70"),
      m_jetAlg("AntiKt4EMTopoJets"),
      m_taggerName("MV2c10"),
      m_cdi(
          "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/"
          "xAODBTaggingEfficiency/13TeV/"
          "2016-20_7-13TeV-MC15-CDI-June27_v1.root"),
      m_ignoreSF(true),  // ignore the SFs in the TRF computation
      m_usePermutations(true),
      m_useQuantile(true),
      m_calibName("default"),
      m_nbjetMax(4),
      m_ignoreMCMC(true),
      m_trf(0) {

  m_OP = "FixedCutBEff_" + m_trf_config["trf_WP"];
  wp_lab = m_trf_config["trf_WP"];
  std::cout << "------------------------------------\n";
  std::cout << "TRFCalc: \t\t Constructing an instance of TRFCalc \n";
  std::cout << "TRFCalc: \t\t Operating point: " << m_OP << std::endl;

  m_jetAlg = m_trf_config["m_jetAlg"];
  m_taggerName = m_trf_config["ttH_btagAlgo"];
  m_cdi =
      "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/"
      "xAODBTaggingEfficiency/13TeV/" +
      m_trf_config["trf_cdi"];
  m_calibName = m_trf_config["trf_calibName"];
  MapIndex = std::atoi(m_trf_config["MapIndex"].c_str());
  m_nbjetMax = std::atoi(m_trf_config["trf_nbjetMax"].c_str());

  // Get the ignoreSF flag
  std::string s_ignoreSF = m_trf_config["trf_ignoreSF"];
  if (s_ignoreSF.find("YES", 0, 3) == 0) {
    m_ignoreSF = true;  // ignore the SFs in the TRF computation
    std::cout
        << "TRFCalc \t\t  B-tagging SFs ignored in the TRF weights calculation"
        << std::endl;
  } else {
    m_ignoreSF = false;  // do NOT ignore the SFs in the TRF computation
    std::cout
        << "TRFCalc \t\t  B-tagging SFs included in the TRF weights calculation"
        << std::endl;
  }

  // Get the ignoreMCMC flag
  std::string s_ignoreMCMC = m_trf_config["trf_ignoreMCMC"];
  if (s_ignoreMCMC.find("YES", 0, 3) == 0) {
    m_ignoreMCMC = true;  // ignore the SFs in the TRF computation
    std::cout << "TRFCalc \t\t  MC-MC SFs for b-tagging ignored in the TRF "
                 "weights calculation"
              << std::endl;
  } else {
    m_ignoreMCMC = false;  // do NOT ignore the SFs in the TRF computation
    std::cout << "TRFCalc \t\t  MC-MC SFs for b-tagging included in the TRF "
                 "weights calculation"
              << std::endl;
  }

  // Create an instance of the BTaggingTruthTaggingTool
  m_trf = new BTaggingTruthTaggingTool("BTagTruth" + m_OP);

  StatusCode code = m_trf->setProperty("JetAuthor", m_jetAlg);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "JetAuthor property");

  code = m_trf->setProperty("TaggerName", m_taggerName);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "TaggerName property");

  code = m_trf->setProperty("OperatingPoint", m_OP);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "OperatingPoint property");

  code = m_trf->setProperty("ScaleFactorFileName", m_cdi);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "ScaleFactorName property");

  code = m_trf->setProperty("IgnoreScaleFactors", m_ignoreSF);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "IgnoreScaleFactor property");

  code = m_trf->setProperty("UsePermutations", m_usePermutations);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "UsePermutations property");

  code = m_trf->setProperty("UseQuantile", m_useQuantile);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "UseQuantile property");

  code = m_trf->setProperty("EfficiencyBCalibrations",
                            "default;364443;410501;410250;default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "EfficiencyBCalibrations property");

  code = m_trf->setProperty("EfficiencyCCalibrations",
                            "default;364443;410501;410250;default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "EfficiencyCCalibrations property");

  code = m_trf->setProperty("EfficiencyLightCalibrations",
                            "default;364443;410501;410250;default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "EfficiencyLightCalibrations property");

  code = m_trf->setProperty("EfficiencyTCalibrations",
                            "default;364443;410501;410250;default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "EfficiencyTCalibrations property");

  code = m_trf->setProperty("ScaleFactorBCalibration", "default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "ScaleFactorBCalibrations property");

  code = m_trf->setProperty("ScaleFactorCCalibration", "default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "ScaleFactorCCalibrations property");

  code = m_trf->setProperty("ScaleFactorTCalibration", "default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "ScaleFactorTCalibrations property");

  code = m_trf->setProperty("ScaleFactorLightCalibration", "default");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "ScaleFactorLightCalibrations property");

  code = m_trf->setProperty("EigenvectorReductionB", "Loose");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "EigenvectorReductionB property");

  code = m_trf->setProperty("EigenvectorReductionC", "Loose");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "EigenvectorReductionC property");

  code = m_trf->setProperty("EigenvectorReductionLight", "Loose");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "EigenvectorReductionLight property");

  code = m_trf->setProperty("ExcludeFromEigenVectorTreatment", "");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "ExcludeFromEigenVectorTreatment property");

  code = m_trf->setProperty("SystematicsStrategy", "SFEigen");
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool "
             "SystematicsStrategy property");

  code = m_trf->initialize();
  top::check(code,
             "Error \t\t TRFCalc.cxx: initialising BTaggingTruthTaggingTool");

  std::cout << "TRFCalc::TRFCalc constructor is finished.\n";
  std::cout << "------------------------------------\n";
}

TTHbbTRFCalc::~TTHbbTRFCalc() { delete m_trf; }

CombinedTRF TTHbbTRFCalc::GetCombinedTRF() {
  CombinedTRF members;
  members.m_TRFPerm_in = m_TRFPerm_in;
  members.m_TRFPerm_ex = m_TRFPerm_ex;
  members.m_map_TTweight_ex = m_map_TTweight_ex;
  members.m_map_TTweight_in = m_map_TTweight_in;
  return members;
}

bool TTHbbTRFCalc::Clean() {
  m_map_TTweight_ex.clear();
  m_map_TTweight_in.clear();
  m_TRFPerm_in.resize(0);
  m_TRFPerm_ex.resize(0);

  return true;
}

bool TTHbbTRFCalc::getTRF(std::vector<std::shared_ptr<TTHbb::Jet> >& m_jets,
                          int eventNumber) {
  bool check = TTHbbTRFCalc::Clean();
  if (!check) {
    std::cout << "TRFCalc \t\t Failing to clean the result vectors."
              << std::endl;
    return false;
  }

  // Set the map index
  if (m_ignoreMCMC) {
    MapIndex = 0;
  }

  // std::cout << "TRFCalc \t\t Running with MapIndex = " << MapIndex << std::endl;
  StatusCode code = m_trf->setEffMapIndex("B", MapIndex);
  top::check(code, "Error \t\t TRFCalc.cxx: setEffMapIndex, B");
  code= m_trf->setEffMapIndex("C", MapIndex);
  top::check(code, "Error \t\t TRFCalc.cxx: setEffMapIndex, C");
  code = m_trf->setEffMapIndex("Light", MapIndex); 
  top::check(code, "Error \t\t TRFCalc.cxx: setEffMapIndex, Light");
  code = m_trf->setEffMapIndex("T", MapIndex);
  top::check(code, "Error \t\t TRFCalc.cxx: setEffMapIndex, T");

  // Retrieving the TRF results
  unsigned int njets = m_jets.size();
  unsigned int nbtags;

  if (m_nbjetMax > njets)
    nbtags = njets;
  else{
    nbtags = m_nbjetMax;
  }
  code = m_trf->setSeed(eventNumber + njets);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool seed");

  std::vector<double> v_pt;
  std::vector<double> v_eta;
  std::vector<double> v_tagw;
  std::vector<int> v_flav;

  for (auto jet : m_jets) {
    v_pt.push_back(jet->Pt());
    v_eta.push_back(jet->Eta());
    v_flav.push_back(jet->intVariable("truthflav"));
    v_tagw.push_back(jet->floatVariable("mv2"));
  }

  // Set the jets
  code = m_trf->setJets(&v_pt, &v_eta, &v_flav, &v_tagw);
  top::check(code,
             "Error \t\t TRFCalc.cxx: setting BTaggingTruthTaggingTool jets");

  // Getting systematic weights
  code =
      m_trf->getTruthTagWei(nbtags, m_map_TTweight_ex, m_map_TTweight_in);
  top::check(code, "Error \t\t TRFCalc.cxx: retrieving systematic weights");

  // std::cout << "TRFCalc \t\t Getting the tagged permutation." << std::endl;
  code = m_trf->getTagPermutation(nbtags, m_TRFPerm_ex, m_TRFPerm_in);
  top::check(code, "Error \t\t TRFCalc.cxx: retrieving tagged permutations");

  // Getting pseudo-continuous weights
  code = m_trf->getQuantiles(m_trf_bin_ex, m_trf_bin_in);
  top::check(code, "Error \t\t TRFCalc.cxx: retrieving quantile permutations");

  return true;
}

bool TTHbbTRFCalc::DecorateJets(
    std::vector<std::shared_ptr<TTHbb::Jet> >& m_jets) {
  // Decorate jets with the trf tag permutation info
  unsigned int nbtags = m_TRFPerm_ex.size();
  if (nbtags > m_nbjetMax + 1) nbtags = m_nbjetMax + 1;
  unsigned int jet_itr = 0;
  for (auto jetPtr : m_jets) {
    for (unsigned int nbtag_itr = 2; nbtag_itr < nbtags; ++nbtag_itr) {
      std::string label =
          "isbtagged_trf_" + wp_lab + "_" + std::to_string(nbtag_itr);
      jetPtr->charVariable(label + "ex") = m_TRFPerm_ex.at(nbtag_itr).at(jet_itr);
      jetPtr->charVariable(label + "in") = m_TRFPerm_in.at(nbtag_itr).at(jet_itr);
      label = "isbtagged_trf_tagWeightBin_" + std::to_string(nbtag_itr);
      jetPtr->intVariable(label + "ex") = m_trf_bin_ex.at(nbtag_itr).at(jet_itr);
      jetPtr->intVariable(label + "in") = m_trf_bin_in.at(nbtag_itr).at(jet_itr);
    }
    ++jet_itr;
  }
  return true;
}
