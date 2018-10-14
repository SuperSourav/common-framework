# Auto-generated on: 2017-11-16 17:46:42.435938

# Declare the name of this package:
atlas_subdir( TTHbbTruthHelper None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbTruthHelper _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHbbTruthHelper Root/*.cxx Root/*.h Root/*.icc
                   TTHbbTruthHelper/*.h TTHbbTruthHelper/*.icc TTHbbTruthHelper/*/*.h
                   TTHbbTruthHelper/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHbbTruthHelper
                   LINK_LIBRARIES TTHbbObjects
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

