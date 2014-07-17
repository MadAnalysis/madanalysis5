////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
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


#ifndef SAMPLE_ANALYZER_H
#define SAMPLE_ANALYZER_H

// STL headers
#include <iostream>
#include <string>
#include <vector>


// SampleAnalyzer headers
// |- core functions
#include "SampleAnalyzer/Commons/Base/StatusCode.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
// |- data format
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
// |-manager headers
#include "SampleAnalyzer/Process/Reader/ReaderManager.h"
#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"
#include "SampleAnalyzer/Process/Writer/WriterManager.h"
#include "SampleAnalyzer/Process/JetClustering/JetClustererManager.h"
#include "SampleAnalyzer/Process/Detector/DetectorManager.h"


namespace MA5
{

class ProgressBar;
class Configuration;

class SampleAnalyzer
{
 private :
 
  std::string analysisName_; 
  std::string datasetName_;
  bool LastFileFail_;

  /// Configuration of SampleAnalyzer
  Configuration cfg_;

  /// List of input files
  std::vector<std::string> inputs_;

  /// List of managers
  WriterManager       fullWriters_;
  ReaderManager       fullReaders_;
  AnalyzerManager     fullAnalyses_;
  JetClustererManager fullJetClusterers_;
  DetectorManager     fullDetectors_;

  /// List of managers
  std::vector<WriterBase*>       writers_;
  std::vector<ReaderBase*>       readers_;
  std::vector<AnalyzerBase*>     analyzers_;
  std::vector<JetClusterer*> clusters_;
  std::vector<DetectorBase*>     detectors_;

  /// Reading status
  unsigned int file_index_;
  bool next_file_;

  /// Counters
  std::vector<ULong64_t> counter_read_;
  std::vector<ULong64_t> counter_passed_;

  /// The only one pointer to the reader
  ReaderBase* myReader_;

  /// Progress bar for event reading
  ProgressBar* progressBar_;

  
 public:

  /// Constructor withtout arguments
  SampleAnalyzer();

  /// Adding Analyzer
  AnalyzerManager& AnalyzerList()
  { return fullAnalyses_; }
  ReaderManager& ReaderList()
  { return fullReaders_; }
  WriterManager& WriterList()
  { return fullWriters_; }
  JetClustererManager& JetClustererList()
  { return fullJetClusterers_; }
  DetectorManager& DetectorSimList()
  { return fullDetectors_; }

  /// Initialization of the SampleAnalyzer
  bool Initialize(int argc, char **argv, const std::string& filename,bool=false);

  /// Getting pointer to an analyzer
  AnalyzerBase* InitializeAnalyzer(const std::string& name, 
                                   const std::string& outputname,
                        const std::map<std::string,std::string>& parameters);

  AnalyzerBase* InitializeAnalyzer(const std::string& name, 
                                   const std::string& outputname);

  /// Getting pointer to a writer
  WriterBase* InitializeWriter(const std::string& name, 
                               const std::string& outputname);

  /// Getting pointer to a jet clusterer
  JetClusterer* InitializeJetClusterer(const std::string& name, 
                  const std::map<std::string,std::string>& parameters);

  /// Getting pointer to a detector
  DetectorBase* InitializeDetector(const std::string& name,
                                  const std::string& configFile,
                  const std::map<std::string,std::string>& parameters);

  /// Reading the next event
  StatusCode::Type NextEvent(SampleFormat& mysample, EventFormat& myevent);

  /// Reading the next file
  StatusCode::Type NextFile(SampleFormat& mysample);

  /// Finalization of the SampleAnalyzer
  bool Finalize(std::vector<SampleFormat>& mysamples, EventFormat& myevent);

  /// Updating the progress bar
  void UpdateProgressBar();

  /// Creating the directory structure associated with the SRM
  bool PostInitialize();

 private:

  /// Filling the summary format
  void FillSummary(SampleFormat& summary,
                   const std::vector<SampleFormat>& mysamples);


  /// Creating the directory structure associated with the SRM
  bool CreateDirectoryStructure();


};

}

#endif
