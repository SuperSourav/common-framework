# ljetsOS-framework


## Introduction

This project contains the framework of the SM4tops analysis for the ntuples production for both 1L/2LOS and 2LSS/3L channels.

It is currently structured as follows:
* *Framework*: Directory containing the TTHbb framework and the SM4tops-related packages.
* *preProd*: Python scripts and config files to be used for the ntuple pre-production.
* *postProd*: Python scripts and config files to be used for the ntuple post-production.

Throughout this documentation, you'll see certain example commands followed by the commented string `!!`. This allows the CI of this repository to both test that the instructions are correct and that the code compiles when changes are made. Feel free to ignore these strings as you set the code up yourself!

## Useful links

* SM4tops twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/SMFourTopsXS13TeV
* TTHbb git page: https://gitlab.cern.ch/atlasHTop/TTHbbAnalysis/
* Release 21 software tutorial: https://atlassoftwaredocs.web.cern.ch/ABtutorial/
* AnalysisTop tutorial: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AnalysisTopTutorials
* Git tutorial: https://www.atlassian.com/git/


## Download the framework

The framework can be downlaoded as follows:
```
lsetup git || echo ignore alrb
git clone --recursive ssh://git@gitlab.cern.ch:7999/atlasphys-top/xs/4tops/frameworks/common-framework.git
cd common-framework/ #!!
```
Assuming that your private ssh key was properly uploaded in gitlab (go to [settings](https://gitlab.cern.ch/profile/keys/)
if this is not the case).
The option `--recursive` will automatically download the submodules of the repository (e.g. TTHbbAnalysis).

If TTHbbAnalysis module wasn't properly downloaded, you can retry it manually with the following commands:
```
git submodule init
git submodule update
```

In order for the CI in this repository to run successfully, the following command must be run in order to remove the extra top-level `CMakeLists.txt` from within the `TTHbbAnalysis` framework. You can compile and run locally without running this yourself, as the error is produces is benign.

```
rm Framework/TTHbbAnalysis/CMakeLists.txt #!!
```

---

## Ntuple Pre-production

The ntuple pre-production refers to the production of the MC and data ntuples from the DAOD derivations.
It includes the extraction of the objects needed for the analysis (e.g. electrons, muons, jets, etc) using
the object definitions specified in the config files. It also extracts various useful variables needed for
the different studies (e.g. Monte-Carlo truth informations).
The events are also pre-skimmed according to the pre-selections specified in the config files.

The TTHbb framework is used here as a submodule. To compile it for the first time, you can do the following
commands **inside the common-framework/ directory**:
```
mkdir build #!!
cd build #!!
asetup AnalysisTop,21.2.38 || echo ignore alrb #!!
cmake ../Framework #!!
cmake --build ./ #!!
source */setup.sh   #!!
```
The AnalysisTop version needs to be updated with the one specified in the README file inside the TTHbbAnalysis
subdirectory. If the compilation fails, try to do `cmake --build ./` several times. If you want to pass commands to the native compiler (e.g. the numbers of cores used for the compilation) you can do `cmake --build ./ -- -j N` where N is the number of cores. Alternatively you could invoke the compiler more directly by doing `make -j N` instead.

One can then simply set the environment back up by doing the following:
```
cd build/
asetup --restore
source */setup.sh
```

The python script and the config files for the ntuple pre-production are kept inside the `preProd`
subdirectory. It includes:
* *cuts*SM4tops.txt*: the config files used for the SM4tops analysis.
* *mc16*_SM4tops.py*: list of the Monte-Carlo samples used for the ntuples production.
* *data_SM4tops.py*: list of the data samples used for the ntuples production.
* *01SubmitToGrid_simple.py*: scripts used to launch ntuples pre-production in the grid.


If you want to produce ntuples locally, you can do the following command:
```
cd preProd/
top-xaod <config> <list-input-files>
```
`<config>` should be replaced by the name of your config file (e.g. cuts_mc16a_SM4tops.txt), and
`<list-input-files>` should be replaced by either the name on an input DAOD file or the name of
a txt file that contains a list of input DAOD file names.

If you want to launch the ntuple production in the grid, you can use the script `01SubmitToGrid_simple.py`.
You would have to modify it with your own grid username (*gridUsername*), the destination of the grid site
to store the output ntuples (*destSE*, leave that empty to use the scratch area), your config files
(*settingsFile*), the suffix of the output datatsets (*suffix*) and the MC/data samples to process.

---

## Ntuple Post-production

The ntuple post-production refers to the generation of mini-ntuples for dedicated analysis using the pre-produced
ntuple in input. This includes the addition of new variables from dedicated tools (BDTs, Fake, ttTRF, etc) and
some extra-skimming steps.

For the moment, no official code is supported, but your can use the offline code of the TTHbbAnalysis framework.
First, you need to compile the code as follows:
```
mkdir build
cd build
asetup AnalysisTop,21.2.38
cmake -DATLAS_PACKAGE_FILTER_FILE=../Framework/TTHbbAnalysis/PackageFilters/offline_packages.txt ../Framework/
cmake --build ./
source */setup.sh
```

Then, you can  produce the mini-ntuples using the following command:
```
cd run/
ttH-offline configuration.txt <list-input-files>
```
Examples of config scripts are avaialable in `postProd/` subdirectory or in Framework/TTHbbAnalysis/OfflineTTHbb/share/.
The required config files are 'weights.txt', 'VariablesToRead.txt', 'VariablesToSave.txt' and 'configuration.txt'


---

## Framework code modifications

To make any modification, you need to follow the system of branches explained in the
[TTHbb repository](https://gitlab.cern.ch/atlasHTop/TTHbbAnalysis/):

* At first pull the latest version of `master`, so you're sure you have a good basis: `git pull origin master`
* Create a branch and switch to it:  `git checkout -b user/<username>/description`
* Stage your modifications: `git add <file>` or `git add .` or `git add -a` and so on
* Commit your modifications: `git commit -m "useful message"`
* Push your branch to the remote repository: `git push -u origin user/username/description`

Once you're done with your modifications and you tested it locally, you can then request to merge the
branch into the master branch using the gitlab web interface.

in my (@lrustige) opinion you should create a merge request with WIP: in front of the title as soon as possible so that other people can already see and discuss the changes being made. This reduces merge problems and it's generally a nice starting point for good discussions.

**Important**: since we are trying to separate the SM4tops code from the TTHbb framework as much as possible in order to be as independent as possible from the TTHbb team, we ask every analyzer to modify the TTHbb framework as little as possible, preferably not at all.

## Updating the TTHbb submodule

In the current set-up, the TTHbb submodule points to our fork of that repository, so we are responsible for updating it, although it is not necessary to do this too often. There are multiple workflows of how to do it..here's the one I've been using:

1. get into the right directory (so that we're using the git of the submodule): `cd common-framework/Framework/TTHbbAnalysis`
2. check that you've already added the "upstream" original package (only once) `git remote -v`, which should result in
```
origin	ssh://git@gitlab.cern.ch:7999/atlasphys-top/xs/4tops/frameworks/TTHbbAnalysis.git (fetch)
origin	ssh://git@gitlab.cern.ch:7999/atlasphys-top/xs/4tops/frameworks/TTHbbAnalysis.git (push)
upstream	ssh://git@gitlab.cern.ch:7999/atlasHTop/TTHbbAnalysis.git (fetch)
upstream	ssh://git@gitlab.cern.ch:7999/atlasHTop/TTHbbAnalysis.git (push)
```
If you're missing the `upstream` part, do `git remote add upstream ssh://git@gitlab.cern.ch:7999/atlasHTop/TTHbbAnalysis.git`.

3. We want to update master so let's go there: `git checkout master`
4. Update it: `git pull upstream master`. This should pull everything nicely from the original TTHbb master branch, which will **very likely** result in a merge conflict which we need to resolve because we changed the CMakeLists a bit. Resolve the merge conflict in your preferred way [like so](https://help.github.com/articles/resolving-a-merge-conflict-using-the-command-line/) or through gitLab, for example.
5. Make sure the merged and updated master is in our remote fork (on gitlab), so if you resolved the merge conflict locally, you need to push the changes to `origin`: `git push origin master`, if you did it through gitLab it should already be there.
6. Create a merge request to merge the newly updated master with the branch we're actually using `group/4Top/master`. (This step might be removed as we might just use the master branch directly in the future)
7. Checkout and pull the group branch locally: `git checkout group/4Top/master` and `git pull origin group/4Top/master`
8. Done

This sounds fairly "elaborate", but it really does not take much time and might even be simplified quite a bit in the future. On the other hand, this allows us to be very independent from the TTHbb team, do all the merge conflict resolution on our side and it allows us to use their framework without us disturbing them.

<!--
** Important **: Because we use the TTHbbAnalysis framework as a submodule, one need to keep in mind that all
the commits will be separate based on which repository they are applied to. That means that any modification
inside the TTHbb framework will be propagated in the TTHbb repository, not in the 4tops repository.

One can update the TTHbb submodule to a newer version inside the 4tops repository. To do so, just pull the git
project inside the TTHbbAnalysis subdirectory. Then go to the parent directory and commit the update by doing
`git add TTHbbAnalysis`.

You can also modify the TTHbb framework by creating a branch as described above, inside the TTHbbAnalysis
subdirectory. The merging request would then be approved by the TTHbb team.
-->
