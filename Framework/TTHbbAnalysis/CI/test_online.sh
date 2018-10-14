#!/bin/bash
# Script to run the ttH online code
# Ian Connelly
# 15 Jan 2018

# Take the cutfile as input from the CI
CUT=$1
ANALYSISTOP_VERSION=$2

# List the directory
ls -l

# Change into the build directory and source
cd build_online

set +e
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
asetup AnalysisTop,${ANALYSISTOP_VERSION}
source */setup.sh
set -e

# Make a unique directory to run in
mkdir ../run_${CUT} && cd ../run_${CUT}

# Get ReleaseData
cp $AnalysisTop_DIR/ReleaseData ReleaseData
cat ReleaseData

# Use the latest file when available with 21.2.14
TOPQ1="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/ContinuousIntegration/MC/p3390/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e6337_e5984_s3126_r9364_r9315_p3390/DAOD_TOPQ1.12720282._000339.pool.root.1"

# Make input file
echo $TOPQ1 > input.file
# Copy the cut file locally
cp $TTHbbAnalysis_DIR/data/TTHbbLeptonic/${CUT}.txt ./
(echo "NEvents 1000" && cat ${CUT}.txt) > ${CUT}.txt.tmp
mv ${CUT}.txt.tmp ${CUT}.txt
# Print this file
cat ${CUT}.txt
# Run top-xaod with TTHbbAnalysis configuration
top-xaod ${CUT}.txt input.file

if [[ $? == 0 ]]; then
    # Work out if this is a merge request (assume the work merge in the comment) and append to directory name
    SUFFIX=""
    git show --name-status
    # I think I need to stop it throwing an error if grep doesn't find a result
    set +e
    # -c means count -> if 0, not found
    RES=`git show --name-status | grep -c -i merge`
    set -e
    # Mark folder if it was a merged request
    if [[ "$RES" == "0" ]]; then
	SUFFIX=""
    else
	SUFFIX="_(M)"
    fi
    # Copy the files to the web area using the CI variable
    dateNOW=`date +%Y-%m-%d`
    # Get the time from the build_online job which is persited in artifact and fixed for all tests
    timeNOW=`cat ../build_online/build_time`
    outDIR=${dateNOW}"/"${timeNOW}_${GITLAB_USER_LOGIN}_${CI_JOB_NAME}${SUFFIX}
    mkdir -p ${outDIR}
    # Write out some git info to info.txt
    echo "Successful test of commit: ${CI_COMMIT_REF_NAME} #${CI_COMMIT_SHA} </br>" > ${outDIR}/info.txt
    git show --name-status >> gitlog
    # Format the git logging information
    while read line; do echo "$line </br>" >> ${outDIR}/info.txt; done < "gitlog"
    # Find everything and plot
    python ../CI/drawTTHOutFile.py out.root
    mv *.png ${outDIR}
    # Parse and copy release data
    while read line; do echo "$line </br>" >> ${outDIR}/ReleaseData; done < "ReleaseData"
    tar -C ./ -jcf - ${dateNOW} | ssh ${TTHBBANA_USER}@lxplus.cern.ch 'tar -C /eos/user/t/tthbbana/www/gitlab/pages/plots/ -jxf -'
fi