#include "SM4topsCommon/MuonDecorator.h"
#include <algorithm>
#include <sstream>
#include "SM4topsCommon/ToolBox.h"
#include "TopEvent/EventTools.h"
#include "xAODMuon/MuonContainer.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "../TTHbbAnalysis/TTHbbLeptonic/TTHbbLeptonic/EventSaverHelper.h"
#include "../TTHbbAnalysis/TTHbbObjects/TTHbbObjects/Event.h"

namespace SM4topsCommon {
/**
Decorate muons with:
- isTrigMatch (depending on the runNumber/runYear)
**/

MuonDecorator::MuonDecorator(const std::string &selectorName, const std::string &params,
                             std::shared_ptr<top::TopConfig> config) {
  auto *esh = EventSaverHelper::get();
  auto *toolBox = ToolBox::get();

  m_muonTriggerNames = toolBox->getMuonTriggerNames(toolBox->getUniqueTriggerNamesAll(config, selectorName));

  m_muonSLTriggerNames = toolBox->getSLTriggerNames(m_muonTriggerNames);
  m_muonDLTriggerNames = toolBox->getDLTriggerNames(m_muonTriggerNames);

  std::cout << "Decorating muons for selector: " << selectorName << std::endl;
  std::cout << "TriggerMatch variables for: " << std::endl;
  for (auto &triggerName : m_muonTriggerNames) {
    std::cout << "-- " << triggerName << std::endl;
    /* At the moment, TTHbb already creates this variable with a lower case T (i.e. trigMatch_), but
     * their trigger list is hardcoded.*/
    // esh->addMuonVariableToBeSaved(std::string("TrigMatch_") + triggerName,
    //                               TTHbb::DecorationType::Char, false, "MuonDecorator");
  }

  std::cout << "Of those are single lepton:" << std::endl;
  for_each(m_muonSLTriggerNames.begin(), m_muonSLTriggerNames.end(),
           [](string &s) { std::cout << "- " << s << std::endl; });
  std::cout << "And dilepton:" << std::endl;
  for_each(m_muonDLTriggerNames.begin(), m_muonDLTriggerNames.end(),
           [](string &s) { std::cout << "- " << s << std::endl; });

  esh->addMuonVariableToBeSaved("isSLTMatch", TTHbb::DecorationType::Char, false, "MuonDecorator");
  esh->addMuonVariableToBeSaved("isDLTMatch", TTHbb::DecorationType::Char, false, "MuonDecorator");
}

bool MuonDecorator::apply(const top::Event &event) const {
  // Check if TTHbb::Event exists
  if (!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables")) {
    std::cout << "SM4topsCommonMuonDecorator: TTHbbEventVariables "
                 "(TTHbb::Event*) object not found"
              << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt =
      event.m_info->auxdecor<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables");

  // Create all the necessary accessors for all triggers.
  std::map<std::string, SG::AuxElement::ConstAccessor<char> *> accessors = {};
  for (auto &triggerName : m_muonTriggerNames) {
    auto acc = new SG::AuxElement::ConstAccessor<char>(std::string("TRIGMATCH_") + triggerName);
    accessors[triggerName] = acc;
  }

  /* Loop over muons. Use an index loop to be able to access the TTHbb muon as well as the
   * xAODMuon */
  for (unsigned int imu = 0; imu < tthevt->m_muons.size(); imu++) {
    // for (auto &triggerName : m_muonTriggerNames) {
    //   auto acc = *accessors[triggerName];
    //   tthevt->m_muons.at(imu)->charVariable(std::string("TrigMatch_") + triggerName) =
    //       acc.isAvailable(*event.m_muons.at(imu)) ? acc(*event.m_muons.at(imu)) : false;
    //   /* This means that if the Aux store is not available it still runs through at sets false as
    //    * default. This might not be the desired behaviour. */
    // }

    for (auto &triggerName : m_muonSLTriggerNames) {
      auto acc = *accessors[triggerName];
      if (acc.isAvailable(*event.m_muons.at(imu))) {
        tthevt->m_muons.at(imu)->charVariable("isSLTMatch") = acc(*event.m_muons.at(imu));
        if (acc(*event.m_muons.at(imu))) break;
      } else {
        ANA_MSG_WARNING("Trigger " << triggerName << " is not available on a Muon.");
        tthevt->m_muons.at(imu)->charVariable("isSLTMatch") = false;
      }
    }
    for (auto &triggerName : m_muonDLTriggerNames) {
      auto acc = *accessors[triggerName];
      if (acc.isAvailable(*event.m_muons.at(imu))) {
        tthevt->m_muons.at(imu)->charVariable("isDLTMatch") = acc(*event.m_muons.at(imu));
        if (acc(*event.m_muons.at(imu))) break;
      } else {
        ANA_MSG_WARNING("Trigger " << triggerName << " is not available on a Muon.");
        tthevt->m_muons.at(imu)->charVariable("isDLTMatch") = false;
      }
    }
  }

  // if (!tthevt->m_muons.empty()) { // TODO: look up how to check that event
  // has m_muons in an analysisTop way. This is actually useless for (unsigned
  // int muon_ind = 0; muon_ind < tthevt->m_muons.size(); muon_ind++) {
  //   tthevt->m_muons.at(muon_ind)->charVariable("passTightSelection") =
  //   event.m_muons.at(muon_ind)->auxdataConst<char>("passPreORSelection");
  // }

  // }

  return true;
}

std::string MuonDecorator::name() const { return "SM4TopsCommonMuonDecorator"; }

}  // namespace SM4topsCommon
