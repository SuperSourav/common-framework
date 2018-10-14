/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SM4topsSS3L/SM4topsSS3LLoader.h"
#include "SM4topsSS3L/LeptonChannelSelector.h"
#include "SM4topsSS3L/TrigMatch.h"
#include "SM4topsSS3L/EventDecorator.h"
#include "SM4topsSS3L/TruthMatchSelector.h"
#include "SM4topsSS3L/MllSelector.h"
#include "SM4topsSS3L/MllWindowSelector.h"
#include "SM4topsSS3L/SSOSDilepSelector.h"
#include "SM4topsSS3L/ZVetoSelector.h"
#include "SM4topsSS3L/LeptonVectorDecorator.h"

#include <string>
#include <sstream>
#include <iostream>

/**
  * When top-xaod recognises a cut specification in the configuration file, it
  * attempts to load the corresponding cut tool.
  */

top::EventSelectorBase* SM4topsSS3LLoader::initTool(
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
  if (line.size() > toolname.size()) param = line.substr(toolname.size() + 1);

  if (toolname == "SM4TOPSSS3LCHANNEL")
    return new SM4topsSS3L::LeptonChannelSelector(param, config);

  if (toolname == "SM4TOPSSS3LTRIGMATCH")
    return new SM4topsSS3L::TrigMatch(name, config);

  if (toolname == "SM4TOPSSS3LEVENTDECORATOR")
    return new SM4topsSS3L::EventDecorator(param, config);

  if (toolname == "SM4TOPSSS3LTRUTHMATCH")
    return new SM4topsSS3L::TruthMatchSelector(param, config);

  if (toolname == "SM4TOPSSS3LMLL")
    return new SM4topsSS3L::MllSelector(param, false);

  if (toolname == "SM4TOPSSS3LMLLWIN")
    return new SM4topsSS3L::MllWindowSelector(param);

  if (toolname == "SM4TOPSSS3LSSOSDILEPSELECTOR")
    return new SM4topsSS3L::SSOSDilepSelector(param, config);

  if (toolname == "SM4TOPSS3LZVETO")
    return new SM4topsSS3L::ZVetoSelector(param);

  if (toolname == "LEPTONVARS")
   return new SM4topsSS3L::LeptonVectorDecorator(param, config);

  return nullptr;
}
