# MadAnalysis 5 Validation Interface

## Installation
`$ pip install -e .` or `$ make install`

## Quick Validation

The validation script is located in `scripts/validation_bootstrap`. To see available options simply type
```bash
$ ./validation_bootstrap -h
```
This will show command line arguments to validate your MadAnalysis session. For instance, to validate MadAnalysis
Parton level mode simply type
```bash
$ ./validation_bootstrap -P
```

## Extended validation
`validation_bootstrap` uses the samples within `madanalysis5/samples`. These are small samples to ensure MadAnalysis 
executes analysis successfuly. By adding larger MC samples to this folder one can enable more extensive validation of 
the software.

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

#### Script keywords
```
#TITLE: Title of the script
#MODE : MadAnalysis 5 execution mode
#CPP : expert mode cpp file
#HEADER: expert mode header file
#COMMANDLINE: expert mode command line info

$MA5PATH : Will be replaced with MadAnalysis 5 path
$SMP_PATH: will be replaced with full path to `madanalysis5/path`
$EXPERT_LEVEL_PATH: will be replaced with full path for the expert level script location
```

### Validating MadAnalysis 5
Validation scripts are available under `scripts` folder. `validation_bootstrap` includes options to validate
various level of analyses available withing MadAnalysis interface. For more information about the inner workings
of the script simpy type `$ ./validation_bootstrap -h`. 

## Adding validation scripts

Each validation script needs to be written in the format [shown above](#format-of-the-madanalysis-5-scripts). 
Depending on the mode and nature of the validation script, it can be added to the script collection under 
`validation/src/ma5_validation/ma5_scripts`. This folder includes various levels of scripts that ensures backwards
compatibility of MadAnalysis 5 software. Before adding the validation script please run it via `validation_bootstrap`
by using the following command;

```
$ ./validation_bootstrap --custom-script /PATH/TO/MY/SCRIPT/my_script.ma5
```