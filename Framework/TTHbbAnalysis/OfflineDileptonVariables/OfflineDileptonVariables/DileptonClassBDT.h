#ifndef _TTHBB_LJETSCLASSBDT_H_
#define _TTHBB_LJETSCLASSBDT_H_

#include "TTHbbToolManager/ToolBase.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbClassification/ClassBDTCalculatorTool.h"
#include "TTHbbMVAInterfaces/TMVAConfiguration.h"

namespace TTHbb {

  class DileptonClassBDT : public ToolBase {

  public:

    struct BDTContainer {
        std::string outputName;
        std::string XMLBaseFileName;
        std::string regionFlag;
        std::vector<std::string> vars;
    };


    DileptonClassBDT();                                                                                                                                                                                                      
    ~DileptonClassBDT();

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();

  private:
    
    EventSampling initEventSampling(unsigned int modulo, unsigned int rest, std::string pattern);
    void fillBDTMap(std::vector<std::string> parameterString);
    void printBDTInfo();
    void defaultValues(TTHbb::Event* event) const;
    
    std::string m_xmlPath;
    std::map<std::string, BDTContainer> m_mapBDTs;
    mutable ClassBDTCalculatorTool m_classBDTCalculatorTool;

  };

}

#endif

