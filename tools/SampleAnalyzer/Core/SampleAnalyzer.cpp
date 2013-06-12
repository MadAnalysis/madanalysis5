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


// SampleAnalyzer headers
#include "SampleAnalyzer/Core/SampleAnalyzer.h"
#include "SampleAnalyzer/Writer/SAFWriter.h"
#include "SampleAnalyzer/Service/ExceptionService.h"
#include "SampleAnalyzer/Service/TimeService.h"
#include "SampleAnalyzer/Service/Terminate.h"
#include "SampleAnalyzer/Service/PDGService.h"
#include "SampleAnalyzer/Service/Terminate.h"


using namespace MA5;

/// Constructor without arguments
SampleAnalyzer::SampleAnalyzer() 
{
  // Initializing service
  MA5::Terminate::Initialize();
  MA5::LogService::GetInstance();
  MA5::ExceptionService::GetInstance();
  MA5::TimeService::GetInstance();
  MA5::PDGService::GetInstance();

  // Initializing pointer to 0
  progressBar_=0;

  // Header
  INFO << "    * SampleAnalyzer 2.0 for MadAnalysis 5 - Welcome.";
  INFO << endmsg;
 
}

/// Initialization of the SampleAnalyzer
bool SampleAnalyzer::Initialize(int argc, char **argv, 
                                const std::string& pdgFileName)
{
  // Initializing general pointers
  myReader_ = 0;

  // Configuration
  if (!cfg_.Initialize(argc,argv)) return false;

  // Displaying configuration
  cfg_.Display();
  
  // Treating PDG MA5 : to do
  if (pdgFileName=="") {}

  // Getting the list of input files
  std::string filename = cfg_.GetInputListName();

  // Checks if a file has been provided
  INFO << "      - extracting event samples..." << endmsg;
  std::ifstream input(filename.c_str());
  if (!input)
  {
    ERROR << "The file list '"<< filename << "' is not existing." << endmsg;
    return 1;
  }

  // Extracting the event filenames from the list
  std::string tmp;
  while(!input.eof() && !input.fail())
  {
    getline(input,tmp);
    std::stringstream str; str<<tmp; tmp=""; str>>tmp;
    if (!tmp.empty()) inputs_.push_back(tmp);
  }
  input.close();

  // Checking if the list is empty
  if (inputs_.size()==0)
  {
      ERROR << "The file list '"<< filename << "' is empty." << endmsg;
      return false;
  }

  // Extracting the analysis name
  datasetName_ = filename;
  std::string::size_type pos=datasetName_.rfind('.');
  if (pos!=std::string::npos) datasetName_.resize(pos);
  pos=datasetName_.rfind('/');
  if (pos!=std::string::npos) datasetName_.erase(0,pos+1);

  // Initializing counters
  counter_read_.resize(inputs_.size(),0);
  counter_passed_.resize(inputs_.size(),0);

  // Build tables
  fullWriters_.BuildTable();
  fullReaders_.BuildTable();
  fullAnalyses_.BuildPredefinedTable();
  fullAnalyses_.BuildUserTable();
  fullFilters_.BuildPredefinedTable();
  fullFilters_.BuildUserTable();
  fullJetClusterers_.BuildTable();

  // Reset counter
  file_index_=0;
  next_file_=true;

  return true;
}


AnalyzerBase* SampleAnalyzer::InitializeAnalyzer(const std::string& name, 
                                                 const std::string& outputname,
                           const std::map<std::string,std::string>& parameters)
{
  // Display 
  INFO << "      - analyzer '"
       << name << "'" << endmsg;

  // Getting the analysis
  AnalyzerBase* myAnalysis = fullAnalyses_.Get(name);

  // Analysis found ?
  if (myAnalysis==0)
  {
    ERROR << "analysis called '" << name << "' is not found" 
          << endmsg;
    return 0;
  }

  // Putting the analysis in container
  analyzers_.push_back(myAnalysis);

  // Initialize (common part to all analyses)
  if (!myAnalysis->PreInitialize(outputname,
                                 cfg_.IsNoEventWeight()))
  {
    ERROR << "problem during the pre-initialization of the analysis called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Initialize (specific to the analysis)
  if (!myAnalysis->Initialize(cfg_,parameters))
  {
    ERROR << "problem during the initialization of the analysis called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Returning the analysis
  return myAnalysis;
}


FilterBase* SampleAnalyzer::InitializeFilter(const std::string& name, 
                                             const std::string& outputname,
                           const std::map<std::string,std::string>& parameters)
{
  // Display 
  INFO << "      - filter '"
       << name << "'" << endmsg;

  // Getting the analysis
  FilterBase* myFilter = fullFilters_.Get(name);

  // Filter found ?
  if (myFilter==0)
  {
    ERROR << "filter called '" << name << "' is not found" 
          << endmsg;
    return 0;
  }

  // Putting the analysis in container
  filters_.push_back(myFilter);

  // Initialize (common part to all filters)
  if (!myFilter->PreInitialize(outputname,
                               cfg_.IsNoEventWeight()))
  {
    ERROR << "problem during the pre-initialization of the filter called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Initialize (specific to the filter)
  if (!myFilter->Initialize(cfg_,parameters))
  {
    ERROR << "problem during the initialization of the filter called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Returning the filter
  return myFilter;
}


WriterBase* SampleAnalyzer::InitializeWriter(const std::string& name, 
                                   const std::string& outputname)
{
  // Display
  INFO << "      - writer corresponding to output file '"
       << outputname << "'" << endmsg;

  // Getting the analysis
  WriterBase* myWriter = fullWriters_.Get(name);

  // Analysis found ?
  if (myWriter==0)
  {
    ERROR << "writer called '" << name << "' is not found" 
          << endmsg;
    return 0;
  }

  // Putting the analysis in container
  writers_.push_back(myWriter);

  // Initializing
  if (!myWriter->Initialize(outputname))
  {
    ERROR << "problem during the initialization of the writer called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Returning the analysis
  return myWriter;
}


JetClustererBase* SampleAnalyzer::InitializeJetClusterer(
                  const std::string& name, 
                  const std::map<std::string,std::string>& parameters)
{
  // Getting the analysis
  JetClustererBase* myClusterer = fullJetClusterers_.Get(name);

  // Analysis found ?
  if (myClusterer==0)
  {
    ERROR << "jet clustering algorithm called '" << name << "' is not found" 
          << endmsg;
    return 0;
  }

  // Display
  INFO << "      - jet clusterer '"
       << myClusterer->GetName() << "'" << endmsg;

  // Putting the analysis in container
  clusters_.push_back(myClusterer);

  // Initialize (specific to the analysis)
  if (!myClusterer->Initialize(parameters))
  {
    ERROR << "problem during the initialization of the jet clusterer called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Display
  INFO << "        with algo: " << myClusterer->GetParameters() << endmsg;
  INFO << "        with bjet: " << myClusterer->bParameters() << endmsg;
  INFO << "        with tau:  " << myClusterer->tauParameters() << endmsg;

  // Returning the clusterer
  return myClusterer;
}


/// Reading the next event
StatusCode::Type SampleAnalyzer::NextFile(SampleFormat& mySample)
{
  // Finalize previous file
  if (myReader_!=0)
  {
    myReader_->Finalize();
  }  

  // Finalize previous progress bar
  if (progressBar_!=0)
  {
    progressBar_->Finalize();
    INFO << "        => total number of events: " << counter_read_[file_index_-1] 
         << " ( analyzed: " << counter_passed_[file_index_-1] 
         << " ; skipped: " << counter_read_[file_index_-1] - counter_passed_[file_index_-1]
         << " ) " << endmsg;
  }

  // Next file
  file_index_++;

  // Have we read the last file ?
  if (file_index_>inputs_.size()) return StatusCode::FAILURE;
  next_file_=false;

  // Progression bar
  INFO << "    * " << file_index_ <<"/" << inputs_.size() << " ";  
  INFO << " " << inputs_[file_index_-1] << endmsg;

  // Data format
  mySample.setName(inputs_[file_index_-1]);

  // Find an appropiate reader for the file
  myReader_ = fullReaders_.GetByFileExtension(inputs_[file_index_-1]);
  if (myReader_==0)
  {
    ERROR << "the format of the input file is not supported. "
          << "The file is skipped."
          << endmsg;
    return StatusCode::SKIP;
  }

  // Initialize the reader
  myReader_->Initialize(inputs_[file_index_-1], cfg_);

  // Displaying the size of the file
  Long64_t length = myReader_->GetFileSize();
  if (length<0) INFO << "        => file size : unknown" << endmsg;
  else
  {
    UInt_t unit = 0;
    Double_t value=0;
    if (length>1e12)
    {
      value = static_cast<Double_t>(length)/(1024.*1024.*1024.*1024.);
      unit=5;
    }
    if (length>1e9) 
    {
      value = static_cast<Double_t>(length)/(1024.*1024.*1024.);
      unit=4;
    }
    else if (length>1e6)
    {
      value = static_cast<Double_t>(length)/(1024.*1024.);
      unit=3;
    }
    else if (length>1e3)
    {
      value = static_cast<Double_t>(length)/1024.;
      unit=2;
    }
    else
    {
      value = length;
      unit=1;
    }
    std::stringstream str;
    if (unit==1) str << static_cast<UInt_t>(value);
    else str << std::fixed << std::setprecision(2) << value;
    str << " ";
    if (unit==1) str << "octets";
    else if (unit==2) str << "ko";
    else if (unit==3) str << "Mo";
    else if (unit==4) str << "Go";
    else if (unit==5) str << "To";
    else str << "muf";

    INFO << "        => file size : " << str.str() << endmsg;
  }

  // Read the header block
  if (!myReader_->ReadHeader(mySample))
  {
    ERROR << "No header has been found. " 
          << "The file is skipped." << endmsg;
    myReader_->Finalize();
    return StatusCode::SKIP;
  }

  // Finalize the header block
  myReader_->FinalizeHeader(mySample);

  // Dump the header block
  if(mySample.mc()!=0) mySample.mc()->printSubtitle();

  // Initialize the progress bar
  if (progressBar_==0) progressBar_ = new ProgressBar();
  progressBar_->Initialize(35,0,length);

  // Ok !
  return StatusCode::KEEP;
}


/// Reading the next event
StatusCode::Type SampleAnalyzer::NextEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // Read an event
  StatusCode::Type test=myReader_->ReadEvent(myEvent, mySample);

  // GOOD case
  if (test==StatusCode::KEEP)
  {
    // Incrementing counter of number of read events
    counter_read_[file_index_-1]++;

    // Finalize the event and filter the event
    if (!myReader_->FinalizeEvent(mySample,myEvent)) return StatusCode::SKIP;

    // Incrementing counter of number of good events
    counter_passed_[file_index_-1]++;

    // Ok !
    return StatusCode::KEEP;
  }

  // SKIP case
  else if (test==StatusCode::SKIP)
  {
    counter_read_[file_index_-1]++;
    return test;
  }

  // FAILURE case
  else if (test==StatusCode::FAILURE) return test;

  return StatusCode::KEEP;
}


bool SampleAnalyzer::Finalize(std::vector<SampleFormat>& mySamples, 
                              EventFormat& myEvent)
{
  // -----------------------------------------------------------------------
  //                      DUMP NUMBER OF EVENT
  // -----------------------------------------------------------------------
  INFO << "    * Total number of processed events: ";
  ULong64_t nInitial = 0;
  ULong64_t nPassed  = 0;

  for (unsigned int i=0;i<counter_read_.size();i++)   
      nInitial+=counter_read_[i];

  for (unsigned int i=0;i<counter_passed_.size();i++)
      nPassed+=counter_passed_[i];

  if ((nInitial-nPassed)==0)
  {
    INFO << nInitial << "." << endmsg;
  }
  else
  {
    INFO << nPassed << " (" << nInitial-nPassed
         << " events failed)." << endmsg;
  }

  // Saving global information
  SampleFormat summary;
  for (unsigned int i=0;i<counter_read_.size();i++)
  { mySamples[i].setNEvents(counter_read_[i]); }
  FillSummary(summary,mySamples);

  // Finalize analysis
  for (unsigned int i=0;i<analyzers_.size();i++)
  {
    analyzers_[i]->PreFinalize(summary,mySamples);
    analyzers_[i]->Finalize(summary,mySamples);
    analyzers_[i]->PostFinalize(summary,mySamples);
  }

  // Finalize writers
  for (unsigned int i=0;i<writers_.size();i++)
  {
    writers_[i]->WriteFoot(summary);
    writers_[i]->Finalize();
  }

  // Finalize clusters
  for (unsigned int i=0;i<clusters_.size();i++)
  {
    clusters_[i]->Finalize();
  }

  // Display reports
  MA5::TimeService::GetInstance()->WriteGenericReport();
  MA5::ExceptionService::GetInstance()->WarningReport().WriteGenericReport();
  MA5::ExceptionService::GetInstance()->ErrorReport().WriteGenericReport();

  // Kill all singleton services
  MA5::LogService::Kill();
  MA5::ExceptionService::Kill();
  MA5::TimeService::Kill();
  MA5::PDGService::Kill();
  MA5::PhysicsService::kill();

  // -----------------------------------------------------------------------
  //                      Bye bye message
  // -----------------------------------------------------------------------
  INFO << "    * Goodbye."<<endmsg;
  return true;
}


void SampleAnalyzer::FillSummary(SampleFormat& summary,
                                 const std::vector<SampleFormat>& samples)
{
  // Create a SampleFormat container for summary info
  summary.setName("FINAL");
  summary.InitializeMC();
  summary.InitializeRec();

  // Loop over samples
  for (unsigned int i=0;i<samples.size();i++)
  {
    // Total number of events
    summary.nevents_              += samples[i].nevents_;

    // Requiring MC info
    if(samples[i].mc()==0) {continue ;}

    // Mean cross-section
    summary.mc()->xsection_       += samples[i].mc()->xsection_ * 
                                     samples[i].nevents_;
    summary.mc()->xsection_error_ += samples[i].mc()->xsection_error_ * 
                                     samples[i].mc()->xsection_error_ *
                                     samples[i].nevents_ *
                                     samples[i].nevents_;

    // Sum of weights
    summary.mc()->sumweight_positive_ += samples[i].mc()->sumweight_positive_;
    summary.mc()->sumweight_negative_ += samples[i].mc()->sumweight_negative_;
  }
  if (samples.size()!=0)
  {
    summary.mc()->xsection_       /= summary.nevents_;
    summary.mc()->xsection_error_  = sqrt(summary.mc()->xsection_error_)
                                   / summary.nevents_;
  }
}



/// Updating the progress bar
void SampleAnalyzer::UpdateProgressBar()
{
  progressBar_->Update(myReader_->GetPosition());
}
