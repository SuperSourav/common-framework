# Declare the name of this package:
atlas_subdir( SM4topsSS3L )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( SM4topsSS3L _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( SM4topsSS3L STATIC Root/*.cxx Root/*.h Root/*.icc
                  SM4topsSS3L/*.h SM4topsSS3L/*.icc SM4topsSS3L/*/*.h
                  SM4topsSS3L/*/*.icc ${_cintDictSource}
                  PUBLIC_HEADERS SM4topsSS3L
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
