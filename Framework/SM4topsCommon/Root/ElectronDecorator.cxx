#include "SM4topsCommon/ElectronDecorator.h"
#include "SM4topsCommon/ToolBox.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include <AsgTools/MessageCheck.h>
#include <AthContainers/exceptions.h>

#include "../TTHbbAnalysis/TTHbbLeptonic/TTHbbLeptonic/EventSaverHelper.h"
#include "../TTHbbAnalysis/TTHbbObjects/TTHbbObjects/Event.h"

using namespace asg::msgUserCode;

namespace SM4topsCommon {

ElectronDecorator::ElectronDecorator(const std::string &selectorName, const std::string &params,
                                     std::shared_ptr<top::TopConfig> config)
    : m_electronChargeIDLoose("ElectronChargeIDSelectorLoose"),
      m_electronChargeIDMedium("ElectronChargeIDSelectorMedium"),
      m_electronChargeIDTight("ElectronChargeIDSelectorTight") {
  auto *esh = EventSaverHelper::get();
  auto *toolBox = ToolBox::get();

  top::check(
      m_electronChargeIDLoose.setProperty(
          "TrainingFile", "ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root"),
      "ElectronChargeIDLoose: Failed to set training file.");
  top::check(m_electronChargeIDLoose.setProperty("OutputLevel", MSG::ERROR),
             "ElectronChargeIDLoose: Failed to set output level.");
  top::check(m_electronChargeIDLoose.initialize(), "ElectronChargeIDLoose: Failed to initialize.");
  top::check(
      m_electronChargeIDMedium.setProperty(
          "TrainingFile", "ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root"),
      "ElectronChargeIDMedium: Failed to set training file.");
  top::check(m_electronChargeIDMedium.setProperty("OutputLevel", MSG::ERROR),
             "ElectronChargeIDMedium: Failed to set output level.");
  top::check(m_electronChargeIDMedium.initialize(),
             "ElectronChargeIDMedium: Failed to initialize.");
  top::check(
      m_electronChargeIDTight.setProperty(
          "TrainingFile", "ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root"),
      "ElectronChargeIDTight: Failed to set training file.");
  top::check(m_electronChargeIDTight.setProperty("OutputLevel", MSG::ERROR),
             "ElectronChargeIDTight: Failed to set output level.");
  top::check(m_electronChargeIDTight.initialize(), "ElectronChargeIDTight: Failed to initialize.");

  esh->addElecVariableToBeSaved("chargeIDBDTLoose", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("chargeIDBDTMedium", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("chargeIDBDTTight", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");


  m_electronTriggerNames = toolBox->getElectronTriggerNames(toolBox->getUniqueTriggerNamesAll(config, selectorName));
  m_electronSLTriggerNames = toolBox->getSLTriggerNames(m_electronTriggerNames);
  m_electronDLTriggerNames = toolBox->getDLTriggerNames(m_electronTriggerNames);

  std::cout << "Decorating electrons for selector: " << selectorName << std::endl;
  std::cout << "TriggerMatch variables for: " << std::endl;
  for (auto &triggerName : m_electronTriggerNames) {
    std::cout << "-- " << triggerName << std::endl;
    /* At the moment, TTHbb already creates this variable with a lower case T (i.e. trigMatch_), but
     * their trigger list is hardcoded.*/
    // esh->addElecVariableToBeSaved(std::string("TrigMatch_") + triggerName,
    //                               TTHbb::DecorationType::Char, false, "ElectronDecorator");
  }

  std::cout << "Of those are single lepton:" << std::endl;
  for_each(m_electronSLTriggerNames.begin(), m_electronSLTriggerNames.end(),
           [](string &s) { std::cout << "- " << s << std::endl; });
  std::cout << "And dilepton:" << std::endl;
  for_each(m_electronDLTriggerNames.begin(), m_electronDLTriggerNames.end(),
           [](string &s) { std::cout << "- " << s << std::endl; });

  esh->addElecVariableToBeSaved("isSLTMatch", TTHbb::DecorationType::Char, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("isDLTMatch", TTHbb::DecorationType::Char, false,
                                "ElectronDecorator");
}

bool ElectronDecorator::apply(const top::Event &event) const {
  // Check if TTHbb::Event exists
  if (!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables")) {
    std::cout << "ElectronDecorator: TTHbbEventVariables (TTHbb::Event*) "
                 "object not found"
              << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  // Extract TTHbb::Event
  std::shared_ptr<TTHbb::Event> tthevt =
      event.m_info->auxdecor<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables");

  // Create all the necessary accessors for all triggers.
  std::map<std::string, SG::AuxElement::ConstAccessor<char> *> accessors = {};
  for (auto &triggerName : m_electronTriggerNames) {
    auto acc = new SG::AuxElement::ConstAccessor<char>(std::string("TRIGMATCH_") + triggerName);
    accessors[triggerName] = acc;
  }

  /* Loop over electrons. Use an index loop to be able to access the TTHbb electron as well as the
   * xAODElectron */
  for (unsigned int iel = 0; iel < tthevt->m_electrons.size(); iel++) {
    // for (auto &triggerName : m_electronTriggerNames) {
    //   auto acc = *accessors[triggerName];
    //   tthevt->m_electrons.at(iel)->charVariable(std::string("TrigMatch_") + triggerName) =
    //       acc.isAvailable(*event.m_electrons.at(iel)) ? acc(*event.m_electrons.at(iel)) : false;
    //   /* This means that if the Aux store is not available it still runs through at sets false as
    //    * default. This might not be the desired behaviour. */
    // }

    for (auto &triggerName : m_electronSLTriggerNames) {
      auto acc = *accessors[triggerName];
      if (acc.isAvailable(*event.m_electrons.at(iel))) {
        tthevt->m_electrons.at(iel)->charVariable("isSLTMatch") = acc(*event.m_electrons.at(iel));
        if (acc(*event.m_electrons.at(iel))) break;
      } else {
        ANA_MSG_WARNING("Trigger: " << triggerName << " is not available for an electron.");
        tthevt->m_electrons.at(iel)->charVariable("isSLTMatch") = false;
      }
    }
    for (auto &triggerName : m_electronDLTriggerNames) {
      auto acc = *accessors[triggerName];
      if (acc.isAvailable(*event.m_electrons.at(iel))) {
        tthevt->m_electrons.at(iel)->charVariable("isDLTMatch") = acc(*event.m_electrons.at(iel));
        if (acc(*event.m_electrons.at(iel))) break;
      } else {
        ANA_MSG_WARNING("Trigger: " << triggerName << " is not available for an electron.");
        tthevt->m_electrons.at(iel)->charVariable("isDLTMatch") = false;
      }
    }

    tthevt->m_electrons.at(iel)->floatVariable("chargeIDBDTLoose") =
        (float)m_electronChargeIDLoose.calculate(event.m_electrons.at(iel));
    tthevt->m_electrons.at(iel)->floatVariable("chargeIDBDTMedium") =
        (float)m_electronChargeIDMedium.calculate(event.m_electrons.at(iel));
    tthevt->m_electrons.at(iel)->floatVariable("chargeIDBDTTight") =
        (float)m_electronChargeIDTight.calculate(event.m_electrons.at(iel));

  }  // End electron loop
  return true;
}

std::string ElectronDecorator::name() const { return "DECORATEELECTRONS"; }

// bool ElectronDecorator::isSLTMatch(const std::map<std::string,
// SG::AuxElement::ConstAccessor<char> *> &accessors, const xAOD::Electron &el) const{
//   for (auto trigNameAccessorPair : accessors) {
//     std::smatch match;
//     while(std::regex_search (trigNameAccessorPair.first, match, std::regex("(e|mu)[0-9]{1,3}")))
//     {
//       // for_each(match.begin(),match.end(), [] (std::string s) {std::cout << s << std::endl;});
//       std::cout << match[0] << std::endl;
//     }
//   }
//   return false;
// }

}  // namespace SM4topsCommon
