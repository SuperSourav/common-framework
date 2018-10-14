/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TRFCalc.h"
#include "TTHbbTRFCalc/TTHbbTRFCalc.h"
#include "TopConfiguration/ConfigurationSettings.h"

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TopConfiguration/TopConfig.h"

#include <algorithm>
#include <sstream>
#include "TLorentzVector.h"
#include "TString.h"

TRFCalc::TRFCalc(std::string params, std::shared_ptr<top::TopConfig> config)
    : m_params(params), m_config(config) {
  // m_params=params;
  auto trf_config = TTHbb::util::vectoriseString(params, " ");
  for (auto config_tmp : trf_config) {
    auto conf_tmp = TTHbb::util::vectoriseString(config_tmp, ":");
    m_trf_config[conf_tmp.at(0)] = conf_tmp.at(1);
  }
  wp_lab = m_trf_config["trf_WP"].c_str();
  top::ConfigurationSettings* configSettings =
      top::ConfigurationSettings::get();
  m_trf_config["m_jetAlg"] = configSettings->value("JetCollectionName");
  m_trf_config["MapIndex"] = std::to_string(m_config->getMapIndex());
  m_nbtags = std::atoi(m_trf_config["trf_nbjetMax"].c_str());

  auto* esh = EventSaverHelper::get();
  std::vector<std::string> suffixes = {"ex", "in"};
  std::vector<std::string> weight_types = {"Nominal"};
  for (auto suffix : suffixes) {
    for (auto weight_type : weight_types){
      for (unsigned int nb = 2; nb <= 4; ++nb) {
        std::string label =
            "trf_weight_" + wp_lab + "_" + std::to_string(nb) + suffix + "_" + weight_type;
        esh->addVariableToBeSaved(label, TTHbb::DecorationType::Float, false,
                                  "TRFCALC");
        label = "isbtagged_trf_" + wp_lab + "_" + std::to_string(nb) + suffix;
        esh->addJetVariableToBeSaved(label, TTHbb::DecorationType::Char, false,
                                     "TRFCALC");
        label = "isbtagged_trf_tagWeightBin_" + std::to_string(nb) + suffix;
        esh->addJetVariableToBeSaved(label, TTHbb::DecorationType::Int, false,
                                     "TRFCALC");
        esh->addJetVariableToBeSaved(label, TTHbb::DecorationType::Int, false,
                                     "TRFCALC");
      }
    }
  }
  calc = new TTHbbTRFCalc(m_trf_config);
}

TRFCalc::~TRFCalc() { delete calc; }

bool TRFCalc::apply(const top::Event& event) const {
  std::shared_ptr<TTHbb::Event> tthevt =
      event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >(
          "TTHbbEventVariables");
  auto tthjets = tthevt->m_jets;
  top::check(calc->getTRF(tthjets, event.m_info->eventNumber()), "Failed to execute TRFCalc");
  CombinedTRF trf_objects = calc->GetCombinedTRF();
  Decorate(tthevt, trf_objects);
  calc->DecorateJets(tthjets);
  return true;
}

bool TRFCalc::Decorate(std::shared_ptr<TTHbb::Event> tthevt,
                       CombinedTRF members) const {
  // Determine the maximal nbtags
  unsigned int njets = tthevt->m_jets.size();
  unsigned int nbtags = m_nbtags;
  if (nbtags > njets) nbtags = njets;
  // Count EVs
  // std::map<std::string, std::vector<double> >::iterator it =
  //     members.m_map_TTweight_ex.begin();
  // unsigned int counter_B = 0;
  // unsigned int counter_C = 0;
  // unsigned int counter_Light = 0;
  // for (auto it : members.m_map_TTweight_ex) {
  //   if ((it.first).find("1up") == std::string::npos) continue;
  //   if ((it.first).find("Eigen_B") != std::string::npos)
  //     counter_B++;
  //   else if ((it.first).find("Eigen_C") != std::string::npos)
  //     counter_C++;
  //   else if ((it.first).find("Eigen_Light") != std::string::npos)
  //     counter_Light++;
  // }

  // Prepare vectors to hold EVs
  // std::vector<std::vector<std::vector<std::vector<float> > > >
  // trf_weigt_ev_B;  // [2b/3b/4b][ex/in][up/down][ev]
  // std::vector<std::vector<std::vector<std::vector<float> > > >
  // trf_weigt_ev_C;
  // std::vector<std::vector<std::vector<std::vector<float> > > >
  // trf_weigt_ev_Light;
  // std::vector<std::string> conf_lab;
  // std::vector<std::string> var_lab;
  //
  // // Format the output vectors
  // trf_weigt_ev_B.resize(3);  // 2b/3b/4b
  // trf_weigt_ev_C.resize(3);
  // trf_weigt_ev_Light.resize(3);
  // // nb_lab.resize(3); nb_lab = {"2", "3", "4"};
  // for (unsigned int i = 0; i < 3; ++i) {
  //   trf_weigt_ev_B[i].resize(2);  // ex/in
  //   trf_weigt_ev_C[i].resize(2);
  //   trf_weigt_ev_Light[i].resize(2);
  //   conf_lab.resize(2);
  //   conf_lab = {"ex_", "in_"};
  //   for (unsigned int j = 0; j < 2; ++j) {
  //     trf_weigt_ev_B[i][j].resize(2);  // up/down
  //     trf_weigt_ev_C[i][j].resize(2);
  //     trf_weigt_ev_Light[i][j].resize(2);
  //     var_lab.resize(2);
  //     var_lab = {"up", "down"};
  //     for (unsigned int k = 0; k < 2; ++k) {
  //       trf_weigt_ev_B[i][j][k].resize(counter_B);  // Nev
  //       trf_weigt_ev_C[i][j][k].resize(counter_C);
  //       trf_weigt_ev_Light[i][j][k].resize(counter_Light);
  //     }
  //   }
  // }

  /////////////////////////////////////////////////////////////////////
  // Loop over m_map_TTweight_ex and m_map_TTweight_ex
  // Dressing Nominal and Extrapolation*, filling the EV vectors
  std::map<std::string, std::vector<double> >::iterator it_ex =
      members.m_map_TTweight_ex.begin();
  //iterate only over exclusive and get inclusive from matched name then
  while (it_ex != members.m_map_TTweight_ex.end()) {
    // Decorating Nominal and Extrapolation
    if (((it_ex->first).find("extrapolation") != std::string::npos) ||
        ((it_ex->first).find("Nominal") != std::string::npos)) {
      for (unsigned int nb = 2; nb <= 4; ++nb)
        if (nbtags >= nb) {
          // Exclusive
          std::string label =
              "trf_weight_" + wp_lab + "_" + std::to_string(nb) + "ex_" + it_ex->first;
          tthevt->floatVariable(label) = (float)(it_ex->second).at(nb);
          // Inclusive
          label = "trf_weight_" + wp_lab + "_" + std::to_string(nb) + "in_" + it_ex->first;
          tthevt->floatVariable(label) = (float)(members.m_map_TTweight_in[it_ex->first]).at(nb);
        }
        // else{
        //   // Exclusive
        //   std::string label =
        //       "trf_weight_" + wp_lab + "_" + std::to_string(nb) + "ex_" + it_ex->first;
        //   tthevt->floatVariable(label) = -1;
        //   // Inclusive
        //   label = "trf_weight_" + wp_lab + "_" + std::to_string(nb) + "in_" + it_in->first;
        //   tthevt->floatVariable(label) = -1;
        // }

      // Store the whole vector [nb] in the nominal case
      //
      //
      // if ((it_ex->first).find("Nominal") != std::string::npos) {
      //   // Exclusive
      //   std::string label = "trf_weight_" + wp_lab + "ex";
      //   std::cout << "TRFCalc: \t\t Decorating event with: " << label
      //             << " vector" << std::endl;
      //   std::vector<double> tmp = it_ex->second;
      //   std::vector<float> ftmp;
      //   ftmp.resize(0);
      //   for (auto w : tmp) ftmp.push_back(float(w));
      //   event->m_info->auxdecor<std::vector<float> >(label) = ftmp;
      //
      //   // Inclusive
      //   label = "trf_weight_" + wp_lab + "in";
      //   std::cout << "TRFCalc: \t\t Decorating event with: " << label
      //             << " vector" << std::endl;
      //   tmp.resize(0);
      //   ftmp.resize(0);
      //   tmp = it_in->second;
      //   for (auto w : tmp) ftmp.push_back(float(w));
      //   event->m_info->auxdecor<std::vector<float> >(label) = ftmp;
      // }
    }
    // else {
    //   unsigned int var_itr = 0;
    //   if ((it_ex->first).find("1down") != std::string::npos) var_itr = 1;
    //
    //   // Determine the ev number
    //   unsigned int ev_itr = 100;
    //   unsigned pos_end = (it_ex->first).find("__1");
    //   std::string tmp_string = (it_ex->first).substr(0, pos_end);
    //   unsigned pos_beginning = (tmp_string).find_last_of("_") + 1;
    //   std::string ev_string = (tmp_string).substr(pos_beginning);
    //   ev_itr = std::stoi(ev_string);
    //   if ((it_ex->first).find("_B_") != std::string::npos) {
    //     for (unsigned int nb = 2; nb <= 4; ++nb)
    //       if (nbtags >= nb) {
    //         trf_weigt_ev_B[nb - 2][0][var_itr][ev_itr] =
    //         (float)(it_ex->second) [nb];  //
    //         trf_weigt_ev_B[2b/3b/4b][ex][var_itr][ev_itr]
    //         trf_weigt_ev_B[nb - 2][1][var_itr][ev_itr] =
    //         (float)(it_in->second) [nb];  //
    //         trf_weigt_ev_B[2b/3b/4b][in][var_itr][ev_itr]
    //       }
    //   } else if ((it_ex->first).find("_C_") != std::string::npos) {
    //     for (unsigned int nb = 2; nb <= 4; ++nb)
    //       if (nbtags >= nb) {
    //         trf_weigt_ev_C[nb - 2][0][var_itr][ev_itr] =
    //         (float)(it_ex->second) [nb];  //
    //         trf_weigt_ev_B[2b/3b/4b][ex][var_itr][ev_itr]
    //         trf_weigt_ev_C[nb - 2][1][var_itr][ev_itr] =
    //         (float)(it_in->second) [nb];  //
    //         trf_weigt_ev_B[2b/3b/4b][in][var_itr][ev_itr]
    //       }
    //   } else if ((it_ex->first).find("_Light_") != std::string::npos) {
    //     for (unsigned int nb = 2; nb <= 4; ++nb)
    //       if (nbtags >= nb) {
    //         trf_weigt_ev_Light[nb - 2][0][var_itr][ev_itr] =
    //         (float)(it_ex->second) [nb];  //
    //         trf_weigt_ev_B[2b/3b/4b][ex][var_itr][ev_itr]
    //         trf_weigt_ev_Light[nb - 2][1][var_itr][ev_itr] =
    //         (float)(it_in->second) [nb];  //
    //         trf_weigt_ev_B[2b/3b/4b][in][var_itr][ev_itr]
    //       }
    //   }
    // }
    it_ex++;
  }

  // // Decorating EV vectors
  // for (unsigned int i = 0; i < 3; ++i) {      // 2b/3b/4b
  //   for (unsigned int j = 0; j < 2; ++j) {    // ex/in
  //     for (unsigned int k = 0; k < 2; ++k) {  // up/down
  //       std::string label = "trf_weight_" + wp_lab + std::to_string(i + 2) +
  //       conf_lab[j] + "eigenvars_B_" + var_lab[k];
  //       event->m_info->auxdecor<std::vector<float> >(label) =
  //       trf_weigt_ev_B[i][j][k];
  //       label = "trf_weight_" + wp_lab + std::to_string(i + 2) + conf_lab[j]
  //       + "eigenvars_C_" + var_lab[k];
  //       event->m_info->auxdecor<std::vector<float> >(label) =
  //       trf_weigt_ev_C[i][j][k];
  //       label = "trf_weight_" + wp_lab + std::to_string(i + 2) + conf_lab[j]
  //       + "eigenvars_Light_" + var_lab[k];
  //       event->m_info->auxdecor<std::vector<float> >(label) =
  //       trf_weigt_ev_Light[i][j][k];
  //     }
  //   }
  // }
  // Decorate jets with the trf tag permutation info
  // nbtags = members.m_TRFPerm_ex.size();
  // if (nbtags > m_nbjetMax + 1) nbtags = m_nbjetMax + 1;
  // unsigned int jet_itr = 0;
  // for (auto jetPtr : tthevt->m_jets) {
  //   for (unsigned int nbtag_itr = 2; nbtag_itr < nbtags; ++nbtag_itr) {
  //     std::string label = "trf_tagged_" + wp_lab + std::to_string(nbtag_itr);
  //     jetPtr->charVariable(label + "ex") =
  //     members.m_TRFPerm_ex[nbtag_itr][jet_itr];
  //     jetPtr->charVariable(label + "in") =
  //     members.m_TRFPerm_in[nbtag_itr][jet_itr];
  //   }
  //   ++jet_itr;
  // }
  return true;
}

std::string TRFCalc::name() const { return "TRFCALC"; }
