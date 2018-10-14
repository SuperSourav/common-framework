# Auto-generated on: 2017-11-16 17:46:44.349248

# Declare the name of this package:
atlas_subdir( OfflineDileptonVariables None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
			  TTHbbObjects 
			  TTHbbConfiguration 
			  MVAVariables 
			  TTHbbToolManager 
			  NNLOReweighter 
			  TTHbbClassification 
			  TTHbbMVAInterfaces)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

atlas_add_root_dictionary( OfflineDileptonVariables _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( OfflineDileptonVariables Root/*.cxx Root/*.h Root/*.icc
                   OfflineDileptonVariables/*.h OfflineDileptonVariables/*.icc OfflineDileptonVariables/*/*.h
                   OfflineDileptonVariables/*/*.icc ${_cintDictSource}
                   PUBLIC_HEADERS OfflineDileptonVariables
                   LINK_LIBRARIES PUBLIC
		   		  TTHbbObjects
		   		  TTHbbConfiguration 
				  MVAVariables	   
				  TTHbbToolManager   
				  NNLOReweighter
				  TTHbbClassification
				  TTHbbMVAInterfaces
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

