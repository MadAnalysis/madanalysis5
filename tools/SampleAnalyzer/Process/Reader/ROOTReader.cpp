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


// STL headers
#include <sstream>

// SampleHeader headers
#include "SampleAnalyzer/Process/Reader/ROOTReader.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#ifdef DELPHES_USE
  #include "SampleAnalyzer/Interfaces/delphes/DelphesTreeReader.h"
#endif
#ifdef DELPHESMA5TUNE_USE
  #include "SampleAnalyzer/Interfaces/delphesMA5tune/DelphesMA5tuneTreeReader.h"
#endif

// ROOT headers
#include <TROOT.h>


using namespace MA5;

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
bool ROOTReader::Initialize(const std::string& rawfilename,
                            const Configuration& cfg)
{
  // Set configuration
  cfg_=cfg;

  // Is the file stored in Rfio
  rfio_ = IsRfioMode(rawfilename);

  // Check consistency with compilation option
  if (rfio_)
  {
#ifndef RFIO_USE
    ERROR << "'-rfio' is not allowed. Please set the RFIO_USE"
          << " variable in the Makefile to 1 and recompile the program if"
          << " you would like to use this option." << endmsg;
    exit(1);
#endif
  }

  // Cleaning the file (remove rfio or local location)
  filename_ = rawfilename;
  CleanFilename(filename_);

  // Opening the file
  source_ = TFile::Open(filename_.c_str());
  
  // Check if the input is properly opened
  bool test=true;
  if (source_==0) test=false;
  else if (!source_->IsOpen() || source_->IsZombie()) test=false;
  if (!test)
  {
    ERROR << "Opening file " << filename_ << " failed" << endmsg;
    source_=0;
    return false;
  }

  // SelectTreeReader
  return SelectTreeReader(); 
}
 

// -----------------------------------------------------------------------------
// SelectTreeReader
// -----------------------------------------------------------------------------
bool ROOTReader::SelectTreeReader()
{
  TTree* mytree = 0;
  bool DelphesMA5tune_Tag=false;
  bool Delphes_Tag=false;

  // First case: Delphes
#ifdef DELPHES_USE
  Delphes_Tag=true;
  mytree = dynamic_cast<TTree*>(source_->Get("Delphes"));
  if (mytree!=0)
  {
     treeReader_ = new DelphesTreeReader(source_, mytree);
     treeReader_->Initialize();
     return true;
  }
#endif

  // Second case: DelphesMA5tune
#ifdef DELPHESMA5TUNE_USE
  DelphesMA5tune_Tag=true;
  mytree = dynamic_cast<TTree*>(source_->Get("DelphesMA5tune"));
  if (mytree!=0)
  {
      treeReader_ = new DelphesMA5tuneTreeReader(source_, mytree);
      treeReader_->Initialize();
      return true;
  }
#endif

  // Other case: error
  if (DelphesMA5tune_Tag && Delphes_Tag)
  {
    ERROR << "The input file seems to have not the Delphes or the DelphesMA5tune ROOT format" << endmsg;
  }
  else if (DelphesMA5tune_Tag)
  {
    ERROR << "The input file seems to have not the DelphesMA5tune ROOT format" << endmsg;
    mytree = dynamic_cast<TTree*>(source_->Get("Delphes"));
    if (mytree!=0)
    {
      ERROR << "In fact, Delphes ROOT format is detected." << endmsg;
      ERROR << "Please uninstall DelphesMA5tune and install Delphes if you would like to read this file." << endmsg;    
    }
  }
  else if (Delphes_Tag)
  {
    ERROR << "The input file seems to have not the Delphes ROOT format" << endmsg;
    mytree = dynamic_cast<TTree*>(source_->Get("DelphesMA5tune"));
    if (mytree!=0)
    {
      ERROR << "In fact, DelphesMA5tune ROOT format is detected." << endmsg;
      ERROR << "Please uninstall Delphes and install DelphesMA5tune if you would like to read this file." << endmsg;    
    }
  }


  return false;
}


// -----------------------------------------------------------------------------
// Finalize
// -----------------------------------------------------------------------------
bool ROOTReader::Finalize()
{
  // OK!
  if (source_!=0) source_->Close();
  return true;
}

