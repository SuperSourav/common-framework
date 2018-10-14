/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/TTHbbLeptonicLoader.h"

// headers for all package tools
#include "TTHbbLeptonic/DummyVariableTool.h"
#include "TTHbbLeptonic/MCFakes.h"
#include "TTHbbLeptonic/DilepFakes.h"
#include "TTHbbLeptonic/OnlineMVAVariables.h"
#include "TTHbbLeptonic/HplusDiscriminant.h"
#include "TTHbbLeptonic/HplusBDT.h"
#include "TTHbbLeptonic/TTHbbEDMConverter.h"
#include "TTHbbLeptonic/TTHbbJetHLTmatch.h"
#include "TTHbbLeptonic/NeutrinoReconstruction.h"
#include "TTHbbLeptonic/TTHSemilepReco.h"
#include "TTHbbLeptonic/TTHSemilepLikelihoodDiscriminant.h"

#include "TTHbbLeptonic/TTHDilepReco.h"
#include "TTHbbLeptonic/TTHbbCreateEvent.h"
#include "TTHbbLeptonic/TTHbbTruthEDMConverter.h"
#include "TTHbbLeptonic/TTHSemilepClassBDT.h"
#include "TTHbbLeptonic/TTHDilepClassBDT.h"
#include "TTHbbLeptonic/TTHBoostedBDT.h"
#include "TTHbbLeptonic/DummyVariableToolTruth.h"
#include "TTHbbLeptonic/NLargeJetSelector.h"
#include "TTHbbLeptonic/JetTruthMatchingTool.h"
#include "TTHbbLeptonic/H4bAnalysisVariables.h"
#include "TTHbbLeptonic/TriggerPrescales.h"
#include "TTHbbLeptonic/TTbarReweightingTool.h"
#include "TTHbbLeptonic/TTHbbLargeJetEDMConverter.h"
#include "TTHbbLeptonic/BoostedSelection.h"
#include "TTHbbLeptonic/TTHbbRegionDefinition.h"
#include "TTHbbLeptonic/TRFCalc.h"
#include "TTHbbLeptonic/TTHMultiClassBDT.h"

//#include "FourTopTools/SM4topAnalysisVariables.h"

#include <string>
#include <sstream>
#include <iostream>

/**
  * When top-xaod recognises a cut specification in the configuration file, it
  * attempts to load the corresponding cut tool.
  */

top::EventSelectorBase* TTHbbLeptonicLoader::initTool(
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


  // Load tool
  if (toolname == "LJET_N")
    return new NLargeJetSelector(param);
  if(toolname == "TTHBBCREATEEVENT")
    return new TTHbbCreateEvent(param, config);
  if(toolname == "TTHBBTRUTHEDM")
    return new TTHbbTruthEDMConverter(param, config);
  if(toolname == "TTHBBEDM")
    return new TTHbbEDMConverter(param, config);
  if(toolname == "JETHLTMATCH")
    return new TTHbbJetHLTmatch(param, config);
  if(toolname == "JETTRUTHMATCHING")
    return new JetTruthMatchingTool(param, config);
  if(toolname == "DUMMY")
    return new DummyVariableTool(param, config);
  if(toolname == "MCFAKES")
    return new MCFakes(param, config);
  if(toolname == "DILEPFAKES")
    return new DilepFakes(param, config);
  if(toolname == "MVAVARS")
    return new OnlineMVAVariables(param, config);
  if(toolname == "HplusDiscriminant")
    return new HplusDiscriminant(param, config);
  if(toolname == "HplusBDT")
    return new HplusBDT(param, config);
  if(toolname == "NEUTRINO")
    return new NeutrinoReconstruction(param, config);
  if(toolname == "TTHSEMILEPRECO")
    return new TTHSemilepReco(param, config);
  if(toolname == "TTHSEMILEPLHD")
    return new TTHSemilepLikelihoodDiscriminant(param, config);
  if(toolname == "TTHDILEPRECO")
    return new TTHDilepReco(param, config);
  if(toolname == "TTHSEMILEPCLASSBDT")
    return new TTHSemilepClassBDT(param, config);
  if(toolname == "TTHDILEPCLASSBDT")
    return new TTHDilepClassBDT(param, config);
  if(toolname == "TTHBOOSTEDBDT")
    return new TTHBoostedBDT(param, config);
  if(toolname == "H4BANALYSISVARS")
    return new H4bAnalysisVariables(param, config);
  if(toolname == "TRIGGERPRESCALES")
    return new TriggerPrescales(param, config);
  if(toolname == "DUMMY_TRUTH")
    return new DummyVariableToolTruth(param, config);
  if(toolname == "TTBARREWEIGHTING")
    return new TTbarReweightingTool(param, config);
  if(toolname == "TTHBBLARGEJETEDM")
    return new TTHbbLargeJetEDMConverter(param, config);
  if(toolname == "BOOSTEDSELECTION")
    return new BoostedSelection(param, config);
  if(toolname == "TTHBBREGIONS")
    return new TTHbbRegionDefinition(param, config);
  if(toolname == "TRFCALC")
    return new TRFCalc(param, config);
  if(toolname == "TTHMULTICLASSBDT")
    return new TTHMultiClassBDT(param, config);
/*  if(toolname == "FOURTOPSANALYSISVARS")
    return new SM4topAnalysisVariables(param, config);
*/
  return nullptr;
}
