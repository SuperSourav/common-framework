#ifndef _TTHBB_OFFLINEBUCKETOFTOPS_H_
#define _TTHBB_OFFLINEBUCKETOFTOPS_H_

#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbToolManager/ToolBase.h"

#include "BucketofTops/BucketofTops.h"
//#include "BucketofTops/bucket_all.h"
#include <iostream>
#include <vector>

#include "TLorentzVector.h"
#include "TMath.h"

namespace TTHbb {

  class OfflineBucketofTops : public ToolBase{

  public:
    OfflineBucketofTops(std::string name);
    ~OfflineBucketofTops();

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();
    BucketofTops *m_buckets;
    bool m_DoBuckets;
    std::vector<bucketAlgo::bucket> tbuck;
  private:
  };

}

#endif 

