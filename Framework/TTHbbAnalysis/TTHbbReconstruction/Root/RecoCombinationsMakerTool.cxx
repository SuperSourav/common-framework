/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/RecoCombinationsMakerTool.h"
#include "TTHbbTruthHelper/TruthMatchHelper.h"
#include <bitset>

RecoCombinationsMakerTool::RecoCombinationsMakerTool(){

  m_allowBJetsForW=false; ///always not just in case below
  m_allowBJetsForWAtLowNL=true; /// only if njets>=6 and nlight<2
  m_allowLightJetsAsB=false; /// only if nb>=4 otherwise always allowed
  m_forceTaggedJetsAsB=true; /// only if nb<4 otherwise use allowLightJetsAsB
  m_orderByWeightAtLargeNB=false; /// if nb>=5
  m_forceOneBJetsInHiggs=true;
  m_allowedOrderedWeightForBCut=-1;
  m_allowedOrderedWeightForLightCut=-1;
  m_forceUseForLJetsName="";
  m_forceUseForBJetsName="";
  m_useBTagForComb=true;
  m_btagOrderVariableName="";

  //m_btaggingCut=-999;
  m_btagIdName="";

  /// internally computed
  m_isMC=false;  
  m_nJets=-1; 
  m_nBJets=-1;
  m_neededJets=-1;
  m_neededB=-1;
  m_neededLight=-1;
  m_nUseBJets=-1;
  m_nUseLJets=-1;
}

RecoCombinationsMakerTool::~RecoCombinationsMakerTool(){
  ////
}

bool RecoCombinationsMakerTool::buildStrategyTTHBoostedSemiLep(JetContainer* jets){

  m_nJets = jets->size();
  m_neededJets = (m_nJets>=6)?6:m_nJets;
  m_neededB=4;
  m_neededLight=1;

  m_nBJets = 0;
  for( auto a : *jets){
    if(isBJet(a.get())) ++m_nBJets;
  }
  /// do not put it in the same for loop as before. useAsBJet needs m_nBJets to be set
  m_nUseBJets=0;
  for( auto a : *jets){
    if(useAsBJet(a.get())) ++m_nUseBJets;
  }

  m_nUseLJets=0;
  for( auto a : *jets){
    if(useAsLJet(a.get())) ++m_nUseLJets;
  }
  return true;
}

bool RecoCombinationsMakerTool::buildStrategyTTHSemiLep(JetContainer* jets){
  m_nJets = jets->size();
  m_neededJets=(m_nJets>=6)?6:m_nJets;
  m_neededB=4;
  m_neededLight=2;
  if(m_nJets<6) m_neededLight=1;
  if(m_nJets<5) m_neededLight=0;

  m_nBJets = 0;
  for( auto a : *jets){
    if(isBJet(a.get())) ++m_nBJets;
  }

  /// do not put it in the same for loop as before. useAsBJet needs m_nBJets to be set
  m_nUseBJets=0;
  for( auto a : *jets){
    if(useAsBJet(a.get())) ++m_nUseBJets;
  }

  m_nUseLJets=0;
  for( auto a : *jets){
    if(useAsLJet(a.get())) ++m_nUseLJets;
  }

  return true;

}


bool RecoCombinationsMakerTool::buildStrategyTTHDiLep(JetContainer* jets){


  m_nJets = jets->size();
  m_neededJets=(m_nJets>=4)?4:m_nJets;
  m_neededB=4;
  m_neededLight=0;

  m_nBJets = 0;
  for( auto a : *jets){
    if(isBJet(a.get())) ++m_nBJets;
  }

  /// do not put it in the same for loop as before. useAsBJet needs m_nBJets to be set
  m_nUseBJets=0;
  for( auto a : *jets){
    if(useAsBJet(a.get())) ++m_nUseBJets;
  }

  m_nUseLJets=0;
  for( auto a : *jets){
    if(useAsLJet(a.get())) ++m_nUseLJets;
  }

  return true; 

}

bool RecoCombinationsMakerTool::buildStrategyTTBarSemiLep(JetContainer* jets){

  m_nJets = jets->size();
  m_neededJets=4;
  m_neededB=4;
  m_neededLight=2;
  if(m_nJets<4) m_neededLight=1;
  if(m_nJets<3) m_neededLight=0;

  m_nBJets = 0;
  for( auto a : *jets){
    if(isBJet(a.get())) ++m_nBJets;
  }

  /// do not put it in the same for loop as before. useAsBJet needs m_nBJets to be set
  m_nUseBJets=0;
  for( auto a : *jets){
    if(useAsBJet(a.get())) ++m_nUseBJets;
  }

  m_nUseLJets=0;
  for( auto a : *jets){
    if(useAsLJet(a.get())) ++m_nUseLJets;
  }

  return true; 

}

bool RecoCombinationsMakerTool::buildStrategyTTBarDiLep(JetContainer* jets){

  m_nJets = jets->size();
  m_neededJets=2;
  m_neededB=4;
  m_neededLight=0;

  m_nBJets = 0;
  for( auto a : *jets){
    if(isBJet(a.get())) ++m_nBJets;
  }

  /// do not put it in the same for loop as before. useAsBJet needs m_nBJets to be set
  m_nUseBJets=0;
  for( auto a : *jets){
    if(useAsBJet(a.get())) ++m_nUseBJets;
  }

  m_nUseLJets=0;
  for( auto a : *jets){
    if(useAsLJet(a.get())) ++m_nUseLJets;
  }

  return true; 

}




bool RecoCombinationsMakerTool::isBJet(Jet* jet){
  return jet->charVariable(m_btagIdName);
}


bool RecoCombinationsMakerTool::useAsBJet(Jet* jet){

  /// do not assume btag for comb making
  if(!m_useBTagForComb) return true;

  /// for "upgrading" jets to btagged in control regions
  if(m_forceUseForBJetsName!=""){
    return jet->intVariable(m_forceUseForBJetsName);
  }

  /// using btag order to determine what to use as b-jets
  if( m_allowedOrderedWeightForBCut > 0 ){
    if(jet->intVariable(m_btagOrderVariableName) <= m_allowedOrderedWeightForBCut) return true;
    else return false;
  }

  //// allow light jets to be used as b is not enough bs are available (e.g. 6/3)
  bool allowlight=true;
  if(m_nBJets >= m_neededB) allowlight=m_allowLightJetsAsB;
  if(allowlight) {
    return true;
  }

  if(m_orderByWeightAtLargeNB && m_nBJets>m_neededB && m_neededB>0){
    return isBJet(jet) && jet->intVariable(m_btagOrderVariableName) <= m_neededB; /// isBJet should be useless here
  }

  return isBJet(jet);
}

bool RecoCombinationsMakerTool::useAsLJet(Jet* jet){

  /// do not assume btag for comb making
  if(!m_useBTagForComb) return true;

  /// for "upgrading" jets to btagged in control regions
  if(m_forceUseForLJetsName!=""){
    return jet->intVariable(m_forceUseForLJetsName);
  }

  /// using btag order to determine what to use as b-jets
  if( m_allowedOrderedWeightForLightCut > 0 ){
    if( jet->intVariable(m_btagOrderVariableName) >= m_allowedOrderedWeightForLightCut) return true;
    else return false;
  }

  /// allow b-jets as light when there not enough light (e.g. 6/5)
  bool allowBjets=m_allowBJetsForW;
  if( (m_nJets-m_nUseBJets)<m_neededLight && m_allowBJetsForWAtLowNL) allowBjets=true;

  return allowBjets || !useAsBJet(jet) || !isBJet(jet);

}


CompositeContainer* RecoCombinationsMakerTool::createIncompLepTopContainer(std::shared_ptr<Lepton> lepton, JetContainer* jets, std::string contname){


  if(!lepton || !jets ) return 0;

  CompositeContainer* cont = new CompositeContainer();
  cont->name = contname;

  for(unsigned int i=0; i<jets->size(); ++i){

    auto jet = (*jets)[i];
    if(!useAsBJet(jet.get()) ) continue;

    Composite* comp = m_combinationsDecorator.createIncompLepTop(lepton,jet,m_isMC);
    cont->add(comp);

  }

  return  cont;

}



CompositeContainer* RecoCombinationsMakerTool::createLepWContainer(LeptonContainer* leptons, NeutrinoContainer* neutrinos, std::string contname){

  if(!leptons || ! neutrinos) return 0;

  auto lep = (*leptons)[0];

  CompositeContainer* cont = new CompositeContainer();
  cont->name = contname;

  for( auto nu : *neutrinos){
    Composite* comp =m_combinationsDecorator.createLepW(lep,nu,m_isMC);
    cont->add(comp);
  }

  return cont;

}


CompositeContainer* RecoCombinationsMakerTool::createBoostedHadWContainer(JetContainer* jets, std::string contname){

  if(!jets) return 0;
    
  CompositeContainer* cont = new CompositeContainer();
  cont->name = contname;

  // If theres only 2 subjets, assume the W is boosted into one jet
  // since we require ==1 b and >=1 nonb, we can create the had as just the non-b  in this case
  if (jets->size() == 2){
  // std::cout << "Boosted HadW!" << std::endl;
    
    auto jet1 = (*jets)[0];
    auto jet2 = (*jets)[1];
    
    if (useAsLJet(jet1.get())){
      Composite* comp = m_combinationsDecorator.createBoostedHadW(jet1,m_isMC);
      cont->add(comp);
    }
    else if (useAsLJet(jet2.get())){
      Composite* comp = m_combinationsDecorator.createBoostedHadW(jet2,m_isMC);
      cont->add(comp);
    }
    else {
      std::cout << "ERROR: Hadtop has == 2 subjets and neither of them are light! How did you get here?" << std::endl;
      return 0;
    }
  }
  else{
    // otherwise its a normal hadW
    for(unsigned int i=0; i<jets->size(); ++i){
      auto jet1 = (*jets)[i];
      if( !useAsLJet(jet1.get()) ) continue;
      for(unsigned int j=i+1; j<jets->size(); ++j){
	auto jet2 = (*jets)[j];
	if( !useAsLJet(jet2.get()) ) continue;
	Composite* comp = m_combinationsDecorator.createHadW(jet1,jet2,m_isMC);
	cont->add(comp);
      }
    }
  }

  return cont;

}


CompositeContainer* RecoCombinationsMakerTool::createHadWContainer(JetContainer* jets, std::string contname){

  if(!jets) return 0;

  CompositeContainer* cont = new CompositeContainer();
  cont->name = contname;

  for(unsigned int i=0; i<jets->size(); ++i){
    auto jet1 = (*jets)[i];
    if( !useAsLJet(jet1.get()) ) continue;
    for(unsigned int j=i+1; j<jets->size(); ++j){
      auto jet2 = (*jets)[j];
      if( !useAsLJet(jet2.get()) ) continue;
      Composite* comp = m_combinationsDecorator.createHadW(jet1,jet2,m_isMC);
      cont->add(comp);
    }
  }

  return cont;

}

CompositeContainer* RecoCombinationsMakerTool::createHiggsContainer(JetContainer* jets, std::string contname){
 
  if(!jets) return 0;

  CompositeContainer* cont = new CompositeContainer();
  cont->name = contname;

  for(unsigned int i=0; i<jets->size(); ++i){
    auto jet1 = (*jets)[i];
    if(!useAsBJet(jet1.get()) ) continue;
    for(unsigned int j=i+1; j<jets->size(); ++j){
      auto jet2 = (*jets)[j];
      if(!useAsBJet(jet2.get()) ) continue;

      if(m_forceOneBJetsInHiggs){
	if(!isBJet(jet1.get()) && !isBJet(jet2.get()))continue;
      }
      
      Composite* comp =m_combinationsDecorator.createHiggs(jet1,jet2,m_isMC);
      cont->add(comp);
    }
  }

  return cont;

}


TopCompositeContainer* RecoCombinationsMakerTool::createLepTopContainer(CompositeContainer* lepWContainer, JetContainer* jets, std::string contname){

  if(!lepWContainer || !jets) return 0;

  TopCompositeContainer* cont = new TopCompositeContainer();
  cont->name = contname;

  for( auto j : *jets){
    if(!useAsBJet(j.get())) continue;
    for( auto w : *lepWContainer){
      TopComposite* comp = m_combinationsDecorator.createLepTop(j,w,m_isMC);
      cont->add(comp);
    }
  }

  return cont;

}
TopCompositeContainer* RecoCombinationsMakerTool::createHadTopContainer(CompositeContainer* hadWContainer, JetContainer* jets, std::string contname){

  if(!hadWContainer || !jets) return 0;

  TopCompositeContainer* cont = new TopCompositeContainer();
  cont->name = contname;

  /// take whatever in the w container don't check bs in W
   for( auto j : *jets){
     if(!useAsBJet(j.get())) continue;
     for( auto w : *hadWContainer){
       if(j->id == w->child1->id) continue;
       if(j->id == w->child2->id) continue;  
      TopComposite* comp = m_combinationsDecorator.createHadTop(j,w,m_isMC);
      cont->add(comp);
    }
  }


  return cont;

}


CompositeContainer* RecoCombinationsMakerTool::createBoostedHadTopContainer(CompositeContainer* hadWContainer, JetContainer* jets, std::string contname){

  if(!hadWContainer || !jets) return 0;

  CompositeContainer* cont = new CompositeContainer();
  cont->name = contname;

  /// take whatever in the w container don't check bs in W
  for( auto j : *jets){
    if(!isBJet(j.get())) continue;
    for (auto j2 : *jets){
      if (j2 == j) continue;
      Composite* comp = m_combinationsDecorator.createIncompleteHadTop(j,j2,m_isMC);
      cont->add(comp);
    }
  }

  return cont;

}



CompositeContainer* RecoCombinationsMakerTool::createIncompleteHadTopContainer(JetContainer* jets, std::string contname){

  if(!jets) return 0;

  CompositeContainer* cont = new CompositeContainer();
  cont->name = contname;


 for(unsigned int i=0; i<jets->size(); ++i){
    auto jet1 = (*jets)[i];
    if(!useAsBJet(jet1.get()) ) continue;
    for(unsigned int j=0; j<jets->size(); ++j){

      if(i==j) continue;
      ///can switch i and j depending which is assumed the b from top (in case l/b mixing is allowed

      auto jet2 = (*jets)[j];
      if(!useAsLJet(jet2.get()) ) continue;
      Composite* comp = m_combinationsDecorator.createIncompleteHadTop(jet1,jet2,m_isMC);
      cont->add(comp);
    }
  }


  return cont;

}


CombinationContainer* RecoCombinationsMakerTool::createCombinations(TopCompositeContainer* lepTopContainer, TopCompositeContainer* hadTopContainer, CompositeContainer* higgsContainer, std::string contname){

  if(!lepTopContainer || !hadTopContainer || !higgsContainer) return 0;

  CombinationContainer* cont = new CombinationContainer();
  cont->name = contname;


  for(auto ltop : *lepTopContainer){
    for(auto htop : *hadTopContainer){
      for(auto higgs : *higgsContainer){


	if(jetOverlap(higgs.get(), htop.get())) continue;
	if(jetOverlap(ltop.get(), higgs.get())) continue;
	if(jetOverlap(ltop.get(), htop.get())) continue;
	
	if(m_forceTaggedJetsAsB && m_nBJets<m_neededB){
	  int nUsedNB=0;
	  if(isBJet(ObjectHelper::toJet(ltop->childb.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(htop->childb.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(higgs->child1.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(higgs->child2.get())))++nUsedNB;
	  if(nUsedNB > m_nBJets){
	    std::cout << "Warning RecoCombinationsMakerTool::createCombinations " << " nUsedNB > m_nBJets (" << nUsedNB<<">"<<m_nBJets<<")" <<std::endl;
	  }
	  if(m_nBJets != nUsedNB) continue;
	}

	/// ok have all the basics
	/// add whatever selection you want here

	Combination* comb =m_combinationsDecorator.createCombination(ltop,htop,higgs,m_isMC);
	m_combinationsDecorator.decorateCombination(comb);
	cont->add(comb);

      }
    }
  }

  return cont;

}



CombinationContainer* RecoCombinationsMakerTool::createCombinations(CompositeContainer* lepTop1Container, CompositeContainer* lepTop2Container, CompositeContainer* higgsContainer, double met_met, double met_phi, int eventNumber, std::string contname){

  if(!lepTop1Container || !lepTop2Container || !higgsContainer) return 0;
  

  CombinationContainer* cont = new CombinationContainer();
  cont->name = contname;


  for(auto ltop1 : *lepTop1Container){
    for(auto ltop2 : *lepTop2Container){
      for(auto higgs : *higgsContainer){


	if(jetOverlap(higgs.get(), ltop2.get())) continue;
	if(jetOverlap(ltop1.get(), higgs.get())) continue;
	if(jetOverlap(ltop1.get(), ltop2.get())) continue;

	/// child2 is the jet
	if(m_forceTaggedJetsAsB && m_nBJets<m_neededB){
	  int nUsedNB=0;
	  if(isBJet(ObjectHelper::toJet(ltop1->child2.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(ltop2->child2.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(higgs->child1.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(higgs->child2.get())))++nUsedNB;
	  if(nUsedNB > m_nBJets){
	    std::cout << "Warning RecoCombinationsMakerTool::createCombinations " << " nUsedNB > m_nBJets (" << nUsedNB<<">"<<m_nBJets<<")" <<std::endl;
	  }
	  if(m_nBJets != nUsedNB) continue;
	}

	/// ok have all the basics
	/// add whatever selection you want here

	Combination* comb = m_combinationsDecorator.createCombination(ltop1,ltop2,higgs, met_met, met_phi, eventNumber, m_isMC);
	m_combinationsDecorator.decorateDilepCombination(comb);
	cont->add(comb);

      }
    }
  }

  return cont;

}



CombinationContainer* RecoCombinationsMakerTool::createIncompleteCombinations(TopCompositeContainer* lepTopContainer, CompositeContainer* incompleteHadTopContainer, CompositeContainer* higgsContainer, std::string contname){


  if(!lepTopContainer || !incompleteHadTopContainer || !higgsContainer) return 0;


  CombinationContainer* cont = new CombinationContainer();
  cont->name = contname;


  for(auto ltop : *lepTopContainer){
    for(auto htop : *incompleteHadTopContainer){
      for(auto higgs : *higgsContainer){

	if(jetOverlap(higgs.get(), htop.get())) continue;
	if(jetOverlap(ltop.get(), higgs.get())) continue;
	if(jetOverlap(ltop.get(), htop.get())) continue;

	if(m_forceTaggedJetsAsB && m_nBJets<m_neededB){
	  int nUsedNB=0;
	  if(isBJet(ObjectHelper::toJet(ltop->childb.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(htop->child1.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(higgs->child1.get())))++nUsedNB;
	  if(isBJet(ObjectHelper::toJet(higgs->child2.get())))++nUsedNB;
	  if(nUsedNB > m_nBJets){
	    std::cout << "Warning RecoCombinationsMakerTool::createIncompleteCombinations " << " nUsedNB > m_nBJets (" << nUsedNB<<">"<<m_nBJets<<")" <<std::endl;
	  }
	  if(m_nBJets != nUsedNB) continue;
	}

	/// ok have all the basics
	/// add whatever selection you want here

	Combination* comb =m_combinationsDecorator.createIncompleteCombination(ltop,htop,higgs,m_isMC);
	m_combinationsDecorator.decorateInCompleteCombination(comb);

	cont->add(comb);

      }

    }

  }

  return cont;


}

CombinationContainer* RecoCombinationsMakerTool::createCombinations(TopCompositeContainer* lepTopContainer, TopCompositeContainer* hadTopContainer, JetContainer* jets, std::string contname){

  if(!lepTopContainer || !hadTopContainer) return 0;
  
  CombinationContainer* cont = new CombinationContainer();
  cont->name = contname;

  for(auto ltop : *lepTopContainer){
    for(auto htop : *hadTopContainer){

      if(jetOverlap(ltop.get(), htop.get())) continue;

      /// ok have all the basics
      /// add whatever selection you want here

      Combination* comb =m_combinationsDecorator.createCombination(ltop,htop,m_isMC);
      m_combinationsDecorator.decorateTTbarCombination(comb);
      m_combinationsDecorator.fillAdditionalJets(jets, comb);
      if( (int)comb->additionalJets.size() != (m_nJets-m_neededJets)){
	std::cout << "ERROR: in RecoCombinationsMakerTool::createCombinations ttbar njets = "
		  << m_nJets << " additional jets = " << comb->additionalJets.size() << std::endl;
	return 0;
      }
      m_combinationsDecorator.decorateAdditionalJets(comb);
      cont->add(comb);

    
    }
  }

  return cont;

}


CombinationContainer* RecoCombinationsMakerTool::createCombinations(CompositeContainer* lepTop1Container, CompositeContainer* lepTop2Container, JetContainer* jets, std::string contname){

  if(!lepTop1Container || !lepTop2Container) return 0;
  
  CombinationContainer* cont = new CombinationContainer();
  cont->name = contname;

  for(auto ltop1 : *lepTop1Container){
    for(auto ltop2 : *lepTop2Container){

      /// child2 is the jet
      if(jetOverlap(ltop1.get(),ltop2.get())) continue;

      /// ok have all the basics
      /// add whatever selection you want here

      Combination* comb =m_combinationsDecorator.createCombination(ltop1,ltop2,-10e10, -10e10, -1, m_isMC);
      m_combinationsDecorator.decorateTTbarDilepCombination(comb);
      m_combinationsDecorator.fillAdditionalJets(jets, comb);
      if( (int)comb->additionalJets.size() != (m_nJets-m_neededJets)){
	std::cout << "ERROR: in RecoCombinationsMakerTool::createCombinations dilep ttbar njets = "
		  << m_nJets << " additional jets = " << comb->additionalJets.size() << std::endl;
	return 0;
      }
      m_combinationsDecorator.decorateAdditionalJets(comb);
      cont->add(comb);

    }
  }

  return cont;

}

bool RecoCombinationsMakerTool::jetOverlap(Composite* comp1, Composite* comp2){

  if( comp1->child1->id == comp2->child1->id && comp1->child1->isJet() && comp2->child1->isJet()) return true;
  if( comp1->child1->id == comp2->child2->id && comp1->child1->isJet() && comp2->child2->isJet()) return true;
  if( comp1->child2->id == comp2->child1->id && comp1->child2->isJet() && comp2->child1->isJet()) return true;
  if( comp1->child2->id == comp2->child2->id && comp1->child2->isJet() && comp2->child2->isJet()) return true;
  return false;

}


bool RecoCombinationsMakerTool::jetOverlap(TopComposite* comp1, Composite* comp2){

  if(jetOverlap(comp1->childw.get(), comp2)) return true;
  if(comp1->childb->id == comp2->child1->id && comp2->child1->isJet()) return true;
  if(comp1->childb->id == comp2->child2->id && comp2->child2->isJet()) return true;
  
  return false;

}

bool RecoCombinationsMakerTool::jetOverlap(Composite* comp1, TopComposite* comp2){
  return jetOverlap(comp2, comp1);
}

bool RecoCombinationsMakerTool::jetOverlap(TopComposite* comp1, TopComposite* comp2){

  if(jetOverlap(comp1->childw.get(), comp2)) return true;
  if(comp1->childb->id == comp2->childb->id) return true;
  if(comp1->childb->id == comp2->childw->child1->id && comp2->childw->child1->isJet()) return true;
  if(comp1->childb->id == comp2->childw->child2->id && comp2->childw->child2->isJet()) return true;

  return false;

}
