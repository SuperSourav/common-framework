/**
This code is _heavily_ based on the package of a previous analysis looking for the four top signal:
https://svnweb.cern.ch/trac/atlasphys-exo/browser/Physics/Exotic/HQT/SameSignLeptonsPlusBJets/Run2/Code/SSbjetsTools
*/

#ifndef Muon_H_
#define Muon_H_

#include "TopObjectSelectionTools/MuonSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"

#include "AsgTools/ToolHandle.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include <AsgTools/MessageCheck.h>

using namespace asg::msgUserCode;

namespace SM4topsSS3L {

class Muon : public top::MuonSelectionBase {
public:
    /**
     * @brief Construct the tool to select good muons.
     *
     * @param ptcut The minimum pT cut for good muons.
     * @param isolation The isolation the user wants to apply.  Don't want any
     * isolation to be applied?  Then leave this as a nullptr.
     */
    Muon(double ptcut, top::IsolationBase* isolation, const bool applyTTVACut=true);

    ///Does nothing.
    ~Muon() = default;

    /**
     * @brief Implements the logic to select good muons.
     *
     * @param mu The muon that we want to check.
     * @return True if the muon is good, false otherwise.
     */
    virtual bool passSelection(const xAOD::Muon& mu) const override final;

    /**
     * @brief The loose selection needed by some background estimates.
     *
     * @param mu
     * @return
     */
    virtual bool passSelectionLoose(const xAOD::Muon& mu) const override final;

    /**
     * @brief The track-to-vertex association (TTVA) cuts.
     *
     * @param mu
     * @return True if passes, false otherwise
     */
    virtual bool passTTVACuts(const xAOD::Muon& mu) const;

    ///Print the cuts to the ostream.
    void print(std::ostream& os) const override final;


protected:
    ///Lower pT threshold to apply to object selection.
    double m_ptcut;
    ///Proper tool to select muons.
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolLoose;

    ///Isolation tool, can be nullptr meaning "no isolation requirement"
    std::unique_ptr<top::IsolationBase> m_isolation;
    bool m_applyTTVACut;
};

}

#endif
