# TTHbbAnalysis

The software for ttHbb in R21.

---

### Pipeline CI Status
We compile the online and offline projects with every commit. This badge shows the latest pipeline status.

[![pipeline status](https://gitlab.cern.ch/atlasHTop/TTHbbAnalysis/badges/master/pipeline.svg)](https://gitlab.cern.ch/atlasHTop/TTHbbAnalysis/commits/master)

### Pipeline CI Plots
Successful CI tests of the online code will now plot everything inside the output root file. These plots will be copied to a new service user web area (which is protected to be accessible to ATLAS users only) and will be viewable there. 

:frame_photo: :link: [TTHbbAnalysis CI Plot Monitor](https://atlas-project-tthbbanalysis.web.cern.ch/atlas-project-tthbbanalysis/gitlab/atlas-plots/index.php) :link: :frame_photo:

Please note that if you fork the repository, you will need to add two new Gitlab CI secret variables to your repository.

   TTHBBANA_USER
   TTHBBANA_PASSWORD

Please contact [Ian](mailto:ian.connelly@NOSPAM.cern.ch) to provide you with the login details if you encounter a problem with the CI tests.

---

### Communication

* JIRA: https://its.cern.ch/jira/browse/TTH
* Mailing list: atlas-phys-higgs-code-ttHbb@cern.ch
* Mattermost: https://mattermost.web.cern.ch/atlas-tthbb/channels/analysis-code [subscribe](https://mattermost.web.cern.ch/signup_user_complete/?id=oy5okypjc78mdn65ybgmfkw8te)

---

### Setup (assuming git has been used once already)

    setupATLAS
    lsetup git
    kinit <username>@CERN.CH
    mkdir MyDirectory
    cd MyDirectory
    mkdir build run
    git clone --recursive https://:@gitlab.cern.ch:8443/atlasHTop/TTHbbAnalysis.git
    cd build
    asetup AnalysisTop,21.2.38
    cmake  ../TTHbbAnalysis/
    cmake --build ./
    source */setup.sh

Warning: with these instructions above, we try to compile all packages. But it's advised to either compile for online, or for offline release separately, as explained below.

### Setting up packages purely for online release

Assuming you are inside a clean build directory

    cmake -DATLAS_PACKAGE_FILTER_FILE=../TTHbbAnalysis/PackageFilters/online_packages.txt ../TTHbbAnalysis
    cmake --build ./
    source */setup.sh


### Setting up packages purely for offline release

Assuming you are inside	a clean build directory

    cmake -DATLAS_PACKAGE_FILTER_FILE=../TTHbbAnalysis/PackageFilters/offline_packages.txt ../TTHbbAnalysis
    cmake --build ./
    source */setup.sh

### Making a feature branch

Inside this repository, everyone should be able to make branches. To keep track of changes, a branch should be named in a sensible fashion, and we have implemented some simple push rules. This should make it easier to know what a branch contains from a quick look, and also help track changes when they are merged into master.

To create a branch, use a command like:

    git branch user/iconnell/description
    git branch feature/TTH-123/description
    git branch bugfix/TTH-456/description

To switch to that branch, use a command like:

    git checkout user/iconnell/description

To stage changes to your local repository

    git add file1.h file2.cxx ...

To commit changes to your local repository

    git commit -m "Your useful commit message here"

To push your branch to the remote repository

    git push -u origin user/iconnell/description

If your branch is incorrectly named, you will get an error message when you try to push, and you should create a new branch, check it out, and then it to the remote repository

### Submodules

It is possible to link to other packages stored on gitlab using submodules.
We have a directory `Submodules` to hold any links we require.
Note that our build needs to be configured to use them if they are required by updating the regex for paths in `PackageFilters`.

To add a new submodule

git submodule add ../../atlas/athena Submodules/<reason_for_adding_this_submodule>
cd Submodules/<reason_for_adding_this_submodule>
git checkout <branch> OR <commit-hash>

Edit the package filters so that they pick up this module:

    emacs -nw PackageFilters/*
    ...
    + Submodules/<reason_for_adding_this_submodule>/<path_to_package>     
    - .*

To add to the repository

git add .gitmodules
git add Submodules/<reason_for_adding_this_submodule>
git commit -m "Adding a new submodule for XYZ reason"

To remove a submodule requires three steps, which should be executed carefully

1 git submodule deinit -f Submodules/<reason_for_adding_this_submodule>   
2 rm -rf .git/modules/Submodules/<reason_for_adding_this_submodule>
3 git rm Submodules/<reason_for_adding_this_submodule>	

To update submodules added to the repository

   git submodule update

### Merge Requests

These should be able to be approved by members of the ttHbb code mailing list (via the [TTHbbAnalysisCodeApproval Group](https://gitlab.cern.ch/TTHbbAnalysisCodeApproval)).

### Launching a production

This is still in development - see TTH-325.

Asuming you already did the setup of the project, do the following in the run directory:

    getTTHbbProdScripts

This should give you the relevant files. To launch a production you need to setup grid utilities. Do:

    source setup_production.sh

Or, if you have higgs production role and want to launch an official production:

    source setup_group_production.sh

Then, you can edit the list of samples, according to the format used in the spreadsheet (e.g. [this one used for the March 2018 production](https://espace.cern.ch/atlas-tthbb/_layouts/15/WopiFrame.aspx?sourcedoc=/atlas-tthbb/Shared%20Documents/Ntuple%20Production/July%202018%20production.xlsx&action=default)):

    Data_inputs.txt
    MC16a_inputs.txt
    MC16c_inputs.txt
    MC16d_inputs.txt

Finaly you need to edit the production script itself, and launch it:

    ./TTHbbSubmit.py

Hopefully the syntax is clear enough!

### Production streams

For an official production, each input sample is processed several times: we separate the output samples in different *streams*. In all cases, we run all selections (l+jets resolved and boosted, dilepton), but in the 1l (resp. 2l) streams we save events if they pass *at least* the l+jets (resp. dilepton) selection (although in principle the l+jets and dilepton selections are orthogonal). Then we have several variations which are explained below.

| Steam         | Saved if pass    | Systematics  | Further details |
| ------------- | ---------------- | ------------ | --------------- |
| 1l            | l+jets   | Nominal only | Both Data and MC |
| 1l_sys        | l+jets   | All except large-R jets (or tau) systs. | Only MC |
| 1l_lrj        | l+jets   | Only large-R jets systs. | Only MC |
| 1l_jer        | l+jets   | Only JER, Full smearing (11 NPs) | Both Data and MC |
| 1l_jfl        | l+jets   | Only jet flavour composition and response systs. | Only MC (not yet tested, not in official production) |
| 1l_trf        | l+jets   | Nominal only | Only MC (not yet tested, not in official production) |
| 1l_fakes      | l+jets   | Nominal only | Only Data, for fakes estimate with matri-method |
| 1l_ftrig      | l+jets   | Nominal only | Only Data, for fakes triggers studies, with looser Loose definition |
| 2l            | dilepton | Nominal only | Both Data and MC |
| 2l_sys        | dilepton | All except large-R jets (or tau) systs. | Only MC |
| 2l_jer        | dilepton | Only JER, Full smearing (11 NPs) | Both Data and MC |
| 2l_jfl        | dilepton | Only jet flavour composition and response systs. | Only MC (not yet tested, not in official production) |
| 2l_trf        | dilepton | Nominal only | Only MC (not yet tested, not in official production) |
| 0l            | allhad   | Nominal only | Both Data and MC |
| 0l_sys        | allhad   | All except large-R jets (or tau) systs. | Only MC |

### Fun

![Video made using gource](https://atlas-project-tthbbanalysis.web.cern.ch/atlas-project-tthbbanalysis/gource.webm)

