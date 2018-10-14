/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/JetInCombinationDecorationTool.h"

#include "TTHbbTruthHelper/TruthMatchHelper.h"

using namespace TTHbb;

void JetInCombinationDecorationTool::decorateJets(TTHbb::Combination* comb, std::string flag){


  if(comb->lepTop){
    comb->lepTop->childb->intVariable(flag)= jetReco_bfromlepTop;
  }
  if(comb->lepTop1){
    comb->lepTop1->child1->intVariable(flag)= lepReco_lepfromlepTop1;
    comb->lepTop1->child2->intVariable(flag)= jetReco_bfromlepTop1;
  }
  if(comb->lepTop2){
    comb->lepTop2->child1->intVariable(flag)= lepReco_lepfromlepTop2;
    comb->lepTop2->child2->intVariable(flag)= jetReco_bfromlepTop2;
  }
  if(comb->hadTop){
    comb->hadTop->childb->intVariable(flag)= jetReco_bfromhadTop;
    comb->hadTop->childw->child1->intVariable(flag)= jetReco_q1fromW;
    comb->hadTop->childw->child2->intVariable(flag)= jetReco_q2fromW;
  }
  if(comb->incompleteHadTop){
    comb->incompleteHadTop->child1->intVariable(flag)= jetReco_bfromhadTop;
    comb->incompleteHadTop->child2->intVariable(flag)= jetReco_qincompleteTop;
  }
  if(comb->higgs){
    comb->higgs->child1->intVariable(flag)= jetReco_b1fromHiggs;
    comb->higgs->child2->intVariable(flag)= jetReco_b2fromHiggs;
  }
 
/// also additional jets ?



}


bool JetInCombinationDecorationTool::fillRecoVariables(TTHbb::RecoEvent* eventvar, std::string combname, std::string flag){


  std::shared_ptr<TTHbb::Combination> comb = eventvar->combinationVariable(combname);

  if(!comb){
    std::cout << "ERROR: RecoMVAVariableTool could not find combination " << combname << " in eventvar " << eventvar->name << std::endl;
    return false;
  }
 
  decorateJets(comb.get(), flag);
  return true;

}

bool JetInCombinationDecorationTool::fillRecoVariables(TTHbb::Event* eventvar, std::string combname, std::string flag){

 
  std::shared_ptr<TTHbb::Combination> comb = eventvar->combinationVariable(combname);

  if(!comb){
    std::cout << "ERROR: RecoMVAVariableTool could not find combination " << combname << std::endl;
    return false;
  }
 
  decorateJets(comb.get(), flag);

  return true;

}
