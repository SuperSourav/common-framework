/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _OFFLINETRFFAKES_H_
#define _OFFLINETRFFAKES_H_
 
#include "TTHbbToolManager/ToolBase.h"
#include "MVAVariables/MVAVariables.h"
#include "TRFFakes/TRFFakesTool.h"
 
#include <memory>

namespace TTHbb{
   
    class OfflineTRFFakes : public ToolBase{

      public:
        OfflineTRFFakes(std::string name);
        ~OfflineTRFFakes();
        void initialise();
        void apply(TTHbb::Event* event);
        void finalise();

      private:
        std::map<std::string, std::shared_ptr<TRFFakesTool> > m_mapOfTRFFakes;

        std::vector<std::string> m_btagWPs;
        std::vector<std::string> m_parametrizations;
        std::vector<std::string> m_extrapRegions;
        std::vector<std::string> m_bTagHypotheses;
        std::string              m_effmapPath;
    };
}
#endif
