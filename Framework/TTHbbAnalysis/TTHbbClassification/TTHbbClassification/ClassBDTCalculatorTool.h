/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ClassBDTCalculatorTool_HH
#define ClassBDTCalculatorTool_HH

#include "TTHbbMVAInterfaces/RegionInterface.h"

#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbObjects/Containers.h"

using namespace TTHbb;

class ClassBDTCalculatorTool : public RegionInterface{

public:

  ClassBDTCalculatorTool();
  virtual ~ClassBDTCalculatorTool();


  bool computeBDT(BaseObject* eventvar, 
		  int njets,
		  int nbjets,
		  int leptonType,
		  unsigned long long eventNumber);

  bool computeBDTMultiClass(BaseObject* eventvar,
      int njets,
      int nbjets,
      int leptonType,
      unsigned long long eventNumber);

  //// ... add more config
  bool m_checkVariables;

  private:


};

#endif
