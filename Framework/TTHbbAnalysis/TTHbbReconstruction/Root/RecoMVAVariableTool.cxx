/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbReconstruction/RecoMVAVariableTool.h"


RecoMVAVariableTool::RecoMVAVariableTool(){

  m_combClassName="";
  m_nCalcComb=0;
  m_calcFlag="";
  m_nDiffComb=0;
  /// internally computed

  m_isMC=false; 

}

RecoMVAVariableTool::~RecoMVAVariableTool(){
  ////
}


bool RecoMVAVariableTool::fillRecoVariables(RecoEvent* eventvar, CombinationContainer* combinations){


 for(auto a : m_selectedCombinationVars){
    
    std::string combname = a.combinationName;
    std::shared_ptr<Combination> comb = eventvar->combinationVariable(combname);

    if(!comb){
      std::cout << "ERROR: RecoMVAVariableTool could not find combination " << combname << " in eventvar " << eventvar->name << std::endl;
      return false;
    }
    fillRecoVariables(eventvar, comb, a);
 }

 fillRecoCombinations(eventvar, combinations);
 return true;

}

bool RecoMVAVariableTool::fillRecoVariables(TTHbb::Event* eventvar, CombinationContainer* combinations){

  for(auto a : m_selectedCombinationVars){
    
    std::string combname = a.combinationName;
    std::shared_ptr<Combination> comb = eventvar->combinationVariable(combname);

    if(!comb){
      std::cout << "ERROR: RecoMVAVariableTool could not find combination " << combname << std::endl;
      return false;
    }
    fillRecoVariables(eventvar, comb, a);
 }

 fillRecoCombinations(eventvar, combinations);

 return true;

}

void RecoMVAVariableTool::fillRecoVariables(BaseObject* eventvar, std::shared_ptr<Combination> comb, CombinationVarConfig& combConf){


  std::string combname = combConf.combinationName;
  /// add all when empty
  if(!combConf.floatVarNames.size()){
    for(auto b : comb->variableFloatList()){
      eventvar->floatVariable(combname+"_"+b.first)=b.second;
    }
  }
  if(!combConf.intVarNames.size()){
    for(auto b : comb->variableIntList()){
      eventvar->intVariable(combname+"_"+b.first)=b.second;
    }
  }

  for(auto b : combConf.floatVarNames){
    if(""==b) continue;
    eventvar->floatVariable(combname+"_"+b)=comb->floatVariable(b);
  }

  for(auto b : combConf.intVarNames){
    if(""==b) continue;
    eventvar->intVariable(combname+"_"+b)=comb->intVariable(b);
  }

  // if(m_isMC){ /// add it even if not MC (will be 0)
  eventvar->intVariable(combname+"_truthMatchPattern") = comb->truthMatchPattern;

}

void RecoMVAVariableTool::fillRecoCombinations(BaseObject* eventvar, CombinationContainer* combinations){


  if(!combinations) return;

 
  eventvar->floatVariable(m_calcFlag+"_diff0_last")=combinations->at(0)->floatVariable(m_combClassName)-
    combinations->at(combinations->size()-1)->floatVariable(m_combClassName);


  unsigned int ncdloop = (combinations->size()>m_nDiffComb)?m_nDiffComb:combinations->size();
  for(unsigned int i=1; i<ncdloop; ++i){
    float val1 = combinations->at(i)->floatVariable(m_combClassName);
    float val2 = combinations->at(i-1)->floatVariable(m_combClassName);
    float diff = val2-val1;

    std::ostringstream ss;
    ss<<m_calcFlag <<"_order"<<i;
    std::ostringstream ss1;
    ss1<<m_calcFlag <<"_diff"<<i-1<<"_"<<i;

    eventvar->floatVariable(ss.str())=val1;
    eventvar->floatVariable(ss1.str())=diff;

    std::ostringstream ss2;
    ss2<<m_calcFlag <<"_diff0"<<"_"<<i;
    eventvar->floatVariable(ss2.str())=combinations->at(0)->floatVariable(m_combClassName)-val1;
    

  }

  if(m_nCalcComb<=0) return;

 
  //// move to config
  unsigned int ncloop = (combinations->size()>m_nCalcComb)?m_nCalcComb:combinations->size();


  float sum=0;
  float sum2=0;
  for(unsigned int i=0; i<ncloop; ++i){
    float val = combinations->at(i)->floatVariable(m_combClassName);
    sum+=val;
    sum2+=(val*val);
  }
  float mean=sum/ncloop;

  float rms=sqrt(sum2/ncloop - mean*mean);

  
  eventvar->floatVariable(m_calcFlag+"_mean")=mean;
  eventvar->floatVariable(m_calcFlag+"_rms")=rms;


}


void RecoMVAVariableTool::addCombinationVariables(std::string name, std::vector<std::string> floatvar, std::vector<std::string> intvar){

  CombinationVarConfig a;
  a.combinationName=name;
  a.floatVarNames=floatvar;
  a.intVarNames=intvar;

  m_selectedCombinationVars.push_back(a);

}
