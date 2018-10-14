# Auto-generated on: 2017-11-16 17:46:44.452812

# Declare the name of this package:
atlas_subdir( NNLOReweighter None )


atlas_depends_on_subdirs(PUBLIC
                         Tools/PathResolver)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )


# Generate a CINT dictionary source file:
atlas_add_root_dictionary( NNLOReweighter _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( NNLOReweighter Root/*.cxx Root/*.h Root/*.icc
                   NNLOReweighter/*.h NNLOReweighter/*.icc NNLOReweighter/*/*.h
                   NNLOReweighter/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS NNLOReweighter
                   LINK_LIBRARIES PathResolver
		   		  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( data/* )

