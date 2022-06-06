////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


#ifndef ANALYSISBASE_h
#define ANALYSISBASE_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Process/Plot/PlotManager.h"
#include "SampleAnalyzer/Process/Counter/CounterManager.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/Utils.h"
#include "SampleAnalyzer/Commons/Service/SortingService.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelectionManager.h"
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// FJcontrib tools
#ifdef MA5_FASTJET_MODE
    #include "SampleAnalyzer/Interfaces/substructure/SoftDrop.h"
    #include "SampleAnalyzer/Interfaces/substructure/Cluster.h"
    #include "SampleAnalyzer/Interfaces/substructure/Recluster.h"
    #include "SampleAnalyzer/Interfaces/substructure/Nsubjettiness.h"
    #include "SampleAnalyzer/Interfaces/substructure/VariableR.h"
    #include "SampleAnalyzer/Interfaces/substructure/Pruner.h"
    #include "SampleAnalyzer/Interfaces/substructure/Selector.h"
    #include "SampleAnalyzer/Interfaces/substructure/Filter.h"
    #include "SampleAnalyzer/Interfaces/substructure/EnergyCorrelator.h"
    #include "SampleAnalyzer/Interfaces/HEPTopTagger/HTT.h"
#endif

// STL headers
#include <set>
#include <string>
#include <cmath>
#include <vector>
#include <map>

// initializing MACRO 
#define INIT_ANALYSIS(CLASS,NAME) public: CLASS() {setName(NAME);} virtual ~CLASS() {} private:

// Introduce shorthand for widely used reconstructed objects
#define RecJet     MA5::RecJetFormat *
#define RecJets    std::vector<const MA5::RecJetFormat *>
#define RecTau     MA5::RecTauFormat *
#define RecTaus    std::vector<const MA5::RecTauFormat *>
#define RecLepton  MA5::RecLeptonFormat *
#define RecLeptons std::vector<const MA5::RecLeptonFormat *>
#define RecPhoton  MA5::RecPhotonFormat *
#define RecPhotons std::vector<const MA5::RecPhotonFormat *>
#define RecTrack   MA5::RecTrackFormat *
#define RecTracks  std::vector<const MA5::RecTrackFormat *>

namespace MA5 {
    class AnalyzerBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    public :

        /// name of the analysis
        std::string name_;

        /// Weighted events mode
        MAbool weighted_events_;

        /// A RS manager is associated with each analysis
        RegionSelectionManager manager_;
        std::string outputdir_;

        /// Writer SAF
        SAFWriter out_;

        // options
        std::map<std::string, std::string> options_;

        // parameters
        std::map<std::string, std::string> parameters_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public :

        /// Constructor without argument
        AnalyzerBase()
        { name_="unknown"; outputdir_=""; }

        /// Destructor
        virtual ~AnalyzerBase()
        { }

        /// Initialize (common part to all analyses)
        MAbool PreInitialize(const std::string& outputName,
                             const Configuration* cfg)
        {
            weighted_events_ = !cfg->IsNoEventWeight();
            out_.Initialize(cfg,outputName.c_str());
            options_ = cfg->Options();
            return true;
        }

        /// Initialize (specific to the analysis)
        virtual MAbool Initialize(const Configuration& cfg,
                                  const std::map<std::string,std::string>& parameters)=0;

        MAbool Initialize(const Configuration& cfg)
        {
            return Initialize(cfg, parameters_);
        }

        /// PreFinalize
        void PreFinalize(const SampleFormat& summary,
                         const std::vector<SampleFormat>& samples)
        {
            out_.WriteHeader(summary);
            out_.WriteFiles(samples);
        }

        /// Finalize
        virtual void Finalize(const SampleFormat& summary,
                              const std::vector<SampleFormat>& samples)=0;

        /// PostFinalize
        void PostFinalize(const SampleFormat& summary,
                          const std::vector<SampleFormat>& samples)
        {
            // Closing output file
            out_.WriteFoot(summary);
            out_.Finalize();
        }

        /// Execute
        MAbool PreExecute(const SampleFormat& mySample,
                          const EventFormat& myEvent)
        {
            PHYSICS->Id->SetFinalState(myEvent.mc());
            PHYSICS->Id->SetInitialState(myEvent.mc());
            return true;
        }

        virtual MAbool Execute(SampleFormat& mySample,
                               const EventFormat& myEvent)=0;

        /// Accessor to analysis name
        const std::string name() const {return name_;}

        /// Accessor to the manager
        RegionSelectionManager *Manager() { return &manager_; }

        /// Mutator to analysis name
        void setName(const std::string& Name) {name_=Name;}

        /// Accessor to the output directory name
        const std::string Output() const {return outputdir_;}

        /// Mutator to the output directory name
        void SetOutputDir(const std::string &name) {outputdir_=name;}


        SAFWriter& out()
        { return out_; }

        // Set command line options
        void SetOptions(std::map<std::string, std::string> options) {options_=options;}

        // Set parameters, initialized in main.cpp
        void SetParameters(std::map<std::string, std::string> params) {parameters_=params;}

        // Accessor to parameters
        const std::map<std::string, std::string> GetParameters() {return parameters_;}

        /// Get an option for this analysis instance as a string.
        std::string getOption(std::string optname) const
        {
            if ( options_.find(optname) != options_.end() )
                return options_.find(optname)->second;
            return "";
        }

        /// Get an option for this analysis instance converted to a specific type
        /// The return type is given by the specified @a def value, or by an explicit template
        /// type-argument, e.g. getOption<double>("FOO", 3).
        template<typename T>
        T getOption(std::string optname, T def) const {
            if (options_.find(optname) == options_.end()) return def;
            std::stringstream ss;
            ss << options_.find(optname)->second;
            T ret;
            ss >> ret;
            return ret;
        }

        /// overload for literal character strings (which don't play well with stringstream)
        /// Note this isn't a template specialisation, because we can't return a non-static
        /// char*, and T-as-return-type is built into the template function definition.
        std::string getOption(std::string optname, const char* def) {
            return getOption<std::string>(optname, def);
        }


        // backwards compatibility
        void AddDefaultHadronic()
        {
            try {
                throw EXCEPTION_WARNING("This function is deprecated.", "", 1);
            } catch (const std::exception& err) {
                MANAGE_EXCEPTION(err);
            }
        }

        void AddDefaultInvisible()
        {
            try {
                throw EXCEPTION_WARNING("This function is deprecated.", "", 1);
            } catch (const std::exception& err) {
                MANAGE_EXCEPTION(err);
            }
        }

    protected :

    };
}


#endif
