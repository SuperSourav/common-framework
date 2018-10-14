#ifndef SM4TOPSCOMMONOBJECTLOADER_H_
#define SM4TOPSCOMMONOBJECTLOADER_H_

#include "TopAnalysis/ObjectLoaderBase.h"


/**
 * @brief This loads object selection used by the Top/Exotics SS+bjets analysis.
 */
class SM4topsSS3LObjectLoader : public top::ObjectLoaderBase {
public:

    /**
     * @brief Setup the cuts.  Probably easiest if you look at the code, no?
     *
     * @param topConfig The config that has been loaded.
     * @return A fully configured object that can be used to control which
     * objects (physics ones, not computer ones) are selected by the program.
     */
    virtual top::TopObjectSelection* init(std::shared_ptr<top::TopConfig> topConfig);

    ClassDef(SM4topsSS3LObjectLoader, 0)
};


#endif
