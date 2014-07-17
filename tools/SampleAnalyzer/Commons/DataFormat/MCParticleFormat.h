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


#ifndef MCParticleFormat_h
#define MCParticleFormat_h

// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer
#include "SampleAnalyzer/Commons/DataFormat/ParticleBaseFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class STDHEPreader;
class HEPMCReader;
class ROOTReader;
class LHEWriter;
class MergingPlots;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;

class MCParticleFormat : public ParticleBaseFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class STDHEPreader;
  friend class HEPMCReader;
  friend class ROOTReader;
  friend class LHEWriter;
  friend class MergingPlots;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
   
  Float_t 		    ctau_;	    /// proper lifetime ctau (in mm)
  Float_t 		    spin_;	    /// cosine of the angle btwn the spin vector and
                              /// its 3-momentum, in the lab frame
  Int_t	          pdgid_;		  /// PDG numbering of the particle
  Short_t	        statuscode_;/// status code (-1 for initial state, 
                              /// 2 intermediate state, 1 final state)
  Int_t           extra1_;
  Int_t           extra2_;

  std::vector<MCParticleFormat*> daughters_;

  MCParticleFormat *mother1_ ;  // mother particle
  MCParticleFormat *mother2_ ;  // mother particle

 public:
  UInt_t 	        mothup1_;   /// first mother index
  UInt_t 	        mothup2_;   /// second mother index
  UInt_t 	        daughter1_;   /// first mother index
  UInt_t 	        daughter2_;   /// second mother index


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor without arguments
  MCParticleFormat()
  { Reset(); }

  /// Destructor
  virtual ~MCParticleFormat()
  {}

  /// Clear all information
  virtual void Reset()
  {
    momentum_.SetPxPyPzE(0.,0.,0.,0.);
    ctau_=0.; spin_=0.; pdgid_=0; 
    statuscode_=0; mothup1_=0; mothup2_=0; mother1_=0; mother2_=0; 
    daughter1_=0; daughter2_=0;
    extra1_=0; extra2_=0;
  }

  /// Print particle informations
  virtual void Print() const
  {
    INFO << "momentum=(" << /*set::setw(8)*/"" << std::left << momentum_.Px()
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Py()  
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Pz() 
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.E() << ") - " << endmsg;
    INFO << "ctau=" << /*set::setw(8)*/"" << std::left << ctau_ << " - "
         << "spin=" << /*set::setw(8)*/"" << std::left << spin_ << " - "
         << "PDGID=" << /*set::setw(8)*/"" << std::left << pdgid_ << " - "
         << "StatusCode=" << /*set::setw(3)*/"" << std::left 
         << static_cast<signed int>(statuscode_) << " - " << endmsg;

    if (mother1_==0) ERROR << "NoMum1" << " - ";
    else ERROR << "Mum1  " << " - ";

    if (mother2_==0) ERROR << "NoMum2" << endmsg;
    else ERROR << "Mum2  " << endmsg;
  }


  const Float_t& ctau() const {return ctau_;}
  const Float_t& spin() const {return spin_;}
  const Int_t& pdgid() const {return pdgid_;}
  const Short_t& statuscode() const {return statuscode_;}
  const MCParticleFormat* mother1() const {return mother1_;}
  const MCParticleFormat* mother2() const {return mother2_;}

  /// Accessor to the daughters (read-only)
  const std::vector<MCParticleFormat*>& daughters() const {return daughters_;}

  /// Accessor to the daughters
  std::vector<MCParticleFormat*>& daughters() {return daughters_;}

  MCParticleFormat* mother1() {return mother1_;}
  MCParticleFormat* mother2() {return mother2_;}

  // mutators
  void setCtau(Float_t v)  {ctau_=v;}
  void setSpin(Float_t v)  {spin_=v;}
  void setPdgid(Int_t v)   {pdgid_=v;}
  void setStatuscode(Short_t v)  {statuscode_=v;}
  void setMomentum(const TLorentzVector& v)  {momentum_=v;}
  void setMothUp1(UInt_t v) {mothup1_=v;}
  void setMothUp2(UInt_t v) {mothup2_=v;}

  /// Boosting the four momentum to the restframe of another particle
  void ToRestFrame(const MCParticleFormat* boost)
  {
    if (boost==0) return;
    ToRestFrame(*boost);
  }

  void ToRestFrame(const MCParticleFormat& boost)
  {
    TVector3 b = -1. * boost.momentum().BoostVector();
    momentum().Boost(b);
  }




};

}

#endif
