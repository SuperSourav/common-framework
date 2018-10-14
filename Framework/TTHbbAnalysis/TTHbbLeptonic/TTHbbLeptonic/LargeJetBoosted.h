/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEJETBOOSTED_H_
#define LARGEJETBOOSTED_H_
 
#include "TopObjectSelectionTools/JetSelectionBase.h"
#include "TTHbbLeptonic/BoostedTools.h"
 
class LargeJetBoosted : public top::JetSelectionBase {
 public:
  LargeJetBoosted(double ptcut, double etamax);

  ~LargeJetBoosted();

  virtual bool passSelection(const xAOD::Jet& jet) override;

  virtual void print(std::ostream& os) const override;

 private:
  double m_ptcut;
  BoostedTools* m_bvariables;
  double m_etamax;


};

#endif
