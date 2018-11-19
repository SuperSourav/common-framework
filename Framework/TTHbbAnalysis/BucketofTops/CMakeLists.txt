# Auto-generated on: 2017-11-16 17:46:44.206512

# Declare the name of this package:
atlas_subdir( BucketofTops None )

atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          Tools/PathResolver )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( BucketofTops _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( BucketofTops Root/*.cxx Root/*.h Root/*.icc
                   BucketofTops/*.h BucketofTops/*.icc BucketofTops/*/*.h
                   BucketofTops/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS BucketofTops
                   LINK_LIBRARIES PathResolver
		   		  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

