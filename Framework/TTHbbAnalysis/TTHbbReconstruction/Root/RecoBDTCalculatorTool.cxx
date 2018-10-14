/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/RecoBDTCalculatorTool.h"


RecoBDTCalculatorTool::RecoBDTCalculatorTool() : RegionInterface(){


  m_nBestCombination=1;
  m_addBestBHiggsSwitchComb=false;
  /// internally computed


}

RecoBDTCalculatorTool::~RecoBDTCalculatorTool(){
  ////
}




bool RecoBDTCalculatorTool::computeBDT(RecoEvent* eventvar, CombinationContainer* combinations, unsigned long long eventNumber){

  int njets = eventvar->nJets;
  int nbjets = eventvar->nBJets;
  int lepton = eventvar->leptonType;

  computeBDT_T(njets, nbjets, lepton, eventvar, combinations, eventNumber);

  return true; 

}


void RecoBDTCalculatorTool::computeBDT(int njets, int nbjets, int lepton,
				       TTHbb::Event* eventvar,
				       CombinationContainer* combinations,
				       unsigned long long eventNumber){


  computeBDT_T(njets, nbjets, lepton, eventvar, combinations, eventNumber);
}

template<class T>
void RecoBDTCalculatorTool::computeBDT_T(int njets, int nbjets, int lepton,
					 T* eventvar,
					 CombinationContainer* combinations,
					 unsigned long long eventNumber){


  std::vector<RegionDefinition> regions;
  getRegions(regions, njets, nbjets, lepton, eventvar);
  if(!regions.size()) return;

  for(auto r : regions){

    TMVAConfiguration tmvaconfig = r.tmvaConfiguration;

    for(auto comb : *combinations){
      
      for(unsigned int i=0; i<tmvaconfig.inputFloatVariables->size(); ++i){
	*(tmvaconfig.floatVariableHolder->at(i)) = comb->floatVariable(tmvaconfig.inputFloatVariables->at(i));
      }
      for(unsigned int i=0; i<tmvaconfig.inputIntVariables->size(); ++i){
	*(tmvaconfig.intVariableHolder->at(i)) = comb->intVariable(tmvaconfig.inputIntVariables->at(i));
      }

      double weight = tmvaconfig.getMVAOutput(eventNumber);
      comb->floatVariable(tmvaconfig.name) = weight;

    }
    

    sortCombinations(combinations, tmvaconfig.name);

    for(int i=0; i<m_nBestCombination; ++i){

      std::ostringstream ss;
      ss<<tmvaconfig.name<<"_best";
      if(i!=0){
	ss<<i;
      }
      eventvar->combinationVariable(ss.str())=combinations->at(i);

    }

    if(m_addBestBHiggsSwitchComb){

      auto bestcomb = combinations->at(0);

      int bestswitch=0;
      std::string bestswitchname="not found";
      

      
      int bs1 = findCombination(combinations,
				bestcomb->lepTop->childb,
				bestcomb->higgs->child2,
				bestcomb->higgs->child1,
				bestcomb->hadTop->childb,
				bestcomb->hadTop->childw->child1,
				bestcomb->hadTop->childw->child2);
      
      int bs2 = findCombination(combinations,
				bestcomb->hadTop->childb,
				bestcomb->higgs->child2,
				bestcomb->lepTop->childb,
				bestcomb->higgs->child1,
				bestcomb->hadTop->childw->child1,
				bestcomb->hadTop->childw->child2);

      
      int bs3 = findCombination(combinations,
				bestcomb->higgs->child1,
				bestcomb->lepTop->childb,
				bestcomb->higgs->child2,
				bestcomb->hadTop->childb,
				bestcomb->hadTop->childw->child1,
				bestcomb->hadTop->childw->child2);


      int bs4 = findCombination(combinations,
				bestcomb->higgs->child1,
				bestcomb->hadTop->childb,
				bestcomb->lepTop->childb,
				bestcomb->higgs->child2,
				bestcomb->hadTop->childw->child1,
				bestcomb->hadTop->childw->child2);

      if(bs1<=0){
	std::cout << "WARNING: RecoBDTCalculatorTool could not find b1<->lb switch comb"<< std::endl;
      }
      if(bs2<=0){
	std::cout << "WARNING: RecoBDTCalculatorTool could not find b1<->hb switch comb"<< std::endl;
      }
      if(bs3<=0){
	std::cout << "WARNING: RecoBDTCalculatorTool could not find b2<->lb switch comb"<< std::endl;
      }
      if(bs4<=0){
	std::cout << "WARNING: RecoBDTCalculatorTool could not find b2<->hb switch comb"<< std::endl;
      }

      bestswitch=bs1;
      //bestswitchname="b1<->lb";
      
      if(bs2<bestswitch && bs2>0){
	bestswitch=bs2;
	//bestswitchname="b1<->hb";
      }
      if(bs3<bestswitch && bs3>0){
	bestswitch=bs3;
	//bestswitchname="b2<->lb";
      }
      if(bs4<bestswitch && bs4>0){
	bestswitch=bs4;
	//bestswitchname="b2<->hb";
      }

      if(bestswitch<=0){
	std::cout << "WARNING: RecoBDTCalculatorTool could not find bestb switch comb"<< std::endl;
      }
      else{
	
	eventvar->combinationVariable(tmvaconfig.name+"_bestBSwitch")=combinations->at(bestswitch);
	//m_cuts->ocount("best B switch name is "+bestswitchname,100);
      }
  

    }
    
  }

}


void RecoBDTCalculatorTool::sortCombinations(CombinationContainer* combinations, std::string varname){

  for(unsigned int i=0; i<combinations->size(); ++i){
    bool ok=true;
    for(unsigned int j=0; j<combinations->size()-1; ++j){
      if(combinations->at(j)->floatVariable(varname)<combinations->at(j+1)->floatVariable(varname)){
	ok=false;
	combinations->swap(j,j+1);
      }
    }
    if(ok)break;
  }

}

int RecoBDTCalculatorTool::findCombination(CombinationContainer* combinations,
				       std::shared_ptr<Particle> bh1,
				       std::shared_ptr<Particle> bh2,
				       std::shared_ptr<Particle> ltb,
				       std::shared_ptr<Particle> htb,
				       std::shared_ptr<Particle> wj1,
				       std::shared_ptr<Particle> wj2){


  for(unsigned int i=0; i<combinations->size(); ++i){

    auto comb = combinations->at(i);

    /// assume we always create the higgs with the same jet order (b1,b2)==(b2,b1)
    /// same for w
    /// this the case with the way we do combinations now
    if(!compositeMatch(comb->higgs, bh1, bh2) ) continue;
    if(comb->lepTop->childb->id != ltb->id) continue;
    if(comb->hadTop->childb->id != htb->id) continue;
    if(!compositeMatch(comb->hadTop->childw, wj1, wj2) ) continue;

    return i;

  }


  return -1;

}

bool RecoBDTCalculatorTool::compositeMatch(std::shared_ptr<Composite> comp, 
					   std::shared_ptr<Particle> bh1,
					   std::shared_ptr<Particle> bh2){

  if(comp->child1->id==bh1->id && comp->child2->id==bh2->id) return true;
  if(comp->child1->id==bh2->id && comp->child2->id==bh1->id) return true;

  return false;

}


