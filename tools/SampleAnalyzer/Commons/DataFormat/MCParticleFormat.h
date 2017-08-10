////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/ParticleBaseFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"


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
   
  MAfloat32 ctau_;       /// proper lifetime ctau (in mm)
  MAfloat32 spin_;       /// cosine of the angle btwn the spin vector and
                         /// its 3-momentum, in the lab frame
  MAint32   pdgid_;      /// PDG numbering of the particle
  MAbool    isPU_;       /// is PileUp particle or not
  MAint16   statuscode_; /// status code (-1 for initial state, 
                         /// 2 intermediate state, 1 final state)
  MAint32   extra1_;
  MAint32   extra2_;

  std::vector<MCParticleFormat*> daughters_; /// list of daughter particles
  std::vector<MCParticleFormat*> mothers_;   /// list of mother particles

  MCParticleFormat* mother1_ ;  /// mother particle
  MCParticleFormat* mother2_ ;  /// mother particle

 public:
  MAuint32 mothup1_;     /// first mother index
  MAuint32 mothup2_;     /// second mother index
  MAuint32 daughter1_;   /// first mother index
  MAuint32 daughter2_;   /// second mother index


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
    ctau_       = 0.; 
    spin_       = 0.; 
    pdgid_      = 0; 
    statuscode_ = 0; 

    mothup1_    = 0; mothup2_   = 0; 
    mother1_    = 0; mother2_   = 0; 
    daughter1_  = 0; daughter2_ = 0;
    extra1_     = 0; extra2_    = 0;
    isPU_=false;
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

    try
    {
      if (mother1_==0) throw EXCEPTION_ERROR("NoMum1","",0);
      if (mother2_==0) throw EXCEPTION_ERROR("NoMum2","",0);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
  }

  const MAbool& isPU()  const {return isPU_;}
  const MAfloat32& ctau() const {return ctau_;}
  const MAfloat32& spin() const {return spin_;}
  const MAint32& pdgid()  const {return pdgid_;}
  const MAint16& statuscode() const {return statuscode_;}
  const MCParticleFormat* mother1() const {return mother1_;}
  const MCParticleFormat* mother2() const {return mother2_;}

  /// Accessor to the daughters (read-only)
  const std::vector<MCParticleFormat*>& daughters() const {return daughters_;}

  /// Accessor to the daughters
  std::vector<MCParticleFormat*>& daughters() {return daughters_;}

  /// Accessor to the daughters (read-only)
  const std::vector<MCParticleFormat*>& mothers() const {return mothers_;}

  /// Accessor to the daughters
  std::vector<MCParticleFormat*>& mothers() {return mothers_;}

  MCParticleFormat* mother1() {return mother1_;}
  MCParticleFormat* mother2() {return mother2_;}

  // mutators
  void setIsPU(MAbool v)   {isPU_=v;}
  void setCtau(MAfloat32 v)  {ctau_=v;}
  void setSpin(MAfloat32 v)  {spin_=v;}
  void setPdgid(MAint32 v)   {pdgid_=v;}
  void setStatuscode(MAint16 v)  {statuscode_=v;}
  void setMomentum(const MALorentzVector& v)  {momentum_=v;}
  void setMothUp1(MAuint32 v) {mothup1_=v;}
  void setMothUp2(MAuint32 v) {mothup2_=v;}

  /// Boosting the four momentum to the restframe of another particle
  void ToRestFrame(const MCParticleFormat* boost)
  {
    if (boost==0) return;
    ToRestFrame(*boost);
  }

  void ToRestFrame(const MCParticleFormat& boost)
  {
    MALorentzVector momentum = boost.momentum();
    momentum.SetPx(-momentum.X());
    momentum.SetPy(-momentum.Y());
    momentum.SetPz(-momentum.Z());

    MABoost convertor;
    convertor.setBoostVector(momentum);
    convertor.boost(momentum_);
  }

};

}

#endif
