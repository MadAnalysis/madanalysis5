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


#ifndef MCEventFormat_h
#define MCEventFormat_h

// STL headers
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCParticleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class STDHEPreader;
class HEPMCReader;
class LHEWriter;
class ROOTReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;

class MCEventFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class STDHEPreader;
  friend class HEPMCReader;
  friend class ROOTReader;
  friend class LHEWriter;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private : 

  UInt_t nparts_;       /// number of particles in the event
  UInt_t processId_;    /// identity of the current process
  Double_t weight_;      /// event weight
  Double_t scale_;       /// scale Q of the event
  Double_t alphaQED_;    /// ALPHA_em value used
  Double_t alphaQCD_;    /// ALPHA_s value used
  Double_t PDFscale_;
  std::pair<Double_t,Double_t> x_;
  std::pair<Double_t,Double_t> xpdf_;

  /// List of generated particles
  std::vector<MCParticleFormat> particles_;

  /// Computed Missing Transverse Energy
  MCParticleFormat MET_;
  
  /// Computed Missing Hadronic Transverse Energy
  MCParticleFormat MHT_;

  /// Computed Scalar sum of transverse energy
  Double_t TET_;

  /// Computed Scalar sum of hadronic transverse energy
  Double_t THT_;



  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  MCEventFormat()
  { Reset(); }

  /// Destructor
  ~MCEventFormat()
  { }

  /// Accessor to the Missing Transverse Energy (read-only)
  const MCParticleFormat& MET() const {return MET_;}

  /// Accessor to the Missing Hadronic Transverse Energy (read-only)
  const MCParticleFormat& MHT() const {return MHT_;}

  /// Accessor to the Total Transverse Energy (read-only)
  const Double_t& TET() const {return TET_;}

  /// Accessor to the Total Hadronic Transverse Energy (read-only)
  const Double_t& THT() const {return THT_;}

  /// Accessor to the Missing Transverse Energy
  MCParticleFormat& MET() {return MET_;}

  /// Accessor to the Missing Hadronic Transverse Energy
  MCParticleFormat& MHT() {return MHT_;}

  /// Accessor to the Total Transverse Energy
  Double_t& TET() {return TET_;}

  /// Accessor to the Total Hadronic Transverse Energy
  Double_t& THT() {return THT_;}

  /// Accessor to the process identity
  const UInt_t& processId()  const {return processId_;}

  /// Accessor to the event weight
  const Double_t& weight()    const {return weight_;   }

  /// Accessor to the scale
  const Double_t& scale()     const {return scale_;    }

  /// Accessor to alpha_QED
  const Double_t& alphaQED()  const {return alphaQED_; }

  /// Accessor to alpha_QCD
  const Double_t& alphaQCD()  const {return alphaQCD_; }

  /// Accessor to the generated particle collection (read-only)
  const std::vector<MCParticleFormat>& particles() const {return particles_;}

  /// Accessor to the generated particle collection
  std::vector<MCParticleFormat>& particles() {return particles_;}

  /// Setting the process identity
  void setProcessId(UInt_t v)  {processId_=v;}

  /// Setting the event weight
  void setWeight   (Double_t v) {weight_=v;   }

  /// Setting the scale
  void setScale    (Double_t v) {scale_=v;    }

  /// Setting AlphaQED
  void setAlphaQED (Double_t v) {alphaQED_=v; }

  /// Setting AlphaQCD
  void setAlphaQCD (Double_t v) {alphaQCD_=v; }

  /// Clearing all information
  void Reset()
  { nparts_=0; processId_=0; weight_=1.;
    scale_=0.; alphaQED_=0.; alphaQCD_=0.;
    particles_.clear(); 
    MET_.Reset();
    MHT_.Reset();
    TET_=0.;
    THT_=0.; 
  }

  /// Displaying data member values
  void Print() const
  {
    INFO << "nparts="      << nparts_
         << " - processId=" << processId_
         << " - weight="    << weight_
         << " - scale="     << scale_
         << " - alphaQED="  << alphaQED_
         << " - alphaQCD="  << alphaQCD_ << endmsg;
  }

  /// Giving a new particle
  MCParticleFormat* GetNewParticle()
  {
    particles_.push_back(MCParticleFormat());
    return &particles_.back();
  }
};

}

#endif
