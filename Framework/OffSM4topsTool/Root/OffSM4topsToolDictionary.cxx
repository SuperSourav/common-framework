#include "OffSM4topsTool/OffSM4topsToolDictionary.h"
#include "OffSM4topsTool/Add1LOSVar.h"

void TTHbb::OffSM4topsToolDictionary::buildDictionary(TTHbb::ToolManager * toolManager){
 
   toolManager->addToolToDict("Add1LOSVar", "Add1LOSVar",
			      []{ return new Add1LOSVar();});

}
