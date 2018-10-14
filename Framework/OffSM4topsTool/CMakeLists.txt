
# Declare the name of this package:
atlas_subdir( OffSM4topsTool None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
			  TTHbbObjects 
			  TTHbbConfiguration 
			  TTHbbToolManager )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( OffSM4topsTool _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( OffSM4topsTool Root/*.cxx Root/*.h Root/*.icc
                   OffSM4topsTool/*.h OffSM4topsTool/*.icc OffSM4topsTool/*/*.h
                   OffSM4topsTool/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS OffSM4topsTool
                   LINK_LIBRARIES PUBLIC
		   		  TTHbbObjects
		   		  TTHbbConfiguration 
				  TTHbbToolManager
				  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

