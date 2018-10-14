/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGINGTRUTHTOOL_BTAGGINGTRUTHTOOLDICT_H
#define BTAGGINGTRUTHTOOL_BTAGGINGTRUTHTOOLDICT_H

// Special handling for Eigen vectorization (relevant because of inclusion of Jet.h)
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif

#include "BTaggingTruthTagging/IBTaggingTruthTaggingTool.h"
#include "BTaggingTruthTagging/BTaggingTruthTaggingTool.h"

#endif // BTAGGINGTRUTHTOOL_BTAGGINGTRUTHTOOLDICT_H
