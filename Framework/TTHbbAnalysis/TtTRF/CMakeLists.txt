# Declare the name of this package:
atlas_subdir( TtTRF None )

# This package depends on other packages:
atlas_depends_on_subdirs( CustomTRF )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TtTRF _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TtTRF Root/*.cxx Root/*.h Root/*.icc
                   TtTRF/*.h TtTRF/*.icc TtTRF/*/*.h
                   TtTRF/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TtTRF
                   LINK_LIBRARIES CustomTRF 
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

