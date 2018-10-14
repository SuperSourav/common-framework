/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbLeptonic/H4bObjectLoaderStandardCuts.h"

// enable reading of configuration text file at run time
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/ElectronLikelihoodMC15.h"
#include "TopObjectSelectionTools/ElectronCutBasedMC15.h"
#include "TopObjectSelectionTools/MuonMC15.h"
#include "TopObjectSelectionTools/JetMC15.h"
#include "TopObjectSelectionTools/TauMC15.h"
#include "TopObjectSelectionTools/TrackJetMC15.h"
#include "TopObjectSelectionTools/PhotonMC15.h"
#include "TopObjectSelectionTools/OverlapRemovalASG.h"
#include "TopObjectSelectionTools/IsolationTools.h"
#include "TTHbbLeptonic/LargeJetBoosted.h"

// run once at the start of the program to set up the object selection and
// overlap removal
namespace H4b {

  top::TopObjectSelection* H4bObjectLoaderStandardCuts::init(std::shared_ptr<top::TopConfig> topConfig){

    // create new object selection
    top::TopObjectSelection* objectSelection = new top::TopObjectSelection(topConfig->objectSelectionName());
    top::check(objectSelection->setProperty( "config" , topConfig ) , "Failed to setProperty for top::TopObjectSelection" );
    top::check(objectSelection->initialize() , "Failed to initialize top::TopObjectSelection" );

    ///-- Photons --//                                                                                                                                                       
    if(topConfig->usePhotons()) {
      objectSelection->photonSelection(new top::PhotonMC15(25000., 2.5, new top::StandardIsolation()));
    }   

    ///-- Electrons --///
    if (topConfig->useElectrons()) {
      if (topConfig->electronID().find("LH") == std::string::npos && topConfig->electronIDLoose().find("LH") == std::string::npos) {
        //both the tight and loose user settings do not contain LH -> cut based
        objectSelection->electronSelection( 
            new top::ElectronCutBasedMC15(
              topConfig->electronPtcut(),
              topConfig->electronVetoLArCrack(),
              topConfig->electronID(),
              topConfig->electronIDLoose(),
              new top::StandardIsolation(
                topConfig->electronIsolation(), 
                topConfig->electronIsolationLoose()
                )
              )
            );

      } else if (topConfig->electronID().find("LH") != std::string::npos && topConfig->electronIDLoose().find("LH") != std::string::npos) {
        //user wants likelihood electrons
        objectSelection->electronSelection(
            new top::ElectronLikelihoodMC15(
              topConfig->isPrimaryxAOD(),
              topConfig->electronPtcut(),
              topConfig->electronVetoLArCrack(),
              topConfig->electronID(),
              topConfig->electronIDLoose(),
              new top::StandardIsolation(
                topConfig->electronIsolation(), 
                topConfig->electronIsolationLoose())
              ) 
            );

      } else {
        std::cout << "\nHo hum\n";
        std::cout << "Not sure it makes sense to use a mix of LH and cut-based electrons for the tight/loose definitions\n";
        std::cout << "Tight electron definition is " << topConfig->electronID() << "\n";
        std::cout << "Loose electron definition is " << topConfig->electronIDLoose() << "\n";
        std::cout << "If it does make sense, feel free to fix this\n";
        exit(1);
      }
    }

    ///-- Muons --///
    if (topConfig->useMuons()) {
      objectSelection -> muonSelection(new top::MuonMC15(topConfig->muonPtcut(), 
            new top::StandardIsolation(
              topConfig->muonIsolation(), 
              topConfig->muonIsolationLoose())
            ) );
    }
    ///-- Taus --///
    if (topConfig->useTaus()) {
        objectSelection->tauSelection(new top::TauMC15());
    } 

    ///-- Jets --///
    if (topConfig->useJets()) {
      objectSelection -> jetSelection(
          new top::JetMC15(topConfig->jetPtcut(), 
            topConfig->jetEtacut() 
            )
          );
    }

    ///-- Large R Jets --///
    if (topConfig->useLargeRJets()) {
      objectSelection->largeJetSelection(
          new LargeJetBoosted(
            topConfig->largeRJetPtcut(), 
            topConfig->largeRJetEtacut()
            )
          );
    }

    ///-- Track Jets --///
    if (topConfig->useTrackJets()) {
      objectSelection->trackJetSelection(new top::TrackJetMC15(topConfig->trackJetPtcut(), topConfig->trackJetEtacut()));
    }

    objectSelection->overlapRemovalPostSelection(
        new top::OverlapRemovalASG()
        );

    return objectSelection;
  }
} // namespace H4b
