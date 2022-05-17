# Usage of the substructure module shipped with MadAnalysis 5

The Substructure module of MadAnalysis 5 has been designed to be used within the
[SFS framework](https://arxiv.org/abs/2006.09387), in which jet clustering
information is preserved and used throughout the analysis execution. This allows
users to use the `Substructure` interface to the `FastJet` libraries without
having to deal with how jets are treated internally (in particular with respect
to memory management). The`Substructure` interface uses the clustering history
to link `RecJetFormat` objects to `FastJet`'s `PseudoJet` objects, and converts
the two types of object seamlesly in the background.

Each class that has been constructed under the `Substructure` namespace has been
equiped with an `Initialize` and an `Execute` function. The `Initialize`
function creates a memory allocation for the corresponding `FastJet` (or
`fjcontib` backend).

**Note:** Initialising any given of the `Substructure` classes during the
execution of the analysis can slow down the code and might create memory
allocation problems. The best practice requires to **always** initialise the
used substructure classes in the analysis initialisation routine. Examples are
provided below.

Depending on the nature of the jet substructure tool considered, the `Execute`
function can take as an argument a `const RecJetFormat *` pointer or a
`std::vector<const RecJetFormat *>` vector of pointers. The former possibility
is designed to work with a single jet, whereas the latter is designed to work
with a collection of jets. The precise action of the `Execute` function depends
on the substructure tools to be used. The `Execute` function, as suggested by
its name, needs to be executed during the analysis execution.

## Outline
* [Examples](#examples)
  * [SoftDrop](#softdrop)
  * [Filtering](#filter)
  * [Energy Correlator](#energy-correlator)
  * [Nsubjettiness](#nsubjettiness)
  * [Clustering](#cluster)
  * [VariableR](#variabler)
  * [Pruning](#pruner)
  * [Reclustering](#recluster)

# Examples
The examples below are designed to illutrate only analysis header files, but can
easily be generalised to be used separately in a header and a source file. We
only made this choice as it is easier to examplify the functionalities through a
single file. To create an environment compatible with the substructure module,
we begin with writing a file named `substructure_example.ma5` including the
commands shown below:
```
# Define standard AK04 jet
set main.fastsim.package = fastjet
set main.fastsim.algorithm = antikt
set main.fastsim.radius = 0.4
set main.fastsim.JetID = AK04
set main.fastsim.bjet_id.matching_dr = 0.3
set main.fastsim.bjet_id.exclusive = true
set main.fastsim.exclusive_id = false
set main.fastsim.ptmin = 20.0

# Define AK08 jet
define jet_algorithm AK08 antikt radius=0.8 ptmin=200

# Define a VariableR algorithm name "varR"
define jet_algorithm varR VariableR rho=2000
set varR.minR = 0
set varR.maxR = 2
set varR.ptmin = 20
```
This will generate a MadAnalysis 5 workspace which includes a definition of a
primary jet class named `AK04`, and clustered according to the `antikt`
algorithm with the parameters `R=0.4` and `ptmin=20` GeV. Furthermore, the
commands lead to the initialisation of two additional jet classes named `AK08`
and `varR`. In the former case, jets are clustered with the `antikt` algorithm
with parameters `R=0.8` and `ptmin=200` GeV. The latter definition leads to a
jet definition through the `VariableR` algorithm. All available `jet_algorithm`
definitions are shown with their default values in the table below.

|       Algorithm       | Parameters & Default values                                                        |
|:---------------------:|:-----------------------------------------------------------------------------------|
| `antikt`, `cambridge` | `radius=0.4`, `ptmin=5.`                                                           |
|        `genkt`        | `radius=0.4`, `ptmin=5.`, `exclusive=False`, `p=-1`                                |
|         `kt`          | `radius=0.4`, `ptmin=5.`, `exclusive=False`                                        |
|       `gridjet`       | `ymax=3.`, `ptmin=5.`                                                              |
|      `cdfjetclu`      | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`                    |
|     `cdfmidpoint`     | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`, `areafraction=1.` |
|       `siscone`       | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `input_ptmin=5.`, `npassmax=1.`           |
|      `VariableR`      | `rho=2000.`, `minR=0.`, `maxR=2.`, `ptmin=20.` `exclusive=False`                   |
|                       | `clustertype=CALIKE` `strategy=Best`                                               |

The above MadAnalysis 5 script can be executed as:
```bash
cd madanalysis5
./bin/ma5 -Re my_workspace test substructure_example.ma5
```
This will create a MadAnalysus 5 workspace in the Expert mode of the code. The
workspace is named `my_workspace` and the analysis file is named `test`. The
`test.h` and `test.cpp` files can be found under
  `my_workspace/Build/SampleAnalyzer/User/Analyzer`.
The structure of `test.h` reads:

`my_workspace/Build/SampleAnalyzer/User/Analyzer/test.h`:
```c++
#ifndef analysis_test_h
#define analysis_test_h

#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"

namespace MA5
{
class test : public AnalyzerBase
{
  INIT_ANALYSIS(test,"test")

 public:
  virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters);
  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);
  virtual bool Execute(SampleFormat& sample, const EventFormat& event);

 private:
};
}

#endif
```
For the purposes of this example `test.cpp` is not be used and can hence be
removed. We will only modify `test.h` from now on.

[Back to top](#outline)

## SoftDrop:

First we need to create a private `softdrop` object to be able to access it
thorough the different functions of the `test` class
```c++
private: 
    Substructure::SoftDrop softDrop;
```
Then during the `Initialize` function, it can be called as
```c++
virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
    // Initialize SoftDrop
    MAfloat32 z_cut = 0.10;
    MAfloat32 beta  = 2.0;
    softDrop.Initialize(beta, z_cut);
    return true;
}
```
Note that it is possible to define a `SoftDrop` object through a pointer,
  `Substructure::SoftDrop* softDrop;`
In this case, the class should be initialised as:
  `softDrop = new Substructure::SoftDrop(beta, z_cut);`.

The `SoftDrop` object can then be further used during the execution of the code.
```c++
virtual bool Execute(SampleFormat& sample, const EventFormat& event)
{
    // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
    RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);
    
    if (AK08.size() > 0)
    {
        INFO << "SoftDrop on leading AK08 jet:" << endmsg;
        const RecJet softdrop_jet = softDrop.Execute(AK08[0]);
        INFO << "M_SD = " << softdrop_jet->m()  << " M(j_1) = " << AK08[0]->m() << endmsg;
    }
    return true;
}
```
Note that if `SoftDrop` is defined as a pointer it needs to be executed as
  `const RecJet softdrop_jet = softDrop->Execute(AK08[0]);`.
Here the `filter` function cleans the "AK08" jet collection defined
[above](#examples) and removes from it any jet with pT<200 GeV and |eta|>2.5. In
the following we apply the `softDrop` method on the leading `AK08` jet, which
results in a `const RecJetFormat *` object, the `RecJet` definition used here
having been defined in the `SampleAnalyzer` backend.

**Note:** All substructure classes are desinged in the same way. Therefore, from
now on we only show simple examples of usage without any detailed explanations.

[Back to top](#outline)

## Filtering
```c++
    private:
        Substructure::Filter JetFilter;
    public:
        virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
        {
            // Initializing PhysicsService for MC
            PHYSICS->mcConfig().Reset();
            AddDefaultHadronic();
            AddDefaultInvisible();

            // Initializing PhysicsService for RECO
            PHYSICS->recConfig().Reset();

            // Initialize filtering
            MAfloat32 Rfilt = 0.2;
            INFO << "Initializing Filter" << endmsg;
            JetFilter.Initialize(Rfilt, Substructure::SelectorPtFractionMin(0.03));
            return true;
        }
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);

            if (AK08.size() > 0)
            {
                INFO << "Filter the leading AK08 jet:" << endmsg;
                const RecJet filteredJet = JetFilter.Execute(AK08[0]);
                INFO << "pT(j_filt) = " << filteredJet->pt()  << " pT(j_1) = " << AK08[0]->pt() << endmsg;
            }

            // RecJets shorthand for `std::vector<const RecJetFormat *>`
            RecJets AK08_filtered = JetFilter.Execute(AK08);
            for (MAuint32 i=0; i<AK08_filtered.size(); i++)
            {
                INFO << "Filtered Jet(" << i << ") pT = " <<  AK08_filtered[i]->pt()
                     << "Original Jet(" << i << ") pT = " << AK08[i]->pt() << endmsg;
            }
            return true;
        }
```
[Back to top](#outline)
## Energy Correlator
```c++
    private:
        Substructure::EnergyCorrelator EC;
    public:
        virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
        {
            // Initializing PhysicsService for MC
            PHYSICS->mcConfig().Reset();
            AddDefaultHadronic();
            AddDefaultInvisible();

            // Initializing PhysicsService for RECO
            PHYSICS->recConfig().Reset();

            // Initialize Energy Correlator
            MAuint32 N = 1;
            beta = 0.1;
            Substructure::EnergyCorrelator::Measure measure = Substructure::EnergyCorrelator::Measure::pt_R;
            Substructure::EnergyCorrelator::Strategy strategy = Substructure::EnergyCorrelator::Strategy::storage_array;
            EC.Initialize(N, beta, measure, strategy);
            return true;
        }
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);
            if (AK08.size() > 0)
            {
                INFO << "EnergyCorrelator:" << endmsg;
                MAdouble64 ec = EC.Execute(AK08[0]);
                INFO << "EC = " << ec << endmsg;
            }
            return true;
        }
```
[Back to top](#outline)
## Nsubjettiness
```c++
    private:
        Substructure::Nsubjettiness nsub;
    public:
        virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
        {
            // Initializing PhysicsService for MC
            PHYSICS->mcConfig().Reset();
            AddDefaultHadronic();
            AddDefaultInvisible();

            // Initializing PhysicsService for RECO
            PHYSICS->recConfig().Reset();

            // Initialize Nsubjettiness
            MAfloat32 beta = 0.1, R0 = 0.2;
            nsub.Initialize(
                1.,
                Substructure::Nsubjettiness::KT_Axes,
                Substructure::Nsubjettiness::NormalizedMeasure,
                beta,
                R0,
            );
            return true;
        }
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);
            if (AK08.size() > 0)
            {
                MAdouble64 tau1 = nsub.Execute(AK08[0]);
                INFO << "tau1 = " << tau1 << endmsg;
            }
            return true;
        }
```
[Back to top](#outline)
## Clustering
```c++
    private:
        Substructure::Cluster cluster;
    public:
        virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
        {
            // Initializing PhysicsService for MC
            PHYSICS->mcConfig().Reset();
            AddDefaultHadronic();
            AddDefaultInvisible();

            // Initializing PhysicsService for RECO
            PHYSICS->recConfig().Reset();

            // Initialize Cluster
            cluster.Initialize(Substructure::cambridge, 0.8, 200., true);
            return true;
        }
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);
            if (AK08.size() > 0)
            {
                RecJets clusteredFromLeading = cluster.Execute(AK08[0]);
                RecJets CAJets = cluster.Execute(AK08);
                RecJets CAJetsFiltered = cluster.Execute(
                        AK08[0], [](const RecJet mainjet, const RecJet subjet) 
                                 { return (subjet->pt() > mainjet->pt()*0.05);}
                );
            }
            return true;
        }
```
[Back to top](#outline)
## VariableR
```c++
    private:
        Substructure::VariableR varR;
    public:
        virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
        {
            // Initializing PhysicsService for MC
            PHYSICS->mcConfig().Reset();
            AddDefaultHadronic();
            AddDefaultInvisible();

            // Initializing PhysicsService for RECO
            PHYSICS->recConfig().Reset();

            // Initialize Energy Correlator
            varR.Initialize(
                2000., // mass scale for effective radius (i.e. R ~ rho/pT)
                0., //minimum jet radius
                2., // maximum jet radius
                Substructure::VariableR::CALIKE,
                Substructure::VariableR::Best,
                200.,// Minimum pT
                false // isexclusive
            );
            return true;
        }
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);
            if (AK08.size() > 0)
            {
                RecJets clusteredFromLeading = varR.Execute(AK08[0]);
                RecJets varRJets = varR.Execute(AK08);
                RecJets varRJetsFiltered = varR.Execute(
                        AK08[0], [](const RecJet mainjet, const RecJet subjet) 
                                 { return (subjet->pt() > mainjet->pt()*0.05);}
                );
            }
            return true;
        }
```
[Back to top](#outline)
## Pruning
```c++
    private:
        Substructure::Pruner pruner;
    public:
        virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
        {
            // Initializing PhysicsService for MC
            PHYSICS->mcConfig().Reset();
            AddDefaultHadronic();
            AddDefaultInvisible();

            // Initializing PhysicsService for RECO
            PHYSICS->recConfig().Reset();

            // Initialize pruner
            pruner.Initialize(Substructure::cambridge, 0.2, 0.2, 0.3);
            return true;
        }
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);

            if (AK08.size() > 0)
            {
                INFO << "Prune the leading AK08 jet:" << endmsg;
                const RecJet prunedjet = pruner.Execute(AK08[0]);
                INFO << "pT(j_filt) = " << prunedjet->pt()  << " pT(j_1) = " << AK08[0]->pt() << endmsg;
            }
            
            // RecJets shorthand for `std::vector<const RecJetFormat *>`
            RecJets AK08_pruned = pruner.Execute(AK08);
            for (MAuint32 i=0; i<AK08_pruned.size(); i++)
            {
                INFO << "Pruned Jet(" << i << ") pT = " <<  AK08_pruned[i]->pt()
                     << "Original Jet(" << i << ") pT = " << AK08[i]->pt() << endmsg;
            }
            return true;
        }
```
[Back to top](#outline)

## Reclustering
```c++
    private:
        Substructure::Recluster recluster;
    public:
        virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
        {
            // Initializing PhysicsService for MC
            PHYSICS->mcConfig().Reset();
            AddDefaultHadronic();
            AddDefaultInvisible();

            // Initializing PhysicsService for RECO
            PHYSICS->recConfig().Reset();

            // Initialize Energy Correlator
            recluster.Initialize(Substructure::kt, 0.2);
            return true;
        }
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);
            if (AK08.size() > 0)
            {
                const RecJet reclusteredJet = recluster.Execute(AK08[0]);
                RecJets reclusteredJets = recluster.Execute(AK08);
            }
            return true;
        }
```
[Back to top](#outline)
