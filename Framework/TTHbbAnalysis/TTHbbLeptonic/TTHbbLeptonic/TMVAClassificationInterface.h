/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TMVAClassificationInterface_HH
#define TMVAClassificationInterface_HH

#include "TTHbbObjects/Event.h"
#include "TTHbbClassification/ClassBDTCalculatorTool.h"
#include <string>
#include <vector>


class TMVAClassificationInterface{

public:

  TMVAClassificationInterface();
  virtual ~TMVAClassificationInterface();



  protected:


  virtual void defaultValues(TTHbb::Event* tthevt) const;
  virtual bool passRegion(TTHbb::Event* eventvar) const = 0;

  virtual void initTool();
  virtual void initReaders(std::string xml);
  virtual void registerSaverVariables();

  virtual void getBDTVars(std::vector<std::string> &Vars) = 0;

  mutable ClassBDTCalculatorTool m_classBDTCalculatorTool;
  std::string m_params;
  std::string m_xmlPath;

  std::unordered_map<std::string,std::string> m_configMap;
  std::string m_strategy;
  std::string m_btagOP;
  std::string m_trf;
  std::string m_flag;
  std::string m_region;
  std::string m_xmlFlag;
  std::string m_varOption;

};

#endif
