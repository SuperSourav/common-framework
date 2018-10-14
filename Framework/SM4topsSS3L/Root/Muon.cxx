#include "SM4topsSS3L/Muon.h"

#include "TopEvent/EventTools.h"

namespace SM4topsSS3L {

Muon::Muon(double ptcut, top::IsolationBase *isolation, const bool applyTTVACut)
    : m_ptcut(ptcut), m_muonSelectionTool("CP::MuonSelectionTool"),
      m_muonSelectionToolLoose("CP::MuonSelectionToolLoose"),
      m_isolation(isolation), m_applyTTVACut(applyTTVACut) {
  top::check(m_muonSelectionTool.retrieve(),
             "Failed to retrieve muonSelectionTool");
  top::check(m_muonSelectionToolLoose.retrieve(),
             "Failed to retrieve muonSelectionToolLoose");
}

// Muon::~Muon() {
// }

bool Muon::passSelection(const xAOD::Muon &mu) const {
  if (!this->passSelectionLoose(mu))
    return false;

  ///--
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15
  ///--///
  if (!m_muonSelectionTool->accept(mu))
    return false;

  // isolation, if m_isolation != nullptr
  if (m_isolation && !m_isolation->passSelection(mu))
    return false;

  return true;
}

bool Muon::passSelectionLoose(const xAOD::Muon &mu) const {
  ///--
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15
  ///--///
  if (!m_muonSelectionToolLoose->accept(mu))
    return false;

  if (mu.pt() < m_ptcut)
    return false;

  // isolation, if m_isolation != nullptr
  if (m_isolation && !m_isolation->passSelectionLoose(mu))
    return false;

  // Track-to-vertex association
  if (m_applyTTVACut) {
    if (!passTTVACuts(mu))
      return false;
  }

  return true;
}

bool Muon::passTTVACuts(const xAOD::Muon &mu) const {

  // Almost exact copy from
  // https://gitlab.cern.ch/atlas/athena/blob/master/PhysicsAnalysis/TopPhys/xAOD/TopObjectSelectionTools/Root/MuonMC15.cxx#L67

  // TTVA:
  // see
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPEOYE2015#Track_to_Vertex_Association
  if (!mu.isAvailable<float>("d0sig")) {
    std::cout << "d0 significance not found for muon. "
              << "Maybe no primary vertex? Won't accept." << std::endl;
    return false;
  }

  float d0sig = mu.auxdataConst<float>("d0sig");
  if (std::abs(d0sig) >= 3)
    return false;

  if (!mu.isAvailable<float>("delta_z0_sintheta")) {
    std::cout << "delta z0*sin(theta) not found for muon. "
              << "Maybe no primary vertex? Won't accept." << std::endl;
    return false;
  }

  float delta_z0_sintheta = mu.auxdataConst<float>("delta_z0_sintheta");
  if (std::abs(delta_z0_sintheta) >= 0.5)
    return false;

  return true;
}

void Muon::print(std::ostream &os) const {
  os << "Muon\n"
     << "    * pT > " << m_ptcut << "\n"
     << "    * apply TTVA cuts: " << (m_applyTTVACut ? "true" : "false") << "\n"
     << "    * Everything else from muon tool - fill this in?\n";

  if (!m_isolation)
    os << "    * No isolation requirement\n";
  else
    m_isolation->print(os);
}

} // namespace SM4topsSS3L
