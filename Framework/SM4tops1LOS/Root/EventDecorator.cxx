#include <cmath>
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <cstdlib>
#include <sstream>

#include "SM4tops1LOS/EventDecorator.h"

#include "TopEvent/EventTools.h"

namespace SM4tops1LOS {

  // ----------------------------- public -----------------------------------------

  EventDecorator::EventDecorator(std::string params,std::shared_ptr<top::TopConfig> config):
  m_event(0),
  m_config(config)
  {
    m_params=params;
    m_debug = false;

    if ( asg::ToolStore::contains<TTHbbAsgToolHelper>("TTHbbAsgHelper") ) {
      m_asgHelper = asg::ToolStore::get<TTHbbAsgToolHelper>("TTHbbAsgHelper");
    }
    else {
      m_asgHelper = new TTHbbAsgToolHelper("TTHbbAsgHelper");
      top::check( m_asgHelper->initialize() , "Failed to initialize TTHbbAsgHelper" );
    }

    /// Event decoration :
    auto* ehs = EventSaverHelper::get();
    // esh->addVariableToBeSaved(name, type, nominalOnly, origin)
    ehs->addVariableToBeSaved("mll", TTHbb::DecorationType::Float, false, "EventDecorator");
    ehs->addVariableToBeSaved("mtw", TTHbb::DecorationType::Float, false, "EventDecorator");
    ehs->addVariableToBeSaved("lepFlavChannel", TTHbb::DecorationType::Int, false, "EventDecorator");
  }

  bool EventDecorator::apply(const top::Event & event) const
  {
    m_event = &event;

    // Check if TTHbb::Event exists
    if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
      std::cout << "EventDecorator: TTHbbEventVariables (TTHbb::Event*) object not found" 
		<< std::endl;
      std::cout << "-----> more info: <params: " << m_params
		<< "> <systname: " << m_config->systematicName( event.m_hashValue) << ">" 
		<< std::endl;
      std::cout << "------> aborting :-( " << std::endl;
      abort();
    }

    // Extract TTHbb::Event
    std::shared_ptr<TTHbb::Event> tthevt = 
      m_event->m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");
    TTHbb::Event* m_tthevt = tthevt.get();

    // Add new variables
    decorateMll(m_tthevt);
    decorateMtw(m_tthevt);
    tthevt->intVariable("lepFlavChannel") = lepFlavChannel(m_tthevt);

    return true;
  }

  std::string EventDecorator::name() const{
    return "DECORATEEVENT1LOS";
  }

  // ----------------------------- private ----------------------------------------

  /// ______________ Invariant mass _________________
  void EventDecorator::decorateMll(TTHbb::Event* tthevt) const
  {
    // Discard non-dilepton events
    if(tthevt->m_leptons.size() != 2) return;

    // Extract the 2 first leptons
    std::shared_ptr<TTHbb::Lepton> lep1 = tthevt->m_leptons[0];
    std::shared_ptr<TTHbb::Lepton> lep2 = tthevt->m_leptons[1];
    
    tthevt->floatVariable("mll") = (lep1->p4()+lep2->p4()).M();
  }

  /// ______________ Transverse mass _________________
  void EventDecorator::decorateMtw(TTHbb::Event* tthevt) const
  {
    // Discard non-single-lepton events
    if(tthevt->m_leptons.size() != 1) return;

    // Extract the lepton
    std::shared_ptr<TTHbb::Lepton> lep = tthevt->m_leptons[0];

    tthevt->floatVariable("mtw") = 
      std::sqrt( 2*lep->Pt()*tthevt->met_met*(1-std::cos(lep->Phi()-tthevt->met_phi)) );
  }

  /// ______________ Lepton Flavor Channel _________________
  int EventDecorator::lepFlavChannel(TTHbb::Event* tthevt) const
  {
    // Check the number of leptons
    int nLepLoose = tthevt->m_leptons.size();

    // Discard events w/ more than 2 leptons
    if( nLepLoose > 2 ) return 0;

    // Discard any event that contain a non-tight lepton
    for(auto lep : tthevt->m_leptons)
      if(!lep->charVariable("isTight")) return 0;    

    // Lepton categorization for 1L channel
    if( nLepLoose == 1 ){
      std::shared_ptr<TTHbb::Lepton> lep = tthevt->m_leptons[0];
      if( lep->type() == TTHbb::leptonType::electron ) return 1; //e+jets
      if( lep->type() == TTHbb::leptonType::muon ) return 2; //mu+jets
    }

    // Lepton categorization for 2LOS channel
    else if( nLepLoose == 2 ){
      std::shared_ptr<TTHbb::Lepton> lep1 = tthevt->m_leptons[0];
      std::shared_ptr<TTHbb::Lepton> lep2 = tthevt->m_leptons[1];
      // Discard same-sign leptons
      if( lep1->charge()*lep2->charge()>0 ) return 0;
      if( lep1->type() == TTHbb::leptonType::electron && lep2->type() == TTHbb::leptonType::electron )
        return 3; //ee OS
      else if( lep1->type() == TTHbb::leptonType::muon && lep2->type() == TTHbb::leptonType::muon )
        return 4; //mumu OS
      else
        return 5; //emu OS
    }

    return 0;
  }

}
