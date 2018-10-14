# Auto-generated on: 2017-11-16 17:46:44.295896

# Declare the name of this package:
atlas_subdir( TTHbbMatchingTools None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          TTHbbTruthHelper )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbMatchingTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHbbMatchingTools Root/*.cxx Root/*.h Root/*.icc
                   TTHbbMatchingTools/*.h TTHbbMatchingTools/*.icc TTHbbMatchingTools/*/*.h
                   TTHbbMatchingTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHbbMatchingTools
                   LINK_LIBRARIES TTHbbObjects
                                  TTHbbTruthHelper
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

