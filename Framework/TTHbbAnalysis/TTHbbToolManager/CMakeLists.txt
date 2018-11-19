# Auto-generated on: 2017-11-16 17:46:42.642847

# Declare the name of this package:
atlas_subdir( TTHbbToolManager None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          TTHbbConfiguration
                          MVAVariables
                          TopFakes
                          TRFFakes
                          MCFakes
                          BucketofTops
			  TTHbbTtbarCorrections
                          TtTRF
                          NNLOReweighter )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbToolManager _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHbbToolManager Root/*.cxx Root/*.h Root/*.icc
                   TTHbbToolManager/*.h TTHbbToolManager/*.icc TTHbbToolManager/*/*.h
                   TTHbbToolManager/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHbbToolManager
                   LINK_LIBRARIES TTHbbObjects
                                  TTHbbConfiguration
                                  MVAVariables
                                  TopFakes
                                  TRFFakes
                                  MCFakes
                                  BucketofTops
				  TTHbbTtbarCorrections
                                  NNLOReweighter
                                  TtTRF
                                  MCFakes
				  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

