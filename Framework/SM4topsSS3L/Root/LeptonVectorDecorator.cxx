#include "SM4topsSS3L/LeptonVectorDecorator.h"

#include <algorithm>
#include <sstream>

#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "../TTHbbAnalysis/TTHbbLeptonic/TTHbbLeptonic/EventSaverHelper.h"
#include "../TTHbbAnalysis/TTHbbObjects/TTHbbObjects/Event.h"

namespace SM4topsSS3L {

LeptonVectorDecorator::LeptonVectorDecorator(std::string params,
                                             std::shared_ptr<top::TopConfig> config) {
  auto* esh = EventSaverHelper::get();

  m_indices = {0, 1, 2, 3};
  std::vector<std::pair<std::string, TTHbb::DecorationType>> variables = {
      std::make_pair("pt", TTHbb::DecorationType::Float),
      std::make_pair("eta", TTHbb::DecorationType::Float),
      std::make_pair("phi", TTHbb::DecorationType::Float),
      std::make_pair("e", TTHbb::DecorationType::Float),
      std::make_pair("charge", TTHbb::DecorationType::Float),
      std::make_pair("pdgId", TTHbb::DecorationType::Int),
      std::make_pair("isTight", TTHbb::DecorationType::Char),
      std::make_pair("origIndex", TTHbb::DecorationType::Int)};

  for (auto var : variables) {
    for (auto ind : m_indices) {
      esh->addVariableToBeSaved(getVarName(ind, var.first), var.second, false,
                                "LeptonVectorDecorator");
    }
  }
}

bool LeptonVectorDecorator::apply(const top::Event& event) const {
  if (!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables")) {
    std::cout << "LeptonVectorDecorator: TTHbbEventVariables (TTHbb::Event*) "
                 "object not found"
              << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  std::shared_ptr<TTHbb::Event> tthevt =
      event.m_info->auxdecor<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables");

  std::vector<Lepton> leptons = {};
  for (unsigned int iel = 0; iel < event.m_electrons.size(); iel++) {
    if (event.m_electrons.at(iel)->auxdataConst<char>("passPreORSelection")) {
      struct Lepton lep = {event.m_electrons.at(iel)->pt(),
                           event.m_electrons.at(iel)->eta(),
                           event.m_electrons.at(iel)->phi(),
                           event.m_electrons.at(iel)->e(),
                           event.m_electrons.at(iel)->charge(),
                           11,
                           static_cast<char>(1),
                           iel};
      leptons.emplace_back(lep);
    }
  }

  for (unsigned int imu = 0; imu < event.m_muons.size(); imu++) {
    if (event.m_muons.at(imu)->auxdataConst<char>("passPreORSelection")) {
      struct Lepton lep = {event.m_muons.at(imu)->pt(),
                           event.m_muons.at(imu)->eta(),
                           event.m_muons.at(imu)->phi(),
                           event.m_muons.at(imu)->e(),
                           event.m_muons.at(imu)->charge(),
                           13,
                           static_cast<char>(1),
                           imu};
      leptons.emplace_back(lep);
    }
  }

  if (!leptons.empty()) {
    std::sort(leptons.begin(), leptons.end(),
              [](Lepton const& a, Lepton const& b) { return a.pt > b.pt; });
  }

  for (auto& ind : m_indices) {
    if (ind < leptons.size()) {
      tthevt->floatVariable(getVarName(ind, "pt")) = leptons[ind].pt;
      tthevt->floatVariable(getVarName(ind, "eta")) = leptons[ind].eta;
      tthevt->floatVariable(getVarName(ind, "phi")) = leptons[ind].phi;
      tthevt->floatVariable(getVarName(ind, "e")) = leptons[ind].e;
      tthevt->floatVariable(getVarName(ind, "charge")) = leptons[ind].charge;
      tthevt->intVariable(getVarName(ind, "pdgId")) = leptons[ind].pdgId;
      tthevt->charVariable(getVarName(ind, "isTight")) = leptons[ind].isTight;
      tthevt->intVariable(getVarName(ind, "origIndex")) = leptons[ind].origIndex;
    } else {
      tthevt->floatVariable(getVarName(ind, "pt")) = -std::numeric_limits<float>::max();
      tthevt->floatVariable(getVarName(ind, "eta")) = -std::numeric_limits<float>::max();
      tthevt->floatVariable(getVarName(ind, "phi")) = -std::numeric_limits<float>::max();
      tthevt->floatVariable(getVarName(ind, "e")) = -std::numeric_limits<float>::max();
      tthevt->floatVariable(getVarName(ind, "charge")) = -std::numeric_limits<float>::max();
      tthevt->intVariable(getVarName(ind, "pdgId")) = -std::numeric_limits<int>::max();
      tthevt->charVariable(getVarName(ind, "isTight")) = -std::numeric_limits<char>::max();
      tthevt->intVariable(getVarName(ind, "origIndex")) = -std::numeric_limits<int>::max();
    }
    }

  return true;
}

std::string LeptonVectorDecorator::name() const { return "SM4topsSS3LLeptonVectorDecorator"; }

std::string LeptonVectorDecorator::getVarName(const unsigned int& index,
                                              const std::string& varName) const {
  return std::string("lep_") + std::to_string(index) + std::string("_") + varName;
}

}  // namespace SM4topsSS3L
