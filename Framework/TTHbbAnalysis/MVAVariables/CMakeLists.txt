# Auto-generated on: 2017-11-16 17:46:46.061907

# Declare the name of this package:
atlas_subdir( MVAVariables None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Math Matrix TMVA Graf Physics )

# Build a library that other components can link against:
atlas_add_library( MVAVariables Root/*.cxx Root/*.h Root/*.icc
                   MVAVariables/*.h MVAVariables/*.icc MVAVariables/*/*.h
                   MVAVariables/*/*.icc 
                   PUBLIC_HEADERS MVAVariables
                   LINK_LIBRARIES TTHbbObjects
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

