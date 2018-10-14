# Auto-generated on: 2017-11-16 17:46:44.211230

# Declare the name of this package:
atlas_subdir( TTHbbConfiguration None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
			  Tools/PathResolver )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Build a library that other components can link against:
atlas_add_library( TTHbbConfiguration Root/*.cxx Root/*.h Root/*.icc
                   TTHbbConfiguration/*.h TTHbbConfiguration/*.icc TTHbbConfiguration/*/*.h
                   TTHbbConfiguration/*/*.icc 
                   PUBLIC_HEADERS TTHbbConfiguration
                   LINK_LIBRARIES TTHbbObjects
		   		  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

