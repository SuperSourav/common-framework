/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _DICTBASE_H_
#define _DICTBASE_H_

#include "TTHbbObjects/Event.h"
#include "TTHbbToolManager/ToolManager.h"

#include <string>
#include <utility>
#include <map>
#include <functional>

namespace TTHbb{

  class DictBase{
    /**
     * Implementation of the TTHbb::Dictbase abstract base class
     * Tools should inherit from this to be useable in the OfflineTTHbb code.
     * In an inherited class, the functions all need to be defined and
     * the m_name property should be set in the constructor.
     **/
  public:
    /// Default constructor
    DictBase(){};

    virtual ~DictBase(){};

    virtual void buildDictionary(TTHbb::ToolManager * toolManager) = 0;

  };
}
#endif
