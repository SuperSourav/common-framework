# Auto-generated on: 2017-11-16 17:46:45.487214

# Declare the name of this package:
atlas_subdir( TTHbbClassification None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          TTHbbTruthHelper
                          TTHbbMVAInterfaces
                          MVAVariables )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbClassification _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHbbClassification Root/*.cxx Root/*.h Root/*.icc
                   TTHbbClassification/*.h TTHbbClassification/*.icc TTHbbClassification/*/*.h
                   TTHbbClassification/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHbbClassification
                   LINK_LIBRARIES TTHbbObjects
                                  TTHbbTruthHelper
                                  TTHbbMVAInterfaces
                                  MVAVariables
                                  TTHbbObjects
                                  TTHbbTruthHelper
                                  TTHbbMVAInterfaces
                                  MVAVariables
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

