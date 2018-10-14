# Auto-generated on: 2017-11-16 17:46:44.171533

# Declare the name of this package:
atlas_subdir( TTHbbObjects None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TopDataPreparation 
			  Tools/PathResolver)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Build a library that other components can link against:
atlas_add_library( TTHbbObjects Root/*.cxx Root/*.h Root/*.icc
                   TTHbbObjects/*.h TTHbbObjects/*.icc TTHbbObjects/*/*.h
                   TTHbbObjects/*/*.icc 
                   PUBLIC_HEADERS TTHbbObjects
                   LINK_LIBRARIES TopDataPreparation	
		   		  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

