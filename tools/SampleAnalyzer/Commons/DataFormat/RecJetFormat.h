////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


#ifndef RecJetFormat_h
#define RecJetFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/IsolationConeType.h"
#include "SampleAnalyzer/Commons/DataFormat/RecParticleFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHCOReader;
class ROOTReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DetectorDelphes;
class DetectorDelphesMA5tune;
class DelphesMemoryInterface;

class RecJetFormat : public RecParticleFormat
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class ClusterAlgoFastJet;
  friend class bTagger;
  friend class TauTagger;
  friend class cTagger;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class DelphesMemoryInterface;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  MAbool btag_;        /// b-tag
  MAbool ctag_;        /// c-tag
  MAbool true_ctag_;   /// c-tag (before id or misid)
  MAbool true_btag_;   /// b-tag (before id or misid)
  std::vector<MAint32> Constituents_;  /// indices of the MC particles
  std::vector<IsolationConeType> isolCones_; // isolation cones

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  RecJetFormat()
  { Reset(); }

  /// Destructor
  virtual ~RecJetFormat()
  {}

  /// Dump information
  virtual void Print() const
  {
    INFO << "ntracks ="   << /*set::setw(8)*/"" << std::left << ntracks_  << ", "  
         << "btag = " << /*set::setw(8)*/"" << std::left << btag_ << ", "
         << "ctag = " << /*set::setw(8)*/"" << std::left << ctag_ << ", ";
    RecParticleFormat::Print();
  }

  /// Clear all information
  virtual void Reset()
  {
    btag_      = false;
    ctag_      = false;
    true_btag_ = false;
    true_ctag_ = false;
    isolCones_.clear();
  }

  /// Accessor to the b-tag
  const MAbool& btag() const
  {return btag_;}

  /// Accessor to the c-tag
  const MAbool& ctag() const
  {return ctag_;}

  /// Accessor to the true c-tag
  const MAbool& true_ctag() const
  {return true_ctag_;}

  /// Accessor to the true b-tag
  const MAbool& true_btag() const
  {return true_btag_;}

  /// Setting a new true_btag_ value
  void setTrueBtag(const MAbool& tag)
  {true_btag_=tag;}

  /// Setting a new btag_ value
  void setBtag(const MAbool& tag)
  {btag_=tag;}

  /// Setting a new true_ctag_ value
  void setCtag(const MAbool& tag)
  {ctag_=tag;}

  /// Setting a new ctag_ value
  void setTrueCtag(const MAbool& tag)
  {true_ctag_=tag;}

  /// Setting ntracks
  void setNtracks(MAuint16 ntracks)
  { ntracks_=ntracks; }

  /// Add one constituent
  void AddConstituent (const MAint32& index)
  {Constituents_.push_back(index);}

  /// get constituent collections
  const std::vector<MAint32>& constituents() const
  { return Constituents_; }

  /// Add one isolation cone
  void AddIsolCone (const IsolationConeType& cone)
  {isolCones_.push_back(cone);}

  /// get the collection of isolation cones
  const std::vector<IsolationConeType>& isolCones() const
  { return isolCones_; }

};

}

#endif
