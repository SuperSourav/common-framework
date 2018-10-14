/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBB_TOOLDICTIONARY_H_
#define _TTHBB_TOOLDICTIONARY_H_

#include "TTHbbToolManager/ToolManager.h"

namespace TTHbb{

  class ToolDictionary{
    /**
     * @brief A class to interface with TTHbbToolManager.
     * It makes sense to move the adding of tools to the dictionary here
     * Fewer packages for the user to modify when adding their own tool
     **/
  public:
    /// Constructor, does nothing
    ToolDictionary();

    /// Destructor, does nothing
    ~ToolDictionary();

    /// Function for adding all the tools to the dictionary in TTHbb::ToolManager
    /// Moved to separate class to avoid circular dependencies
    /// Also means that with a little poking around the user can have their own dictionary, which may be an area of development in future
    /// Means that there isn't a large block of text adding tools in the main exe
    /// @param toolManager The tool manager to add the dictionary too
    /// This supports the idea that a ToolManager may not always be a singleton
    void buildDictionary(TTHbb::ToolManager* toolManager);

  };

}

#endif
