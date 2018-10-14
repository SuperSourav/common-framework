/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "TTHbbToolManager/ToolManager.h"
#include "OfflineTTHbb/ToolDictionary.h"
#include "OfflineTTHbb/EventHandler.h"

#include "TFile.h"
#include "TSystem.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <ctime>

int main(int argc, char* argv[]){
  /**
   * This is the main executable of OfflineTTHbb. It is used to run over output ntuples from TTHbbLeptonic
   * and produce fit inputs for TtH/TRExFitter. Tools can be rerun at this level to recalculate variables
   * or to add extra variables to the output ntuples.
   * It is not intended to use this code as a general purpose studies code, and the main goal is to
   * produce slimmed down and small flat ntuples.
   **/

  /// Check usage of ttH-offline
  /// ttH-offline -h (or --help) will print out the config file help screen
  /// ttH-offline config.txt input.txt will run the executable
  /// Command line arguments can be added with -o followed by ConfigName "Setting" for each config to change
  if(argc < 3){
    /// If only one option after ttH-offline, check whether it is for help dialog
    if(argc == 2){
      std::string param1 = argv[1];
      if(TTHbb::util::iequals(param1,"-h") || TTHbb::util::iequals(param1,"--help")){
	  auto* config = TTHbb::GlobalConfiguration::get();
	  config->printHelp();
	}
    }
    /// Print out executable usage
    std::cout << "USAGE: " << argv[0] << " <config.txt> <inputlist.txt> [-o SETTING \"Option\"]" << std::endl;
    exit(1);
  }

  /// If extra parameters are provided, check that this is with the -o flag
  if(argc > 4){
    std::string param1 = argv[3];
    if(param1 != "-o"){
      std::cout << "USAGE: " << argv[0] << " <config.txt> <inputlist.txt> [-o SETTING \"Option\"]" << std::endl;
      exit(1);
    }
  }
  
  /// Get config and inputs from command line arguments
  std::string configFile = argv[1];
  std::string inputList = argv[2];

  /// Parse command line settings
  std::vector<std::string> settingVec;
  std::vector<std::string> paramVec;
  if(argc > 4){
    /// For every two arguments, add the first to the setting vector and the second to the param vector
    for(int i_arg = 4; i_arg < argc; ++i_arg){
      settingVec.push_back(argv[i_arg]);
      ++i_arg;
      /// If there aren't an even number of arguments, complain and quit
      if(i_arg == argc){
	std::cout << "ERROR: Unequal number of settings and options in command line" << std::endl;
	exit(2);
      }
      paramVec.push_back(argv[i_arg]);
    }
  }

  /// Get global configuration
  auto config = TTHbb::GlobalConfiguration::get();
  /// Load from file
  std::cout << "INFO:\tLoading global configuration from " << configFile << std::endl;
  config->loadConfig(configFile);

  /// Pass command line arguements and change corresponding settings
  /// This can only be done once!!! Always run setting so CLparamsSet bool is set to true
  config->parseCLOpts(settingVec, paramVec);

  /// Print out Configuration after command line arguments have been set
  /// This provides a log of how everything was set up including settings using default values
  config->printConfig();
  
  /******************************************************/

  /// Read input files list and convert to vector, easier adding to TChain
  std::vector<std::string> inputFilesVec = TTHbb::util::fileToVector(inputList);
  if(inputFilesVec.size() < 1){
    std::cout << "ERROR: No files in provided input list. Exiting." << std::endl;
    exit(3);
  }

  /// Get list of systematics to run over
  std::vector<std::string> vecOfSysts;
  if(TTHbb::util::iequals((*config)("SystematicsFromFile"),"true")){
    vecOfSysts = TTHbb::util::fileToVector((*config)("SystematicsFile"));
  }
  else{
    vecOfSysts = TTHbb::util::vectoriseString((*config)("Systematics"));
  }
  /// Tool manager and Event handler are two important and powerful obects
  /// They drive and control the offline running

  /// Load all tools
  auto* toolManager = TTHbb::ToolManager::get();
  /// Get the dictionary class and pass it the tool manager
  TTHbb::ToolDictionary* dict = new TTHbb::ToolDictionary();
  dict->buildDictionary(toolManager);
  delete dict;
  /// Load all the tools, based on the config file parameter
  toolManager->loadTools();
  toolManager->initialiseAllTools();

  /// Make the event handler which is our guide to the land of events
  std::shared_ptr<TTHbb::EventHandler> eventHandler = std::make_shared<TTHbb::EventHandler>();

  /// Check for whether the user wants to perform the ttbar filters
  if(TTHbb::util::iequals((*config)("DoTTbarBfilter"),"true")){
    eventHandler->doBfilter = true;
  }
  if(TTHbb::util::iequals((*config)("DoTTbarDileptonFilter"),"true")){
    eventHandler->doDilFilter = true;
  }
  
  /// Get normalisation weights if MC
  if(!eventHandler->isData){
    std::map<int,float> normWeights;
    std::map<int, std::map<std::string, float> > mcNormWeights;
    /// Check whether the user wants normalisation from totalEventsWeighted or the cutflow
    /// Could also not want a normalisation at all (option other than the two provided)
    std::string normOpt = (*config)("Normalisation");
    if(TTHbb::util::iequals(normOpt,"totalEventsWeighted")){
      normWeights = TTHbb::util::makeNormalisationMaps(inputFilesVec);
      mcNormWeights = TTHbb::util::makeMCWeightNormalisationMaps(inputFilesVec);
    }
    else if(TTHbb::util::iequals(normOpt,"Cutflow")){
      normWeights = TTHbb::util::makeNormalisationMapsFromCutflow(inputFilesVec);
    }    
    /// add to event handler for saving
    eventHandler->addNormalisationWeights(normWeights);
    /// adding all mc weights
    eventHandler->addMCNormalisationWeights(mcNormWeights);
  }
  
  /// Get the regions defined in the config file
  /// Only needs to be done once, this is interprets the config file
  eventHandler->defineRegions();

  /// Begin loop over systematics
  /// Here effort can be put in to parallelise the running
  /// Can use parallel loops or just have mutliple jobs running at one
  for(auto currentSyst : vecOfSysts){
    std::cout << "INFO: Current systematic is " << currentSyst << std::endl;

    /// Make chain for the systematic and add all the input files
    /// Add a string to say what the systematic is
    /// This will create the chain
    eventHandler->setSystematic(currentSyst);
    eventHandler->addFiles(inputFilesVec);

    /// Determine which events are in which region
    /// Needs to be done per systematic as the events change
    eventHandler->calculateRegions();

    /// Set all branches to build event
    /// The weights are determined from the weights list file
    if(!eventHandler->isData){
      eventHandler->setWeightBranches((*config)("WeightsFile"));
    }
    /// The custom variable list to dump from the input file is determined from the variable list file
    //    eventHandler->setVariableBranches((*config)("DumpVarFile"));
    /// The event decorations are include all basic branches needed and all the hard coded decorations
    eventHandler->setEventDecorBranchAddresses();

    /// Loop over all events
    unsigned int nEntries = eventHandler->GetEntries();
    if((*config)("NEvents") != ""){
      std::cout << "WARNING: Only running over maximum of " << (*config)("NEvents") 
		<< " events. Normalisation weights in this file will be calculated assuming"
		<< " all events have been run over." << std::endl;
      unsigned int maxEntries = std::stoi((*config)("NEvents"));
      if(maxEntries < nEntries){
	nEntries = maxEntries;
      }
    }
    

    /// If to run with temporary output file, need to create it in the tmp folder
    TFile* ftemp = 0;
    std::string tmpfilename;
    if(TTHbb::util::iequals((*config)("CreateTempOutputFile"),"true")){
      tmpfilename = gSystem->TempDirectory();
      tmpfilename += "/ttH-offline" + std::to_string(std::time(0)) + ".root";
      ftemp = TFile::Open(tmpfilename.c_str(),"RECREATE");
      ftemp->cd();
    }

    //TTHbb::Event* m_event = 0;
    std::shared_ptr<TTHbb::Event> m_event;
    bool firstEvent = true;
    for(unsigned int iEntry = 0; iEntry < nEntries; ++iEntry){
      /// Print out that the first entry is being processed, and then every 10000th
      if(iEntry == 0 || ((iEntry + 1) % 10000 == 0)){
	std::cout << "INFO: Processing entry " << std::setw(7) << iEntry + 1
		  << "/" << nEntries << std::endl;
      }
      /// Get the entry from the chain
      /// Don't want the loop internal to the class
      eventHandler->GetEntry(iEntry);

      /// Skip the event if it is not in any region or it is skimmed away
      /// No point in performing the tools on it or building it!
      if(eventHandler->skipEventCheck()){
	continue;
      }

      /// Use a separate class to handle the building of the event
      /// Need to store many variables for reading from input file before dressing, easier to contain them
      /// Build event
      m_event = eventHandler->buildEvent();

      /// Loop through tools, passing event to each
      toolManager->applyAllTools(m_event.get());

      /// Stop here is the post-processing requirement of a region failed
      if(eventHandler->skipEventCheckPostProc()){
	continue;
      }
      
      /// At the end of the first event, the variables to save to the output will be checked, not before the loop
      /// The variables saved are dependent on what is decorated to the event
      /// Functionality exists to provide a list of variables instead of dumping all
      if(firstEvent){
	firstEvent = false;
	eventHandler->createOutputs();
      }
      /// Fill all outputs trees
      /// If there are regions defined it will be those, otherwise a generic outputs tree
      eventHandler->fillOutputs();
    }
    /// End of loop over events
    std::cout << "INFO: End of event loop" << std::endl;
    /// Save all outputs for the current systematic
    /// This will support multiple saving methods defined in the config file
    /// Contained ntuples with TDirectories or ntuples per region
    /// All systematics are saved in each ntuple
    eventHandler->saveOutputs();
    /// If running with temporary files, make sure to clean and close
    if(ftemp != 0){
      ftemp->Flush();
      ftemp->Close();
      ftemp = 0;
      gSystem->Unlink(tmpfilename.c_str());
    }
  }
  /// End of loop over systematics
  std::cout << "INFO: End of systematics" << std::endl;

  /// Finalise all the tools
  /// This won't be saving anything but mostly dealing with closing files and memory management
  toolManager->finaliseAllTools();
  delete toolManager;
  /// End of executable, return 0
  return 0;
}
