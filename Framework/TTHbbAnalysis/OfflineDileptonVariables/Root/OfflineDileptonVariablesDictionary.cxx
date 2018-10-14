#include "OfflineDileptonVariables/OfflineDileptonVariablesDictionary.h"
#include "OfflineDileptonVariables/DilepVars.h"
#include "OfflineDileptonVariables/DilepRegions.h"
//#include "OfflineDileptonVariables/pp6reweighting.h"
#include "OfflineDileptonVariables/DileptonClassBDT.h"

void TTHbb::OfflineDileptonVariablesDictionary::buildDictionary(TTHbb::ToolManager * toolManager){
 
   toolManager->addToolToDict("ComputeDilepVars",
                             "DilepVars",
                             []{ return new DilepVars();}
                             );
   
   toolManager->addToolToDict("AddDilepRegions",
			      "DilepRegions",
			      []{ return new DilepRegions();}
			      );
   /*
   toolManager->addToolToDict("ComputePP6ttbarWeights",
			      "pp6reweighter",
			      []{ return new pp6reweighting();}
			      );
   */			      
   toolManager->addToolToDict("DileptonClassBDT.TurnOn",
			      "DileptonClassBDT",
			      []{ return new DileptonClassBDT();}
			       );
}
