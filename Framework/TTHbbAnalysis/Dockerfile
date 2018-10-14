# See https://gitlab.cern.ch/akraszna/GitLabCITests/blob/master/Dockerfile for reference

# Set up the base image:
FROM atlas/analysistop:21.2.38

# Copy the project's sources into the image:
RUN echo $PWD

COPY CMakeLists.txt          source/CMakeLists.txt
COPY BTaggingTruthTagging    source/BTaggingTruthTagging
COPY BoostedVariables        source/BoostedVariables
COPY CustomTRF               source/CustomTRF
COPY DilepFakes              source/DilepFakes
COPY ExtFiles 		     source/ExtFiles
COPY HplusDiscriminant       source/HplusDiscriminant
COPY MCFakes                 source/MCFakes
COPY MVAVariables            source/MVAVariables
COPY NNLOReweighter          source/NNLOReweighter
COPY OfflineTTHbb            source/OfflineTTHbb
COPY PackageFilters          source/PackageFilters
COPY TRFFakes                source/TRFFakes
COPY TTHFClassificationTools source/TTHFClassificationTools
COPY TTHbbClassification     source/TTHbbClassification
COPY TTHbbConfiguration      source/TTHbbConfiguration
COPY TTHbbLeptonic           source/TTHbbLeptonic
COPY TTHbbMVAInterfaces      source/TTHbbMVAInterfaces
COPY TTHbbMatchingTools      source/TTHbbMatchingTools
COPY TTHbbObjects            source/TTHbbObjects
COPY TTHbbReconstruction     source/TTHbbReconstruction
COPY TTHbbTRFCalc            source/TTHbbTRFCalc
COPY TTHbbToolManager        source/TTHbbToolManager
COPY TTHbbTruthHelper        source/TTHbbTruthHelper
COPY TTHbbTtbarCorrections   source/TTHbbTtbarCorrections
COPY TtTRF          	     source/TtTRF

RUN ls -l source

# Build the project inside a build/ directory (needs to be in a single RUN command)
RUN mkdir build && cd build/ && \
    source /home/atlas/release_setup.sh && \
    echo `which top-xaod` && \
    cmake -DATLAS_PACKAGE_FILTER_FILE=../source/PackageFilters/online_packages.txt ../source/ && \
    cmake --build ./ && \
    cpack

# Install the created RPM as root:
USER root
RUN rpm -i /home/atlas/build/TTHbbAnalysis_*.rpm

# Clean up:
RUN rm -rf /home/atlas/source/ /home/atlas/build/

# Switch back to the ATLAS account:
USER atlas

# Set up the environment setup script:
RUN mv release_setup.sh analysis_release_setup.sh
COPY Docker/release_setup.sh /home/atlas/

# Start the image with BASH by default:
CMD /bin/bash
