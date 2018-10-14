/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTHBBLEPTONICOBJECTLOADER_H_
#define TTHBBLEPTONICOBJECTLOADER_H_
#include "TopAnalysis/ObjectLoaderBase.h"

/**
 * class that can be loaded by name at run time that creates object selection
 */

class TTHbbLeptonicObjectLoader : public top::ObjectLoaderBase{
  
 public:
  // a method that creates a pointer to a TopObjectSelection tool which
  // contains information on which cuts to enable and configure
  top::TopObjectSelection* init(std::shared_ptr<top::TopConfig> topConfig);
  // ROOT
  ClassDef(TTHbbLeptonicObjectLoader, 0)
};

#endif
