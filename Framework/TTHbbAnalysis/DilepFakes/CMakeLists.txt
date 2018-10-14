# Auto-generated on: 2017-11-16 17:46:44.247204

# Declare the name of this package:
atlas_subdir( DilepFakes None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TTHbbObjects
                          MCFakes )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( DilepFakes _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( DilepFakes Root/*.cxx Root/*.h Root/*.icc
                   DilepFakes/*.h DilepFakes/*.icc DilepFakes/*/*.h
                   DilepFakes/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS DilepFakes
                   LINK_LIBRARIES TTHbbObjects
                                  MCFakes
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

