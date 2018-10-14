#ifndef _OFFSM4TOPSTOOLDICTIONARY_H_
#define _OFFSM4TOPSTOOLDICTIONARY_H_

#include "TTHbbToolManager/DictBase.h"

#include <string>
#include <utility>
#include <map>
#include <functional>

namespace TTHbb{

  class OffSM4topsToolDictionary : public DictBase{
    /**
     * Implementation of the TTHbb::Dictbase abstract base class
     * Tools should inherit from this to be useable in the OfflineTTHbb code.
     * In an inherited class, the functions all need to be defined and
     * the m_name property should be set in the constructor.
     **/
  public:
    /// Default constructor
    OffSM4topsToolDictionary(){};
    ~OffSM4topsToolDictionary(){};

    void buildDictionary(TTHbb::ToolManager * toolManager);

    ClassDef(TTHbb::OffSM4topsToolDictionary, 0);
  };
}

#endif
