# Auto-generated on: 2017-11-16 17:46:44.208876

# Declare the name of this package:
atlas_subdir( CustomTRF None )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( CustomTRF _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( CustomTRF Root/*.cxx Root/*.h Root/*.icc
                   CustomTRF/*.h CustomTRF/*.icc CustomTRF/*/*.h
                   CustomTRF/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS CustomTRF
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

