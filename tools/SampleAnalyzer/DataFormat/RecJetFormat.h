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


#ifndef RecJetFormat_h
#define RecJetFormat_h

// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// RecParticleFormat
#include "SampleAnalyzer/DataFormat/RecParticleFormat.h"
#include "SampleAnalyzer/Service/LogService.h"

namespace MA5
{

class LHCOReader;
class ROOTReader;

class RecJetFormat : public RecParticleFormat
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class JetClusteringFastJet;
  friend class bTagger;
  friend class TauTagger;
  friend class cTagger;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  UShort_t ntracks_;   /// number of tracks
  Bool_t btag_;        /// b-tag
  Bool_t ctag_;        /// c-tag
  std::vector<Int_t> Constituents_;  /// indices of the MC particles

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
         << "btag = " << /*set::setw(8)*/"" << std::left << btag_ << ", ";
    RecParticleFormat::Print();
  }

  /// Clear all information
  virtual void Reset()
  {
    ntracks_ = 0; 
    btag_    = false;
  }

  /// Accessor to the number of tracks
  virtual const UShort_t ntracks() const
  {return ntracks_;}

  /// Accessor to the b-tag
  const Bool_t& btag() const
  {return btag_;}

  /// Accessor to the c-tag
  const Bool_t& ctag() const
  {return ctag_;}

  /// Add one constituent
  void AddConstituent (const int& index)
  {Constituents_.push_back(index);}
};

}

#endif
