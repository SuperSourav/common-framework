# Auto-generated on: 2017-11-16 17:46:44.349248

# Declare the name of this package:
atlas_subdir( OfflineTTHbb None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TopDataPreparation
                          TTHbbObjects
                          TTHbbConfiguration
                          TTHbbToolManager )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Build a library that other components can link against:
atlas_add_library( OfflineTTHbb Root/*.cxx Root/*.h Root/*.icc
                   OfflineTTHbb/*.h OfflineTTHbb/*.icc OfflineTTHbb/*/*.h
                   OfflineTTHbb/*/*.icc 
                   PUBLIC_HEADERS OfflineTTHbb
                   LINK_LIBRARIES TopDataPreparation
                                  TTHbbObjects
                                  TTHbbConfiguration
                                  TTHbbToolManager
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

# Build the executables of the package:
atlas_add_executable( ttH-offline
                      util/ttH-offline.cxx
                      LINK_LIBRARIES TopDataPreparation
                                     TTHbbObjects
                                     TTHbbConfiguration
                                     TTHbbToolManager
                                     ${ROOT_LIBRARIES}
                                     OfflineTTHbb )

