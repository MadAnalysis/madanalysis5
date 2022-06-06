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


#ifndef RecVertexFormat_h
#define RecVertexFormat_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
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

class RecVertexFormat
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class JetClusteringFastJet;
  friend class bTagger;
  friend class TauTagger;
  friend class cTagger;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;
  friend class DelphesMemoryInterface;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  MAint32 ndf_;   /// number of degree of freedom
  MALorentzVector position_;
  MALorentzVector error_;
  std::vector<MCParticleFormat*> constituents_; // link to MCParticle used for that

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  RecVertexFormat()
  { Reset(); }

  /// Destructor
  virtual ~RecVertexFormat()
  {}

  /// Dump information
  virtual void Print() const
  {
    INFO << "ndf = " << ndf_ << endmsg;
  }

  /// Clear all information
  virtual void Reset()
  {
    ndf_ = 0;
    position_.Reset();
    error_.Reset();
  }

  /// Accessor to the pdgid
  const MAint32 ndf() const
  {return ndf_;}

  /// Accessor to the position
  const MALorentzVector& position() const
  {return position_;}

  /// Accessor to the error
  const MALorentzVector& error() const
  {return error_;}

  /// Accessor to the constituents
  const std::vector<MCParticleFormat*>& constituents() const
  {return constituents_;}
  
};

}

#endif
