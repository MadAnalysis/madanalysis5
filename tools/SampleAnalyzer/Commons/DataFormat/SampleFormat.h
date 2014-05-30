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


#ifndef SAMPLE_DATAFORMAT_H
#define SAMPLE_DATAFORMAT_H

// STL headers
#include <map>
#include <iostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/GeneratorInfo.h"
#include "SampleAnalyzer/Commons/DataFormat/MCSampleFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecSampleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace MA5
{

class LHEReader;
class LHCOReader;
class HEPMCReader;
class SampleAnalyzer;


class SampleFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class SampleAnalyzer;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  std::string                 name_;      /// file name
  ULong64_t                   nevents_;   /// number of events in the file
  MCSampleFormat  *           mc_;
  RecSampleFormat *           rec_;
  MA5GEN::GeneratorType       sample_generator_;
  MA5FORMAT::SampleFormatType sample_format_;
  std::vector<std::string>    header_;    /// file header


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  SampleFormat()
  { 
    rec_=0;
    mc_=0; 
    nevents_=0;
    sample_generator_  = MA5GEN::UNKNOWN;
    sample_format_     = MA5FORMAT::UNKNOWN;
    header_.clear();
  }

  /// Destructor
  ~SampleFormat()
  { 
  }
 
  /// Accessor to Monte Carlo information (read-only)
  const MCSampleFormat * mc() const
  { return mc_; }

  /// Accessor to reconstruction information (read-only)
  const RecSampleFormat * rec() const
  { return rec_; } 

  /// Accessor to Monte Carlo information
  MCSampleFormat * mc()
  { return mc_; }

  /// Accessor to reconstruction information
  RecSampleFormat * rec()
  { return rec_; }

  /// Accessor to the sample name
  const std::string& name() const
  { return name_; }

  /// Accessor to the number of events
  const ULong64_t& nevents() const
  { return nevents_; }

  /// Set the sample name
  void setName(const std::string& name)
  {name_=name;}

  /// Set the number of events in the sample
  void setNEvents(ULong64_t v)
  {nevents_=v;}

  /// Initialize MonteCarlo part
  void InitializeMC()
  {
    if (mc_!=0) 
    {
      WARNING << "MC part of the SampleFormat is already initialized" << endmsg;
    }
    else mc_=new MCSampleFormat(&sample_generator_);
  }

  /// Initialize Rec part
  void InitializeRec()
  {
    if (rec_!=0) 
    {
      WARNING << "REC part of the SampleFormat is already initialized" << endmsg;
    }
    else rec_=new RecSampleFormat();
  }

  /// Free allocates memory
  void Delete()
  {
    if (rec_!=0) delete rec_;
    if (mc_!=0)  delete mc_;
  }

  /// Set the Generator Format
  void SetSampleGenerator(MA5GEN::GeneratorType value)
  { sample_generator_ = value; }

  /// Set the Sample Format
  void SetSampleFormat(MA5FORMAT::SampleFormatType value)
  { sample_format_ = value; }

  /// Accessor to the Generator Format
  const MA5GEN::GeneratorType& sampleGenerator() const
  { return sample_generator_; }

  /// Accessor to the Sample Format
  const MA5FORMAT::SampleFormatType& sampleFormat() const
  { return sample_format_; }

  /// Displaying subtitle for file
  void printSubtitle() const
  {
    // Sample format
    INFO << "        => sample format: ";
    if (sample_format_==MA5FORMAT::UNKNOWN) INFO << "unknown-format";
    else if (sample_format_==MA5FORMAT::LHE) INFO << "LHE";
    else if (sample_format_==MA5FORMAT::SIMPLIFIED_LHE) INFO << "simplified LHE";
    else if (sample_format_==MA5FORMAT::STDHEP) INFO << "STDHEP";
    else if (sample_format_==MA5FORMAT::HEPMC) INFO << "HEPMC";
    else if (sample_format_==MA5FORMAT::LHCO) INFO << "LHCO";
    else if (sample_format_==MA5FORMAT::DELPHES) INFO << "Delphes-ROOT";
    else if (sample_format_==MA5FORMAT::DELPHESMA5TUNE) INFO << "Delphes-MA5tune ROOT";
    INFO << " file produced by ";

    // Generator
    if (sample_generator_==MA5GEN::UNKNOWN) 
              INFO << "an unknown generator " 
                   << "(cross section assumed in pb)";
    else if (sample_generator_==MA5GEN::MG5) INFO << "MadGraph5";
    else if (sample_generator_==MA5GEN::MA5) INFO << "MadAnalysi5";
    else if (sample_generator_==MA5GEN::PYTHIA6) INFO << "Pythia6";
    else if (sample_generator_==MA5GEN::PYTHIA8) INFO << "Pythia8";
    else if (sample_generator_==MA5GEN::HERWIG6) INFO << "Herwig6";
    else if (sample_generator_==MA5GEN::HERWIGPP) INFO << "Herwig++";
    else if (sample_generator_==MA5GEN::DELPHES) INFO << "Delphes";
    else if (sample_generator_==MA5GEN::DELPHESMA5TUNE) INFO << "Delphes-MA5tune";
    else if (sample_generator_==MA5GEN::CALCHEP) INFO << "CalcHEP";
    INFO << "." << endmsg;
  }

  /// Accessor to the header
  const std::vector<std::string>& header() const
  { return header_; }

  /// Mutator relative to the header
  void AddHeader(const std::string& line)
  { header_.push_back(line); }


};

}

#endif
