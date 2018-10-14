/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/Event.h"

#ifndef BOOSTEDTOOLS_H_
#define BOOSTEDTOOLS_H_

class BoostedTools{

 public:
  
  BoostedTools();
  ~BoostedTools();
  void initialise(const xAOD::Jet &jet);
  float Tau21_wta();
  float Tau32_wta();
  float D2();
  float C2();
  bool TopTag();
  bool BosonTag();
  bool TopTag_loose();
  bool BosonTag_loose();

 private:

  const xAOD::Jet* m_boostedjet;

};

#endif
