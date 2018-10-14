/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTHBBEVENTHANDLER_H_
#define _TTHBBEVENTHANDLER_H_

#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/EventData.h"
#include "TChain.h"
#include "TTree.h"
#include "TEventList.h"
#include "TFile.h"

#include <map>
#include <unordered_map>
#include <utility>
#include <string>
#include <memory>

#include <iostream>

namespace TTHbb{

  /// Region struct which holds information about the defined regions
  struct region{
    std::string cut;
    std::string flag;
    std::shared_ptr<TEventList> eventList;
    std::shared_ptr<TTree> tree;
  };
  
  /// Event handler class to interface the input and output ntuples within the main code
  class EventHandler{
    /**
     * @brief Handler class for all the events in the file.
     * Manages all the input and output branches. 
     * Simple functions are provided to keep everything user friendly.
     * It remains to be seen whether the adding of decorations will be
     * done from within this class or the tools. For the latter I propose
     * making a singleton of this class so that it need not be passed around.
     * However, I would argue for it to be contained in the constructor.
     */

  public:
    /// Constructor
    EventHandler();

    /// Destructor
    ~EventHandler();

    /// A function to build and return the current TTHbb::Event
    std::shared_ptr<TTHbb::Event> buildEvent();

    /// A function to tell the eventHandler to get a variable from the TChain and to add it to the event decorations
    /// @param type The variable type, can be either "int" or "float"
    /// @param varName The name of the variable as it is in the input
    /// @param varToStore The name to give the variable when decorated, be default the same as varName
    void addVariableToEventDecor(std::string type, std::string varName, std::string varToStore="");

    /// A function to tell the eventHandler to get a vector variable from the TChain and add it to the appropriate object
    /// This is useful in the cases where lepton isolation/quality, and extra jet information may be wanted
    /// @param object The object to dress the variable onto. Can be "jet", "electron", "muon", "ljet"
    /// @param type The variable type, can be either "int" or "float"
    /// @param varName The name of the variable as it is in the input
    /// @param varToStore The name to give the variable when decorated, be default the same as varName
    void addVariableToObjectDecor(std::string object, std::string type, std::string varName, std::string varToStore="");

    /// A funciton to set the memory location of all the branches in the chain to then build an event with
    void setEventDecorBranchAddresses();

    /// Function to set the localtion of all the weights
    /// @param fileName The location of the text file listing all weights
    void setWeightBranches(std::string fileName);

    /// Function to set the localtion of the variables to dump from input ntuple
    /// @param fileName The location of the text file listing variables
    void setEventDecorationsFromFile(std::string fileName);

    /// A function to set the current systematic (could automate by taking it from Chain)
    /// @param Name of the current systematic
    void setSystematic(std::string syst);

    /// A function to add an input file to the internal chain
    /// @param Name of file
    void addFile(std::string file);

    /// A function to add multiple input files to the internal chain
    /// @param files Vector of input files
    void addFiles(std::vector<std::string> files);

    /// A funciton to add a map for all the normalisation weights
    /// @param weights A map with DSID as the key and sumOfWeights for that DISD as the value
    void addNormalisationWeights(std::map<int,float> weights);

    /// A funciton to add a map for all the mc normalisation weights                            
    /// @param weights A map with DSID as the key and sumOfWeights for that DISD as the value
    void addMCNormalisationWeights(std::map<int,std::map<std::string, float> > weights);

    /// A function to return the number of entries in the eventHandler
    //unsigned int GetEntries(){ return ((m_chain) ? m_chain->GetEntries() : 0); };
    unsigned int GetEntries(){ return m_chain->GetEntries(); };

    /// A function to get the entry at index
    /// @param entry The entry index
    void GetEntry(unsigned int entry);

    /// A function to return the current systematic
    std::string getSystematic(){ return m_currentSystematic; };

    /// A function to define the regions declared in the config
    void defineRegions();

    /// A funciton to determine which events are in which region
    void calculateRegions();

    /// A function to see whether an event is in a region, if any are defined
    /// If no regions are defined will return true
    bool eventInAnyRegion();

    /// A function to see whether an event should be skipped
    /// Checks whether it is in a region and whether to skip based on filter skimming
    bool skipEventCheck();
    
    /// Function to see whether an event passes a post selection flag in the region definition
    /// @param flag The post processing flag to pass
    bool eventPassSelPostProc(std::string flag);

    /// A function to determine whether an event is in any region including post processing
    bool eventInAnyRegionPostProc();

    /// A function to see whether an event should be skipped due to post processing
    bool skipEventCheckPostProc();

    /// A function to create the output files
    void createOutputFiles();
    
    /// A function to create the output trees for all the regions defined or one inclusive output
    void createOutputs();

    /// A function to build the output from the decorations on the first event for a given tree
    /// @param tree The pointer to the tree to create output for
    void createSingleOutput(std::shared_ptr<TTree>& tree);

    /// A function to fill the output trees
    void fillOutputs();

    // A function to save the output trees
    void saveOutputs();

    /// Pointer to the current event
    std::shared_ptr<TTHbb::Event> currentEvent;

    /// Pointer to the event metadata
    std::shared_ptr<TTHbb::EventData> eventData;

    /// Vector of strings to all recoBDT that shall be used in the reconstruction
    std::vector<std::string> recobdts;

    /// Easy check bool for whether running on data
    /// Needed to know whether to grab weight and truth branches
    bool isData;

    /// Easy check bool for whether running with bfiltered and inclusive samples
    bool doBfilter;

    /// Easy check bool for whether running with dilepton filtered and inclusive samples
    bool doDilFilter;

  private:
    /// A function to cut down on repeat code. Sets the addresses for all the object decorations
    /// @param intVec The corresponding map for int decorations
    /// @param floatVec The corresponding map for float decorations
    /// @param charVec The corresponding map for char decorations
    void objectDecorGetter(std::unordered_map<std::string, std::pair<std::vector<int>*,std::string>>& intVec, std::unordered_map<std::string, std::pair<std::vector<float>*,std::string>>& floatVec, std::unordered_map<std::string, std::pair<std::vector<char>*,std::string>>& charVec);

    /// A function to set the addresses for all the weights from the weights file
    /// @param ch The chain to get the branches from
    void weightBranchGetter();
    
    /// A function to set the addresses for the variables to dump from the weights file
    /// @param ch The chain to get the branches from
    //void variableBranchGetter();

    /// A function to set the addresses for all the basic variables needed for the construction of an event
    void defaultVariableGetter();

    /// Function to save all object decorations to the output ntuple
    /// @param tree The tree to add the variables to for the output
    void saveAllObjectDecorations(std::shared_ptr<TTree>& tree);
    
    /// Function to fill the obejct decoration branches (used if saving all object decoration)
    void fillAllObjectVectorsFromDecorations();

    /// A function for saving all the default variables needed to run over the ntuples
    /// @param tree The tree to add the variables to for the output
    void saveDefaultVariables(std::shared_ptr<TTree>& tree);

    /// A funciton for saving some of the default variables needed to run over the ntuples
    /// This is a subset of the above function, and is not as fast
    /// It checks each variable for being given in a list
    /// @param list A comma delimited list of the default variables to be saved
    /// @param tree The tree to add the variables to for the output
    void saveSomeDefaultVariables(std::string list, std::shared_ptr<TTree>& tree);

    /// Function to initialise all pointers to zero
    void initialisePointers();

    /// Function to set all decorations in the event to default values
    void resetEventDecorations();

    /// Function to check whether to enable/disable certain collections
    void checkCollections();

    /// A function to handle merging of bfilter samples and inclusive samples
    /// Will return true if the event is to be skipped based on dsid and a branch value
    bool isEventSkippedForTTbarBfilter();

    /// A function to handle merging of dilepton filter and inclusive samples
    /// Will return true if the event is to be skipped based on dsid and a branch value
    bool isEventSkippedForTTbarDilepton();

    /// The decoration stores
    std::unordered_map<std::string, std::pair<int,std::string>> intStore;
    std::unordered_map<std::string, std::pair<float,std::string>> floatStore;
    std::unordered_map<std::string, std::pair<char,std::string>> charStore;
    
    /// The chain
    std::shared_ptr<TChain> m_chain;

    /// In case we need it at any point
    std::string m_currentSystematic;

    /// Current entry number
    unsigned int m_currentEntry;

    /// Map of the defined regions
    std::map<std::string, TTHbb::region> regionMap;

    /// Tree for output for the case no regions are defined
    std::shared_ptr<TTree> outputTree;

    /// Easy check bool for nominal tree
    /// Needed for knowing whether to load systematic weights
    bool isNominalTree;

    /// Easy check bool for whether large jets are being used
    bool useLargeJets;    

    std::vector<float> * vIn_jet_pt;
    std::vector<float> * vIn_jet_eta;
    std::vector<float> * vIn_jet_phi;
    std::vector<float> * vIn_jet_e;
    std::vector<float> * vIn_jet_mv2;
    std::vector<float> * vIn_jet_jvt;

    std::vector<float> * vIn_ljet_pt;
    std::vector<float> * vIn_ljet_eta;
    std::vector<float> * vIn_ljet_phi;
    std::vector<float> * vIn_ljet_e;
    std::vector<char>  * vIn_ljet_topTag;
    std::vector<float> * vIn_ljet_D2;
    std::vector<float> * vIn_ljet_tau32wta;

    std::vector<float> * vIn_el_pt;
    std::vector<float> * vIn_el_eta;
    std::vector<float> * vIn_el_phi;
    std::vector<float> * vIn_el_e;
    std::vector<float> * vIn_el_charge;

    std::vector<float> * vIn_mu_pt;
    std::vector<float> * vIn_mu_eta;
    std::vector<float> * vIn_mu_phi;
    std::vector<float> * vIn_mu_e;
    std::vector<float> * vIn_mu_charge;

    std::vector<std::vector<int>*> reco_bdt_index;

    float met_met;
    float met_phi;

    /// Extra variables needed for the jets and leptons
    std::unordered_map<std::string, std::pair<std::vector<int>*,std::string>> jetIntStore;
    std::unordered_map<std::string, std::pair<std::vector<float>*,std::string>> jetFloatStore;
    std::unordered_map<std::string, std::pair<std::vector<char>*,std::string>> jetCharStore;

    std::unordered_map<std::string, std::pair<std::vector<int>*,std::string>> elIntStore;
    std::unordered_map<std::string, std::pair<std::vector<float>*,std::string>> elFloatStore;
    std::unordered_map<std::string, std::pair<std::vector<char>*,std::string>> elCharStore;

    std::unordered_map<std::string, std::pair<std::vector<int>*,std::string>> muIntStore;
    std::unordered_map<std::string, std::pair<std::vector<float>*,std::string>> muFloatStore;
    std::unordered_map<std::string, std::pair<std::vector<char>*,std::string>> muCharStore;

    std::unordered_map<std::string, std::pair<std::vector<int>*,std::string>> ljetIntStore;
    std::unordered_map<std::string, std::pair<std::vector<float>*,std::string>> ljetFloatStore;
    std::unordered_map<std::string, std::pair<std::vector<char>*,std::string>> ljetCharStore;

    /// Variables needed for the custom objects
    std::unordered_map<std::string, std::unordered_map<std::string, std::pair<std::vector<int>*,std::string>>> customIntStore;
    std::unordered_map<std::string, std::unordered_map<std::string, std::pair<std::vector<float>*,std::string>>> customFloatStore;
    std::unordered_map<std::string, std::unordered_map<std::string, std::pair<std::vector<char>*,std::string>>> customCharStore;
    
    /// Event weights
    std::unordered_map<std::string, float> nomWeights;
    std::unordered_map<std::string, std::vector<float>*> nomVecWeights;
    std::unordered_map<std::string, float> systWeights;
    std::unordered_map<std::string, std::vector<float>*> systVecWeights;

    /// Normalisation weights
    std::map<int,float> normalisationWeights;
    std::map<int, std::map<std::string, float> > mcNormalisationWeights;

    /// For the option CreateOutputFileFirst
    std::unordered_map<std::string,std::shared_ptr<TFile>> m_files;
  };
    
}

#endif
