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


#ifndef MCSAMPLE_DATAFORMAT_H
#define MCSAMPLE_DATAFORMAT_H

// STL headers
#include <map>
#include <iostream>
#include <vector>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/GeneratorInfo.h"
#include "SampleAnalyzer/Commons/DataFormat/MCProcessFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h" 

namespace MA5
{

class LHEReader;
class LHCOReader;
class HEPMCReader;
class STDHEPReader;
class STDHEPreader;
class ROOTReader;
class LHEWriter;
class SampleAnalyzer;


class MCSampleFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class HEPMCReader;
  friend class ROOTReader;
  friend class SampleAnalyzer;
  friend class LHEWriter;
  friend class STDHEPReader;
  friend class STDHEPreader;


  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  // ---------------------- physics info -------------------------
  std::pair<Int_t,Int_t>        beamPDGID_;    
  std::pair<Double_t,Double_t>  beamE_;        
  std::pair<UInt_t,UInt_t>      beamPDFauthor_;
  std::pair<UInt_t,UInt_t>      beamPDFID_;
  Int_t                         weightMode_;
  std::vector<ProcessFormat>    processes_;
  const MA5GEN::GeneratorType*  sample_generator_;


  // ----------------------- file info ---------------------------
  Double_t xsection_;
  Double_t xsection_error_;
  Double_t sumweight_positive_;  // all events with positive weights
  Double_t sumweight_negative_;  // all events with negative weights


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  MCSampleFormat(const MA5GEN::GeneratorType* gen)
  { 
    sample_generator_=gen;
    Reset();
  }

  /// Destructor
  ~MCSampleFormat()
  { }

  /// Clear all the content
  void Reset()
  {
    // Physics info
    beamPDGID_          = std::make_pair(0,0); 
    beamE_              = std::make_pair(0,0); 
    beamPDFauthor_      = std::make_pair(0,0); 
    beamPDFID_          = std::make_pair(0,0);
    weightMode_         = 0; 
    sumweight_positive_ = 0.;
    sumweight_negative_ = 0.;
    processes_.clear();

    // File info
    xsection_           = 0.;
    xsection_error_     = 0.;
    sumweight_positive_ = 0.;
    sumweight_negative_ = 0.;
  }

  /// Accessor to PDG ID of the intial partons
  const std::pair<Int_t,Int_t>& beamPDGID() const
  { return beamPDGID_; }

  /// Accessor to the beam energy
  const std::pair<Double_t,Double_t>& beamE() const
  { return beamE_; } 

  /// Accessor to the PDF authors
  const std::pair<UInt_t,UInt_t>& beamPDFauthor() const
  { return beamPDFauthor_; }

  /// Accessor to the PDF identity
  const std::pair<UInt_t,UInt_t>& beamPDFID() const
  { return beamPDFID_; }

  /// Accessor to the weight mode
  const Int_t& weightMode() const
  { return weightMode_; }

  /// Accessor to the xsection mean
  const Double_t& xsection() const
  { return xsection_; }

  /// Accessor to the xsection mean
  const Double_t& xsection_mean() const
  { return xsection_; }

  /// Accessor to the xsection error
  const Double_t& xsection_error() const
  { return xsection_error_; }

  /// Accessor to the number of events with positive weight
  const Double_t& sumweight_positive() const
  { return sumweight_positive_; }

  /// Accessor to the number of events with negative weight
  const Double_t& sumweight_negative() const
  { return sumweight_negative_; }

  /// Accessor to the process collection (read-only)
  const std::vector<ProcessFormat>& processes() const
  { return processes_; }

  /// Accessor to the process collection
  std::vector<ProcessFormat>& processes()
  { return processes_; }

  /// Set the PDG ID of the intial partons
  void setBeamPDGID(Int_t a, Int_t b) 
  {beamPDGID_=std::make_pair(a,b); }

  /// Set the beam energy
  void setBeamE(Double_t a, Double_t b)
  {beamE_=std::make_pair(a,b); }

  /// Set the PDF authors
  void setBeamPDFauthor(UInt_t a, UInt_t b)
  {beamPDFauthor_=std::make_pair(a,b); }

  /// Set the the PDF identity
  void setBeamPDFid(UInt_t a, UInt_t b)
  {beamPDFID_=std::make_pair(a,b); }

  /// Set the weight mode
  void setWeightMode(Int_t v) 
  {weightMode_=v;}

  /// Set the cross section mean
  // BENJ: the normalization in the pythia lhe output by madgraph has been changed
  //       the 1e9 factor is not needed anymore
  void setXsection(Double_t value) 
//  { xsection_=value*getXsectionUnitFactor();}
  { xsection_=value;}

  /// Set the cross section mean
  void setXsectionMean(Double_t value) 
  { xsection_=value;}

  /// Set the cross section mean
  void setXsectionError(Double_t value) 
  { xsection_error_=value;}

  /// Adding a weight
  void addWeightedEvents(Double_t weight)
  { if (weight>=0) sumweight_positive_ += std::abs(weight);
    else sumweight_negative_ += std::abs(weight); }

  /// Accessor to the number of events with positive weight
  void setSumweight_positive(Double_t sum)
  { sumweight_positive_ += sum; }

  /// Accessor to the number of events with negative weight
  void setSumweight_negative(Double_t sum)
  { sumweight_negative_ += sum; }

  /// Giving a new process entry
  ProcessFormat* GetNewProcess()
  {
    processes_.push_back(ProcessFormat());
    return &processes_.back();
  }

  /// Get scale factor required to set the cross section in pb unit
  Double_t getXsectionUnitFactor()
  { 
    if (*sample_generator_==MA5GEN::PYTHIA6) return 1e9;
    else return 1.;
  }

};

}

#endif
