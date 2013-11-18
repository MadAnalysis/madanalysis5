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
#include "SampleAnalyzer/Reader/ROOTReader.h"
#include "SampleAnalyzer/Service/LogService.h"
#include "SampleAnalyzer/Reader/DelphesTreeReader.h"
#include "SampleAnalyzer/Reader/DelfesTreeReader.h"

// ROOT headers
#include <TROOT.h>

#ifdef ROOT_USE

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
  source_ = new TFile(filename_.c_str());
  
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

  // First case: Delphes
#ifdef DELPHES_USE
  mytree = dynamic_cast<TTree*>(source_->Get("Delphes"));
  if (mytree!=0)
  {
     treeReader_ = new DelphesTreeReader(source_, mytree);
     treeReader_->Initialize();
     return true;
  }
#endif

  // Second case: Delfes
#ifdef DELFES_USE
  mytree = dynamic_cast<TTree*>(source_->Get("Delfes"));
  if (mytree!=0)
  {
      treeReader_ = new DelfesTreeReader(source_, mytree);
      treeReader_->Initialize();
      return true;
  }
#endif

  // Other case: error
  ERROR << "Impossible to access a known tree in the input file" << endmsg;
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

#endif
