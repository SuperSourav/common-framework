/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TMVAConfiguration_HH
#define TMVAConfiguration_HH

#include <TMVA/Reader.h>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

class EventSampling{

public:

  EventSampling();

  unsigned int modulo;
  std::vector<unsigned int> rest;
  std::string patternName;

  bool passSample(unsigned long long eventNumber);

};



class TMVAConfiguration{

public:

  TMVAConfiguration();
  ~TMVAConfiguration();
  void bookReader();
  double getMVAOutput(unsigned long long eventNumber);
  std::vector<float> getMVAOutputMultiClass(unsigned long long eventNumber);
  void setVariables(const std::vector<std::string>& vars){m_allVariables=vars;}
  void setMultiClassName(std::vector<std::string> names);

  std::string name;
  std::vector<std::string> multiclass_name;
  /// should be exclusive and cover the full sample, not checked
  std::vector<EventSampling> samplings;
  std::shared_ptr<std::vector<std::string> > inputFloatVariables;
  std::shared_ptr<std::vector<std::string> > inputIntVariables;
  inline void setCalibFilePattern(std::string c){ m_calibFilePattern=c;} 
  //// these should be pointer to conserve address while moving arround.
  std::shared_ptr<std::vector<float*> > intVariableHolder; //// reader takes only floats even for int variables
  std::shared_ptr<std::vector<float*> > floatVariableHolder;

private:
  std::string m_calibFilePattern;
  std::shared_ptr<TMVA::Reader> reader;
  std::vector<std::string> m_allVariables;
  std::shared_ptr<std::vector<float*> > m_allVariableHolder;  

  void decodeVariableName(std::string& varconfig, char& type, std::string& name, std::string& trainName);

};


#endif
