/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDVARIABLES_BOOSTEDVARIABLES_H
#define BOOSTEDVARIABLES_BOOSTEDVARIABLES_H

#include <string>
#include <iostream>
#include <vector>

#include "BoostedVariables/ClassBDT.h"

namespace TTHbb {

class Event;

class BoostedVariables {

public:
  BoostedVariables();
  ~BoostedVariables(){};

  void initialise();
  void apply(TTHbb::Event* event);
  void finalise();

  TMVA::Reader *reader_reg5_EvenOdd, *reader_reg5_OddEven;

private:

  ClassBDT *m_ClassifyBDT;

  int m_nPass3211;
  int m_nPass1T1H;
  int m_nPassReclustered1T1H;
  std::vector<int> m_passesCut3211;
  std::vector<int> m_passesCut1T1H;
  std::vector<int> m_passesCutReclustered1T1H;
  void apply3211(TTHbb::Event* event);
  void apply1T1H(TTHbb::Event* event);
  void applyReclustered1T1H(TTHbb::Event* event, std::string prefix, int requireTopBVeto);

};

} // namespace TTHbb

#endif//BOOSTEDVARIABLES_BOOSTEDVARIABLES_H
