/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GLOBALCONFIGURATION_H_
#define _GLOBALCONFIGURATION_H_

#include "TTHbbConfiguration/Configuration.h"

namespace TTHbb{

  class GlobalConfiguration : public Configuration{
    /**
     * @brief Reimplementation of TTHbb::Configuration with a singleton
     */

    
  public:
    /// Singleton access function
    /// Will return the pointer the the instance if already created, otherwise creates it
    static GlobalConfiguration* get();

    virtual ~GlobalConfiguration();

  private:
    GlobalConfiguration();
    /// Singleton
    static GlobalConfiguration* m_instance;

  };


}

#endif
