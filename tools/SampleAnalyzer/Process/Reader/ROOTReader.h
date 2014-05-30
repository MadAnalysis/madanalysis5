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


#ifndef ROOT_READER_h
#define ROOT_READER_h

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/ReaderBase.h"
#include "SampleAnalyzer/Commons/Base/TreeReaderBase.h"

// ROOT header
#include <TChain.h>
#include <TLorentzVector.h>
#include <TObject.h>
#include <TFile.h>
#include <TROOT.h>

// STL header
#include <iostream>


namespace MA5
{

class ROOTReader : public ReaderBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  TFile *         source_;
  TreeReaderBase* treeReader_;
  std::string     filename_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  ROOTReader()
  { 
    source_=0;
    treeReader_=0;
  } 

	/// Destructor
  virtual ~ROOTReader()
  { }

  /// Initialize
  virtual bool Initialize(const std::string& rawfilename,
                          const Configuration& cfg);

  /// Finalize
  virtual bool Finalize();

  /// Read the header
  virtual bool ReadHeader(SampleFormat& mySample)
  {
    // Checking ROOT version
    Int_t file_version = source_->GetVersion();
    Int_t lib_version = gROOT->GetVersionInt();
    if (file_version!=lib_version)
    {
      WARNING << "the input file has been produced with ROOT version " << file_version
              << " whereas the loaded ROOT libs are related to the version " << lib_version << endmsg;
    }
    return treeReader_->ReadHeader(mySample);
  }

  /// Finalize the header
  virtual bool FinalizeHeader(SampleFormat& mySample)
  { return true; }

  /// Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
  { return treeReader_->ReadEvent(myEvent,mySample); }

  /// Finalize the event
  virtual bool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
  { return treeReader_->FinalizeEvent(mySample,myEvent); }


  /// Get the file size
  virtual Long64_t GetFinalPosition()
  { return treeReader_->GetFinalPosition(); }

  /// Get the file size
  virtual Long64_t GetFileSize()
  {
    Long64_t length = 0;
    std::ifstream myinput(filename_.c_str());
    myinput.seekg(0,std::ios::beg);
    myinput.seekg(0,std::ios::end);
    length = myinput.tellg();
    myinput.close();
    return length;
  }

  /// Get the position in file (in octet)
  virtual Long64_t GetPosition()
  { return treeReader_->GetPosition(); }


 private:
  bool SelectTreeReader();


};

};

#endif
