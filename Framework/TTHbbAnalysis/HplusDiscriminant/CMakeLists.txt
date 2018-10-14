# Auto-generated on: 2017-11-16 17:46:42.640840

# Declare the name of this package:
atlas_subdir( HplusDiscriminant None )

# This package uses ROOT:
find_package( ROOT COMPONENTS REQUIRED Core Gpad Tree Hist RIO MathCore Graf Physics )

atlas_depends_on_subdirs(PUBLIC
			 Tools/PathResolver)

# Custom definitions needed for this package:
add_definitions( -g )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( HplusDiscriminant _cintDictSource
                           ROOT_HEADERS HplusDiscriminant/*.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( HplusDiscriminant Root/*.cxx Root/*.h Root/*.icc
                   HplusDiscriminant/*.h HplusDiscriminant/*.icc HplusDiscriminant/*/*.h
                   HplusDiscriminant/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS HplusDiscriminant
		   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES PathResolver
		   		  ${ROOT_LIBRARIES})

# Install data files from the package:
atlas_install_data( data/* )
