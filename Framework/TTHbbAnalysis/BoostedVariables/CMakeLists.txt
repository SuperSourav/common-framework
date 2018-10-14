# Auto-generated on: 2017-11-16 17:46:44.113363

# Declare the name of this package:
atlas_subdir( BoostedVariables None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          TTHbbToolManager 
			  Tools/PathResolver)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics XMLIO XMLParser )

# Build a library that other components can link against:
atlas_add_library( BoostedVariables Root/*.cxx Root/*.h Root/*.icc
                   BoostedVariables/*.h BoostedVariables/*.icc BoostedVariables/*/*.h
                   BoostedVariables/*/*.icc 
                   PUBLIC_HEADERS BoostedVariables
                   LINK_LIBRARIES TTHbbObjects
                                  TTHbbToolManager
				  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( data/* )

