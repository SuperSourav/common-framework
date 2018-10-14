#include "OfflineDileptonVariables/DilepVars.h"
#include "TTHbbToolManager/ToolManager.h"
#include "TTHbbObjects/TTHbbUtils.h"

#include "TTHbbToolManager/OfflineMVAVariables.h"
#include "MVAVariables/MVAVariables.h"

#include "TTHbbConfiguration/GlobalConfiguration.h"

#include <memory>

namespace TTHbb{

  DilepVars::DilepVars(){
    
    auto* config = TTHbb::GlobalConfiguration::get();
    wps = TTHbb::util::vectoriseString((*config)("MVAVariables.bTagWPs"));

    // sized to store b-tag levels from leading 4 jets
    bTagLevels.resize(4);
    
  }
  
  void DilepVars::apply(TTHbb::Event* event){

    auto* tManager = TTHbb::ToolManager::get();
    OfflineMVAVariables* OffMVAVars = (OfflineMVAVariables*)tManager->getInstanceOfTool("MVAVariables");

    std::shared_ptr<MVAVariables> mva;
    for(auto wp : wps){
      mva = OffMVAVars->getMVAVariables(wp);

      event->floatVariable("dEtabb_Avg_" + wp) = mva->deltaEtaofPair(pairing::bb, variable::Avg);
      event->floatVariable("dRbb_MaxPt_" + wp)  = mva->deltaRofPair(pairing::bb, variable::MaxPt);
      event->floatVariable("Mbb_HiggsMass_" + wp) = mva->MassofPair(pairing::bb, variable::HiggsMass);
      event->floatVariable("dRbb_HiggsMass_" + wp) = mva->deltaRofPair(pairing::bb, variable::HiggsMass);
      event->intVariable  ("nHiggsbb30_" + wp) = mva->nHiggsCandidatesMassWindow(pairing::bb, 30);
      event->floatVariable("Mbb_MinM_" + wp) = mva->MassofPair(pairing::bb, variable::MinM);
      event->floatVariable("Mbb_MindR_" + wp) = mva->MassofPair(pairing::bb, variable::MindR);
      event->floatVariable("dEtabb_MaxdEta_" + wp) = mva->deltaEtaofPair(pairing::bb, variable::MaxdEta);
      event->floatVariable("dEtajj_Avg") = mva->deltaEtaofPair(pairing::jj, variable::Avg);
      event->floatVariable("dRbb_MaxM_" + wp) = mva->deltaRofPair(pairing::bb, variable::MaxM);
      event->floatVariable("Mbb_MaxPt_" + wp) = mva->MassofPair(pairing::bb, variable::MaxPt);
      event->floatVariable("pTbb_MindR_" + wp) = mva->PtofPair(pairing::bb, variable::MindR);

      event->floatVariable("dRbb_MindR_" + wp)  = mva->deltaRofPair(pairing::bb, variable::MindR);
    }

    if(mva){
      event->floatVariable("dEtajl_MaxdEta") = mva->deltaEtaofPair(pairing::jl, variable::MaxdEta);
      event->floatVariable("H0_all") = mva->FirstFoxWolframMoment(collection::all);
      event->floatVariable("Aplanarity_jets") = mva->Aplanarity(collection::jets);
      event->floatVariable("Centrality_all") = mva->Centrality(collection::all);
      event->intVariable  ("nHiggsjj30") = mva->nHiggsCandidatesMassWindow(pairing::jj, 30);
      event->floatVariable("HT_all") = mva->HT(collection::all);
      event->floatVariable("H2_jets") = mva->ThirdFoxWolframMoment(collection::jets);
      event->intVariable  ("nJets_Pt40") = mva->nJetsAbovePt(40);
    }

    // // calculate b-tag levels for leading 4 jets
    // bTagLevels[0] = getTagLevel( event , 0 );
    // bTagLevels[1] = getTagLevel( event , 1 );
    // bTagLevels[2] = getTagLevel( event , 2 );
    // bTagLevels[3] = getTagLevel( event , 3 );
    
    // // Create variables for bTag levels for each of the leading 4 jets, ordered by Pt
    // event->intVariable("bTagLevel_jetPt_0") = bTagLevels[0];
    // event->intVariable("bTagLevel_jetPt_1") = bTagLevels[1];
    // event->intVariable("bTagLevel_jetPt_2") = bTagLevels[2];
    // event->intVariable("bTagLevel_jetPt_3") = bTagLevels[3];

    // // Create variables for bTag levels for each of the leading 4 jets, ordered by MV2
    // std::sort( bTagLevels.rbegin() , bTagLevels.rend() ); // sort levels in descending order
    // event->intVariable("bTagLevel_jetMV2_0") = bTagLevels[0];
    // event->intVariable("bTagLevel_jetMV2_1") = bTagLevels[1];
    // event->intVariable("bTagLevel_jetMV2_2") = bTagLevels[2];
    // event->intVariable("bTagLevel_jetMV2_3") = bTagLevels[3];
    
  }

  // Return the b-tag level for a given jet --
  // This is an integer specifying the tightest WP satisfied by the jet.
  int DilepVars::getTagLevel( TTHbb::Event* event , std::size_t ijet) {

    // If the jet doesn't exist then return -1
    if( ijet >= event->m_jets.size() ) return -1;

    if( event->m_jets.at(ijet)->charVariable("jet_isbtagged_60") ) return 4;
    if( event->m_jets.at(ijet)->charVariable("jet_isbtagged_70") ) return 3;
    if( event->m_jets.at(ijet)->charVariable("jet_isbtagged_77") ) return 2;
    if( event->m_jets.at(ijet)->charVariable("jet_isbtagged_85") ) return 1;
    return 0; // 0 => untagged

  }
  
}
