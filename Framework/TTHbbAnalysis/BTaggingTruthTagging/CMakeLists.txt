# Auto-generated on: 2017-11-16 17:46:43.846441

# Declare the name of this package:
atlas_subdir( BTaggingTruthTagging None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODBTaggingEfficiency
                          AsgTools
                          PATCore
                          xAODJet
                          xAODEventInfo
                          CalibrationDataInterface
                          RootCoreUtils
                          PATInterfaces
                          PathResolver )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Build a library that other components can link against:
atlas_add_library( BTaggingTruthTagging Root/*.cxx Root/*.h Root/*.icc
                   BTaggingTruthTagging/*.h BTaggingTruthTagging/*.icc BTaggingTruthTagging/*/*.h
                   BTaggingTruthTagging/*/*.icc 
                   PUBLIC_HEADERS BTaggingTruthTagging
                   LINK_LIBRARIES xAODBTaggingEfficiencyLib
                                  AsgTools
                                  PATCoreLib
                                  xAODJet
                                  xAODEventInfo
                                  CalibrationDataInterfaceLib
                                  RootCoreUtils
                                  PATInterfaces
                                  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Build the executables of the package:
atlas_add_executable( BTaggingTruthTaggingTester
                      util/BTaggingTruthTaggingTester.cxx
                      LINK_LIBRARIES xAODBTaggingEfficiencyLib
                                     AsgTools
                                     PATCoreLib
                                     xAODJet
                                     xAODEventInfo
                                     CalibrationDataInterfaceLib
                                     RootCoreUtils
                                     PATInterfaces
                                     PathResolver
                                     ${ROOT_LIBRARIES}
                                     BTaggingTruthTagging )

