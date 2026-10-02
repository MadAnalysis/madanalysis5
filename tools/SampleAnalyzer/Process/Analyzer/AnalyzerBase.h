////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file AnalyzerBase.h
 * @brief Base class of all analyses (normal mode, expert mode and PAD).
 */

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
#include "SampleAnalyzer/Commons/Service/HEPData.h"

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

/**
 * @brief Declare the constructor and the destructor of an analysis class and set its name.
 *
 * To be placed at the beginning of the class declaration: `INIT_ANALYSIS(MyAna, "MyAna")`.
 */
#define INIT_ANALYSIS(CLASS,NAME) public: CLASS() {setName(NAME);} virtual ~CLASS() {} private:

/** @brief Shorthands for pointers to reconstructed objects and their collections. */
#define RecJet MA5::RecJetFormat *
typedef std::vector<const RecJet> RecJets;
#define RecTau MA5::RecTauFormat *
typedef std::vector<const RecTau> RecTaus;
#define  RecLepton MA5::RecLeptonFormat *
typedef std::vector<const RecLepton> RecLeptons;
#define RecPhoton MA5::RecPhotonFormat *
typedef std::vector<const RecPhoton> RecPhotons;
#define RecTrack MA5::RecTrackFormat *
typedef std::vector<const RecTrack> RecTracks;


namespace MA5 {
    /**
     * @brief Base class of the analyses.
     *
     * An analysis implements Initialize() (declaration of regions, cuts and histograms
     * through Manager()), Execute() (called for each event) and Finalize(). Options given
     * on the command line (`--name=value`) are available through getOption().
     */
    class AnalyzerBase
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    public :

        /** @brief Name of the analysis. */
        std::string name_;

        /** @brief Use the event weights? */
        MAbool weighted_events_;

        /** @brief Manager of the signal regions, cuts and histograms of the analysis. */
        RegionSelectionManager manager_;
        /** @brief Output directory of the analysis. */
        std::string outputdir_;

        /** @brief Writer of the SAF output file. */
        SAFWriter out_;

        /** @brief Command-line options. */
        std::map<std::string, std::string> options_;

        /** @brief Parameters given in main.cpp. */
        std::map<std::string, std::string> parameters_;

        // -------------------------------------------------------------
        //                       method members
        // -------------------------------------------------------------
    public :

        /** @brief Constructor. */
        AnalyzerBase()
        { name_="unknown"; outputdir_=""; }

        /** @brief Destructor. */
        virtual ~AnalyzerBase()
        { }

        /**
         * @brief Initialisation common to all analyses (weights mode, SAF writer, options).
         *
         * @param outputName name of the SAF output file.
         * @param cfg run configuration.
         * @return true.
         */
        MAbool PreInitialize(const std::string& outputName,
                             const Configuration* cfg)
        {
            weighted_events_ = !cfg->IsNoEventWeight();
            out_.Initialize(cfg,outputName.c_str());
            options_ = cfg->Options();
            return true;
        }

        /**
         * @brief Initialise the analysis (to be implemented by the user).
         *
         * @param cfg run configuration.
         * @param parameters parameters given in main.cpp.
         * @return false to stop the job.
         */
        virtual MAbool Initialize(const Configuration& cfg,
                                  const std::map<std::string,std::string>& parameters)=0;

        /**
         * @brief Initialise the analysis with the stored parameters.
         *
         * @param cfg run configuration.
         * @return the result of the user Initialize().
         */
        MAbool Initialize(const Configuration& cfg)
        {
            return Initialize(cfg, parameters_);
        }

        /**
         * @brief Write the header and the list of files in the SAF output file.
         *
         * @param summary summary of the samples.
         * @param samples samples.
         */
        void PreFinalize(const SampleFormat& summary,
                         const std::vector<SampleFormat>& samples)
        {
            out_.WriteHeader(summary);
            out_.WriteFiles(samples);
        }

        /**
         * @brief Finalise the analysis (to be implemented by the user).
         *
         * @param summary summary of the samples.
         * @param samples samples.
         */
        virtual void Finalize(const SampleFormat& summary,
                              const std::vector<SampleFormat>& samples)=0;

        /**
         * @brief Write the footer of the SAF output file and close it.
         *
         * @param summary summary of the samples.
         * @param samples samples (unused).
         */
        void PostFinalize(const SampleFormat& summary,
                          const std::vector<SampleFormat>& samples)
        {
            // Closing output file
            out_.WriteFoot(summary);
            out_.Finalize();
        }

        /**
         * @brief Common part of the event processing: set the initial/final-state status codes from the event.
         *
         * @param mySample current sample.
         * @param myEvent current event.
         * @return true.
         */
        MAbool PreExecute(const SampleFormat& mySample,
                          const EventFormat& myEvent)
        {
            PHYSICS->Id->SetFinalState(myEvent.mc());
            PHYSICS->Id->SetInitialState(myEvent.mc());
            return true;
        }

        /**
         * @brief Process an event (to be implemented by the user).
         *
         * @param mySample current sample.
         * @param myEvent current event.
         * @return the status of the event (the return value is not used by the generated main program).
         */
        virtual MAbool Execute(SampleFormat& mySample,
                               const EventFormat& myEvent)=0;

        /**
         * @brief Accessor to the name of the analysis.
         *
         * @return the name.
         */
        const std::string name() const {return name_;}

        /**
         * @brief Accessor to the region-selection manager.
         *
         * @return a pointer to the manager.
         */
        RegionSelectionManager *Manager() { return &manager_; }

        /**
         * @brief Set the name of the analysis.
         *
         * @param Name name.
         */
        void setName(const std::string& Name) {name_=Name;}

        /**
         * @brief Accessor to the output directory.
         *
         * @return the directory.
         */
        const std::string Output() const {return outputdir_;}

        /**
         * @brief Set the output directory.
         *
         * @param name directory.
         */
        void SetOutputDir(const std::string &name) {outputdir_=name;}


        /**
         * @brief Accessor to the SAF writer.
         *
         * @return the writer.
         */
        SAFWriter& out()
        { return out_; }

        /**
         * @brief Set the command-line options.
         *
         * @param options options.
         */
        void SetOptions(std::map<std::string, std::string> options) {options_=options;}

        /**
         * @brief Set the parameters (from main.cpp).
         *
         * @param params parameters.
         */
        void SetParameters(std::map<std::string, std::string> params) {parameters_=params;}

        /**
         * @brief Accessor to the parameters.
         *
         * @return a copy of the parameters.
         */
        const std::map<std::string, std::string> GetParameters() {return parameters_;}

        /**
         * @brief Get a command-line option as a string.
         *
         * @param optname name of the option.
         * @return the value (empty if not given).
         */
        std::string getOption(std::string optname) const
        {
            if ( options_.find(optname) != options_.end() )
                return options_.find(optname)->second;
            return "";
        }

        /**
         * @brief Get a command-line option converted to the type of the default value.
         *
         * Example: `getOption<double>("FOO", 3.)`.
         *
         * @tparam T type of the value.
         * @param optname name of the option.
         * @param def default value.
         * @return the converted value, or def if the option is not given.
         */
        template<typename T>
        T getOption(std::string optname, T def) const {
            if (options_.find(optname) == options_.end()) return def;
            std::stringstream ss;
            ss << options_.find(optname)->second;
            // NOTE: ret is left uninitialised if the conversion fails.
            T ret;
            ss >> ret;
            return ret;
        }

        /**
         * @brief Get a command-line option as a string (overload for string literals).
         *
         * @param optname name of the option.
         * @param def default value.
         * @return the value, or def.
         */
        std::string getOption(std::string optname, const char* def) {
            return getOption<std::string>(optname, def);
        }


        /** @brief Deprecated (prints a warning). */
        void AddDefaultHadronic()
        {
            try {
                throw EXCEPTION_WARNING("This function is deprecated.", "", 1);
            } catch (const std::exception& err) {
                MANAGE_EXCEPTION(err);
            }
        }

        /** @brief Deprecated (prints a warning). */
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
