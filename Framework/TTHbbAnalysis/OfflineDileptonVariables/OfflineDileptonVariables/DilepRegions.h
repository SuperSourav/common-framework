#ifndef _TTHBB_DILEPREGIONS_H_
#define _TTHBB_DILEPREGIONS_H_

#include "TTHbbToolManager/ToolBase.h"
#include "TTHbbObjects/Event.h"
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "TH2F.h"


namespace TTHbb {

  class DilepRegions : public ToolBase {

  public:
    DilepRegions(){};
    ~DilepRegions(){};

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();

  private:
    std::map<std::string, TH2F> regionhists;

    std::pair<int,int> getBin(std::vector<std::shared_ptr<TTHbb::Jet> > & jets);

  };
}

#endif
