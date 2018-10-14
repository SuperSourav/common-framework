# README

This Readme is a work in project (if even) and should explain a bit more the structure of the package. Where and how you should add things when you want to do so.

## Subsidiarity

As you can see, the framework is split into 4 different folders. One of them - TTHbbAnalysis - even is a git submodule and links to our [fork of the TTHbbAnalysis framework](https://gitlab.cern.ch/atlasphys-top/xs/4tops/frameworks/TTHbbAnalysis). In there you can find a lot of nice things already shipped by their framework which we can completely and fully use for our purposes. The goal is NOT to change any code in that folder as any changes would result in constant merge issues with the TTHbb group.

Next to it are:
- SM4tops1LOS: All the files / tools only concerning the 1l OS channels that are of no use to the SS channel
- SM4topsCommon: All the files / tools that can potentially (or will surely) be used by all channels of the SM4top analysis
- SM4topsSS3L: All the files / tools that only concern the SS / Multilepton channels and not the 1l OS ones

The hope with this setting is that we can factorise the code as much as possible, while taking advantage of any synergies between the channels (and analyses) that might occur.

## General structure

Looking at `SM4topsCommon` as an example. We try to follow the structure of athena / AnalysisTop tools, where we separate the headers into a folder of the same name as the tool, `SM4topsCommon` in this case, the implementations as `.cxx` into a folder named `Root`. If there were any steering files, they would probably be put in a folder called `share`.

In the SM4topsCommon/SM4topsCommon folder you see already quite a few files. To name a few:
- `SM4topsCommonLoader`: Here you create the "link" between the AnalysisTop config file and your tool. If you want to create a new Keyword that can be used in that config file, it would be here.
- `SM4topsObjectLoader`: This will be made an available class for ROOT, therefore it needs a ClassDef and an entry in the `LinkDef.h` in the `Root` directory. This connects the object configurations of our config file with the athena code. Here we can choose which pieces of code we want to use for which objects. For example, we create our own Muon class in `SM4topsCommonMuon` which we tell the loader to use, whereas we stick with the official Electron class from AnalysisTop (for now). Any object related changes go here
- `*Selector`: The structure of these (and of the `*Decorator`) are all similar the naming originates more from the idea. In the selector class the importance conceptually lies on the boolean return value of the `apply` function, which decides whether or not an event passes the selection, whereas in the decorators the return should be mostly `true` and the importance lies on actually doing things with the object. So if you want to create a new complex cut, put a `Selector` at the end of the name, if you're trying to create new interesting variables, a `Decorator` is your choice.


By the way, for an interesting illustration on how you can add variables to the output in a brilliantly clean and automated way, have a look at:
```
Framework/TTHbbAnalysis/TTHbbLeptonic/Root/DummyVariableTool.cxx
```
