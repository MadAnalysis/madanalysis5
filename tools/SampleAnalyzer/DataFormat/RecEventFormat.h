////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


#ifndef RecEventFormat_h
#define RecEventFormat_h

// STL headers
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/DataFormat/RecLeptonFormat.h"
#include "SampleAnalyzer/DataFormat/RecTauFormat.h"
#include "SampleAnalyzer/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/DataFormat/RecMETFormat.h"
#include "SampleAnalyzer/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class ROOTReader;

class RecEventFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class ROOTReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private : 

  /// Collection of reconstructed electrons
  std::vector<RecLeptonFormat> electrons_;

  /// Collection of reconstructed muons
  std::vector<RecLeptonFormat> muons_;

  /// Collection of reconstructed taus
  std::vector<RecTauFormat>    taus_;

  /// Collection of reconstructed jets
  std::vector<RecJetFormat>    jets_;

  /// Reconstructed Missing Transverse Energy
  RecParticleFormat MET_;
  
  /// Reconstructed Missing Hadronic Transverse Energy
  RecParticleFormat MHT_;

  /// Reconstructed Scalar sum of transverse energy
  Float_t TET_;

  /// Reconstructed Scalar sum of hadronic transverse energy
  Float_t THT_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  RecEventFormat()
  { Reset(); }

  /// Destructor
  ~RecEventFormat()
  { }

  /// Accessor to the electron collection (read-only)
  const std::vector<RecLeptonFormat>& electrons() const {return electrons_;}

  /// Accessor to the muon collection (read-only)
  const std::vector<RecLeptonFormat>& muons() const {return muons_;}

  /// Accessor to the tau collection (read-only)
  const std::vector<RecTauFormat>& taus() const {return taus_;}

  /// Accessor to the jet collection (read-only)
  const std::vector<RecJetFormat>& jets() const {return jets_;}

  /// Accessor to the Missing Transverse Energy (read-only)
  const RecParticleFormat& MET() const {return MET_;}

  /// Accessor to the Missing Hadronic Transverse Energy (read-only)
  const RecParticleFormat& MHT() const {return MHT_;}

  /// Accessor to the electron collection
  std::vector<RecLeptonFormat>& electrons() {return electrons_;}

  /// Accessor to the muon collection
  std::vector<RecLeptonFormat>& muons() {return muons_;}

  /// Accessor to the tau collection
  std::vector<RecTauFormat>& taus() {return taus_;}

  /// Accessor to the jet collection
  std::vector<RecJetFormat>& jets() {return jets_;}

  /// Accessor to the Missing Transverse Energy
  RecParticleFormat& MET() {return MET_;}

  /// Accessor to the Missing Hadronic Transverse Energy
  RecParticleFormat& MHT() {return MHT_;}

  /// Clearing all information
  void Reset()
  { 
    electrons_.clear(); 
    muons_.clear(); 
    taus_.clear();
    jets_.clear();
    MET_.Reset();
    MHT_.Reset();
    TET_=0.;
    THT_=0.; 
  }

  /// Displaying data member values
  void Print() const
  {
  }

  /// Giving a new electron entry
  RecLeptonFormat* GetNewElectron()
  {
    electrons_.push_back(RecLeptonFormat());
    return &electrons_.back();
  }

  /// Giving a new muon entry
  RecLeptonFormat* GetNewMuon()
  {
    muons_.push_back(RecLeptonFormat());
    return &muons_.back();
  }

  /// Giving a new tau entry
  RecTauFormat* GetNewTau()
  {
    taus_.push_back(RecTauFormat());
    return &taus_.back();
  }

  /// Giving a new jet entry
  RecJetFormat* GetNewJet()
  {
    jets_.push_back(RecJetFormat());
    return &jets_.back();
  }

  /// Giving a pointer to the Missing Transverse Energy
  RecParticleFormat* GetNewMet()
  { return &MET_; }

  /// Giving a pointer to the Missing Transverse Energy
  RecParticleFormat* GetNewMht()
  { return &MHT_; }

};

}

#endif
