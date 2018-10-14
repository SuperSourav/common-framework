#ifndef _OFFLINEDILEPTONVARIABLESDICTIONARY_H_
#define _OFFLINEDILEPTONVARIABLESDICTIONARY_H_

#include "TTHbbToolManager/DictBase.h"

#include <string>
#include <utility>
#include <map>
#include <functional>

namespace TTHbb{

  class OfflineDileptonVariablesDictionary : public DictBase{
    /**                                                                                                                                                                                                                                                                                                                                            
     * Implementation of the TTHbb::Dictbase abstract base class                                                                                                                                                                                                                                                                                   
     * Tools should inherit from this to be useable in the OfflineTTHbb code.                                                                                                                                                                                                                                                                      
     * In an inherited class, the functions all need to be defined and                                                                                                                                                                                                                                                                             
     * the m_name property should be set in the constructor.                                                                                                                                                                                                                                                                                       
     **/
  public:
    /// Default constructor                                                                                                                                                                                                                                                                                                                        
    OfflineDileptonVariablesDictionary(){};

    ~OfflineDileptonVariablesDictionary(){};

    void buildDictionary(TTHbb::ToolManager * toolManager);


    ClassDef(TTHbb::OfflineDileptonVariablesDictionary, 0);
  };
}

#endif
