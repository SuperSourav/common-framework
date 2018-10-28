#include "BucketofTops/BucketofTops.h"
#include "BucketofTops/Bucket.h"
#include <iostream>
#include <vector>

#include "TLorentzVector.h"
#include "TMath.h"

namespace TTHbb {

  class OfflineBucketofTops : public ToolBase{

  public:
    OfflineBucketofTops();
    ~OfflineBucketofTops();

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();
    
  private:


  };

}

#endif 

