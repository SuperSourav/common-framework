# Auto-generated on: 2017-11-16 17:46:45.662651

# Declare the name of this package:
atlas_subdir( TRFFakes None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbConfiguration
                          TTHbbObjects
                          MVAVariables
                          CustomTRF )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TRFFakes _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TRFFakes Root/*.cxx Root/*.h Root/*.icc
                   TRFFakes/*.h TRFFakes/*.icc TRFFakes/*/*.h
                   TRFFakes/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TRFFakes
                   LINK_LIBRARIES TTHbbConfiguration
                                  TTHbbObjects
                                  MVAVariables
                                  CustomTRF
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

