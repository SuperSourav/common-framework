# Declare the name of this package:
atlas_subdir( FourTopTools None )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( FourTopTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( FourTopTools STATIC Root/*.cxx Root/*.h Root/*.icc
                  FourTopTools/*.h FourTopTools/*.icc FourTopTools/*/*.h
                  FourTopTools/*/*.icc ${_cintDictSource}
                  PUBLIC_HEADERS FourTopTools
                  LINK_LIBRARIES AsgTools
                                 TopEvent
                                 TopConfiguration
                                 TopAnalysis
                                 TopEventSelectionTools
                                 TTHbbObjects
                                 TTHbbLeptonic
                                 PathResolver
                                 ${ROOT_LIBRARIES}
                  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )
