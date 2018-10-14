/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TTbarReweightingTool_H_
#define TTbarReweightingTool_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbTtbarCorrections/FractionReweighting.h"
#include "TTHbbTtbarCorrections/ttbbNormReweighting.h"
#include "TTHbbTtbarCorrections/ttbbShapeReweighting.h"
#include "TTHbbTtbarCorrections/ttcReweighting.h"
#include "NNLOReweighter/NNLOReweighter.h"

#include "TTHbbObjects/Event.h"

class TTbarReweightingTool:public top::EventSelectorBase {

 public:

  TTbarReweightingTool(std::string params, std::shared_ptr<top::TopConfig> config);
  ~TTbarReweightingTool();
  // void initialise(const top::Event& event) const;
  bool apply(const top::Event & event) const override;
  bool applyParticleLevel(const top::ParticleLevelEvent & plEvent) const override;

  std::string name() const override;

 private:
  std::shared_ptr<top::TopConfig> m_config;
  std::string m_params;

  void decorateFractionWeight(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateTtbbNormWeight(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateTtbbNormWeight_pp8(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateTtbbNormWeight_new(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateTtbbShapeWeight(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateTtbbShapeWeight_new(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateTtbbMPIWeight(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateTtccWeight(std::shared_ptr<TTHbb::Event> tthevt) const;
  void decorateNNLOWeight(std::shared_ptr<TTHbb::Event> tthevt) const;
  bool doNNLOReweighting(int dsid) const;
  int getNNLODSID(int dsid) const;
  void decorateSherpaOLWeight(std::shared_ptr<TTHbb::Event> tthevt) const;

  mutable FractionReweighting* m_fractionReweighting;
  mutable FractionReweighting* m_fractionReweighting_pp6;
  mutable ttbbNormReweighting* m_ttbbNormReweighting;
  mutable ttbbNormReweighting* m_ttbbNormReweighting_pp8;
  mutable ttbbNormReweighting* m_ttbbNormReweighting_new;
  mutable ttbbShapeReweighting* m_ttbbShapeReweighting;
  mutable ttbbShapeReweighting* m_ttbbShapeReweighting_new;
  mutable ttcReweighting* m_ttcReweighting;
  mutable NNLOReweighter* m_ttbarNNLOReweighter;

  std::vector< std::string > m_shapeRWTargets;
  std::vector< std::string > m_shapeRWVarNames;
  std::vector< std::string > m_NormRWTargets;
  std::vector< std::string > m_NormRWNames;

};

#endif
