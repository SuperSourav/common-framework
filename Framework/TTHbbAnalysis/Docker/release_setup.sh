#
# Environment configuration file setting up the installed GitLabCITests project.
#

# Set up the base environment using the base image's setup script:
source ~/analysis_release_setup.sh

# Set up the GitLabCITests installation:
source /usr/TTHbbAnalysis/*/InstallArea/*/setup.sh
echo "Configured TTHbbAnalysis from: ${TTHbbAnalysis_DIR}"

# Set up the prompt:
export PS1='\[\033[01;35m\][bash]\[\033[01;31m\][\u TTHbbAnalysis-$TTHbbAnalysis_VERSION]\[\033[01;34m\]:\W >\[\033[00m\] ';
