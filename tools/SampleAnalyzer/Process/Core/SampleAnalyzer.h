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
 * @file SampleAnalyzer.h
 * @brief Main driver of a SampleAnalyzer job (files, readers, analyses, writers, clusterers, detectors).
 */

#ifndef SAMPLE_ANALYZER_H
#define SAMPLE_ANALYZER_H

// STL headers
#include <iostream>
#include <string>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/StatusCode.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Process/Reader/ReaderManager.h"
#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"
#include "SampleAnalyzer/Process/Writer/WriterManager.h"
#include "SampleAnalyzer/Process/JetClustering/JetClustererManager.h"
#include "SampleAnalyzer/Process/Detector/DetectorManager.h"

namespace MA5
{

    class ProgressBar;
    class Configuration;

    /**
     * @brief Main driver of a SampleAnalyzer job.
     *
     * Typical usage in the generated main.cpp:
     * 1. Initialize() decodes the command line and the list of samples;
     * 2. InitializeAnalyzer/Writer/JetClusterer/Detector() set up the components;
     * 3. PostInitialize() creates the output directories and initialises the analyses;
     * 4. NextFile()/NextEvent() loop over the samples and events, the analyses being
     *    executed by the main program;
     * 5. Finalize() writes the SAF output files and prints the reports.
     */
    class SampleAnalyzer
    {
    private:
        /** @brief Name of the analysis and of the dataset, and failure flag of the last file. */
        std::string analysisName_;
        std::string datasetName_;
        MAbool LastFileFail_;

        /** @brief Configuration decoded from the command line. */
        Configuration cfg_;

        /** @brief Input files. */
        std::vector<std::string> inputs_;

        /** @brief Registries of all the available components. */
        WriterManager fullWriters_;
        ReaderManager fullReaders_;
        AnalyzerManager fullAnalyses_;
        JetClustererManager fullJetClusterers_;
        DetectorManager fullDetectors_;

        /** @brief Components used in this job. */
        std::vector<WriterBase *> writers_;
        std::vector<ReaderBase *> readers_;
        std::vector<AnalyzerBase *> analyzers_;
        std::vector<JetClusterer *> clusters_;
        std::vector<DetectorBase *> detectors_;

        /** @brief Index (1-based) of the current file and flag requesting the next file. */
        MAuint32 file_index_;
        MAbool next_file_;

        /** @brief Numbers of read and accepted events, per file. */
        std::vector<MAuint64> counter_read_;
        std::vector<MAuint64> counter_passed_;

        /** @brief Reader of the current file. */
        ReaderBase *myReader_;

        /** @brief Progress bar. */
        ProgressBar *progressBar_;

    public:
        /** @brief Constructor (starts the services and checks the data types). */
        SampleAnalyzer();

        /**
         * @brief Accessor to the registry of the analyses.
         *
         * @return the registry.
         */
        AnalyzerManager &AnalyzerList() { return fullAnalyses_; }
        /**
         * @brief Accessor to the registry of the readers.
         *
         * @return the registry.
         */
        ReaderManager &ReaderList() { return fullReaders_; }
        /**
         * @brief Accessor to the registry of the writers.
         *
         * @return the registry.
         */
        WriterManager &WriterList() { return fullWriters_; }
        /**
         * @brief Accessor to the registry of the jet clusterers.
         *
         * @return the registry.
         */
        JetClustererManager &JetClustererList() { return fullJetClusterers_; }
        /**
         * @brief Accessor to the registry of the detector simulations.
         *
         * @return the registry.
         */
        DetectorManager &DetectorSimList() { return fullDetectors_; }

        /**
         * @brief Decode the command line, read the list of samples and build the registries.
         *
         * @param argc number of arguments.
         * @param argv arguments.
         * @param filename PDG file name (unused).
         * @return false in case of error (see the FIXME in the source for a missing list).
         */
        MAbool Initialize(MAint32 argc, MAchar **argv, const std::string &filename);

        /**
         * @brief Set up an analysis.
         *
         * @param name name of the analysis.
         * @param outputname name of the output.
         * @param parameters options passed to the analysis.
         * @return the analysis, or 0 in case of error.
         */
        AnalyzerBase *InitializeAnalyzer(const std::string &name,
                                         const std::string &outputname,
                                         const std::map<std::string, std::string> &parameters);

        /**
         * @brief Set up an analysis without option.
         *
         * @param name name of the analysis.
         * @param outputname name of the output.
         * @return the analysis, or 0 in case of error.
         */
        AnalyzerBase *InitializeAnalyzer(const std::string &name,
                                         const std::string &outputname);

        /**
         * @brief Set up a writer (the output is written in a new subdirectory of Output/SAF/<dataset>).
         *
         * @param name name of the writer (file format).
         * @param outputname name of the output file.
         * @return the writer, or 0 in case of error.
         */
        WriterBase *InitializeWriter(const std::string &name,
                                     const std::string &outputname);

        /**
         * @brief Set up a jet clusterer.
         *
         * @param name name of the clusterer.
         * @param parameters parameters of the clustering and of the taggers.
         * @return the clusterer, or 0 in case of error.
         */
        JetClusterer *InitializeJetClusterer(const std::string &name,
                                             const std::map<std::string, std::string> &parameters);

        /**
         * @brief Set up a detector simulation.
         *
         * @param name name of the detector simulation.
         * @param configFile detector card.
         * @param parameters options.
         * @return the detector simulation, or 0 in case of error.
         */
        DetectorBase *InitializeDetector(const std::string &name,
                                         const std::string &configFile,
                                         const std::map<std::string, std::string> &parameters);

        /**
         * @brief Read the next event of the current file.
         *
         * @param mysample current sample.
         * @param myevent event to fill.
         * @return KEEP if the event is accepted, SKIP if it must be skipped, FAILURE at the end of the file.
         */
        StatusCode::Type NextEvent(SampleFormat &mysample, EventFormat &myevent);

        /**
         * @brief Initialise the region managers of the analyses with the weights of the event.
         *
         * @param mysample current sample.
         * @param myevent current event.
         */
        void PrepareForExecution(SampleFormat &mysample, EventFormat &myevent);

        /**
         * @brief Open the next file (the previous one is closed).
         *
         * @param mysample sample to fill with the header of the file.
         * @return KEEP if the file is ready, SKIP if it cannot be read, FAILURE when all the files have been read.
         */
        StatusCode::Type NextFile(SampleFormat &mysample);

        /**
         * @brief Write the SAF output files, finalise the components, print the reports and stop the services.
         *
         * @param mysamples samples.
         * @param myevent last event.
         * @return true.
         */
        MAbool Finalize(std::vector<SampleFormat> &mysamples, EventFormat &myevent);

        /** @brief Update the progress bar with the position in the current file. */
        void UpdateProgressBar();

        /**
         * @brief Create the output directories and initialise the analyses.
         *
         * @return false in case of error.
         */
        MAbool PostInitialize();

        /**
         * @brief Write the event counters of the signal regions of all the analyses.
         *
         * @param outwriter output stream.
         */
        void DumpSR(std::ostream &);
        /**
         * @brief Write the header of the signal-region counters.
         *
         * @param outwriter output stream.
         */
        void HeadSR(std::ostream &);

        /**
         * @brief Accessor to the command-line options.
         *
         * @return the options.
         */
        std::map<std::string, std::string> options() { return cfg_.Options(); }

        /** @brief Register the default hadronic PDG codes. */
        void AddDefaultHadronic();

        /** @brief Register the default invisible PDG codes (neutrinos, neutralino, gravitino). */
        void AddDefaultInvisible();

    private:
        /** @brief Check the sizes of the portable data types (warnings if unexpected). */
        void CheckDatatypes() const;

        /**
         * @brief Build the summary sample (total number of events, averaged cross section, sums of weights).
         *
         * @param summary summary to fill.
         * @param mysamples samples.
         */
        void FillSummary(SampleFormat &summary,
                         const std::vector<SampleFormat> &mysamples);

        /**
         * @brief Create Output/SAF/<dataset>/<analysis>_<n>/{Histograms,Cutflows} for each analysis.
         *
         * @return false in case of error.
         */
        MAbool CreateDirectoryStructure();
    };

}

#endif
