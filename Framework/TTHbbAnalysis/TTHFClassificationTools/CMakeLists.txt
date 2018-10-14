# Auto-generated on: 2017-11-16 17:46:43.799426

# Declare the name of this package:
atlas_subdir( TTHFClassificationTools None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          TTHbbTruthHelper )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHFClassificationTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHFClassificationTools Root/*.cxx Root/*.h Root/*.icc
                   TTHFClassificationTools/*.h TTHFClassificationTools/*.icc TTHFClassificationTools/*/*.h
                   TTHFClassificationTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHFClassificationTools
                   LINK_LIBRARIES TTHbbObjects
                                  TTHbbTruthHelper
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

