# Auto-generated on: 2017-11-16 17:46:42.942983

# Declare the name of this package:
atlas_subdir( TTHbbTRFCalc None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          BTaggingTruthTagging
                          xAODPrimitives
                          TopEventSelectionTools
                          TopEvent
                          xAODTau
                          xAODTruth
                          TTHbbObjects
                          TopConfiguration
                          TopPartons
                          xAODMuon
                          TopParticleLevel
                          xAODMissingET
                          MuonIdHelpers
                          xAODEgamma )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbTRFCalc _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHbbTRFCalc Root/*.cxx Root/*.h Root/*.icc
                   TTHbbTRFCalc/*.h TTHbbTRFCalc/*.icc TTHbbTRFCalc/*/*.h
                   TTHbbTRFCalc/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHbbTRFCalc
                   LINK_LIBRARIES BTaggingTruthTagging
                                  xAODPrimitives
                                  TopEventSelectionTools
                                  TopEvent
                                  xAODTau
                                  xAODTruth
                                  TTHbbObjects
                                  TopConfiguration
                                  TopPartons
                                  xAODMuon
                                  TopParticleLevel
                                  xAODMissingET
                                  MuonIdHelpersLib
                                  xAODEgamma
                                  BTaggingTruthTagging
                                  xAODPrimitives
                                  TopEventSelectionTools
                                  TopEvent
                                  xAODTau
                                  xAODTruth
                                  TTHbbObjects
                                  TopConfiguration
                                  TopPartons
                                  xAODMuon
                                  TopParticleLevel
                                  xAODMissingET
                                  xAODEgamma
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

