#ifndef ToolBox_H_
#define ToolBox_H_

#include <AsgTools/MessageCheck.h>
#include <vector>
#include <string>

#include "TopConfiguration/TopConfig.h"


using namespace asg::msgUserCode;
using namespace std;

namespace SM4topsCommon {
class ToolBox {

 public:
   static ToolBox* get();
   ~ToolBox () = default;

   vector<string> getSLTriggerNames(const vector<string> &allTrigNames) const;
   vector<string> getDLTriggerNames(const vector<string>& allTrigNames) const;
   vector<string> getUniqueTriggerNamesAll(std::shared_ptr<top::TopConfig> conf, const string& selectorName) const;
   vector<string> getElectronTriggerNames(const vector<string>& allTrigNames) const;
   vector<string> getMuonTriggerNames(const vector<string>& allTrigNames) const;
   bool isDileptonTrigger(const string &triggerName) const;



 private:
   // Let's make this a singleton
   static ToolBox* m_instance;
   ToolBox();
  };
}

#endif
