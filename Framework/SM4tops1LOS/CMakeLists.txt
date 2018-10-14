# Declare the name of this package:
atlas_subdir( SM4tops1LOS )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( SM4tops1LOS _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( SM4tops1LOS STATIC Root/*.cxx Root/*.h Root/*.icc
                  SM4tops1LOS/*.h SM4tops1LOS/*.icc SM4tops1LOS/*/*.h
                  SM4tops1LOS/*/*.icc ${_cintDictSource}
                  PUBLIC_HEADERS SM4tops1LOS
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
