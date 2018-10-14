/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbMVAInterfaces/TMVAConfiguration.h"


EventSampling::EventSampling(){
  modulo=1;
  patternName="unset";
}

bool EventSampling::passSample(unsigned long long eventNumber){

  if(1==modulo) return true;
  
  for(unsigned int a : rest){
    if(eventNumber%modulo == a) return true;
  }

  return false;

}



TMVAConfiguration::TMVAConfiguration(){
  name="unset";  
  m_calibFilePattern="unset";

  inputFloatVariables = std::make_shared<std::vector<std::string> >();
  inputIntVariables = std::make_shared<std::vector<std::string> >();
  intVariableHolder = std::make_shared<std::vector<float*> >();
  floatVariableHolder = std::make_shared<std::vector<float*> >();
  m_allVariableHolder = std::make_shared<std::vector<float*> >();
  reader = std::make_shared<TMVA::Reader>("silent");
}

TMVAConfiguration::~TMVAConfiguration(){
}

void TMVAConfiguration::bookReader(){

  intVariableHolder->resize(inputIntVariables->size(),0);
  floatVariableHolder->resize(inputFloatVariables->size(),0);

  // for(unsigned int i=0;i<floatVariableHolder->size(); ++i){
  //   reader->AddVariable((*inputFloatVariables)[i],&((*floatVariableHolder)[i]));
  // }
  // for(unsigned int i=0;i<intVariableHolder->size(); ++i){
  //   reader->AddVariable((*inputIntVariables)[i],&((*intVariableHolder)[i]));
  // }
  //// never resize,copy,add,remove,clear holder vectors after this line otherwise loose the reference
  //// just change values with [] operator

  for(unsigned int i=0; i<m_allVariables.size(); ++i){
    char type;
    std::string name;
    std::string trainName;

    decodeVariableName(m_allVariables[i], type, name, trainName);
    float* var = new float;
    m_allVariableHolder->push_back(var);
    if('I'==type){
      intVariableHolder->push_back(var);
      inputIntVariables->push_back(name);
    }
    else if('F'==type){
      floatVariableHolder->push_back(var);
      inputFloatVariables->push_back(name);
    }
    else{
      std::cout << "ERROR: TMVAConfiguration::bookReader unkown type " << type << std::endl;
    }
    reader->AddVariable(trainName,(*m_allVariableHolder)[i]);
  }

  for(auto a : samplings){
    reader->BookMVA( (name+"__"+a.patternName).c_str(), (m_calibFilePattern+"_"+a.patternName+".weights.xml").c_str());
  }

}

double TMVAConfiguration::getMVAOutput(unsigned long long eventNumber){

  std::vector<double> valueHolder;

  for(auto a : samplings){
    if(a.passSample(eventNumber)){
      valueHolder.push_back(reader->EvaluateMVA( (name+"__"+a.patternName).c_str() ));
    }
  }

  if(valueHolder.size()!=1){
    std::cout << "WARNING: TMVAConfiguration::getMVAOutput multiple or no definition for output of " <<name << " size = " << valueHolder.size() << " event number " << eventNumber<<std::endl;
    return -1;
  }

  return valueHolder[0];

}


void TMVAConfiguration::decodeVariableName(std::string& varconfig, char& type, std::string& name, std::string& trainName){


  type='F';
  if(varconfig.size()<2){
    name=varconfig;
    trainName=varconfig;
    return;
  }

  std::string names=varconfig;
  if(varconfig.at(varconfig.size()-2) == '/'){
    type=varconfig.at(varconfig.size()-1);
    names = varconfig.substr(0,varconfig.size()-2);
  }

  std::size_t found = names.find(":");
  if(found!=std::string::npos){
    name=names.substr(0,found);
    trainName=names.substr(found+1,names.size()-found);
  }
  else{
    name=names;
    trainName=names;
  }

}

void TMVAConfiguration::setMultiClassName(std::vector<std::string> names){
  multiclass_name = names;
}

std::vector<float> TMVAConfiguration::getMVAOutputMultiClass(unsigned long long eventNumber){
  std::vector<float> valueHolder;
  for(auto a : samplings){
    if(a.passSample(eventNumber)){
      valueHolder = reader->EvaluateMulticlass( (name+"__"+a.patternName).c_str() );
    }
  }
  return valueHolder;
}
