////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file RecVertexFormat.h
 * @brief Reconstructed vertex.
 */

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

/** @brief Reconstructed vertex (Delphes). */
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

  /** @brief Number of degrees of freedom of the vertex fit. */
  MAint32 ndf_;   /// number of degree of freedom
  /** @brief Position of the vertex (x, y, z, t). */
  MALorentzVector position_;
  /** @brief Uncertainties on the position. */
  MALorentzVector error_;
  /** @brief Monte Carlo particles associated with the vertex. */
  std::vector<MCParticleFormat*> constituents_; // link to MCParticle used for that

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor (members reset). */
  RecVertexFormat()
  { Reset(); }

  /** @brief Destructor. */
  virtual ~RecVertexFormat()
  {}

  /** @brief Print the vertex properties. */
  virtual void Print() const
  {
    INFO << "ndf = " << ndf_ << endmsg;
  }

  /** @brief Reset the members. */
  virtual void Reset()
  {
    ndf_ = 0;
    position_.Reset();
    // NOTE: constituents_ is not cleared.
    error_.Reset();
  }

  /**
   * @brief Accessor to the number of degrees of freedom.
   *
   * @return the number of degrees of freedom.
   */
  const MAint32 ndf() const
  {return ndf_;}

  /**
   * @brief Accessor to the position.
   *
   * @return the position.
   */
  const MALorentzVector& position() const
  {return position_;}

  /**
   * @brief Accessor to the uncertainties on the position.
   *
   * @return the uncertainties.
   */
  const MALorentzVector& error() const
  {return error_;}

  /**
   * @brief Accessor to the associated Monte Carlo particles.
   *
   * @return the particles.
   */
  const std::vector<MCParticleFormat*>& constituents() const
  {return constituents_;}
  
};

}

#endif
