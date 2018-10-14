#!/bin/bash
# Ian Connelly
# 8th March 2018
# We need to update regularly to the latest AnalysisTop release
# Preferable to do so in a conistent way to avoid any mistakes
# Using https://natelandau.com/bash-scripting-utilities/
# Font
bold=$(tput bold)
underline=$(tput sgr 0 1)
reset=$(tput sgr0)
# Colour
purple=$(tput setaf 171)
red=$(tput setaf 1)
green=$(tput setaf 34)
tan=$(tput setaf 208)
blue=$(tput setaf 38)
# Formatting
e_header() { printf "\n${bold}${purple}==========  %s  ==========${reset}\n" "$@" 
}
e_arrow() { printf "➜ $@\n"
}
e_success() { printf "${green}✔ %s${reset}\n" "$@"
}
e_error() { printf "${red}✖ %s${reset}\n" "$@"
}
e_warning() { printf "${tan}➜ %s${reset}\n" "$@"
}

# Functions
seek_confirmation() {
  printf "\n${bold}$@${reset}"
  read -p " (y/n) " -n 1
  printf "\n"
}

help() {
  e_header  "Script to update AnalysisTop versions"
  e_success "Usage: ./updateAnalysisTop.sh AT_VER1 AT_VER2"
  e_success "The script will check if AT_VER1 is used, and replace with AT_VER2"
  exit 1
}

# Input
AT_VER1=$1
AT_VER2=$2

# Check arguments are provided
if [ -z "$AT_VER1" ]; then
    help
elif [ -z "$AT_VER2" ]; then
    help
fi

# Files where we need to update the AnalysisTop version
places_to_check="CMakeLists.txt README.md Dockerfile .gitlab-ci.yml"

# Run a check first of all before doing any sed replacements
for F in ${places_to_check}; do
    e_warning "Checking $F for $AT_VER1"
    R=`grep $AT_VER1 $F`
    if [[ "$?" != "0" ]]; then
	e_error "Unable to find this version number, please intercede."
	exit 2
    fi
    e_success "Found $R"
done

# Passed our check, so now we can proceed
for F in ${places_to_check}; do
    e_warning "Replacing $AT_VER1 with $AT_VER2 in $F"
    # string concatanation to allow sed to function correctly
    sed -i -e 's/'"$AT_VER1"'/'"$AT_VER2"'/' $F
    if [[ "$?" != "0" ]]; then
        e_error "Error occured running sed command, please intercede."
        exit 3
    fi
    e_success "Update successful."
done
e_success "Script has finished running."