# Context

**Remove this file once the branch is merged to the main branch**

This branch is dedicated to implement substructure tools to MadAnalysis framework.

# Description of the change and benefits

### Interface updates

- MultiJet clustering : `madanalysis/jet_clustering/.`

`jet_algorithm`: This module is separate from the original jet clustering interface within Ma5. 
This module can be activated using following command
```
ma5> define jet_algorithm my_jet antikt radius=0.5
```
where `my_jet` is a user-defined jet identifier, `antikt` is the algorithm to be used which
can be choosen from `antikt`, `cambridge`, `genkt`, `kt`, `gridjet`, `cdfjetclu`, `cdfmidpoint`,
and `siscone`. Rest of the arguments are optional, if user won't define radius, ptmin etc.
default parameters will be choosen. Each algorithm has its own unique set of parameters i.e.

|       Algorithm       | Parameters & Default values                                                        |
|:---------------------:|------------------------------------------------------------------------------------|
| `antikt`, `cambridge` | `radius=0.4`, `ptmin=5.`                                                           |
|        `genkt`        | `radius=0.4`, `ptmin=5.`, `exclusive=False`, `p=-1`                                |
|         `kt`          | `radius=0.4`, `ptmin=5.`, `exclusive=False`                                        |
|       `gridjet`       | `ymax=3.`, `ptmin=5.`                                                              |
|      `cdfjetclu`      | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`                    |
|     `cdfmidpoint`     | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`, `areafraction=1.` |
|       `siscone`       | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `input_ptmin=5.`, `npassmax=1.`           |

It is also possible to modify the entry after defining it

```
ma5> define jet_algorithm my_jet cambridge
ma5> set my_jet.ptmin = 200.
ma5> set my_jet.radius = 0.8
```
Note that when a `jet_algorithm` is defined MadAnalysis interface will automatically swithch
to constituent smearing mode. `set my_jet.+<tab>` will show the dedicated options available
for that particular algorithm.

It is possible to display all the jets available in the current session by using `display jet_algorithm`
command:
```
$ ./bin/ma5 -R
ma5>set main.fastsim.package = fastjet
ma5>define jet_algorithm my_jet cdfmidpoint
ma5>display jet_algorithm
MA5: * Primary Jet Definition :
MA5:  fast-simulation package : fastjet
MA5:  clustering algorithm : antikt
MA5:   + Jet ID : Ma5Jet
MA5:   + cone radius = 0.4
MA5:   + PT min (GeV) for produced jets = 5.0
MA5:   + exclusive identification = true
MA5:   + b-jet identification:
MA5:     + DeltaR matching = 0.5
MA5:     + exclusive algo = true
MA5:     + id efficiency = 1.0
MA5:     + mis-id efficiency (c-quark)      = 0.0
MA5:     + mis-id efficiency (light quarks) = 0.0
MA5:   + hadronic-tau identification:
MA5:     + id efficiency = 1.0
MA5:     + mis-id efficiency (light quarks) = 0.0
MA5:    --------------------
MA5: * Other Jet Definitions:
MA5:    1. Jet ID = my_jet
MA5:       - algorithm       : cdfmidpoint
MA5:       - radius          : 0.4
MA5:       - ptmin           : 5.0
MA5:       - overlap         : 0.5
MA5:       - seed            : 1.0
MA5:       - iratch          : 0.0
MA5:       - areafraction    : 1.0
```
Here primary jet is defined with the original jet definition syntax of MadAnalysis 5 where
since we did not specify anything, it uses default `antikt` configuration. For more info on
how to define primary jet see [arXiv:2006.09387](https://arxiv.org/abs/2006.09387). Other jet
definitions shows all the jets which are defined via `jet_algorithm` keyword.

To remove a `jet_algorithm` definition one can use `remove my_jet` command. Note that one can also
change the name of the primary jet which is `Ma5Jet` by default.
```
ma5>set main.fastsim.JetID = my_primary_jet
```
**Link to SFS:** There can only be one jet smearing/tagging definition where in case of existing
multi-jet definitions smearing will be applied in constituent level which are used by all jets defined
in the framework. Jet tagging is only available for primary jet.

### Updates in expert mode structure
Expert mode is designed to automatically realize and construct the MadAnalysis framework according to 
the given `.ma5` command script. This command script can include all the SFS construction mentioned in
[arXiv:2006.09387](https://arxiv.org/abs/2006.09387) and it can also include multijet definitions

`sfs_card_cms_exo_xx_yy.ma5`:
```
set main.fastsim.package = fastjet

# Define multijet
define jet_algorithm AK08 antikt
set AK08.radius = 0.8
set AK08.ptmin = 200
define jet_algorithm CA15 cambridge radius=1.5 ptmin=200.0

# Define Jet reconstruction efficiencies
define reco_efficiency j 0.925 [abseta <= 1.5]
define reco_efficiency j 0.875 [abseta > 1.5 and abseta <= 2.5]
define reco_efficiency j 0.80  [abseta > 2.5]

# Define Jet smearer
define smearer j with PT sqrt(0.06^2 + pt^2*1.3e-3^2) [abseta <= 0.5 and pt > 0.1]
define smearer j with PT sqrt(0.10^2 + pt^2*1.7e-3^2) [abseta > 0.5 and abseta <= 1.5 and pt > 0.1]
define smearer j with PT sqrt(0.25^2 + pt^2*3.1e-3^2) [abseta > 1.5 and abseta <= 2.5 and pt > 0.1]

# Define B-tagging
define tagger j as b 0.01+0.000038*pt
define tagger c as b 0.25*tanh(0.018*pt)/(1.0+ 0.0013*pt)     [abseta < 2.5]
define tagger c as b 0.0                                      [abseta >=2.5]
define tagger b as b 0.85*tanh(0.0025*pt)*(25.0/(1+0.063*pt)) [abseta < 2.5]
define tagger b as b 0.0                                      [abseta >= 2.5]
```
`sfs_card_cms_exo_xx_yy.ma5` shows a simple example of multi-jet clustering and detector simulation implementation. First, it 
chooses FastJet package as fastsim interpreter, then defines multijet and in the following defines a simple detector 
simulation. This file can be executed as follows;
```
$ ./bin/ma5 -Re cms_exo_xx_yy cms_exo_xx_yy sfs_card_cms_exo_xx_yy.ma5
```
here `cms_exo_xx_yy` is a given analysis and `sfs_card_cms_exo_xx_yy.ma5` holds the information for the detector simulation
(**PAD requires `sfs_card_cms_exo_xx_yy.ma5` to setup detector simualtion for the analysis code, it automatically writes
the detector simulation according to the given `sfs_card` file. Note that if there is a card with same name for multiple
analysis files, those analyses can be executed at the same time, hence allowing more efficient execution.**). This command
will write a folder called `cms_exo_xx_yy` including all MadAnalysis 5 framework within. 

- Multijet definitions: Multijets are automatically defined in `cms_exo_xx_yy/Build/Main/main.cpp` (**do not change**) as follows;
```cpp
  //Adding new jet with ID AK08
  std::map<std::string, std::string> JetConfiguration1;
  JetConfiguration1["JetID"            ] = "AK08";
  JetConfiguration1["algorithm"        ] = "antikt";
  JetConfiguration1["cluster.R"        ] = "0.8";
  JetConfiguration1["cluster.PTmin"    ] = "200.0";
  cluster1->LoadJetConfiguration(JetConfiguration1);

  //Adding new jet with ID CA15
  std::map<std::string, std::string> JetConfiguration2;
  JetConfiguration2["JetID"            ] = "CA15";
  JetConfiguration2["algorithm"        ] = "cambridge";
  JetConfiguration2["cluster.R"        ] = "1.5";
  JetConfiguration2["cluster.PTmin"    ] = "200.0";
  cluster1->LoadJetConfiguration(JetConfiguration2); 
```
all these inputs are interpreted by `JetClusterer` machinery within MadAnalysis 5. Additionally `Makefile` has been modified
via `CXXFLAGS += -DMA5_FASTJET_MODE` flag to enable FastJet use within MadAnalysis datastructure, without this flag fastjet
dependent accessors will not work.

- Analysis folder: `cms_exo_xx_yy/Build/SampleAnalyzer/User/Analyzer` This folder includes all the definitions written
for the detector simulation;
```bash
$ ls
analysisList.h        cms_exo_xx_yy.h       new_smearer_reco.cpp  new_tagger.cpp        reco.h
cms_exo_xx_yy.cpp     efficiencies.h        new_smearer_reco.h    new_tagger.h          sigmas.h
```
`cms_exo_xx_yy.*` are the analysis files and the rest are detector simulation modules (**Please do not change those files
when the analysis submitted in PAD only `cms_exo_xx_yy.*`, `cms_exo_xx_yy.info` and `sfs_card_cms_exo_xx_yy.ma5` files are 
allowed. If you need to modify detector simulation, please modify `sfs_card_cms_exo_xx_yy.ma5` and re-execute the workspace
or include your personal modifications in `cms_exo_xx_yy.cpp`**).

 - Multijet accessor: Each multijet is accessible within `c++` interface through their unique identifiers. Primary jet is 
a special case where one can use `event.rec()->jet()` accessor to see all primary jets. Rest of the jets can be found using
`event.rec()->jet("AK08")` or `event.rec()->jet("CA15")` respectively.

# Tests done for backwards compatibility
Not tested.

# TODO

 - [ ] Remove accesibility of FastJet through analysis.
 - [ ] Add fj-contrib wrappers. 
   - Priority list: Recluster, softdrop, nsubjettines.

# Drawbacks

- Currently `SampleAnalyzer` module is compiling with `-DMA5_FASTJET_MODE` flag which allows certain data structures
to allow FastJet dependent functions to be accessible. The availablity of FastJet within datastructure is crutial 
for efficient execution. This also allow more efficient execution of the event. In the ma5-legacy code there are 2 
for loops running over entire particle list. First identifies particle types and organizes the data structure accordingly
and second runs to construct the list of particles to be reconstructed as jet. In this branch this has been eliminated by
collecting both for loops under one. However, this change is clashing with Delphes since it has a different workflow where 
the jet clustering occurs within Delphes with its own FastJet (Ma5 can be linked to Delphes without the link to FastJet 
since delphes has its own FJ module thats why this flexibility is required). 

**Possible solutions:** 
 - [ ] One option is creating two separate `Makefile` one for FastJet and Delphes and let user to choose 
which one to compile in expert mode. This is not an issue in normal mode since Ma5 knows the input content and can take 
action accordingly.
 - [ ] Possibly only reason for this problem is merging the for loops. This can be avoided by reformating the structure 
back into 2 for-loops but this will sacrifice the speed of the algorithm.
