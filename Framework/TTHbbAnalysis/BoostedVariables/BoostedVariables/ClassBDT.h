/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLASSBDT_H_
#define CLASSBDT_H_

#include <memory>
#include <vector>
#include <map>
#include <string>

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include "Riostream.h"
#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include <TList.h>



class ClassBDT{

 public:
  
 ClassBDT();
  ~ClassBDT();
  int region;
  std::string score_name;
  int index;
  float event_score(std::vector<float> varsToClass_float_values,UInt_t eventNumber);
  void setRegion(int Njet, int NBtag);
  void setRegion(int regionFlag);
  std::vector<std::string> varsToClass_reg5;
  void defineVariables(TMVA::Reader *reader,std::vector<std::string> varsToClass);
  void parseVariablesXml(std::string pathToXml,std::vector<std::string> &varsToClass);
  void ParseContext(TXMLNode *node,std::vector<std::string> &varsToClass);
  const std::vector<std::string> returnVarsToClass(); 
  TMVA::Reader *reader_reg5_EvenOdd, *reader_reg5_OddEven;
  Float_t target_var[100]; //need something dynamic
  void loadModel(std::string pathToXml);
  void set_all_variables(std::vector<float> varsToClass_float_values);
  void add_spectator(std::string spec_name, float spec);
  void set_weight(std::string weight);
  void setScoreName(std::string name);
  std::string getScoreName();
  //some of these should be private
 
};
#endif
