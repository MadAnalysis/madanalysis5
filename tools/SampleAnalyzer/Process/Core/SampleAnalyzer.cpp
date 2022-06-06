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


// STL headers
#include <string>
#include <sys/stat.h>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Core/SampleAnalyzer.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/TimeService.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Commons/Service/Terminate.h"
#include "SampleAnalyzer/Commons/Service/CompilationService.h"
#include "SampleAnalyzer/Process/Core/ProgressBar.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"


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

  // Check datatypes
  CheckDatatypes();

  // Initializing pointer to 0
  progressBar_=0;
  LastFileFail_=false;

  // Header
  INFO << "    * SampleAnalyzer for MadAnalysis 5 - Welcome.";
  INFO << endmsg;
 
}


/// CheckDatatypes
void SampleAnalyzer::CheckDatatypes() const
{
  MAuint32 value = 0;

  // MAint8
  try
  {
    value = sizeof(MAint8);
    if (value!=1) throw EXCEPTION_WARNING("MAint8 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(std::exception const& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAuint8
  try
  {
    value = sizeof(MAuint8);
    if (value!=1) throw EXCEPTION_WARNING("MAuint8 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    


  // MAint16
  try
  {
    value = sizeof(MAint16);
    if (value!=2) throw EXCEPTION_WARNING("MAint16 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAuint16
  try
  {
    value = sizeof(MAuint16);
    if (value!=2) throw EXCEPTION_WARNING("MAuint16 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAint32
  try
  {
    value = sizeof(MAint32);
    if (value!=4) throw EXCEPTION_WARNING("MAint32 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAuint32
  try
  {
    value = sizeof(MAuint32);
    if (value!=4) throw EXCEPTION_WARNING("MAuint32 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAint64
  try
  {
    value = sizeof(MAint64);
    if (value!=8) throw EXCEPTION_WARNING("MAint64 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAuint64
  try
  {
    value = sizeof(MAuint64);
    if (value!=8) throw EXCEPTION_WARNING("MAuint64 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAfloat32
  try
  {
    value = sizeof(MAfloat32);
    if (value!=4) throw EXCEPTION_WARNING("MAfloat32 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    

  // MAfloat64
  try
  {
    value = sizeof(MAfloat64);
    if (value!=8) throw EXCEPTION_WARNING("MAfloat64 type corresponds to "+CONVERT->ToString(value*8)+" bits","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    
}


/// Initialization of the SampleAnalyzer
MAbool SampleAnalyzer::Initialize(MAint32 argc, MAchar**argv, 
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
  INFO << "      - extracting the list of event samples..." << endmsg;
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
  fullJetClusterers_.BuildTable();
  fullDetectors_.BuildTable();

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
                                 &cfg_))
  {
    ERROR << "problem during the pre-initialization of the analysis called '" 
          << name << "'" << endmsg;
    return 0;
  }

  myAnalysis->SetParameters(parameters);

  // Returning the analysis
  return myAnalysis;
}

/// Post initialization: creation of the output directory structure
inline MAint32 CreateDir(std::string dirname)
{
  struct stat myStat;
  if (!((stat(dirname.c_str(), &myStat) == 0) && (((myStat.st_mode) & S_IFMT) == S_IFDIR)))
    { if(mkdir(dirname.c_str(),0755) != 0) { return -1; } }
  else { return 1; }
  return 0;
}

/// Creating the Output directory and its sub-structure
MAbool SampleAnalyzer::CreateDirectoryStructure()
{
  // Check if the output directory exists -> if not: create it
  std::string dirname="../Output";
  if (CreateDir(dirname)==-1) return false;

  // Check if the SAF directory exists -> if not: create it
  dirname="../Output/SAF";
  if (CreateDir(dirname)==-1) return false;

  // Check whether a directory for the investigated dataset exists -> if not create it
  dirname = cfg_.GetInputFileName();
  size_t pos = dirname.find_last_of('/');
  if(pos!=std::string::npos) dirname = "../Output/SAF/" + dirname.substr(pos+1);
  else                       dirname = "../Output/SAF/" + dirname;
  size_t found  = dirname.find(".list");
  if(found!=std::string::npos) dirname.replace(found,5,"");
  if(CreateDir(dirname)==-1) { return false; }

  // Creating one subdirectory for each analysis
  for(MAuint32 i=0;i<analyzers_.size(); i++)
  {
    std::string newdirname = dirname + "/" + analyzers_[i]->name();
    MAint32 check = -1;
    for(MAuint32 ii=0; check!=0 ; ii++)
    {
      std::stringstream ss; ss << ii;
      check = CreateDir(newdirname + "_" + ss.str());
      if(check==-1) { return false; }
      else          { analyzers_[i]->SetOutputDir( newdirname + "_" + ss.str()); }
    }

    // Creating one suybdirectory for the histograms and another one for the cutflow
    if(CreateDir(analyzers_[i]->Output() + "/Histograms")==-1) {  return false; }
    if(CreateDir(analyzers_[i]->Output() + "/Cutflows")==-1) {  return false; }
  }
  
  // Everything is fine
  return true;
}

MAbool SampleAnalyzer::PostInitialize()
{
  // Creating the directory structure
  if(!CreateDirectoryStructure())
  {
    ERROR << "The output directory structure cannot be created properly" << endmsg;
    return false;
  }

    // Initialize (specific to the analysis)
    // @JACK: this way analysis will have access to output and name information at initialization
    //        this is particularly important if one wants to build their own output style where they can
    //        output the file to the proper directory.
    for(auto myAnalysis: analyzers_)
    {
        if (!myAnalysis->Initialize(cfg_))
        {
            ERROR << "problem during the initialization of the analysis called '"
                  << myAnalysis->name() << "'" << endmsg;
            return 0;
        }
    }

  // Everything was fine
  return true;
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

  // Creating the directory structure
  // Check if the output directory exists -> if not: create it
  std::string dirname="../Output";
  if(CreateDir(dirname)==-1) { return 0; }

  // Check if the SAF directory exists -> if not: create it
  dirname="../Output/SAF";
  if(CreateDir(dirname)==-1) { return 0; }

  // Check whether a directory for the investigated dataset exists -> if not create it
  dirname = cfg_.GetInputFileName();
  size_t pos = dirname.find_last_of('/');
  if(pos!=std::string::npos) dirname = "../Output/SAF/" + dirname.substr(pos+1);
  else                       dirname = "../Output/SAF/" + dirname;
  size_t found  = dirname.find(".list");
  if(found!=std::string::npos) dirname.replace(found,5,"");
  if(CreateDir(dirname)==-1) { return 0; }

  // Creating one ouptput subdirectory for the writer
  std::string newoutdirname="";
  std::stringstream ss0; ss0 << (writers_.size()-1);
  MAint32 check = -1;
  for(MAuint32 ii=0; check!=0 ; ii++)
  {
    std::stringstream ss; ss << ii;
    newoutdirname = dirname + "/" + name + "Events" + ss0.str() + "_" + ss.str();
    check = CreateDir(newoutdirname);
    if(check==-1) { return 0; }
  }

  // Initializing
  if (!myWriter->Initialize(&cfg_,newoutdirname+'/'+outputname))
  {
    ERROR << "problem during the initialization of the writer called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Returning the analysis
  return myWriter;
}


JetClusterer* SampleAnalyzer::InitializeJetClusterer(
                  const std::string& name, 
                  const std::map<std::string,std::string>& parameters)
{
  // Getting the analysis
  JetClusterer* myClusterer = fullJetClusterers_.Get(name);

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


DetectorBase* SampleAnalyzer::InitializeDetector(
                  const std::string& name, const std::string& configFile, 
                  const std::map<std::string,std::string>& parameters)
{
  // Getting the detector
  DetectorBase* myDetector = fullDetectors_.Get(name);

  // Detector found ?
  if (myDetector==0)
  {
    ERROR << "detector algorithm called '" << name << "' is not found" 
          << endmsg;
    return 0;
  }

  // Display
  INFO << "      - fast-simulation package '"
       << myDetector->GetName() << "'" << endmsg;

  // Putting the detector in container
  detectors_.push_back(myDetector);

  // Creating the directory structure
  // Check if the output directory exists -> if not: create it
  std::string dirname="../Output";
  if(CreateDir(dirname)==-1) { return 0; }

  // Check if the SAF directory exists -> if not: create it
  dirname="../SAF";
  if(CreateDir(dirname)==-1) { return 0; }

  // Check whether a directory for the investigated dataset exists -> if not create it
  dirname = cfg_.GetInputFileName();
  size_t pos = dirname.find_last_of('/');
  if(pos!=std::string::npos) dirname = "../Output/SAF/" + dirname.substr(pos+1);
  else                       dirname = "../Output/SAF/" + dirname;
  size_t found  = dirname.find(".list");
  if(found!=std::string::npos) dirname.replace(found,5,"");
  if(CreateDir(dirname)==-1) { return 0; }

  // Creating one ouptput subdirectory for the writer
  std::string newoutdirname="";
  std::stringstream ss0; ss0 << (detectors_.size()-1);
  MAint32 check = -1;
  for(MAuint32 ii=0; check!=0 ; ii++)
  {
    std::stringstream ss; ss << ii;
    newoutdirname = dirname + "/RecoEvents" + ss0.str() + "_" + ss.str();
    check = CreateDir(newoutdirname);
    if(check==-1) { return 0; }
  }
  std::map<std::string,std::string> prm2 = parameters;
  prm2["outputdir"] = newoutdirname;

  // Initialize (specific to the detector)
  std::string ma5dir = std::getenv("MA5_BASE");
  //  std::string config = ma5dir+"/tools/SampleAnalyzer/"+configFile;
  std::string config = configFile;
  if (!myDetector->Initialize(config, prm2))
  {
    ERROR << "problem during the initialization of the fast-simulation package called '" 
          << name << "'" << endmsg;
    return 0;
  }

  // Display
  INFO << myDetector->PrintConfigFile() << endmsg;

  // Returning the clusterer
  return myDetector;
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
  if (!LastFileFail_ && progressBar_!=0)
  {
    progressBar_->Finalize();
    INFO << "        => total number of events: " << counter_read_[file_index_-1] 
         << " ( analyzed: " << counter_passed_[file_index_-1] 
         << " ; skipped: " << counter_read_[file_index_-1] - counter_passed_[file_index_-1]
         << " ) " << endmsg;
  }
  LastFileFail_=false;

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
    LastFileFail_=true;
    return StatusCode::SKIP;
  }

  // Initialize the reader
  if (!myReader_->Initialize(inputs_[file_index_-1], cfg_))
  {
    LastFileFail_=true;
    return StatusCode::SKIP;
  }

  // Displaying the size of the file
  MAint64 length = myReader_->GetFileSize();
  if (length<0) INFO << "        => file size: unknown" << endmsg;
  else
  {
    MAuint32 unit = 0;
    MAfloat64 value=0;
    if (length>1e12)
    {
      value = static_cast<MAfloat64>(length)/(1024.*1024.*1024.*1024.);
      unit=5;
    }
    if (length>1e9) 
    {
      value = static_cast<MAfloat64>(length)/(1024.*1024.*1024.);
      unit=4;
    }
    else if (length>1e6)
    {
      value = static_cast<MAfloat64>(length)/(1024.*1024.);
      unit=3;
    }
    else if (length>1e3)
    {
      value = static_cast<MAfloat64>(length)/1024.;
      unit=2;
    }
    else
    {
      value = length;
      unit=1;
    }
    std::stringstream str;
    if (unit==1) str << static_cast<MAuint32>(value);
    else str << std::fixed << std::setprecision(2) << value;
    str << " ";
    if (unit==1) str << "octets";
    else if (unit==2) str << "ko";
    else if (unit==3) str << "Mo";
    else if (unit==4) str << "Go";
    else if (unit==5) str << "To";
    else str << "muf";

    INFO << "        => file size: " << str.str() << endmsg;
  }
  length = myReader_->GetFinalPosition();

  // Read the header block
  if (!myReader_->ReadHeader(mySample))
  {
    ERROR << "No header has been found. " 
          << "The file is skipped." << endmsg;
    LastFileFail_=true;
    return StatusCode::SKIP;
  }

  // Finalize the header block
  myReader_->FinalizeHeader(mySample);

  // Dump the header block
  mySample.printSubtitle();

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


/// Home made functions to make reasonnable filenames
inline void ReplaceAll(std::string &name, const std::string &In, const std::string &Out)
{
  size_t pos = name.find(In);
  while(pos!=std::string::npos)
  {
    name.replace(pos,In.size(),Out);
    pos = name.find(In);
  }
}

inline std::string CleanName(const std::string &name)
{
  std::string tmp=name;
  ReplaceAll(tmp, "/",  "_slash_");
  ReplaceAll(tmp, "->", "_to_");
  ReplaceAll(tmp, ">=", "_greater_than_or_equal_to_");
  ReplaceAll(tmp, ">",  "_greater_than_");
  ReplaceAll(tmp, "<=", "_smaller_than_or_equal_to_");
  ReplaceAll(tmp, "<",  "_smaller_than_");
  ReplaceAll(tmp, " ",  "_");
  ReplaceAll(tmp, ",",  "_");
  ReplaceAll(tmp, "+",  "_");
  ReplaceAll(tmp, "-",  "_");
  ReplaceAll(tmp, "(",  "_lp_");
  ReplaceAll(tmp, ")",  "_rp_");
  return tmp;
}

/// Finalize fuction
MAbool SampleAnalyzer::Finalize(std::vector<SampleFormat>& mySamples, 
                              EventFormat& myEvent)
{
  // -----------------------------------------------------------------------
  //                      DUMP NUMBER OF EVENT
  // -----------------------------------------------------------------------
  INFO << "    * Total number of processed events: ";
  MAuint64 nInitial = 0;
  MAuint64 nPassed  = 0;

  for (MAuint32 i=0;i<counter_read_.size();i++)   
      nInitial+=counter_read_[i];

  for (MAuint32 i=0;i<counter_passed_.size();i++)
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
  for (MAuint32 i=0;i<counter_read_.size();i++)
  { mySamples[i].setNEvents(counter_read_[i]); }
  FillSummary(summary,mySamples);

  // Finalize analysis
  // Creating the general SAF file (sample info)
  std::string datasetname = cfg_.GetInputFileName();
  size_t pos = datasetname.find_last_of('/');
  if(pos!=std::string::npos) datasetname = datasetname.substr(pos+1);
  size_t found  = datasetname.find(".list");
  if(found!=std::string::npos) datasetname.replace(found,5,"");
  std::string general = "../Output/SAF/" + datasetname + "/" + datasetname + ".saf";
  SAFWriter out;
  out.Initialize(&cfg_, general.c_str());
  out.WriteHeader(summary);
  out.WriteFiles(mySamples);
  out.WriteFoot(summary);
  out.Finalize();

  // Creating the histo SAF file
  for(MAuint32 i=0; i<analyzers_.size(); i++)
  {
    std::string safname = analyzers_[i]->Output() + "/Histograms/histos.saf";
    out.Initialize(&cfg_, safname.c_str());
    out.WriteHeader();
    analyzers_[i]->Manager()->GetPlotManager()->Write_TextFormat(out);
    out.WriteFoot();
    out.Finalize();
  }

  // Linking the histos to the SRs
  for(MAuint32 i=0; i<analyzers_.size(); i++)
  {
    std::string safname = analyzers_[i]->Output() + "/" + analyzers_[i]->name() + ".saf";
    out.Initialize(&cfg_, safname.c_str());
    out.WriteHeader();
    analyzers_[i]->Manager()->WriteHistoDefinition(out);
    out.WriteFoot();
    out.Finalize();
  }


  // Saving the cut flows
  for(MAuint32 i=0; i<analyzers_.size(); i++)
  {
    AnalyzerBase* myanalysis = analyzers_[i];
    for(MAuint32 j=0; j<myanalysis->Manager()->Regions().size(); j++)
    {
      RegionSelection *myRS = myanalysis->Manager()->Regions()[j];
      std::string safname = myanalysis->Output() + "/Cutflows/" + 
         CleanName(myRS->GetName()) + ".saf";
      out.Initialize(&cfg_, safname.c_str());
      out.WriteHeader();
      myRS->WriteCutflow(out);
      out.WriteFoot();
      out.Finalize();
    }
  }

  // The user-defined stuff
  for(MAuint32 i=0; i<analyzers_.size(); i++)
    analyzers_[i]->Finalize(summary,mySamples);

  // Finalize clusters
  for (MAuint32 i=0;i<clusters_.size();i++)
  {
    clusters_[i]->Finalize();
  }

  // Finalize wrtiers
  for(MAuint32 i=0; i<writers_.size(); i++)
  {
    writers_[i]->WriteFoot(summary);
    writers_[i]->Finalize();
  }

  // Finalize detectors
  for (MAuint32 i=0;i<detectors_.size();i++)
  {
    detectors_[i]->Finalize();
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
  for (MAuint32 i=0;i<samples.size();i++)
  {
    // Total number of events
    summary.nevents_ += samples[i].nevents_;

    // Requiring MC info
    if(samples[i].mc()==0) continue ;

    // Mean cross-section
    summary.mc()->xsection_       += samples[i].mc()->xsection() * 
                                     samples[i].nevents_;
    summary.mc()->xsection_error_ += samples[i].mc()->xsection_error() * 
                                     samples[i].mc()->xsection_error() *
                                     samples[i].nevents_ *
                                     samples[i].nevents_;

    // Sum of weights
    summary.mc()->sumweight_positive_ += samples[i].mc()->sumweight_positive_;
    summary.mc()->sumweight_negative_ += samples[i].mc()->sumweight_negative_;
  }

  // Finalizing xsection
  if (summary.nevents_!=0)
  {
    summary.mc()->xsection_       /= summary.nevents_;
    summary.mc()->xsection_error_  = sqrt(summary.mc()->xsection_error_) / 
                                     summary.nevents_;
  }
  else
  {
    summary.mc()->xsection_       = 0;
    summary.mc()->xsection_error_ = 0;
  }

}



/// Updating the progress bar
void SampleAnalyzer::UpdateProgressBar()
{
  progressBar_->Update(myReader_->GetPosition());
}



void SampleAnalyzer::HeadSR(std::ostream &outwriter)
{
  outwriter << "#";
  for(MAuint32 i=0; i<analyzers_.size(); i++)
  {
    outwriter <<" ";
    analyzers_[i]->Manager()->HeadSR(outwriter, analyzers_[i]->name());
  }
}


void SampleAnalyzer::DumpSR(std::ostream &outwriter)
{
  for(MAuint32 i=0; i<analyzers_.size(); i++)
    analyzers_[i]->Manager()->DumpSR(outwriter);
  outwriter << std::endl;
}

void SampleAnalyzer::AddDefaultHadronic()
{
    // definition of the multiparticle "hadronic"
    PHYSICS->mcConfig().AddHadronicId(-20543);
    PHYSICS->mcConfig().AddHadronicId(-20533);
    PHYSICS->mcConfig().AddHadronicId(-20523);
    PHYSICS->mcConfig().AddHadronicId(-20513);
    PHYSICS->mcConfig().AddHadronicId(-20433);
    PHYSICS->mcConfig().AddHadronicId(-20423);
    PHYSICS->mcConfig().AddHadronicId(-20413);
    PHYSICS->mcConfig().AddHadronicId(-20323);
    PHYSICS->mcConfig().AddHadronicId(-20313);
    PHYSICS->mcConfig().AddHadronicId(-20213);
    PHYSICS->mcConfig().AddHadronicId(-10543);
    PHYSICS->mcConfig().AddHadronicId(-10541);
    PHYSICS->mcConfig().AddHadronicId(-10533);
    PHYSICS->mcConfig().AddHadronicId(-10531);
    PHYSICS->mcConfig().AddHadronicId(-10523);
    PHYSICS->mcConfig().AddHadronicId(-10521);
    PHYSICS->mcConfig().AddHadronicId(-10513);
    PHYSICS->mcConfig().AddHadronicId(-10511);
    PHYSICS->mcConfig().AddHadronicId(-10433);
    PHYSICS->mcConfig().AddHadronicId(-10431);
    PHYSICS->mcConfig().AddHadronicId(-10423);
    PHYSICS->mcConfig().AddHadronicId(-10421);
    PHYSICS->mcConfig().AddHadronicId(-10413);
    PHYSICS->mcConfig().AddHadronicId(-10411);
    PHYSICS->mcConfig().AddHadronicId(-10323);
    PHYSICS->mcConfig().AddHadronicId(-10321);
    PHYSICS->mcConfig().AddHadronicId(-10313);
    PHYSICS->mcConfig().AddHadronicId(-10311);
    PHYSICS->mcConfig().AddHadronicId(-10213);
    PHYSICS->mcConfig().AddHadronicId(-10211);
    PHYSICS->mcConfig().AddHadronicId(-5554);
    PHYSICS->mcConfig().AddHadronicId(-5544);
    PHYSICS->mcConfig().AddHadronicId(-5542);
    PHYSICS->mcConfig().AddHadronicId(-5534);
    PHYSICS->mcConfig().AddHadronicId(-5532);
    PHYSICS->mcConfig().AddHadronicId(-5524);
    PHYSICS->mcConfig().AddHadronicId(-5522);
    PHYSICS->mcConfig().AddHadronicId(-5514);
    PHYSICS->mcConfig().AddHadronicId(-5512);
    PHYSICS->mcConfig().AddHadronicId(-5503);
    PHYSICS->mcConfig().AddHadronicId(-5444);
    PHYSICS->mcConfig().AddHadronicId(-5442);
    PHYSICS->mcConfig().AddHadronicId(-5434);
    PHYSICS->mcConfig().AddHadronicId(-5432);
    PHYSICS->mcConfig().AddHadronicId(-5424);
    PHYSICS->mcConfig().AddHadronicId(-5422);
    PHYSICS->mcConfig().AddHadronicId(-5414);
    PHYSICS->mcConfig().AddHadronicId(-5412);
    PHYSICS->mcConfig().AddHadronicId(-5403);
    PHYSICS->mcConfig().AddHadronicId(-5401);
    PHYSICS->mcConfig().AddHadronicId(-5342);
    PHYSICS->mcConfig().AddHadronicId(-5334);
    PHYSICS->mcConfig().AddHadronicId(-5332);
    PHYSICS->mcConfig().AddHadronicId(-5324);
    PHYSICS->mcConfig().AddHadronicId(-5322);
    PHYSICS->mcConfig().AddHadronicId(-5314);
    PHYSICS->mcConfig().AddHadronicId(-5312);
    PHYSICS->mcConfig().AddHadronicId(-5303);
    PHYSICS->mcConfig().AddHadronicId(-5301);
    PHYSICS->mcConfig().AddHadronicId(-5242);
    PHYSICS->mcConfig().AddHadronicId(-5232);
    PHYSICS->mcConfig().AddHadronicId(-5224);
    PHYSICS->mcConfig().AddHadronicId(-5222);
    PHYSICS->mcConfig().AddHadronicId(-5214);
    PHYSICS->mcConfig().AddHadronicId(-5212);
    PHYSICS->mcConfig().AddHadronicId(-5203);
    PHYSICS->mcConfig().AddHadronicId(-5201);
    PHYSICS->mcConfig().AddHadronicId(-5142);
    PHYSICS->mcConfig().AddHadronicId(-5132);
    PHYSICS->mcConfig().AddHadronicId(-5122);
    PHYSICS->mcConfig().AddHadronicId(-5114);
    PHYSICS->mcConfig().AddHadronicId(-5112);
    PHYSICS->mcConfig().AddHadronicId(-5103);
    PHYSICS->mcConfig().AddHadronicId(-5101);
    PHYSICS->mcConfig().AddHadronicId(-4444);
    PHYSICS->mcConfig().AddHadronicId(-4434);
    PHYSICS->mcConfig().AddHadronicId(-4432);
    PHYSICS->mcConfig().AddHadronicId(-4424);
    PHYSICS->mcConfig().AddHadronicId(-4422);
    PHYSICS->mcConfig().AddHadronicId(-4414);
    PHYSICS->mcConfig().AddHadronicId(-4412);
    PHYSICS->mcConfig().AddHadronicId(-4403);
    PHYSICS->mcConfig().AddHadronicId(-4334);
    PHYSICS->mcConfig().AddHadronicId(-4332);
    PHYSICS->mcConfig().AddHadronicId(-4324);
    PHYSICS->mcConfig().AddHadronicId(-4322);
    PHYSICS->mcConfig().AddHadronicId(-4314);
    PHYSICS->mcConfig().AddHadronicId(-4312);
    PHYSICS->mcConfig().AddHadronicId(-4303);
    PHYSICS->mcConfig().AddHadronicId(-4301);
    PHYSICS->mcConfig().AddHadronicId(-4232);
    PHYSICS->mcConfig().AddHadronicId(-4224);
    PHYSICS->mcConfig().AddHadronicId(-4222);
    PHYSICS->mcConfig().AddHadronicId(-4214);
    PHYSICS->mcConfig().AddHadronicId(-4212);
    PHYSICS->mcConfig().AddHadronicId(-4203);
    PHYSICS->mcConfig().AddHadronicId(-4201);
    PHYSICS->mcConfig().AddHadronicId(-4132);
    PHYSICS->mcConfig().AddHadronicId(-4122);
    PHYSICS->mcConfig().AddHadronicId(-4114);
    PHYSICS->mcConfig().AddHadronicId(-4112);
    PHYSICS->mcConfig().AddHadronicId(-4103);
    PHYSICS->mcConfig().AddHadronicId(-4101);
    PHYSICS->mcConfig().AddHadronicId(-3334);
    PHYSICS->mcConfig().AddHadronicId(-3324);
    PHYSICS->mcConfig().AddHadronicId(-3322);
    PHYSICS->mcConfig().AddHadronicId(-3314);
    PHYSICS->mcConfig().AddHadronicId(-3312);
    PHYSICS->mcConfig().AddHadronicId(-3303);
    PHYSICS->mcConfig().AddHadronicId(-3224);
    PHYSICS->mcConfig().AddHadronicId(-3222);
    PHYSICS->mcConfig().AddHadronicId(-3214);
    PHYSICS->mcConfig().AddHadronicId(-3212);
    PHYSICS->mcConfig().AddHadronicId(-3203);
    PHYSICS->mcConfig().AddHadronicId(-3201);
    PHYSICS->mcConfig().AddHadronicId(-3122);
    PHYSICS->mcConfig().AddHadronicId(-3114);
    PHYSICS->mcConfig().AddHadronicId(-3112);
    PHYSICS->mcConfig().AddHadronicId(-3103);
    PHYSICS->mcConfig().AddHadronicId(-3101);
    PHYSICS->mcConfig().AddHadronicId(-2224);
    PHYSICS->mcConfig().AddHadronicId(-2214);
    PHYSICS->mcConfig().AddHadronicId(-2212);
    PHYSICS->mcConfig().AddHadronicId(-2203);
    PHYSICS->mcConfig().AddHadronicId(-2114);
    PHYSICS->mcConfig().AddHadronicId(-2112);
    PHYSICS->mcConfig().AddHadronicId(-2103);
    PHYSICS->mcConfig().AddHadronicId(-2101);
    PHYSICS->mcConfig().AddHadronicId(-1114);
    PHYSICS->mcConfig().AddHadronicId(-1103);
    PHYSICS->mcConfig().AddHadronicId(-545);
    PHYSICS->mcConfig().AddHadronicId(-543);
    PHYSICS->mcConfig().AddHadronicId(-541);
    PHYSICS->mcConfig().AddHadronicId(-535);
    PHYSICS->mcConfig().AddHadronicId(-533);
    PHYSICS->mcConfig().AddHadronicId(-531);
    PHYSICS->mcConfig().AddHadronicId(-525);
    PHYSICS->mcConfig().AddHadronicId(-523);
    PHYSICS->mcConfig().AddHadronicId(-521);
    PHYSICS->mcConfig().AddHadronicId(-515);
    PHYSICS->mcConfig().AddHadronicId(-513);
    PHYSICS->mcConfig().AddHadronicId(-511);
    PHYSICS->mcConfig().AddHadronicId(-435);
    PHYSICS->mcConfig().AddHadronicId(-433);
    PHYSICS->mcConfig().AddHadronicId(-431);
    PHYSICS->mcConfig().AddHadronicId(-425);
    PHYSICS->mcConfig().AddHadronicId(-423);
    PHYSICS->mcConfig().AddHadronicId(-421);
    PHYSICS->mcConfig().AddHadronicId(-415);
    PHYSICS->mcConfig().AddHadronicId(-413);
    PHYSICS->mcConfig().AddHadronicId(-411);
    PHYSICS->mcConfig().AddHadronicId(-325);
    PHYSICS->mcConfig().AddHadronicId(-323);
    PHYSICS->mcConfig().AddHadronicId(-321);
    PHYSICS->mcConfig().AddHadronicId(-315);
    PHYSICS->mcConfig().AddHadronicId(-313);
    PHYSICS->mcConfig().AddHadronicId(-311);
    PHYSICS->mcConfig().AddHadronicId(-215);
    PHYSICS->mcConfig().AddHadronicId(-213);
    PHYSICS->mcConfig().AddHadronicId(-211);
    PHYSICS->mcConfig().AddHadronicId(111);
    PHYSICS->mcConfig().AddHadronicId(113);
    PHYSICS->mcConfig().AddHadronicId(115);
    PHYSICS->mcConfig().AddHadronicId(130);
    PHYSICS->mcConfig().AddHadronicId(211);
    PHYSICS->mcConfig().AddHadronicId(213);
    PHYSICS->mcConfig().AddHadronicId(215);
    PHYSICS->mcConfig().AddHadronicId(221);
    PHYSICS->mcConfig().AddHadronicId(223);
    PHYSICS->mcConfig().AddHadronicId(225);
    PHYSICS->mcConfig().AddHadronicId(310);
    PHYSICS->mcConfig().AddHadronicId(311);
    PHYSICS->mcConfig().AddHadronicId(313);
    PHYSICS->mcConfig().AddHadronicId(315);
    PHYSICS->mcConfig().AddHadronicId(321);
    PHYSICS->mcConfig().AddHadronicId(323);
    PHYSICS->mcConfig().AddHadronicId(325);
    PHYSICS->mcConfig().AddHadronicId(331);
    PHYSICS->mcConfig().AddHadronicId(333);
    PHYSICS->mcConfig().AddHadronicId(335);
    PHYSICS->mcConfig().AddHadronicId(411);
    PHYSICS->mcConfig().AddHadronicId(413);
    PHYSICS->mcConfig().AddHadronicId(415);
    PHYSICS->mcConfig().AddHadronicId(421);
    PHYSICS->mcConfig().AddHadronicId(423);
    PHYSICS->mcConfig().AddHadronicId(425);
    PHYSICS->mcConfig().AddHadronicId(431);
    PHYSICS->mcConfig().AddHadronicId(433);
    PHYSICS->mcConfig().AddHadronicId(435);
    PHYSICS->mcConfig().AddHadronicId(441);
    PHYSICS->mcConfig().AddHadronicId(443);
    PHYSICS->mcConfig().AddHadronicId(445);
    PHYSICS->mcConfig().AddHadronicId(511);
    PHYSICS->mcConfig().AddHadronicId(513);
    PHYSICS->mcConfig().AddHadronicId(515);
    PHYSICS->mcConfig().AddHadronicId(521);
    PHYSICS->mcConfig().AddHadronicId(523);
    PHYSICS->mcConfig().AddHadronicId(525);
    PHYSICS->mcConfig().AddHadronicId(531);
    PHYSICS->mcConfig().AddHadronicId(533);
    PHYSICS->mcConfig().AddHadronicId(535);
    PHYSICS->mcConfig().AddHadronicId(541);
    PHYSICS->mcConfig().AddHadronicId(543);
    PHYSICS->mcConfig().AddHadronicId(545);
    PHYSICS->mcConfig().AddHadronicId(551);
    PHYSICS->mcConfig().AddHadronicId(553);
    PHYSICS->mcConfig().AddHadronicId(555);
    PHYSICS->mcConfig().AddHadronicId(1103);
    PHYSICS->mcConfig().AddHadronicId(1114);
    PHYSICS->mcConfig().AddHadronicId(2101);
    PHYSICS->mcConfig().AddHadronicId(2103);
    PHYSICS->mcConfig().AddHadronicId(2112);
    PHYSICS->mcConfig().AddHadronicId(2114);
    PHYSICS->mcConfig().AddHadronicId(2203);
    PHYSICS->mcConfig().AddHadronicId(2212);
    PHYSICS->mcConfig().AddHadronicId(2214);
    PHYSICS->mcConfig().AddHadronicId(2224);
    PHYSICS->mcConfig().AddHadronicId(3101);
    PHYSICS->mcConfig().AddHadronicId(3103);
    PHYSICS->mcConfig().AddHadronicId(3112);
    PHYSICS->mcConfig().AddHadronicId(3114);
    PHYSICS->mcConfig().AddHadronicId(3122);
    PHYSICS->mcConfig().AddHadronicId(3201);
    PHYSICS->mcConfig().AddHadronicId(3203);
    PHYSICS->mcConfig().AddHadronicId(3212);
    PHYSICS->mcConfig().AddHadronicId(3214);
    PHYSICS->mcConfig().AddHadronicId(3222);
    PHYSICS->mcConfig().AddHadronicId(3224);
    PHYSICS->mcConfig().AddHadronicId(3303);
    PHYSICS->mcConfig().AddHadronicId(3312);
    PHYSICS->mcConfig().AddHadronicId(3314);
    PHYSICS->mcConfig().AddHadronicId(3322);
    PHYSICS->mcConfig().AddHadronicId(3324);
    PHYSICS->mcConfig().AddHadronicId(3334);
    PHYSICS->mcConfig().AddHadronicId(4101);
    PHYSICS->mcConfig().AddHadronicId(4103);
    PHYSICS->mcConfig().AddHadronicId(4112);
    PHYSICS->mcConfig().AddHadronicId(4114);
    PHYSICS->mcConfig().AddHadronicId(4122);
    PHYSICS->mcConfig().AddHadronicId(4132);
    PHYSICS->mcConfig().AddHadronicId(4201);
    PHYSICS->mcConfig().AddHadronicId(4203);
    PHYSICS->mcConfig().AddHadronicId(4212);
    PHYSICS->mcConfig().AddHadronicId(4214);
    PHYSICS->mcConfig().AddHadronicId(4222);
    PHYSICS->mcConfig().AddHadronicId(4224);
    PHYSICS->mcConfig().AddHadronicId(4232);
    PHYSICS->mcConfig().AddHadronicId(4301);
    PHYSICS->mcConfig().AddHadronicId(4303);
    PHYSICS->mcConfig().AddHadronicId(4312);
    PHYSICS->mcConfig().AddHadronicId(4314);
    PHYSICS->mcConfig().AddHadronicId(4322);
    PHYSICS->mcConfig().AddHadronicId(4324);
    PHYSICS->mcConfig().AddHadronicId(4332);
    PHYSICS->mcConfig().AddHadronicId(4334);
    PHYSICS->mcConfig().AddHadronicId(4403);
    PHYSICS->mcConfig().AddHadronicId(4412);
    PHYSICS->mcConfig().AddHadronicId(4414);
    PHYSICS->mcConfig().AddHadronicId(4422);
    PHYSICS->mcConfig().AddHadronicId(4424);
    PHYSICS->mcConfig().AddHadronicId(4432);
    PHYSICS->mcConfig().AddHadronicId(4434);
    PHYSICS->mcConfig().AddHadronicId(4444);
    PHYSICS->mcConfig().AddHadronicId(5101);
    PHYSICS->mcConfig().AddHadronicId(5103);
    PHYSICS->mcConfig().AddHadronicId(5112);
    PHYSICS->mcConfig().AddHadronicId(5114);
    PHYSICS->mcConfig().AddHadronicId(5122);
    PHYSICS->mcConfig().AddHadronicId(5132);
    PHYSICS->mcConfig().AddHadronicId(5142);
    PHYSICS->mcConfig().AddHadronicId(5201);
    PHYSICS->mcConfig().AddHadronicId(5203);
    PHYSICS->mcConfig().AddHadronicId(5212);
    PHYSICS->mcConfig().AddHadronicId(5214);
    PHYSICS->mcConfig().AddHadronicId(5222);
    PHYSICS->mcConfig().AddHadronicId(5224);
    PHYSICS->mcConfig().AddHadronicId(5232);
    PHYSICS->mcConfig().AddHadronicId(5242);
    PHYSICS->mcConfig().AddHadronicId(5301);
    PHYSICS->mcConfig().AddHadronicId(5303);
    PHYSICS->mcConfig().AddHadronicId(5312);
    PHYSICS->mcConfig().AddHadronicId(5314);
    PHYSICS->mcConfig().AddHadronicId(5322);
    PHYSICS->mcConfig().AddHadronicId(5324);
    PHYSICS->mcConfig().AddHadronicId(5332);
    PHYSICS->mcConfig().AddHadronicId(5334);
    PHYSICS->mcConfig().AddHadronicId(5342);
    PHYSICS->mcConfig().AddHadronicId(5401);
    PHYSICS->mcConfig().AddHadronicId(5403);
    PHYSICS->mcConfig().AddHadronicId(5412);
    PHYSICS->mcConfig().AddHadronicId(5414);
    PHYSICS->mcConfig().AddHadronicId(5422);
    PHYSICS->mcConfig().AddHadronicId(5424);
    PHYSICS->mcConfig().AddHadronicId(5432);
    PHYSICS->mcConfig().AddHadronicId(5434);
    PHYSICS->mcConfig().AddHadronicId(5442);
    PHYSICS->mcConfig().AddHadronicId(5444);
    PHYSICS->mcConfig().AddHadronicId(5503);
    PHYSICS->mcConfig().AddHadronicId(5512);
    PHYSICS->mcConfig().AddHadronicId(5514);
    PHYSICS->mcConfig().AddHadronicId(5522);
    PHYSICS->mcConfig().AddHadronicId(5524);
    PHYSICS->mcConfig().AddHadronicId(5532);
    PHYSICS->mcConfig().AddHadronicId(5534);
    PHYSICS->mcConfig().AddHadronicId(5542);
    PHYSICS->mcConfig().AddHadronicId(5544);
    PHYSICS->mcConfig().AddHadronicId(5554);
    PHYSICS->mcConfig().AddHadronicId(10111);
    PHYSICS->mcConfig().AddHadronicId(10113);
    PHYSICS->mcConfig().AddHadronicId(10211);
    PHYSICS->mcConfig().AddHadronicId(10213);
    PHYSICS->mcConfig().AddHadronicId(10221);
    PHYSICS->mcConfig().AddHadronicId(10223);
    PHYSICS->mcConfig().AddHadronicId(10311);
    PHYSICS->mcConfig().AddHadronicId(10313);
    PHYSICS->mcConfig().AddHadronicId(10321);
    PHYSICS->mcConfig().AddHadronicId(10323);
    PHYSICS->mcConfig().AddHadronicId(10331);
    PHYSICS->mcConfig().AddHadronicId(10333);
    PHYSICS->mcConfig().AddHadronicId(10411);
    PHYSICS->mcConfig().AddHadronicId(10413);
    PHYSICS->mcConfig().AddHadronicId(10421);
    PHYSICS->mcConfig().AddHadronicId(10423);
    PHYSICS->mcConfig().AddHadronicId(10431);
    PHYSICS->mcConfig().AddHadronicId(10433);
    PHYSICS->mcConfig().AddHadronicId(10441);
    PHYSICS->mcConfig().AddHadronicId(10443);
    PHYSICS->mcConfig().AddHadronicId(10511);
    PHYSICS->mcConfig().AddHadronicId(10513);
    PHYSICS->mcConfig().AddHadronicId(10521);
    PHYSICS->mcConfig().AddHadronicId(10523);
    PHYSICS->mcConfig().AddHadronicId(10531);
    PHYSICS->mcConfig().AddHadronicId(10533);
    PHYSICS->mcConfig().AddHadronicId(10541);
    PHYSICS->mcConfig().AddHadronicId(10543);
    PHYSICS->mcConfig().AddHadronicId(10551);
    PHYSICS->mcConfig().AddHadronicId(10553);
    PHYSICS->mcConfig().AddHadronicId(20113);
    PHYSICS->mcConfig().AddHadronicId(20213);
    PHYSICS->mcConfig().AddHadronicId(20223);
    PHYSICS->mcConfig().AddHadronicId(20313);
    PHYSICS->mcConfig().AddHadronicId(20323);
    PHYSICS->mcConfig().AddHadronicId(20333);
    PHYSICS->mcConfig().AddHadronicId(20413);
    PHYSICS->mcConfig().AddHadronicId(20423);
    PHYSICS->mcConfig().AddHadronicId(20433);
    PHYSICS->mcConfig().AddHadronicId(20443);
    PHYSICS->mcConfig().AddHadronicId(20513);
    PHYSICS->mcConfig().AddHadronicId(20523);
    PHYSICS->mcConfig().AddHadronicId(20533);
    PHYSICS->mcConfig().AddHadronicId(20543);
    PHYSICS->mcConfig().AddHadronicId(20553);
    PHYSICS->mcConfig().AddHadronicId(100443);
    PHYSICS->mcConfig().AddHadronicId(100553);
    PHYSICS->mcConfig().AddHadronicId(9900440);
    PHYSICS->mcConfig().AddHadronicId(9900441);
    PHYSICS->mcConfig().AddHadronicId(9900443);
    PHYSICS->mcConfig().AddHadronicId(9900551);
    PHYSICS->mcConfig().AddHadronicId(9900553);
    PHYSICS->mcConfig().AddHadronicId(9910441);
    PHYSICS->mcConfig().AddHadronicId(9910551);

}

void SampleAnalyzer::AddDefaultInvisible()
{
    // definition of the multiparticle "invisible"
    PHYSICS->mcConfig().AddInvisibleId(-16);
    PHYSICS->mcConfig().AddInvisibleId(-14);
    PHYSICS->mcConfig().AddInvisibleId(-12);
    PHYSICS->mcConfig().AddInvisibleId(12);
    PHYSICS->mcConfig().AddInvisibleId(14);
    PHYSICS->mcConfig().AddInvisibleId(16);
    PHYSICS->mcConfig().AddInvisibleId(1000022);
    PHYSICS->mcConfig().AddInvisibleId(1000039);
}