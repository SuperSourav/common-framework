#!/bin/bash

setupATLAS
mkdir build
cd build
asetup AnalysisTop,21.2.38
cmake -DATLAS_PACKAGE_FILTER_FILE=../Framework/TTHbbAnalysis/PackageFilters/offline_packages.txt ../Framework/TTHbbAnalysis
cmake --build ./
source */setup.sh
cd ..

mkdir run-offline
cd run-offline
ttH-offline configuration.txt list-input.txt

#$ cat list-input.txt 
#/eos/atlas/atlascerngroupdisk/phys-top/4tops/ljetsOS/cutflow/v1/user.sberlend.410080.MadGraphPythia8EvtGen.DAOD_TOPQ1.e4111_e5984_s3126_r9781_r9778_p3390.AZ_mc16c_test10_output.root/user.sberlend.14095570._000001.output.root
