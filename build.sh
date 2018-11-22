#!/bin/bash

setupATLAS
mkdir build
cd build
asetup AnalysisTop,21.2.38
cmake -DATLAS_PACKAGE_FILTER_FILE=../Framework/TTHbbAnalysis/PackageFilters/offline_packages.txt ../Framework/TTHbbAnalysis
cmake --build ./
source */setup.sh
cd ..
