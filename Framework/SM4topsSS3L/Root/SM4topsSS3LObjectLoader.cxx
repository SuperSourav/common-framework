/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SM4topsSS3L/SM4topsSS3LObjectLoader.h"

// Enable reading of configuration text file at run time
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/ElectronLikelihoodMC15.h"
#include "TopObjectSelectionTools/ElectronCutBasedMC15.h"
// #include "TopObjectSelectionTools/MuonMC15.h"
// #include "SM4topsSS3L/SM4topsSS3LElectron.h"
#include "SM4topsSS3L/Muon.h"

#include "TopObjectSelectionTools/JetMC15.h"
#include "TopObjectSelectionTools/OverlapRemovalASG.h"
#include "TopObjectSelectionTools/IsolationTools.h"
#include "TopObjectSelectionTools/TauMC15.h"
#include "../TTHbbLeptonic/TTHbbLeptonic/LargeJetBoosted.h"

// Run once at start of program to set up the object selection and overlap removal
top::TopObjectSelection* SM4topsSS3LObjectLoader::init(std::shared_ptr<top::TopConfig> topConfig){


  // create new object selection
  top::TopObjectSelection* objectSelection = new top::TopObjectSelection(topConfig->objectSelectionName());
  top::check(objectSelection->setProperty( "config" , topConfig ) , "Failed to setProperty for top::TopObjectSelection" );
  top::check(objectSelection->initialize() , "Failed to initialize top::TopObjectSelection" );

  // configure electrons, muons, jets and large-R jets

  if (topConfig->electronID().find("LH") == std::string::npos && topConfig->electronIDLoose().find("LH") == std::string::npos){
    // both the tight and loose user settings do not contain LH -> cut based
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
  } else if (topConfig->electronID().find("LH") != std::string::npos && topConfig->electronIDLoose().find("LH") != std::string::npos){
      //user wants likelihood electrons
      objectSelection->electronSelection(
        new top::ElectronLikelihoodMC15(
          // topConfig->isPrimaryxAOD(), // See https://gitlab.cern.ch/atlas/athena/blob/master/PhysicsAnalysis/TopPhys/xAOD/TopObjectSelectionTools/TopObjectSelectionTools/ElectronLikelihoodMC15.h#L34
          topConfig->electronPtcut(),
          topConfig->electronVetoLArCrack(),
          topConfig->electronID(),
          topConfig->electronIDLoose(),
          new top::StandardIsolation(
            topConfig->electronIsolation(),
            topConfig->electronIsolationLoose()),
            topConfig->applyTTVACut() // is default true in the constructor, but this way it is configurable
        )
      );
  } else {
    std::cout << "\nHo hum\n";
    std::cout << "I'm not sure it makes sense to use a mix of LH and cut-based electrons for the tight/loose definitions.\n";
    std::cout << "The tight electron definition is " << topConfig->electronID() << ".\n";
    std::cout << "The loose electron definition is " << topConfig->electronIDLoose() << ".\n";
    std::cout << "If it does make sense, feel free to fix this.\n";
    exit(1);
    }

    objectSelection -> muonSelection(
      new SM4topsSS3L::Muon(
        topConfig->muonPtcut(),
        new top::StandardIsolation(
          topConfig->muonIsolation(),
          topConfig->muonIsolationLoose()
        ),
        topConfig->applyTTVACut()
      )
    );

    objectSelection->jetSelection(
      new top::JetMC15(
        topConfig->jetPtcut(),
        topConfig->jetEtacut()
      )
    ); // new jet vertex tagger cut
    objectSelection->largeJetSelection(
      new LargeJetBoosted(
        topConfig->largeRJetPtcut(),
        topConfig->largeRJetEtacut()
      )
    );

    if(topConfig->useTaus())
      objectSelection->tauSelection( new top::TauMC15() );

    // configure overlap removal
    /*objectSelection->overlapRemovalPreSelection(
        nullptr
    );*/
    objectSelection->overlapRemovalPostSelection(
      new top::OverlapRemovalASG()
    );

    return objectSelection;
  }
