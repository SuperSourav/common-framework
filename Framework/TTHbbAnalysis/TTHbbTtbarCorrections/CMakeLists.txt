# Auto-generated on: 2017-11-16 17:46:44.206512

# Declare the name of this package:
atlas_subdir( TTHbbTtbarCorrections None )

atlas_depends_on_subdirs(PUBLIC
			 Tools/PathResolver)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TTHbbTtbarCorrections _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TTHbbTtbarCorrections Root/*.cxx Root/*.h Root/*.icc
                   TTHbbTtbarCorrections/*.h TTHbbTtbarCorrections/*.icc TTHbbTtbarCorrections/*/*.h
                   TTHbbTtbarCorrections/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TTHbbTtbarCorrections
                   LINK_LIBRARIES PathResolver
		   		  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

