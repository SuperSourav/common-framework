/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURTOPTOOLSLOADER
#define FOURTOPTOOLSLOADER

#include "TopEventSelectionTools/ToolLoaderBase.h"

/**
 * This class is loaded by top-xaod when it starts. Specifically, it looks for a
 * class that matches the name of the package with the word "Loader" appended to
 * the end.
 */

class FourTopToolsLoader: public top::ToolLoaderBase{
    public:
        // This is a factory for making tools of the package.
        top::EventSelectorBase* initTool(
            const std::string& name,
            const std::string& line,
            TFile* outputFile,
	    std::shared_ptr<top::TopConfig> config,
            EL::Worker* wk = nullptr
        );
    ClassDef(FourTopToolsLoader, 0)
};

#endif
