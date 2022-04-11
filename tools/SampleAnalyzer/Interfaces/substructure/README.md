# Usage of substructure module within MadAnalysis 5

Substructure module has been deviced to link with [SFS](https://arxiv.org/abs/2006.09387) where jet clustering
information has been preserved throughout the analysis execution. This allows user to use `Substructure` interface
to use `FastJet` libraries without dealing with the details about jets that are allocated in the memory. 
`Substructure` interface uses cluster history to link `RecJetFormat` to `FastJet`'s `PseudoJet` format and converts
two types seemlesly in the background.

Each class that has been constructed under `Substructure` namespace has been equiped with `Initialize` and `Execute`
functions. `Initialize` function creates a memory allocation for the corresponding `FastJet` or `fjcontib` 
backend. 

**Note:** Initializing any given `Substructure` class durin execution of the analysis can slow down the code and 
might create memory allocation problems. For the best results **always** initialize the substructure classes 
during analysis initialization. Examples can be found below.

Depending on the nature of the tool `Execute` function can take `const RecJetFormat *` or 
`std::vector<const RecJetFormat *>` where the former is designed for one jet and the latter is for a collection
of jets. The action of the `Execute` function depends on the tool that in use. `Execute` function, as the name 
suggests, needs to be executed during analysis execution.

## Outline
* [Examples](#examples)
  * [SoftDrop](#softdrop)
  * [Filter](#filter)
  * [Energy Correlator](#energy-correlator)
  * [Nsubjettiness](#nsubjettiness)
  * [Cluster](#cluster)
  * [VariableR](#variabler)
  * [Pruner](#pruner)
  * [Recluster](#recluster)

# Examples
The examples are designed to show only on a analysis header file but can easily be used separately in header and
source file. It is easier to examplify the functionality through a single file. To create this environment 
write a file named `substructure_example.ma5` with the commands as shown below
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

# Define VariableR algorithm name: "varR"
define jet_algorithm varR VariableR rho=2000
set varR.minR = 0
set varR.maxR = 2
set varR.ptmin = 20
```
This will create a MadAnalysis 5 workspace which has the information for a primary jet named `AK04` clustered 
with `antikt` algorithm `R=0.4` and `ptmin=20` GeV. Further more it will create two additional jets named `AK08`
and `varR` where former is clustered with `antikt` algorithm `R=0.8` and `ptmin=200` and latter is created with
`VariableR` algorithm. Each additional jet has been initialized in different method to examplify possible ways 
of jet definitions. All available `jet_algorithm` definitions are shown with the default values in the table below

|       Algorithm       | Parameters & Default values                                                        |
|:---------------------:|:-----------------------------------------------------------------------------------|
| `antikt`, `cambridge` | `radius=0.4`, `ptmin=5.`                                                           |
|        `genkt`        | `radius=0.4`, `ptmin=5.`, `exclusive=False`, `p=-1`                                |
|         `kt`          | `radius=0.4`, `ptmin=5.`, `exclusive=False`                                        |
|       `gridjet`       | `ymax=3.`, `ptmin=5.`                                                              |
|      `cdfjetclu`      | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`                    |
|     `cdfmidpoint`     | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `seed=1.`, `iratch=0.`, `areafraction=1.` |
|       `siscone`       | `radius=0.4`, `ptmin=5.`, `overlap=0.5`, `input_ptmin=5.`, `npassmax=1.`           |
|      `VariableR`      | `rho=2000.`, `minR=0.`, `maxR=2.`, `ptmin=20.` `exclusive=False` `clustertype=CALIKE` `strategy=Best` |

This card can be executed as show below
```bash
cd madanalysis5
./bin/ma5 -Re my_workspace test substructure_example.ma5
```
This will create a MadAnalysus 5 workspace in Expert mode. The workspace is named after `my_workspace` 
and the analysis file is named as `test`. `test.h` and `test.cpp` files can be found under 
`my_workspace/Build/SampleAnalyzer/User/Analyzer`. The Initial structure of `test.h` will be as follows

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
For the purposes of this example `test.cpp` will not be used; hence can be removed. We will only modify `test.h`
file from now on.

[Back to top](#outline)

## SoftDrop:

First one need to create the private `softdrop` object to be able to access it thorough different functions 
within `test` class
```c++
private: 
    Substructure::SoftDrop softDrop;
```
Then during the `Initialize` function this can be called as follows;
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
Note that it is possible to define `SoftDrop` as pointer, `Substructure::SoftDrop* softDrop;`, but for that 
case the class should be initialized with `new` keyword `softDrop = new Substructure::SoftDrop(beta, z_cut);`.

In the following `SoftDrop` can be used during execution
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
Note that if `SoftDrop` is defined as a pointer it needs to be executed as `const RecJet softdrop_jet = softDrop->Execute(AK08[0]);`.
Here `filter` function takes only "AK08" jets as defined [above](#examples) and removes the jets with $ p_T < 200 $ GeV
and $ |\eta| > 2.5 $. In the following `softDrop` has been executed with the leading `AK08` jet resulting in 
`const RecJetFormat *` (here we use `RecJet` as a shorthand which has been defined in `SampleAnalyzer` backend).

**Note:** Since all the classes has been desinged in the same way from now on we will only show a usage example 
without detailed explanation.

[Back to top](#outline)

## Filter
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
## Cluster
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
## Pruner
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

## Recluster
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