/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "FourTopTools/FourTopVariables.h"
#include "FourTopTools/FourTopToolsLoader.h"

#include <string>
#include <sstream>
#include <iostream>

/**
  * When top-xaod recognises a cut specification in the configuration file, it
  * attempts to load the corresponding cut tool.
  */

top::EventSelectorBase* FourTopToolsLoader::initTool(
  const std::string& /*name*/,
  const std::string& line,
  TFile* /*outputFile*/,
  std::shared_ptr<top::TopConfig> config,
  EL::Worker* /*wk*/
){
  std::istringstream iss(line);
  std::string toolname;
  getline(iss, toolname, ' ');

  std::string param;
  if (line.size() > toolname.size())
    param = line.substr(toolname.size() + 1);

  if(toolname == "FOURTOPSANALYSISVARS")
    return new FourTopVariables(param, config);

  return nullptr;
}
