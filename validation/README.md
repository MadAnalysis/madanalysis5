# MadAnalysis 5 Validation Interface

## Installation
`$ pip install -e .` or `$ make install`

## Quick Validation

The validation script is located in `scripts/validation_bootstrap`. To see available options simply type
```bash
$ ./validation_bootstrap -h
```
This will show command line arguments available to validate your MadAnalysis 5
session. For instance, to validate your version of MadAnalysis 5 in the parton
level mode, it is sufficient to type
```bash
$ ./validation_bootstrap -P
```

## Extended validation
`validation_bootstrap` uses test Monte Carlo samples located in
`madanalysis5/samples`. These are small-size event samples allowing us to ensure
that MadAnalysis 5 executes an analysis successfuly. By manally adding larger MC
samples to this folder we can enable a more extensive validathe software.

# Usage

### Format of the MadAnalysis 5 Scripts
MadAnalysis 5 scripts which are going to be used for the validation process can
be found in `src/ma5_validation/ma5_scripts`. Each has to follow a certain
structure so that we could find errors quickly.
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
Usual MadAnalysis 5 commands needs to follow the standard syntax. `$MA5PATH`
is an anchor to the current MadAnalysis 5 location. If not indicated, the code
assumes that a full path to the sample is given. `submit` does not require any
name, the folder name being choosen according to the script name and the path to
the log. The default log path is `./scripts/log`. All files and analysis folders
are thus generated at that location.

#### Script keywords
```
#TITLE: Title of the script
#MODE : MadAnalysis 5 execution mode
#CPP : expert mode cpp file
#HEADER: expert mode header file
#COMMANDLINE: expert mode command line info

$MA5PATH : Will be replaced with the path to MadAnalysis 5
$SMP_PATH: will be replaced with the full path to `madanalysis5/path`
$EXPERT_LEVEL_PATH: will be replaced with the full path for the expert-level
                    script location
```

### Validating MadAnalysis 5
Validation scripts are available under the `scripts` folder.
`validation_bootstrap` includes options to validate the various levels of
analyses available withing MadAnalysis 5. For more information about the inner
workings of the validation script, please type `$ ./validation_bootstrap -h`. 

## Adding validation scripts

Each validation script needs to be written in the format [shown above](#format-of-the-madanalysis-5-scripts). 
Depending on the mode and nature of the validation script, it can be added to the script collection stored in
`validation/src/ma5_validation/ma5_scripts`. This folder includes various levels of scripts that ensures backwards
compatibility of the MadAnalysis 5 software. Before adding a validation script there, please run it via `validation_bootstrap`
by using the following command;

```
$ ./validation_bootstrap --custom-script /PATH/TO/MY/SCRIPT/my_script.ma5
```
