/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef H4BOBJECTLOADER_H_
#define H4BOBJECTLOADER_H_
#include "TopAnalysis/ObjectLoaderBase.h"

/**
 * class that can be loaded by name at run time that creates object selection
 */

namespace H4b {

  class H4bObjectLoaderStandardCuts : public top::ObjectLoaderBase{

    public:
      // a method that creates a pointer to a TopObjectSelection tool which
      // contains information on which cuts to enable and configure
      top::TopObjectSelection* init(std::shared_ptr<top::TopConfig> topConfig);
      // ROOT
      ClassDef(H4b::H4bObjectLoaderStandardCuts, 0)
  };

} // namespace H4b

#endif
