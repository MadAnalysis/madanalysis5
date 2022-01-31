# MadAnalysis 5 Validation Interface

## Installation
`$ pip install -e .` or `$ make install`

# Usage

### Format of the MadAnalysis 5 Scripts
MadAnalysis scripts which are going to be validated can be found in `src/ma5_validation/ma5_scripts`. Each
has to follow certain configuration to be able to find the errors quickly.
```
#TITLE <title of the file>
#MODE <MA5 MODE PARTON HADRON RECO RECOFRAC>

####################################
#     Usual MadAnalysis commands   #
####################################
...
import $MA5PATH/samples/...
submit 
```
Usual MadAnalysis 5 commands needs to satisfy the usual conditions. `$MA5PATH` sets the anchor to current 
MadAnalysis 5 session, if not indicated code will assume that full path has been given for the sample. 
`submit` does not require any name, the name will be choosen according to the script name and the log path.
Default log path is `./scripts/log`; hence all the files and analysis folders will be generated at that 
location.