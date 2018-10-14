# Auto-generated on: 2017-11-16 17:46:45.856220

# Declare the name of this package:
atlas_subdir( TTHbbReconstruction None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          TTHbbTruthHelper
                          TTHbbMVAInterfaces 
			  Tools/PathResolver)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics Minuit )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbReconstruction _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Install files
atlas_install_data( share/* )

# Build a library that other components can link against:
atlas_add_library( TTHbbReconstruction Root/*.cxx Root/*.h Root/*.icc
                   TTHbbReconstruction/*.h TTHbbReconstruction/*.icc TTHbbReconstruction/*/*.h
                   TTHbbReconstruction/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHbbReconstruction
                   LINK_LIBRARIES TTHbbObjects
                                  TTHbbTruthHelper
                                  TTHbbMVAInterfaces
				  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

