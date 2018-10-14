#ifndef _TTHBB_DILEPVARS_H_
#define _TTHBB_DILEPVARS_H_

#include "TTHbbToolManager/ToolBase.h"
#include <string>
#include <vector>

namespace TTHbb {

  class DilepVars : public ToolBase {

  public:
    DilepVars();                                                                                                                                                                                                                                                                                                         
    ~DilepVars(){};

    void initialise(){};
    void apply(TTHbb::Event* event);
    void finalise(){};

  private:
    std::vector<std::string> wps;

    // For returning the integer b-tag level of each jet
    int getTagLevel(TTHbb::Event* event, std::size_t ijet);

    // For storing the b-tag levels of the leading 4 jets
    std::vector<int> bTagLevels;

  };
}

#endif
