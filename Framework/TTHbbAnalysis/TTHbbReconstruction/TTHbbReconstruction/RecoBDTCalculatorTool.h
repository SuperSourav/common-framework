/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RecoBDTCalculatorTool_HH
#define RecoBDTCalculatorTool_HH

#include "TTHbbMVAInterfaces/RegionInterface.h"

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/RecoObjects.h"
#include "TTHbbObjects/Containers.h"

using namespace TTHbb;

class RecoBDTCalculatorTool : public RegionInterface{

public:

  RecoBDTCalculatorTool();
  virtual ~RecoBDTCalculatorTool();


  bool computeBDT(RecoEvent* eventvar, CombinationContainer* combinations, unsigned long long eventNumber);

  void computeBDT(int njets, int nbjets, int lepton,
		  TTHbb::Event* eventvar,
		  CombinationContainer* combinations,
		  unsigned long long eventNumber);

  std::string m_nBestCombination;
  bool m_addBestBHiggsSwitchComb;

  //// ... add more config 

  private:

  void sortCombinations(CombinationContainer* combinations, std::string varname);

  int findCombination(CombinationContainer* combinations,
		      std::shared_ptr<Particle> bh1,
		      std::shared_ptr<Particle> bh2,
		      std::shared_ptr<Particle> ltb,
		      std::shared_ptr<Particle> htb,
		      std::shared_ptr<Particle> wj1,
		      std::shared_ptr<Particle> wj2);


  bool compositeMatch(std::shared_ptr<Composite> comp, 
		      std::shared_ptr<Particle> bh1,
		      std::shared_ptr<Particle> bh2);


  /// allows me to implement in cxx
  template<class T>
  void computeBDT_T(int njets, int nbjets, int lepton,
		    T* eventvar,
		    CombinationContainer* combinations,
		    unsigned long long eventNumber);
};

#endif
