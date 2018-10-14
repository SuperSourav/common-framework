# Auto-generated on: 2017-11-16 17:46:44.453858

# Declare the name of this package:
atlas_subdir( TTHbbLeptonic None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TopObjectSelectionTools
                          TopEventSelectionTools
                          MVAVariables
                          TopEvent
                          TopConfiguration
                          TopAnalysis
                          JetSubStructureUtils
                          JetRec
                          JetMomentTools
                          JetEDM
                          AsgTools
                          xAODCaloEvent
                          xAODBase
                          xAODTruth
                          TopDataPreparation
                          TTHbbObjects
                          TTHbbTruthHelper
                          TTHbbMVAInterfaces
                          TTHbbReconstruction
                          TTHbbClassification
                          TTHbbMatchingTools
                          TTHFClassificationTools
                          HplusDiscriminant
                          TTHbbTtbarCorrections
                          TTHbbTRFCalc
                          MCFakes
                          DilepFakes
                          BoostedVariables
                          NNLOReweighter
			  Tools/PathResolver)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbLeptonic _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHbbLeptonic Root/*.cxx Root/*.h Root/*.icc
                   TTHbbLeptonic/*.h TTHbbLeptonic/*.icc TTHbbLeptonic/*/*.h
                   TTHbbLeptonic/*/*.icc ${_cintDictSource}
                   PUBLIC_HEADERS TTHbbLeptonic
                   LINK_LIBRARIES TopObjectSelectionTools
                                  TopEventSelectionTools
                                  MVAVariables
                                  TopEvent
                                  TopConfiguration
                                  TopAnalysis
                                  JetSubStructureUtils
                                  JetRecLib
                                  JetMomentToolsLib
                                  JetEDM
                                  AsgTools
                                  xAODCaloEvent
                                  xAODBase
                                  xAODTruth
                                  TopDataPreparation
                                  TTHbbObjects
                                  TTHbbTruthHelper
                                  TTHbbMVAInterfaces
                                  TTHbbReconstruction
                                  TTHbbClassification
                                  TTHbbMatchingTools
                                  TTHFClassificationTools
                                  HplusDiscriminant
                                  TTHbbTtbarCorrections
                                  TTHbbTRFCalc
                                  MCFakes
                                  DilepFakes
                                  BoostedVariables
                                  NNLOReweighter
                                  TTHbbObjects
                                  MVAVariables
                                  TTHbbTruthHelper
                                  TTHbbMVAInterfaces
                                  TTHbbReconstruction
                                  TTHbbClassification
                                  TTHbbMatchingTools
                                  TTHFClassificationTools
                                  HplusDiscriminant
                                  TTHbbTtbarCorrections
                                  TTHbbTRFCalc
                                  MCFakes
                                  DilepFakes
                                  BoostedVariables
                                  NNLOReweighter
				  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )

# Install data files from the package:
# This installs everything, but we might want to remove xmls from the build
# atlas_install_data( share/* )

message("-- WARNING : HPlus BDT xmls are not being installed into the build" )
atlas_install_data( share/*.*)
atlas_install_data( share/BDTweights_MultiClass )
atlas_install_data( share/BDTweights_boosted )
atlas_install_data( share/BDTweights_dilep )
atlas_install_data( share/BDTweights_semilep )

# Make the script executable from anywhere
atlas_install_scripts( util/create_cut_files util/getTTHbbProdScripts util/TTHbb*.py util/setup*.sh util/download_production_v2.py)

# Build the executables of the package:
atlas_add_executable( addXsec
                      util/addXsec.cxx
                      LINK_LIBRARIES TopObjectSelectionTools
                                     TopEventSelectionTools
                                     MVAVariables
                                     TopEvent
                                     TopConfiguration
                                     TopAnalysis
                                     JetSubStructureUtils
                                     JetRecLib
                                     JetMomentToolsLib
                                     JetEDM
                                     AsgTools
                                     xAODCaloEvent
                                     xAODBase
                                     xAODTruth
                                     TopDataPreparation
                                     TTHbbObjects
                                     TTHbbTruthHelper
                                     TTHbbMVAInterfaces
                                     TTHbbReconstruction
                                     TTHbbClassification
                                     TTHbbMatchingTools
                                     TTHFClassificationTools
                                     HplusDiscriminant
                                     TTHbbTtbarCorrections
                                     TTHbbTRFCalc
                                     MCFakes
                                     DilepFakes
                                     BoostedVariables
                                     NNLOReweighter
                                     TTHbbObjects
                                     MVAVariables
                                     TTHbbTruthHelper
                                     TTHbbMVAInterfaces
                                     TTHbbReconstruction
                                     TTHbbClassification
                                     TTHbbMatchingTools
                                     TTHFClassificationTools
                                     HplusDiscriminant
                                     TTHbbTtbarCorrections
                                     TTHbbTRFCalc
                                     MCFakes
                                     DilepFakes
                                     BoostedVariables
                                     NNLOReweighter
                                     ${ROOT_LIBRARIES}
                                     TTHbbLeptonic )
atlas_add_executable( compareTTHOutFile
                      util/compareTTHOutFile.cxx
                      LINK_LIBRARIES TopObjectSelectionTools
                                     TopEventSelectionTools
                                     MVAVariables
                                     TopEvent
                                     TopConfiguration
                                     TopAnalysis
                                     JetSubStructureUtils
                                     JetRecLib
                                     JetMomentToolsLib
                                     JetEDM
                                     AsgTools
                                     xAODCaloEvent
                                     xAODBase
                                     xAODTruth
                                     TopDataPreparation
                                     TTHbbObjects
                                     TTHbbTruthHelper
                                     TTHbbMVAInterfaces
                                     TTHbbReconstruction
                                     TTHbbClassification
                                     TTHbbMatchingTools
                                     TTHFClassificationTools
                                     HplusDiscriminant
                                     TTHbbTtbarCorrections
                                     TTHbbTRFCalc
                                     MCFakes
                                     DilepFakes
                                     BoostedVariables
                                     NNLOReweighter
                                     TTHbbObjects
                                     MVAVariables
                                     TTHbbTruthHelper
                                     TTHbbMVAInterfaces
                                     TTHbbReconstruction
                                     TTHbbClassification
                                     TTHbbMatchingTools
                                     TTHFClassificationTools
                                     HplusDiscriminant
                                     TTHbbTtbarCorrections
                                     TTHbbTRFCalc
                                     MCFakes
                                     DilepFakes
                                     BoostedVariables
                                     NNLOReweighter
                                     ${ROOT_LIBRARIES}
                                     TTHbbLeptonic )
