////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
#include "SampleAnalyzer/Commons/DataFormat/WeightCollection.h"
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

  MAuint32 processId_;       /// identity of the current process
  mutable MAfloat64 weight_; /// event weight
  MAfloat64 scale_;          /// scale Q of the event
  MAfloat64 alphaQED_;       /// ALPHA_em value used
  MAfloat64 alphaQCD_;       /// ALPHA_s value used
  MAfloat64 PDFscale_;       /// scale for PDF 
  std::pair<MAfloat64,MAfloat64> x_;    /// x values
  std::pair<MAfloat64,MAfloat64> xpdf_; /// xpdf values

  /// List of generated particles
  std::vector<MCParticleFormat> particles_;

  /// Computed Missing Transverse Energy
  MCParticleFormat MET_;
  
  /// Computed Missing Hadronic Transverse Energy
  MCParticleFormat MHT_;

  /// Computed Scalar sum of transverse energy
  MAfloat64 TET_;

  /// Computed Scalar sum of hadronic transverse energy
  MAfloat64 THT_;

  /// Computed total effective mass (sum of jet's PT + MET
  MAfloat64 Meff_;

  /// List of weights
  WeightCollection multiweights_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  MCEventFormat()
  {
    processId_=0; 
    weight_=1.;
    scale_=0.; 
    alphaQED_=0.; 
    alphaQCD_=0.;
    TET_ = 0.;
    THT_ = 0.;
    Meff_= 0.;
  }

  /// Destructor
  ~MCEventFormat()
  { }

  /// Accessor to the Missing Transverse Energy (read-only)
  const MCParticleFormat& MET() const {return MET_;}

  /// Accessor to the Missing Hadronic Transverse Energy (read-only)
  const MCParticleFormat& MHT() const {return MHT_;}

  /// Accessor to the Total Transverse Energy (read-only)
  const MAfloat64& TET() const {return TET_;}

  /// Accessor to the Total Hadronic Transverse Energy (read-only)
  const MAfloat64& THT() const {return THT_;}

  /// Accessor to the Total effective mass (read-only)
  const MAfloat64& Meff() const {return Meff_;}

  /// Accessor to the Missing Transverse Energy
  MCParticleFormat& MET() {return MET_;}

  /// Accessor to the Missing Hadronic Transverse Energy
  MCParticleFormat& MHT() {return MHT_;}

  /// Accessor to the Total Transverse Energy
  MAfloat64& TET() {return TET_;}

  /// Accessor to the Total Hadronic Transverse Energy
  MAfloat64& THT() {return THT_;}

  /// Accessor to the Total effective mass
  MAfloat64& Meff() {return Meff_;}

  /// Accessor to the process identity
  const MAuint32& processId()  const {return processId_;}

  /// Accessor to the event weight
  const MAfloat64& weight()    const {return weight_;   }

  /// Accessor to the scale
  const MAfloat64& scale()     const {return scale_;    }

  /// Accessor to alpha_QED
  const MAfloat64& alphaQED()  const {return alphaQED_; }

  /// Accessor to alpha_QCD
  const MAfloat64& alphaQCD()  const {return alphaQCD_; }

  /// Accessor to multiweights
  const WeightCollection& multiweights()  const {return multiweights_; }

  /// Accessor to multiweights
  WeightCollection& multiweights() {return multiweights_; }

  /// Accessor to multiweights
  const MAfloat64& multiweights(MAuint32 weight)  const {return multiweights_[weight]; }

  /// Accessor to the generated particle collection (read-only)
  const std::vector<MCParticleFormat>& particles() const {return particles_;}

  /// Accessor to the generated particle collection
  std::vector<MCParticleFormat>& particles() {return particles_;}

  /// Setting the process identity
  void setProcessId(MAuint32 v)  {processId_=v;}

  /// Setting the event weight
  void setWeight   (MAfloat64 v) const {weight_=v;   }

  /// Setting the scale
  void setScale    (MAfloat64 v) {scale_=v;    }

  /// Setting AlphaQED
  void setAlphaQED (MAfloat64 v) {alphaQED_=v; }

  /// Setting AlphaQCD
  void setAlphaQCD (MAfloat64 v) {alphaQCD_=v; }

  /// Clearing all information
  void Reset()
  { 
    processId_=0; weight_=1.;
    scale_=0.; alphaQED_=0.; alphaQCD_=0.;
    particles_.clear(); 
    multiweights_.Reset();
    MET_.Reset();
    MHT_.Reset();
    TET_  = 0.;
    THT_  = 0.;
    Meff_ = 0.;
  }

  /// Displaying data member values
  void Print() const
  {
    INFO << "nparts="       << particles_.size()
         << " - processId=" << processId_
         << " - weight="    << weight_
         << " - scale="     << scale_
         << " - alphaQED="  << alphaQED_
         << " - alphaQCD="  << alphaQCD_ << endmsg;
    INFO << "nweights=" << multiweights_.size() << endmsg;
  }

  /// Displaying data 
  void PrintVertices() const;

  /// Displaying mothers
  void PrintMothers() const;

  /// Displaying daughters
  void PrintDaughters() const;

  /// Giving a new particle
  MCParticleFormat* GetNewParticle()
  {
    particles_.push_back(MCParticleFormat());
    return &particles_.back();
  }

};

}

#endif
