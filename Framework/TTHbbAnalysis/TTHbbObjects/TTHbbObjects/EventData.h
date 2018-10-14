/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _EVENTDATA_H_
#define _EVENTDATA_H_

#include "TTHbbObjects/BaseObject.h"

#include <string>
#include <vector>

namespace TTHbb{

  class EventData : virtual public BaseObject{
    /**
     * @brief EventData class for holding meta data.
     */

  public:
    EventData(){};
    virtual ~EventData(){};
    
    /// Metadata variables
    ULong64_t eventNumber;
    UInt_t mcChannelNumber;
    UInt_t runNumber;

    bool isData;

    /// Current systematic variation
    std::string currentSyst;  

    /// Event normalisation weight
    /// Defined as xsec * kfactor / sumOfWeights
    float eventNormWeight;
    /// MC event weights
    std::vector<std::string>* eventMCNormName = new std::vector<std::string>;
    std::vector<float>* eventMCNormWeight = new std::vector<float>;

  };
}

#endif
