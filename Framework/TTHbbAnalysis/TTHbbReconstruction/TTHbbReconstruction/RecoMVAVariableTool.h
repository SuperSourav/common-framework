/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RecoMVAVariableTool_HH
#define RecoMVAVariableTool_HH

#include <sstream>

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbObjects/Containers.h"
using namespace TTHbb;

class RecoMVAVariableTool{

public:

  explicit RecoMVAVariableTool();
  virtual ~RecoMVAVariableTool();



  std::string m_combClassName;
  unsigned int m_nCalcComb;
  std::string m_calcFlag;
  unsigned int m_nDiffComb;
  bool m_isMC; 

  void addCombinationVariables(std::string name, std::vector<std::string> floatvar, std::vector<std::string> intvar);
  bool fillRecoVariables(RecoEvent* eventvar, CombinationContainer* combinations);
  bool fillRecoVariables(TTHbb::Event* eventvar, CombinationContainer* combinations);


  protected:

  struct CombinationVarConfig{
    std::string combinationName;
    std::vector<std::string> floatVarNames;
    std::vector<std::string> intVarNames;
  };

  std::vector<CombinationVarConfig> m_selectedCombinationVars;

  void fillRecoVariables(BaseObject* eventvar, std::shared_ptr<Combination> comb, CombinationVarConfig& combConf);
  void fillRecoCombinations(BaseObject* eventvar, CombinationContainer* combinations);

};

#endif
