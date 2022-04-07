# HEPTopTagger for MadAnalysis 5
This module is based on `Substructure` module and constructed
with the same principle where class is consist of a `Initialize`
and `Execute` function. However due to the complexity of HTT 
algorithm `Execute` function does not return anything but it includes 
various accessors. This example follows the `Substructure` module. Thus
for details see `tools/SampleAnalyzer/Interfaces/substructure/README.md`.

```c++
#ifndef analysis_test_h
#define analysis_test_h

#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"

namespace MA5
{

    class test : public AnalyzerBase
    {
        INIT_ANALYSIS(test,"test")

    private:
        Substructure::HTT tagger;
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
            Substructure::InputParameters parameters;
            parameters.mode = Substructure::HTT::EARLY_MASSRATIO_SORT_MASS; // execution mode

            parameters.do_optimalR = true; // initialize optimal R or set to fixed R
            // optimal R parameters
            parameters.optimalR_min = 0.5; // min jet size
            parameters.optimalR_step = 0.1; // step size
            parameters.optimalR_threshold = 0.2; // step size

            // massdrop - unclustering
            parameters.mass_drop = 0.8;
            parameters.max_subjet = 30.; // set_max_subjet_mass

            // filtering
            parameters.filt_N = 5; // set_nfilt
            parameters.filtering_R = 0.3; // max subjet distance for filtering
            parameters.filtering_minpt = 0.; // min subjet pt for filtering
            // jet algorithm for filtering
            parameters.filtering_algorithm = Substructure::cambridge;

            // Reclustering
            // reclustering jet algorithm
            parameters.reclustering_algorithm = Substructure::Algorithm::cambridge;

            //top mass range
            parameters.top_mass = 172.3;
            parameters.W_mass = 80.4;
            parameters.Mtop_min = 150.;
            parameters.Mtop_max = 200.; //set_top_range(min,max)

            // set top mass ratio range
            parameters.fw = 0.15;
            parameters.mass_ratio_range_min = (1-parameters.fw)*80.379/172.9;
            parameters.mass_ratio_range_max = (1+parameters.fw)*80.379/172.9;

            //mass ratio cuts
            parameters.m23cut = 0.35;
            parameters.m13cutmin = 0.2;
            parameters.m13cutmax = 1.3;

            // pruning
            parameters.prun_zcut = 0.1; // set_prun_zcut
            parameters.prun_rcut = .5; // set_prun_rcut
            tagger.Initialize(parameters);
            return true;
        }
        virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files){}
        virtual bool Execute(SampleFormat& sample, const EventFormat& event)
        {
            // Get antikt R=0.8 jets with pT > 200 |eta| < 2.5
            RecJets AK08 = filter(event.rec()->jets("AK08"), 200., 2.5);

            if (AK08.size() > 0)
            {
                taggedJet = tagger.Execute(AK08[0]);
                if (taggedJet->is_tagged())
                    INFO << "Top pT = " << taggedJet.top()->pt() << endmsg;
            }
            return true;
        }
    };
}

#endif
```