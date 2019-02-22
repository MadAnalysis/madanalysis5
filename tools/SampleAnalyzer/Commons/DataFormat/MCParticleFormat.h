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

  /// PDG numbering of the particle
  MAint32 pdgid_;   

  /// Status code of the particle
  /// For LHE: -1 for initial state, 2 intermediate state, 1 final state
  /// For PYTHIA: more sophisticated 
  MAint16 statuscode_;

  /// Cosine of the angle btwn the spin vector and its 3-momentum, in the lab frame
  MAfloat32 spin_;

  /// Is a PileUp particle or not?
  MAbool isPU_;       

  /// List of daughter particles
  std::vector<MCParticleFormat*> daughters_;

  /// List of mother particles
  std::vector<MCParticleFormat*> mothers_;

  // Decay position in time & space (in s & mm)
  MALorentzVector decay_vertex_; 


 public:


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor without arguments
  MCParticleFormat()
  {
    momentum_.SetPxPyPzE(0.,0.,0.,0.);
    spin_       = 0.; 
    pdgid_      = 0; 
    statuscode_ = 0; 
    isPU_       = false;
  }

  /// Destructor
  virtual ~MCParticleFormat()
  {}

  /// Clear all information
  virtual void Reset()
  {
    momentum_.SetPxPyPzE(0.,0.,0.,0.);
    spin_       = 0.; 
    pdgid_      = 0; 
    statuscode_ = 0; 
    isPU_       = false;
    daughters_.clear();
    mothers_.clear();
    decay_vertex_.clear();
  }

  /// Print particle informations
  virtual void Print() const
  {
    INFO << "momentum=(" << /*set::setw(8)*/"" << std::left << momentum_.Px()
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Py()  
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.Pz() 
         << ", "<</*set::setw(8)*/"" << std::left << momentum_.E() << ") - " << endmsg;
    INFO << "ctau=" << /*set::setw(8)*/"" << std::left << decay_vertex_.T() << " - "
         << "spin=" << /*set::setw(8)*/"" << std::left << spin_ << " - "
         << "PDGID=" << /*set::setw(8)*/"" << std::left << pdgid_ << " - "
         << "StatusCode=" << /*set::setw(3)*/"" << std::left 
         << static_cast<signed int>(statuscode_) << " - " << endmsg;
    INFO << "Number of mothers=" << mothers_.size() << " - " 
         << "Number of daughters=" << daughters_.size() << endmsg;
  }

  const MAbool& isPU()  const {return isPU_;}
  const MAfloat64& ctau() const {return decay_vertex_.T();}
  const MAfloat32& spin() const {return spin_;}
  const MAint32& pdgid()  const {return pdgid_;}
  const MAint16& statuscode() const {return statuscode_;}

  /// Accessor to the daughters (read-only)
  const std::vector<MCParticleFormat*>& daughters() const {return daughters_;}

  /// Accessor to the daughters
  std::vector<MCParticleFormat*>& daughters() {return daughters_;}

  /// Accessor to the daughters (read-only)
  const std::vector<MCParticleFormat*>& mothers() const {return mothers_;}

  /// Accessor to the daughters
  std::vector<MCParticleFormat*>& mothers() {return mothers_;}

  /// Accessor to the decay vertex
  const MALorentzVector& decay_vertex() const {return decay_vertex_;}

  // mutators
  void setIsPU(MAbool v)   {isPU_=v;}
  void setSpin(MAfloat32 v)  {spin_=v;}
  void setPdgid(MAint32 v)   {pdgid_=v;}
  void setStatuscode(MAint16 v)  {statuscode_=v;}
  void setMomentum(const MALorentzVector& v)  {momentum_=v;}

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
