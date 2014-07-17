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


#ifndef RecParticleFormat_h
#define RecParticleFormat_h

// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// MCParticleDataFormat
#include "SampleAnalyzer/Commons/DataFormat/ParticleBaseFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

namespace MA5
{

class MCParticleFormat;
class LHEReader;
class LHCOReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DetectorDelphes;
class DetectorDelphesMA5tune;

class RecParticleFormat : public ParticleBaseFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
   
  Float_t 	        HEoverEE_; /// hadronic energy over electromagnetic energy
  const MCParticleFormat* mc_ ;      /// mother generated particle

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  RecParticleFormat()
  { Reset(); }

  /// Destructor
  virtual ~RecParticleFormat()
  {}

  /// Clear all information
  virtual void Reset()
  {
    momentum_.SetPxPyPzE(0.,0.,0.,0.);
    HEoverEE_=0.; 
    mc_=0;
  }

  /// Print particle informations
  virtual void Print() const
  {
    INFO << "momentum=(" << /*set::setw(8)*/"" << std::left << momentum_.Px()
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Py()  
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Pz() 
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.E() << ") - "
         << "EHoverEE=" << /*set::setw(8)*/"" << std::left << HEoverEE_
	       << " - ";

    if (mc_==0) ERROR << "NoMCmum" << " - ";
    else ERROR << "Mum1  " << " - ";
  }

  /// Accessor to matched Monte Carlo particle 
  const MCParticleFormat* mc() const {return mc_;}

  /// Mutator relatied to matched Monte Carlo particle
  void setMc(const MCParticleFormat* mc) {mc_=mc;}

  /// Accessor to hadronic energy / electromagnetic energy ratio
  const Float_t& HEoverEE() const {return HEoverEE_;}

  /// Accessor to electromagnetic energy / hadronic energy ratio
  const Float_t EEoverHE() const 
  {
    if (HEoverEE_!=0) return 1./HEoverEE_; 
    else return 0.;
  }

  /// Accessor to the number of tracks
  virtual const UShort_t ntracks() const
  { return 0; }

  /// Accessor to the isolation tag
  virtual const Bool_t isolated() const
  { return false; }

  /// Accessor to the electric charge
  virtual const int charge() const
  { return 0; }

};

}

#endif
