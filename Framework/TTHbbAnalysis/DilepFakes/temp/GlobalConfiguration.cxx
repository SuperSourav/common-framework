/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTHbbConfiguration/GlobalConfiguration.h"

namespace TTHbb{
  
  GlobalConfiguration* GlobalConfiguration::m_instance = 0;
  
  GlobalConfiguration::GlobalConfiguration() : Configuration(){
    /// Name defined for debugging
    m_name = "Global";

    /// List of settings needs to be added here for global config

    /// Settings for tools included in OfflineTTHbb

    registerSetting("MVAVariables.TurnOn",
    		    "Set to true if the MVAVariables tool should be used for calculating variables for classification MVAs",
    		    "false"
    		    );

    registerSetting("MVAVariables.bTagWPs",
    		    "Comma delimited list of btagging WPs. This requires the jets to be decorated with btagging information \"jetIsBtagged_'wp'\". (Default 70)",
    		    "70"
    		    );

    registerSetting("OfflineFakes.TurnOn",
		    "Set to true to switch on the OfflineFakes. (Default false)",
		    "false"
		    );
    
    registerSetting("OfflineFakes.DataPath2015", 
		    "Data path for OfflineFakes efficiencies for 2015. (Default ${ROOTCOREBIN}/data/TopFakes/20p7/2015)", 
		    "${ROOTCOREBIN}/data/TopFakes/20p7/2015"
		    );

    registerSetting("OfflineFakes.DataPath2016", 
		    "Data path for OfflineFakes efficiencies for 2016. (Default ${ROOTCOREBIN}/data/TopFakes/20p7/2016)", 
		    "${ROOTCOREBIN}/data/TopFakes/20p7/2016"
		    );

    registerSetting("OfflineFakes.Use2016PrescaledTriggers",
		    "Set to true to use prescaled triggers for low pT range in 2016 data. Otherwise, un-prescaled triggers are used. (Default false)",
		    "false"
		    );

    registerSetting("OfflineFakes.ApplyPrescalesToMMweights",
		    "Set to true to apply the prescale weights to the MM weights. (Default false)",
		    "false"
		    );

    registerSetting("TRFFakes.TurnOn", 
		    "Set to true to switch on the TRF for fakes", 
		    "false"
		    );

    registerSetting("TRFFakes.DataPath", 
		    "Data path for TRF for fakes efficiencies", 
		    "${ROOTCOREBIN}/data/TTHbbToolManager/TRF_b_efficiencies_H2a4b.root"
		    );

    registerSetting("TRFFakes.parametrization", 
		    "Parametrization for the TRF for fakes", 
		    "00001"
		    );

    registerSetting("TRFFakes.regionExtrap", 
		    "b-tagging multiplicity in the inclusive region used to extrapolate", 
		    "I0B"
		    );
    registerSetting("MCFakes.TurnOn", 
		    "Set to true to switch on Monte Carlo fake estimate tools. (Default false)", 
		    "false"
		    );
    registerSetting("ComputeDilepVars", 
		    "Set to true to switch on Monte Carlo fake estimate tools. (Default false)", 
		    "false"
		    );
    registerSetting("AddDilepRegions", 
		    "Set to true to switch on Monte Carlo fake estimate tools. (Default false)", 
		    "false"
		    );
    registerSetting("DilepRegionsFile", 
		    "Set to true to switch on Monte Carlo fake estimate tools. (Default false)", 
		    "DileptonVariables/share/DilepRegions.root"
		    );
    registerSetting("DilepFakes.TurnOn", 
		    "Set to true to switch on Monte Carlo fake estimate tools. (Default false)", 
		    "false"
		    );
    registerSetting("DilepFakes.FakeWeights", 
		    "Set to true to switch on Monte Carlo fake estimate tools. (Default false)", 
		    "1.18,1.31"
		    );
    
    /// Default settings required for running

    registerSetting("OutputDirectory",
		    "Name of the output directory into which the outputs are saved. (Default \"OutputDirectory\")",
		    "OutputDirectory"
		    );

    registerSetting("SampleName",
		    "Name of the sample, used for the file names. (Default \"output\")",
		    "output"
		    );

    registerSetting("NEvents",
		    "Maximum number of events to run over. This does not change the normaliation weight (which assumes all events). To be used for debugging"
		    );

    registerSetting("Systematics",
		    "A list of systematics to run over and save. (Default nominal)",
		    "nominal"
		    );

    registerSetting("SystematicsFromFile",
		    "Whether to use the list of systematics or read from file. (Default false)",
		    "false"
		    );

    registerSetting("SystematicsFile",
		    "File to read the systematics from"
		    );
    
    registerSetting("Normalisation",
		    "If and how to calcuate the normalisation weight for a sample, saved as weight_normalise (xsec * kfactor/sumweights). totalEventsWeighted uses the tree from AnalysisTop. Cutflow uses the first bin of the cutflow_mc histograms. (Default none)",
		    "none"
		    );

    registerSetting("WeightsFile",
		    "File containing all the weights to save"
		    );

    registerSetting("SaveSystematicWeights",
		    "Whether to save all the systematic weights from the WeightsFile in the nominal trees or not. (Default true)",
		    "true"
		    );

    registerSetting("VariableListToDecorate",
		    "File containing a list of variables and their type to decorate onto the event"
		    );
    
    registerSetting("SaveAllDecorations",
		    "Whether to save all the variables decorated onto the event. (Default true)",
		    "true"
		    );

    registerSetting("VariablesToSaveFile",
		    "Path to file containing list of variables to save (if not all decorations)"
		    );

    registerSetting("SaveDefaultVariables",
		    "Whether to save all the default variables needed to build an event to the outputs (true), a subset (comma delimited list), or none (false). (Default false)",
		    "false"
		    );

    registerSetting("Regions",
		    "List of all the regions, separated by a comma, with a name and cut expression separated by a colon"
		    );

    registerSetting("SampleType",
		    "Type of the sample being run over, either data or mc. (Default MC)",
		    "MC"
		    );

    registerSetting("UseLargeJets",
		    "Whether to use the large jet collection. Set to true if ljet information is saved in the ntuple. (Default false)",
		    "false");
    
    registerSetting("DoTTbarBfilter",
		    "Whether to skip events in inclusive ttbar samples which are tt+b and covered by the bfilter samples. (Default false)",
		    "false");

    registerSetting("DoTTbarDileptonFilter",
		    "Whether to skip events in the inclusive ttbar samples which are true dilepton events and covered by the dilepton filter samples. (Default false)",
		    "false");

    registerSetting("BTaggingWP",
		    "List of b-tagging working points to be used. (Default 77,70,60)",
		    "77,70,60");

    registerSetting("CustomObjects",
    		    "Extra collections to load from input ntuple and add to the TTHbb::Event"
    		    );

    registerSetting("Dictionaries",
    		    "Extra dictionaries to load from other packages which are not dependencies of OfflineTTHbb"
    		    );

    registerSetting("CreateTempOutputFile",
		    "Whether to create a temp output file in the /tmp/ directory in order to reduce memory consumption. (Default false)",
		    "false");

    registerSetting("CreateOutputFileFirst",
		    "Whether to create the output file at the beginning of the event loop in order to reduce memory consumption. (Default false)",
		    "false");

    registerSetting("UseTTHbbVariables",
		    "Set it to false if you run on plain AnalysisTop output ntuples. (Default true)",
		    "true");

    registerSetting("SaveAllObjectDecorations",
		    "Save non default object decorations? (Default false)",
		    "false");

    /// Settings for external packages

    //Hplus
    registerSetting("Hplus.VariablesOn",
                    "Set to true if the Hplus Variables tool tool should be used. (Default false)",
                    "false"
                    );

    registerSetting("Hplus.BDTOn",
                    "Set to true if the Hplus BDT tool should be used for evaluating Hplus BDT outputs. (Default false)",
                    "false"
                    );

    registerSetting("Hplus.DiscriminantOn",
                    "Set to true if the Hplus Discriminant should be calculated (Default false)",
                    "false"
                    );

    registerSetting("Hplus.DiscriminantAPMMJSeparately",
		    "Set to true if the Hplus Discriminant should be calculated separately for the APM and MJ case in addition to the combined case (Default false)",
                    "false"
                    );

    registerSetting("Hplus.DiscriminantLikelihood",
		    "Set to true if the Hplus Likelihood for H+ and ttbar from the discriminant calculation should be added to the output as well (Default false)",
                    "false"
                    );

    registerSetting("Hplus.DiscriminantMassesInGeV",
                    "Comma-separated list of the H+ masses for which the discriminant should be calculated (Default 250,400)",
                    "250,400"
                    );

    registerSetting("Hplus.TRFOn",
                    "Set to true if the Hplus BDT tool should also run TRF evaluation. (Default false)",
                    "false"
                    );

    registerSetting("Hplus.Channel",
                    "Type of channel to run Hplus on- Semilep or Dilepn. (Default Semilep)",
                    "Semilep"
                    );

    //H2a4b
    registerSetting("H2a4b.VariablesOn",
                    "Set to true if the H->2a->4b Variables tool should be used. (Default false)",
                    "false"
                    );

    registerSetting("H2a4b.decorateVariables",
                    "Set to All if all variables decorated. Otherwise provide a list comma-separated. (Default All)",
                    "false"
                    );

    registerSetting("H2a4b.computeMT2",
                    "Set to true to enable computation of MT2. (Default false)",
                    "false"
                    );

    registerSetting("H2a4b.VjetsHFClassification",
                    "Set to true to enable V+jets HF classification. (Default false)",
                    "false"
                    );

    registerSetting("H2a4b.computeBDT",
                    "Set to true to enable BDT computation. (Default false)",
                    "false"
                    );

  }

  GlobalConfiguration::~GlobalConfiguration(){
  }

  GlobalConfiguration* GlobalConfiguration::get(){
    if(!m_instance)
      m_instance = new GlobalConfiguration();

    return m_instance;
  }
  
}
