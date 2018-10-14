/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SM4tops1LOS/SM4tops1LOSLoader.h"
#include "SM4tops1LOS/EventDecorator.h"

#include <string>
#include <sstream>
#include <iostream>

/**
  * When top-xaod recognises a cut specification in the configuration file, it
  * attempts to load the corresponding cut tool.
  */

top::EventSelectorBase* SM4tops1LOSLoader::initTool(
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
  if (line.size() > toolname.size()) param = line.substr(toolname.size() + 1);

  if (toolname == "DECORATEEVENT1LOS")
    return new SM4tops1LOS::EventDecorator(param, config);

  return nullptr;
}
