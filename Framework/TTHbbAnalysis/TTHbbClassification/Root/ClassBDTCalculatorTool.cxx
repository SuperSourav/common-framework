/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbClassification/ClassBDTCalculatorTool.h"


ClassBDTCalculatorTool::ClassBDTCalculatorTool() : RegionInterface(){
  m_checkVariables=true;
}

ClassBDTCalculatorTool::~ClassBDTCalculatorTool(){
  ////
}


bool ClassBDTCalculatorTool::computeBDT(BaseObject* eventvar,
					int njets, int nbjets, int leptonType,
					unsigned long long eventNumber){



  std::vector<RegionDefinition> regions;
  getRegions(regions, njets, nbjets, leptonType, eventvar);
  if(!regions.size()) return true;

  for(auto r : regions){

    TMVAConfiguration tmvaconfig = r.tmvaConfiguration;

    for(unsigned int i=0; i<tmvaconfig.inputFloatVariables->size(); ++i){
      if(m_checkVariables){
	if(!eventvar->checkFloatVariable(tmvaconfig.inputFloatVariables->at(i))){
	  std::cout << "WARNING: ClassBDTCalculatorTool float variable " 
		    << tmvaconfig.inputFloatVariables->at(i) << " not found" << std::endl;
	}
      }
      *(tmvaconfig.floatVariableHolder->at(i)) = eventvar->floatVariable(tmvaconfig.inputFloatVariables->at(i));
    }
    for(unsigned int i=0; i<tmvaconfig.inputIntVariables->size(); ++i){
      if(m_checkVariables){
	if(!eventvar->checkIntVariable(tmvaconfig.inputIntVariables->at(i))){
	  std::cout << "WARNING: ClassBDTCalculatorTool int variable " 
		    << tmvaconfig.inputIntVariables->at(i) << " not found" << std::endl;
	}
      }
      *(tmvaconfig.intVariableHolder->at(i)) = eventvar->intVariable(tmvaconfig.inputIntVariables->at(i));
    }

    double weight = tmvaconfig.getMVAOutput(eventNumber);
    
    eventvar->floatVariable(tmvaconfig.name)=weight;
     
  }

  return true; 

}

bool ClassBDTCalculatorTool::computeBDTMultiClass(BaseObject* eventvar,
          int njets, int nbjets, int leptonType,
          unsigned long long eventNumber){
  std::vector<RegionDefinition> regions;
  getRegions(regions, njets, nbjets, leptonType, eventvar);
  if(!regions.size()) return true;

  for(auto r : regions){

    TMVAConfiguration tmvaconfig = r.tmvaConfiguration;

    for(unsigned int i=0; i<tmvaconfig.inputFloatVariables->size(); ++i){
      if(m_checkVariables){
  if(!eventvar->checkFloatVariable(tmvaconfig.inputFloatVariables->at(i))){
    std::cout << "WARNING: ClassBDTCalculatorTool float variable "
        << tmvaconfig.inputFloatVariables->at(i) << " not found" << std::endl;
  }
      }
      *(tmvaconfig.floatVariableHolder->at(i)) = eventvar->floatVariable(tmvaconfig.inputFloatVariables->at(i));
    }
    for(unsigned int i=0; i<tmvaconfig.inputIntVariables->size(); ++i){
      if(m_checkVariables){
  if(!eventvar->checkIntVariable(tmvaconfig.inputIntVariables->at(i))){
    std::cout << "WARNING: ClassBDTCalculatorTool int variable "
        << tmvaconfig.inputIntVariables->at(i) << " not found" << std::endl;
  }
      }
      *(tmvaconfig.intVariableHolder->at(i)) = eventvar->intVariable(tmvaconfig.inputIntVariables->at(i));
    }
    // writing the output to file
    std::vector<float> weight = tmvaconfig.getMVAOutputMultiClass(eventNumber);

    for (unsigned int i = 0; i < weight.size(); i++) {
        std::string this_class(tmvaconfig.multiclass_name.at(i));
        eventvar->floatVariable(this_class) = weight.at(i);
    }
  }
  return true;
}

