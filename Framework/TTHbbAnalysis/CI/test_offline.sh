#!/bin/bash
# Script to run the ttH offline code
# Ian Connelly
# 15 Jan 2018

# Take the cutfile as input from the CI
ONLINECUT=$1
ANALYSISTOP_VERSION=$2
OFFLINECONFIG=$3

# List the directory
ls -l

# Change into the build directory and source
cd build_offline

set +e
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
asetup AnalysisTop,${ANALYSISTOP_VERSION}
source */setup.sh
set -e

# Change into the directory from the online run
cd ../run_${ONLINECUT}
ls -l
echo "./out.root" > onlineoutput.file

# Get the configuration file
cp $TTHbbAnalysis_DIR/data/OfflineTTHbb/${OFFLINECONFIG} ./

# Run the offline code on the output of the online test
ttH-offline ${OFFLINECONFIG} onlineoutput.file