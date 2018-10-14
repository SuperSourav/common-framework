#ifndef _TTHBB_TTTRF_H_
#define _TTHBB_TTTRF_H_

#include "TTHbbToolManager/ToolBase.h"
#include "TtTRF/TtTRFapplication.h"

namespace TTHbb {

  class TtTRF : public ToolBase{

  public:
    TtTRF(std::string name="");
    ~TtTRF(){};

    void initialise(){};
    void apply(TTHbb::Event* event);
    void finalise(){};
    
  private:

    TtTRFapplication *m_ttTRFapp,*m_ttTRFapp_0J,*m_ttTRFapp_ge1J;
  };
}

#endif 
