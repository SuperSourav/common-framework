/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/




#include "SM4topsCommon/SM4topsCommonLoader.h"
#include "SM4topsCommon/TruthDecorator.h"
#include "SM4topsCommon/MuonDecorator.h"
#include "SM4topsCommon/ElectronDecorator.h"
#include "SM4topsCommon/EventDecorator.h"
#include "SM4topsCommon/QGVar.h"

#include <string>
#include <sstream>
#include <iostream>

/**
  * When top-xaod recognises a cut specification in the configuration file, it
  * attempts to load the corresponding cut tool.
  */

top::EventSelectorBase* SM4topsCommonLoader::initTool(
  const std::string& name,
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

  if(toolname == "DECORATETRUTH")
    return new SM4topsCommon::TruthDecorator(param, config);

  if (toolname == "DECORATEMUONS")
    return new SM4topsCommon::MuonDecorator(name, param, config);

  if (toolname == "DECORATEELECTRONS")
    return new SM4topsCommon::ElectronDecorator(name, param, config);

  if (toolname == "DECORATEEVENT")
    return new SM4topsCommon::EventDecorator(param, config);

  if(toolname == "SM4TOPSQGVAR")
    return new SM4topsCommon::QGVar(param, config);

  return nullptr;
}
