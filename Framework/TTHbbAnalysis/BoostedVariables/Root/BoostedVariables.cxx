/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedVariables/BoostedVariables.h"
#include "TTHbbObjects/Event.h"

#include "Riostream.h"
#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include <TList.h>
#include "PathResolver/PathResolver.h"

using namespace std;

// this seems like a problem:
// in Level2, custom decorations have the whole name "ljet_isTopTagged_loose"
// in Level1, they have only the variable name "isTopTagged_loose"
// I'm sure there is a way in Level2 to make aliases, but for now, this should suffice:
#ifdef ROOTCORE_PACKAGE_TTHbbToolManager
  #define IS_LEVEL2 1
#endif

#ifdef IS_LEVEL2
  #define LJET "ljet_"
#else
  #define LJET ""
#endif

#include <sstream>

#define CHECK( ARG )                                    \
   do {                                                 \
      const bool result = ARG;                          \
      if( ! result ) {                                  \
         std::ostringstream os;                         \
         const char *pos = strrchr(__FILE__, '/');      \
         const char *fn = pos ? pos+1 : __FILE__;       \
         os << "Fatal error at " << fn << ":"           \
            << __LINE__ << ": \"" << #ARG << "\"";      \
         throw std::runtime_error(os.str());            \
      }                                                 \
   } while( false )

namespace TTHbb {

BoostedVariables::BoostedVariables(): m_nPass3211(0), m_nPass1T1H(0), m_nPassReclustered1T1H(0), m_passesCut3211(9, 0), m_passesCut1T1H(6, 0), m_passesCutReclustered1T1H(9, 0) {
  std::cout << "INFO: BoostedVariables" << std::endl;
}

void BoostedVariables::initialise() { 

  m_ClassifyBDT = new ClassBDT();

  std::string dirname = PathResolverFindCalibDirectory("BoostedVariables/220517_BDT/weights/");
  m_ClassifyBDT->loadModel(dirname);
  m_ClassifyBDT->setScoreName("TTHBOOSTEDBDTOutput_RC_8Var_xTrained");
  
  std::cout << "INFO: BoostedVariables::initialise" << std::endl; 
  
}


void BoostedVariables::apply(TTHbb::Event* event) {
  // int HF_SimpleClassification = event->intVariable("HF_SimpleClassification");
  // cout << "HF_SimpleClassification: " << HF_SimpleClassification << endl;

  apply3211(event);
  apply1T1H(event);
  // applyReclustered1T1H(event,"Rc1T1H_topNoBVeto", 0);
  applyReclustered1T1H(event, "Rc1T1H", 1);

  bool boosted_passesAny = (
      event->intVariable("boosted_passes3211") ||
      event->intVariable("boosted_passes3311") ||
      event->intVariable("boosted_passes3311_1b1nb") ||
      event->intVariable("boosted_passes1T1H") ||
      event->intVariable("boosted_passesRc1T1H"));
  event->intVariable("boosted_passesAny") = boosted_passesAny;
}


//---------------------------------------------------------
// Baseline selection (3221)
//---------------------------------------------------------

/// Baseline selection
///
/// Performs the following selections:
///   - 3211: Inclusive selection from cutflow challenge
///   - 3311: 2016 baseline
///   - 3311_1b1nb: like 2016 baseline, but the top tag
///     must contain ==1 b and >=1 non-b jet
///
/// Uses 70% b-tagging WP.
///
void BoostedVariables::apply3211(TTHbb::Event* event) {
  int nLjets = 0;
  int nLjets_250 = 0;
  int nLjets_250_eta = 0;
  int nLjets_250_eta_mass = 0;
  int nLjets_250_eta_mass_separated = 0;
  int nLJetsTagged = 0;
  int nBJetsInTop = 0;
  int nBVetoInTop = 0;

  std::string prefix {"3211_"};

  vector<shared_ptr<LargeJet>> taggedLJets;
  taggedLJets.reserve(5);

  for (const auto &ljet: event->m_largeJets) {
    nLjets++;

    if (!( ljet->Pt() > 250e3 && ljet->Pt() < 1500e3 )) continue;
    nLjets_250++;

    if (!( fabs(ljet->Eta()) < 2 )) continue;
    nLjets_250_eta++;
          
    if (!( ljet->M() > 50e3 )) continue;
    nLjets_250_eta_mass++;

    // check for overlapping electrons
    bool separated = true;
    for (const auto &el: event->m_leptons) {
      if (el->type() != leptonType::electron)
          continue;
      
      if (ljet->DeltaR(*el) < 1.0) {
        separated = false;
        break;
      }
    }
    if (!separated)
      continue;

    nLjets_250_eta_mass_separated++;


      // std::cout << "topTag_loose " << ljet->charVariable(LJET "topTag_loose") << std::endl;
    if (ljet->charVariable(LJET "topTag_loose") >= 1) {
      nLJetsTagged++;
      taggedLJets.push_back(ljet);
    }

  } // large jets

  if (taggedLJets.size() >= 1) {
    auto topTag = taggedLJets[0];
    // iterate over jets inside
    for (auto &jet: event->m_jets) {
      if (jet->DeltaR(*topTag) > 1.0) continue;

      if (jet->charVariable("isbtagged_MV2c10_70")) {
        nBJetsInTop++;
      } else {
        nBVetoInTop++;
      }
    }
    event->intVariable(prefix+"nBJetsInTop") = nBJetsInTop;
    event->intVariable(prefix+"nBVetoInTop") = nBVetoInTop;
  }

  int nJets25 = 0;
  int nJetsOutside = 0;
  int nBJetsOutside = 0;
  for (const auto &jet: event->m_jets) {
    if (jet->Pt() <= 25e3) continue;
    nJets25++;
    
    // Jet_N [dR(jet, ljet) > 1.0] >= 3 : there are a minimum of 3 small jets
    // that are separated by at least 1 radian from any top-tagged jet with
    // pT>250 and |eta|<2

    bool outside_all_ljets = true;
    for (const auto &ljet: taggedLJets) {
      if (ljet->DeltaR(*jet) < 1.0) {
        // this large jet is too close
        outside_all_ljets = false;
        break;
      }
    } // large jets

    if (outside_all_ljets) {
      //this small jet passed veto
      nJetsOutside++;
      if (jet->charVariable("isbtagged_MV2c10_70"))
          nBJetsOutside++;
    }

  } // jets

  event->charVariable(prefix+"passes1") = 0;
  event->charVariable(prefix+"passes2") = 0;
  event->charVariable(prefix+"passes3") = 0;
  event->charVariable(prefix+"passes4") = 0;
  event->charVariable(prefix+"passes5") = 0;
  event->charVariable(prefix+"passes6") = 0;
  // event->charVariable(prefix+"passes7") = 0;
  // event->charVariable(prefix+"passes8") = 0;
  event->charVariable(prefix+"passesAll") = 0;
  // event->charVariable(prefix+"passes_topNoBVeto") = 0;
  // event->charVariable(prefix+"passes_topBVeto") = 0;
  // event->floatVariable("higgs_pt") = 0;
  // event->floatVariable("higgs_pt") = higgsTag ? (float) higgsTag->Pt() : 0;

  m_passesCut3211[0]++;
  event->charVariable(prefix+"passes0") = 1;

  do {
    if (!( nLjets_250 >= 1 )) break;
    m_passesCut3211[1]++;
    event->charVariable(prefix+"passes1") = 1;
    
    if (!( nLjets_250_eta_mass >= 1 )) break;
    m_passesCut3211[2]++;
    event->charVariable(prefix+"passes2") = 1;
    
    if (!( nLjets_250_eta_mass_separated >= 1 )) break;
    m_passesCut3211[3]++;
    event->charVariable(prefix+"passes3") = 1;

    if (!( nLJetsTagged >= 1 )) break;
    m_passesCut3211[4]++;
    event->charVariable(prefix+"passes4") = 1;

    if (!( nJetsOutside >= 3 )) break;
    m_passesCut3211[5]++;
    event->charVariable(prefix+"passes5") = 1;

    if (!( nBJetsOutside >= 2 )) break;
    m_passesCut3211[6]++;
    event->charVariable(prefix+"passes6") = 1;
    
    // if (!( nAdditionalJets >= 1 )) break;
    // m_passesCut3211[7]++;
    // event->charVariable(prefix+"passes7") = 1;
    
    // if (!( nAdditionalBJets >= 1 )) break;
    // m_passesCut3211[8]++;
    // event->charVariable(prefix+"passes8") = 1;
  } while (false);

  event->intVariable(prefix+"nLJetsTagged") = nLJetsTagged;
  event->intVariable(prefix+"nJetsOutside") = nJetsOutside;
  event->intVariable(prefix+"nBJetsOutside") = nBJetsOutside;

  bool passes3211 = (nLJetsTagged >= 1 && nJetsOutside >= 3 && nBJetsOutside >= 2);
  bool passes3311 = (nLJetsTagged >= 1 && nJetsOutside >= 3 && nBJetsOutside >= 3);
  bool passes3311_1b1nb = (passes3311 && nBJetsInTop == 1 && nBVetoInTop >= 1);
  if (passes3211) {
    m_nPass3211++;
  }

  event->intVariable("boosted_passes3211") = passes3211;
  event->intVariable("boosted_passes3311") = passes3311;
  event->intVariable("boosted_passes3311_1b1nb") = passes3311_1b1nb;

  // // deprecated
  // event->charVariable(prefix+"passesAll") = passes3211;
}

//---------------------------------------------------------
// Eloi selection (1T1H)
//---------------------------------------------------------

void BoostedVariables::apply1T1H(TTHbb::Event* event) {
  int nLjets = 0;
  int nLjets_250 = 0;
  int nLjets_250_eta = 0;
  int nLjets_250_eta_mass = 0;
  // int nLJetsTagged = 0;

  vector<shared_ptr<LargeJet>> goodLJets;
  goodLJets.reserve(5);

  // vector<shared_ptr<LargeJet>> higgsCandidates;

  double btag_sum_85_H = 0;
  double btag_sum_85_T = 0;

  shared_ptr<LargeJet> higgsTag {nullptr};
  int nHiggsCandidates = 0;

  for (const auto &ljet: event->m_largeJets) {
    nLjets++;

    // if (!( ljet->Pt() > 250e3 )) continue;
    if (!( ljet->Pt() > 250e3 && ljet->Pt() < 1500e3 )) continue;
    nLjets_250++;

    if (!( fabs(ljet->Eta()) < 2 )) continue;
    nLjets_250_eta++;
          
    if (!( ljet->M() > 50e3 )) continue;
    nLjets_250_eta_mass++;

    // check for overlapping electrons
    bool separated = true;
    for (const auto &el: event->m_leptons) {
      if (el->type() != leptonType::electron)
          continue;

      if (ljet->DeltaR(*el) < 1.0) {
        separated = false;
        break;
      }
    }
    if (!separated)
      continue;

    goodLJets.push_back(ljet);

  } // large jets

  // for each jet, find the large jet it is associated to (if any)
  // this prevents double-counting the small jets
  map<Jet*, LargeJet*> jetAssociation;
  // cout << "event->m_jets.size(): " << event->m_jets.size() << endl;
  for (auto &jet: event->m_jets) {
    double mindist = 9999.;
    LargeJet *assocLJet {nullptr};
    for (const auto &ljet: goodLJets) {
      double dist = ljet->DeltaR(*jet);
      if (dist > 1) continue;
      if (dist < mindist) {
        mindist = dist;
        assocLJet = ljet.get();
      }
    }
    jetAssociation[jet.get()] = assocLJet;
  } // small jets

  // Find higgs tag candidates and the higgs tag
  for (const auto &ljet: goodLJets) {
    int nBJetsInside = 0;
    double mv2c10sum = 0;
    for (const auto &jet: event->m_jets) {
      if (!jet->charVariable("isbtagged_MV2c10_85")) continue;
      // check if the small jet is owned by this large jet
      if (jetAssociation[jet.get()] != ljet.get()) continue;

      if (ljet->DeltaR(*jet) < 1.0) {
        nBJetsInside++;

        mv2c10sum += jet->floatVariable("mv2");
      }
    }
    ljet->intVariable("nBJetsInside1T1H") = nBJetsInside;
    // ljet->floatVariable("mv2c10sum") = mv2c10sum;

    if (nBJetsInside >= 2) {
      // float mv2c10sum = mv2c10sum;
      nHiggsCandidates++;
      if (btag_sum_85_H < mv2c10sum) {
          btag_sum_85_H = mv2c10sum;
          higgsTag = ljet;
      }
    }
  } // large jets


  // Find top tag candidates and the top tag
  int nTopCandidates = 0;
  double maxMass = 0;
  shared_ptr<LargeJet> topTag {nullptr};

  for (const auto &ljet: goodLJets) {
    if (ljet->intVariable("nBJetsInside1T1H") < 1) continue;
    if (ljet->charVariable(LJET "topTag_loose") == 0) continue;
    // if (ljet->charVariable(LJET "topTag_loose") != 1) continue;
    if (ljet == higgsTag) continue;

    nTopCandidates++;
    if (maxMass < ljet->M()) {
      maxMass = ljet->M();
      topTag = ljet;
    }
  }

  if (topTag) {
    for (const auto &jet: event->m_jets) {
      if (jet->charVariable("isbtagged_MV2c10_85") && topTag->DeltaR(*jet) <= 1.0) {
        btag_sum_85_T += jet->floatVariable("mv2");
      }
    }
  }

  int nAdditionalJets = 0;
  int nAdditionalBJets = 0;

  // find additional jets
  // "additional" jets are neither in the higgs tag nor in the top tag
  for (const auto &jet: event->m_jets) {
    if (higgsTag && jet->DeltaR(*higgsTag) <= 1.0) continue;
    if (topTag && jet->DeltaR(*topTag) <= 1.0) continue;

    // now we are outside both T and H
    nAdditionalJets++;
    if (jet->charVariable("isbtagged_85") == 1) {
      nAdditionalBJets++;
    }
  }

  // BDT variables
  
  event->intVariable("N_jets_out_TH") = nAdditionalJets;
  event->intVariable("N_bjets_out_TH") = nAdditionalBJets;
  event->floatVariable("btag_sum_85_H") = btag_sum_85_H;
  event->floatVariable("btag_sum_85_T") = btag_sum_85_T;
  event->floatVariable("T_tau32") = topTag ? topTag->floatVariable(LJET "tau32") : 0;
  event->floatVariable("T_m") = topTag ? topTag->M() : 0;
  event->floatVariable("T_Pt") = topTag ? topTag->Pt() : 0;
  event->floatVariable("H_D2") = higgsTag ? higgsTag->floatVariable(LJET "D2") : 0;
  event->floatVariable("H_m") = higgsTag ? higgsTag->M() : 0;

  // Cutflow (debug)

  event->charVariable("passesCut0") = 0;
  event->charVariable("passesCut1") = 0;
  event->charVariable("passesCut2") = 0;
  event->charVariable("passesCut3") = 0;
  event->charVariable("passesCut4") = 0;
  event->charVariable("passesCut5") = 0;
  event->charVariable("boosted_passes1T1H") = 0;

  m_passesCut1T1H[0]++;
  event->charVariable("passesCut0") = 1;

  if (!( goodLJets.size() >= 1 )) return;
  m_passesCut1T1H[1]++;
  event->charVariable("passesCut1") = 1;
  
  if (!( nHiggsCandidates >= 1 )) return;
  m_passesCut1T1H[2]++;
  event->charVariable("passesCut2") = 1;
  
  if (!( nTopCandidates >= 1 )) return;
  m_passesCut1T1H[3]++;
  event->charVariable("passesCut3") = 1;
  
  if (!( nAdditionalJets >= 1 )) return;
  m_passesCut1T1H[4]++;
  event->charVariable("passesCut4") = 1;
  
  if (!( nAdditionalBJets >= 1 )) return;
  m_passesCut1T1H[5]++;
  event->charVariable("passesCut5") = 1;

  if (nHiggsCandidates >= 1 && nTopCandidates >= 1 && nAdditionalBJets >= 1) {
      m_nPass1T1H++;
      event->intVariable("boosted_passes1T1H") = 1;
  }
}

template <class Container> 
bool contains(Container& container, const typename Container::value_type& element) {
    return std::find(container.begin(), container.end(), element) != container.end();
}

//---------------------------------------------------------
// "Emma" selection
// (Reclustered jets, 1T, 1H)
//---------------------------------------------------------

void BoostedVariables::applyReclustered1T1H(TTHbb::Event* event, std::string prefix, int requireTopBVeto) {
 

  int nLjets = 0;
  int nLjets_200 = 0;
  int nLjets_200_eta = 0;
  int nLjets_200_eta_mass = 0;

  // BDT variables
  float btag_sum_all = 0;
  float dR_HT = 0;
  float btag_sum_out_TH = 0;
  float btag_sum_out_TH_over_all = 0;
  float dRbH_out_TH = 0;
  float dRbT_out_TH = 0;
  float dRbb_MaxPt_recal = 0;
  float dRbb_in_H = 0;
  float dR_lepH = 0;

  prefix = prefix + "_";

  vector<shared_ptr<Particle>> goodLJets;
  goodLJets.reserve(5);

  shared_ptr<Particle> electron {nullptr};
  shared_ptr<Particle> muon {nullptr};
  shared_ptr<Particle> lepton {nullptr};

  CHECK( event->checkIntVariable("nElectrons") );
  if (event->intVariable("nElectrons") > 0) {
    // cout << "Electron channel" << endl;
    // pick the first electron
    for (const auto &el: event->m_leptons) {
      if (el->type() != leptonType::electron)
        continue;
      electron = el;
      break;
    }
  }

  CHECK( event->checkIntVariable("nMuons") );
  if (event->intVariable("nMuons") > 0) {
    for (const auto &el: event->m_leptons) {
      if (el->type() != leptonType::muon)
        continue;
      muon = el;
      break;
    }
  }

  // the lepton of interest is the one with highest Pt
  // (or the only one present)
  if ((muon ? muon->Pt() : -9999) > (electron ? electron->Pt() : -9999)) {
    lepton = muon;
  } else {
    lepton = electron;
  }

  auto rcjets = event->m_customObj["rcjet"];
  int i = 0;
  for (auto &rcjet: rcjets) {
    rcjet->intVariable("index") = i; i++;
    nLjets++;

    if (!( rcjet->Pt() > 200e3 && rcjet->Pt() < 1500e3 )) continue;
    nLjets_200++;

    if (!( fabs(rcjet->Eta()) < 2 )) continue;
    nLjets_200_eta++;
    
    // Recalculate mass this way to be compatible with existing code
    double rcjet_m = rcjet -> M(); // sqrt(pow(rcjet->E(),2)-pow(rcjet->Pt(),2));
    rcjet->floatVariable("rcjet_m") = rcjet_m;

    if (!( rcjet_m > 50e3 )) continue;
    nLjets_200_eta_mass++;

    // // check for overlapping electrons
    if (electron) {
      if (electron->DeltaR(*rcjet) < 1.0) {
        // not separated
        continue;
      }
    }

    goodLJets.push_back(rcjet);

  } // large jets
  

  if(goodLJets.size() > 0)
    event->floatVariable("rcjet0_eta") = goodLJets[0] -> Eta();

  double maxSum = 0;
  shared_ptr<Particle> higgsTag {nullptr};
  int nHiggsCandidates = 0;

  // Find higgs tag candidates and the higgs tag
  for (auto &ljet: goodLJets) {
    int nBJetsInside = 0;
    int nBVetoInside = 0;
    double mv2c10sum = 0;

    CHECK( ljet->checkIntVariable("index") );

    // iterate over subjets of this jet
    for (auto &subjet: event->m_jets) {
      CHECK( subjet->checkCharVariable("isbtagged_MV2c10_85") );
      CHECK( subjet->checkFloatVariable("mv2") );

      if (!subjet->checkIntVariable("rcjet_index"))
        continue;

      if (subjet->intVariable("rcjet_index") != ljet->intVariable("index"))
        continue;
      
      if (subjet->charVariable("isbtagged_MV2c10_85")) {
        nBJetsInside++;
        // sub mv2c10 of inside b-tags
        mv2c10sum += subjet->floatVariable("mv2");
      } else {
        nBVetoInside++;
      }
    }
    ljet->intVariable(prefix+"nBJetsInside") = nBJetsInside;
    ljet->intVariable(prefix+"nBVetoInside") = nBVetoInside;
    // ljet->floatVariable("mv2c10sum") = mv2c10sum;

    if (nBJetsInside >= 2) {
      // float mv2c10sum = mv2c10sum;
      nHiggsCandidates++;
      if (maxSum < mv2c10sum) {
          maxSum = mv2c10sum;
          higgsTag = ljet;
      }
    }
  } // reclustered jets


  // Find top tag candidates and the top tag
  int nTopCandidates = 0;

  int nTopCandidates_pt = 0;
  int nTopCandidates_pt_bjet = 0;
  int nTopCandidates_pt_bjet_sjet = 0;

  double maxMass = 0;
  shared_ptr<Particle> topTag {nullptr};

  for (auto &ljet: goodLJets) {
    if (!( ljet->Pt() > 250e3 )) continue;
    nTopCandidates_pt++;

    CHECK( ljet->checkIntVariable(prefix+"nBJetsInside") );
    CHECK( ljet->checkIntVariable(prefix+"nBVetoInside") );
    CHECK( ljet->checkFloatVariable("rcjet_m") );

    if (!( ljet->intVariable(prefix+"nBJetsInside") == 1 )) continue;
    nTopCandidates_pt_bjet++;

    if (requireTopBVeto) {
      if (!( ljet->intVariable(prefix+"nBVetoInside") >= 1 )) continue;
    }
    nTopCandidates_pt_bjet_sjet++;
    
    // if (ljet == higgsTag) continue;

    nTopCandidates++;
    if (maxMass < ljet->floatVariable("rcjet_m")) {
      maxMass = ljet->floatVariable("rcjet_m");
      topTag = ljet;
    }
  }

  // find additional jets
  // "additional" jets are neither in the higgs tag nor in the top tag
  int nAdditionalJets = 0;
  int nAdditionalBJets = 0;

  bool isLeadingOutsideBJet = true;
  std::vector<shared_ptr<Jet>> bjets; bjets.reserve(5);

  for (auto &jet: event->m_jets) {
    CHECK( jet->checkFloatVariable("mv2") );
    CHECK( jet->checkCharVariable("isbtagged_MV2c10_85") );

    //    float mv2 = jet->floatVariable("mv2");
    bool isbtagged_MV2c10_85 = jet->charVariable("isbtagged_MV2c10_85") == 1;
    bool isbtagged_MV2c10_77 = jet->charVariable("isbtagged_MV2c10_77") == 1;
    bool isbtagged_MV2c10_70 = jet->charVariable("isbtagged_MV2c10_70") == 1;
    bool isbtagged_MV2c10_60 = jet->charVariable("isbtagged_MV2c10_60") == 1;

    if (isbtagged_MV2c10_85) {
      
      bjets.push_back(jet);

      if(isbtagged_MV2c10_85)
	btag_sum_all += 1;
      if(isbtagged_MV2c10_77)
        btag_sum_all +=1;
      if(isbtagged_MV2c10_70)
        btag_sum_all +=1;
      if(isbtagged_MV2c10_60)
        btag_sum_all +=1;

    }    

    if (higgsTag) {
      for (auto &subjet: event->m_jets) {
        if (!subjet->checkIntVariable("rcjet_index")) continue;
        if (subjet->intVariable("rcjet_index") != higgsTag->intVariable("index"))
          continue;
        if (jet->DeltaR(*subjet) < 0.0001)
          goto nextJet;
      }
    }
    if (topTag) {
      for (auto &subjet: event->m_jets) {
        if (!subjet->checkIntVariable("rcjet_index")) continue;
        if (subjet->intVariable("rcjet_index") != topTag->intVariable("index"))
          continue;
        if (jet->DeltaR(*subjet) < 0.0001)
          goto nextJet;
      }
    }

    // now this jet is outside of H and T
    nAdditionalJets++;
    if (isbtagged_MV2c10_85) {
      nAdditionalBJets++;

      if(isbtagged_MV2c10_85)
        btag_sum_out_TH +=1;
      if(isbtagged_MV2c10_77)
	btag_sum_out_TH +=1;
      if(isbtagged_MV2c10_70)
	btag_sum_out_TH +=1;
      if(isbtagged_MV2c10_60)
	btag_sum_out_TH +=1;

      if (isLeadingOutsideBJet) {
        // assuming jets are sorted
        if (higgsTag)
          dRbH_out_TH = jet->DeltaR(*higgsTag);
        if (topTag)
          dRbT_out_TH = jet->DeltaR(*topTag);

        isLeadingOutsideBJet = false;
      }
    }

    nextJet:;
  }



  if (higgsTag && topTag) {
    dR_HT = higgsTag->DeltaR(*topTag);
  }

  if (btag_sum_all != 0)
    btag_sum_out_TH_over_all = btag_sum_out_TH / btag_sum_all;

  //  float H_d12 = (higgsTag) ? higgsTag->floatVariable("d12") : 0;
  float Top_d12 = (topTag) ? topTag->floatVariable("d12") : 0;

  if (bjets.size() >= 2) {
    dRbb_MaxPt_recal = bjets[0]->DeltaR(*bjets[1]);
  }

  if (higgsTag) {

    if (higgsTag->intVariable(prefix+"nBJetsInside") >= 2) {
      // dRbb_in_H
      shared_ptr<Particle> firstBSubjet {nullptr};
      shared_ptr<Particle> secondBSubjet {nullptr};

      for (auto &subjet: event->m_jets) {
        if (subjet->intVariable("rcjet_index") != higgsTag->intVariable("index"))
          continue;

        if (!subjet->charVariable("isbtagged_MV2c10_85")) continue;
        if (!firstBSubjet) {
          firstBSubjet = subjet;
          continue;
        }
        if (!secondBSubjet) {
          secondBSubjet = subjet;
          break;
        }
      } // subjets
    
      dRbb_in_H = firstBSubjet->DeltaR(*secondBSubjet);
    }


  } // if (higgsTag)
  
  if (lepton && higgsTag)
    dR_lepH = lepton->DeltaR(*higgsTag);

  event->intVariable(prefix+"nAdditionalJets") = nAdditionalJets;
  event->intVariable(prefix+"nAdditionalBJets") = nAdditionalBJets;
  event->intVariable(prefix+"nTopCandidates") = nTopCandidates;
  event->intVariable(prefix+"nTopCandidates_pt_bjet") = nTopCandidates_pt_bjet;
  event->intVariable(prefix+"nHiggsCandidates") = nHiggsCandidates;

  // BDT Variables
  event->floatVariable("btag_sum_all") = btag_sum_all;
  event->floatVariable("dR_HT") = dR_HT;
  event->floatVariable("btag_sum_out_TH") = btag_sum_out_TH;
  event->floatVariable("btag_sum_out_TH_over_all") = btag_sum_out_TH_over_all;
  //event->floatVariable("H_d12") = H_d12;
  event->floatVariable("Top_d12") = Top_d12;
  event->floatVariable("dRbH_out_TH") = dRbH_out_TH;
  event->floatVariable("dRbT_out_TH") = dRbT_out_TH;
  //event->floatVariable("dRbb_MaxPt_recal") = dRbb_MaxPt_recal;
  //event->floatVariable("dRbb_in_H") = dRbb_in_H;
  event->floatVariable("dR_lepH") = dR_lepH;


  //event->floatVariable("rcjet0_eta") = -1; // only overwritten when no rcjet survives

  // event->charVariable(prefix+"passes0") = 0;
  event->charVariable(prefix+"passes1") = 0;
  event->charVariable(prefix+"passes2") = 0;
  event->charVariable(prefix+"passes3") = 0;
  event->charVariable(prefix+"passes4") = 0;
  event->charVariable(prefix+"passes5") = 0;
  event->charVariable(prefix+"passes6") = 0;
  event->charVariable(prefix+"passes7") = 0;
  event->charVariable(prefix+"passes8") = 0;
  event->charVariable(prefix+"passesAll") = 0;
  if (requireTopBVeto == 1) {
    event->intVariable("boosted_passesRc1T1H") = 0;
  }
  // event->charVariable(prefix+"passes_topNoBVeto") = 0;
  // event->charVariable(prefix+"passes_topBVeto") = 0;
  // event->floatVariable("higgs_pt") = 0;
  event->floatVariable("rc_higgs_pt") = higgsTag ? (float) higgsTag->Pt() : 0;
  event->floatVariable("rc_higgs_m") = higgsTag ? (float) higgsTag->M() : 0;

  m_passesCutReclustered1T1H[0]++;
  event->charVariable(prefix+"passes0") = 1;

  do {
    if (!( goodLJets.size() >= 1 )) break;
    m_passesCutReclustered1T1H[1]++;
    event->charVariable(prefix+"passes1") = 1;
    
    if (!( nHiggsCandidates >= 1 )) break;
    m_passesCutReclustered1T1H[2]++;
    event->charVariable(prefix+"passes2") = 1;
    
    if (!( nTopCandidates_pt >= 1 )) break;
    m_passesCutReclustered1T1H[3]++;
    event->charVariable(prefix+"passes3") = 1;

    if (!( nTopCandidates_pt_bjet >= 1 )) break;

    m_passesCutReclustered1T1H[4]++;
    event->charVariable(prefix+"passes4") = 1;

    if (!( nTopCandidates_pt_bjet_sjet >= 1 )) break;
    m_passesCutReclustered1T1H[5]++;
    event->charVariable(prefix+"passes5") = 1;

    if (!( nTopCandidates >= 1 )) break;
    m_passesCutReclustered1T1H[6]++;
    event->charVariable(prefix+"passes6") = 1;
    
    if (!( nAdditionalJets >= 1 )) break;
    m_passesCutReclustered1T1H[7]++;
    event->charVariable(prefix+"passes7") = 1;
    
    if (!( nAdditionalBJets >= 1 )) break;
    m_passesCutReclustered1T1H[8]++;
    event->charVariable(prefix+"passes8") = 1;
  } while (false);


  // if (nHiggsCandidates >= 1 && nTopCandidates_pt_bjet >= 1 && nAdditionalBJets >= 1) {
  //     event->charVariable(prefix+"passes_topNoBVeto") = 1;
  // }
  if (nHiggsCandidates >= 1 && nTopCandidates >= 1 && nAdditionalBJets >= 1) {
      m_nPassReclustered1T1H++;
      event->charVariable(prefix+"passesAll") = 1;
      if (requireTopBVeto == 1) {
        event->intVariable("boosted_passesRc1T1H") = 1;
      }
  }

  //SBOUTLE
  m_ClassifyBDT->setRegion(event->intVariable("boosted_passesRc1T1H"));

  std::vector<float> varsToClass_event_values;
  for(auto & var : m_ClassifyBDT->returnVarsToClass()){

    //std::cout<<var<<std::endl;
    // if custom variables. map custom variable names to something you calculate above
    if(var=="dR_LepH"){
      varsToClass_event_values.push_back(event->floatVariable("dR_lepH"));
      //std::cout<<var<<" float "<<event->floatVariable("dR_lepH")<<std::endl;
    }
    else if(var=="Higgs_m"){
      varsToClass_event_values.push_back(event->floatVariable("rc_higgs_m"));
      //std::cout<<var<<" float "<<event->floatVariable("higgs_m")<<std::endl;
    }
    else if(var=="dRbb_Max_Pt_recal"){
      varsToClass_event_values.push_back(event->floatVariable("dRbb_MaxPt_recal"));
      //std::cout<<var<<" float " << dRbb_MaxPt_recal <<std::endl;
    }
    else if(var=="rcjet_eta"){
      varsToClass_event_values.push_back(-1.0);  
      //std::cout<<var<<" float -1.0"<<std::endl;
    }
    else if(var=="Top_d12"){
      varsToClass_event_values.push_back(event->floatVariable("Top_d12"));
      //std::cout<<var<<" float " << event->floatVariable("Top_d12") <<std::endl;
    }
    else if(var=="H_d12"){
      varsToClass_event_values.push_back(event->floatVariable("H_d12"));
      //std::cout<<var<<" float " << event->floatVariable("H_d12") <<std::endl;
    }
 
    //else if(var=="rcjet_eta")varsToClass_event_values.push_back(event->floatVariable("rcjet0_eta"));  
    // else for standard variables
    else{
        
      if(event->floatVariable(var)!=-999){
        varsToClass_event_values.push_back(event->floatVariable(var));
	//std::cout<<var<<" float "<<event->floatVariable(var)<<std::endl;
      }
      else if(event->intVariable(var)!=-999){
        varsToClass_event_values.push_back((float)event->intVariable(var));
	//std::cout<<var<<" int "<<event->intVariable(var)<<std::endl;
      }
      else std::cout<<var<<" is not defined"<<std::endl;              


    }
  }

  UInt_t m_eventNumber= event->m_info->eventNumber; // event number to get the even/odd training right
  float score=m_ClassifyBDT->event_score(varsToClass_event_values, m_eventNumber);
  event->floatVariable(m_ClassifyBDT->getScoreName()) = score;

} 


void BoostedVariables::finalise() {
  cout << "INFO: BoostedVariables::finalise" << endl;
  cout << "Pass 3211: " << m_nPass3211 << endl;
  cout << "Preselection:             " << m_passesCut3211[0] << endl;  
  cout << ">= 1 Ljet (pt > 250):     " << m_passesCut3211[1] << endl;
  cout << ">= 1 Ljet (+ eta, mass):  " << m_passesCut3211[2] << endl;
  cout << ">= 1 Ljet (+ e veto):     " << m_passesCut3211[3] << endl;
  cout << ">= 1 Ljet (+ top tag):    " << m_passesCut3211[4] << endl;
  cout << ">= 3 jets outside t:      " << m_passesCut3211[5] << endl;
  cout << ">= 2 bjets outside t:     " << m_passesCut3211[6] << endl;
  cout << endl;
  cout << "Pass 1T1H: " << m_nPass1T1H << endl;
  cout << "Preselection:             " << m_passesCut1T1H[0] << endl;  
  cout << ">= 1 Good large jet:      " << m_passesCut1T1H[1] << endl;
  cout << ">= 1 Higgs-tag candidate: " << m_passesCut1T1H[2] << endl;
  cout << ">= 1 top-tag candidate::  " << m_passesCut1T1H[3] << endl;
  cout << ">= 1 additional jet:      " << m_passesCut1T1H[4] << endl;
  cout << ">= 1 additional b-tag:    " << m_passesCut1T1H[5] << endl;
  cout << endl;
  cout << "Pass Reclustered 1T1H: " << m_nPassReclustered1T1H << endl;
  cout << "Preselection:             " << m_passesCutReclustered1T1H[0] << endl;
  cout << ">= 1 Good large jet:      " << m_passesCutReclustered1T1H[1] << endl;
  cout << ">= 1 Higgs-tag candidate: " << m_passesCutReclustered1T1H[2] << endl;
  cout << ">= 1 good ljet Pt > 250:  " << m_passesCutReclustered1T1H[3] << endl;
  cout << ">= 1 good ljet ==1 bjet:  " << m_passesCutReclustered1T1H[4] << endl;
  cout << ">= 1 good ljet >=1 !bjet: " << m_passesCutReclustered1T1H[5] << endl;
  cout << ">= 1 top-tag candidate::  " << m_passesCutReclustered1T1H[6] << endl;
  cout << ">= 1 additional jet:      " << m_passesCutReclustered1T1H[7] << endl;
  cout << ">= 1 additional b-tag:    " << m_passesCutReclustered1T1H[8] << endl;
}

} // namespace TTHbb
