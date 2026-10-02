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
 * @file LHCOParticleFormat.h
 * @brief Object line of an LHCO file.
 */

#ifndef LHCO_PARTICLE_FORMAT_h
#define LHCO_PARTICLE_FORMAT_h


// STL headers
#include <iostream>
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


namespace MA5
{

/** @brief Content of an object line of an LHCO event. */
class LHCOParticleFormat
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public:

  /** @brief Type (0 photon, 1 electron, 2 muon, 3 tau, 4 jet, 6 MET), kinematics, number of tracks (with the charge sign), b-tag and HAD/EM ratio. */
  MAuint32 id;
  MAfloat32 eta;
  MAfloat32 phi;
  MAfloat32 pt;
  MAfloat32 jmass;
  MAfloat32 ntrk;
  MAfloat32 btag;
  MAfloat32 hadem;

  /** @brief Line reminding the meaning of the columns. */
  static const std::string header;

  /**
   * @brief Write the object line.
   *
   * @param num index of the object in the event.
   * @param out output stream.
   */
  void Print(MAuint32 num, std::ostream* out);
  /**
   * @brief Write the header line of an event.
   *
   * @param numEvent 0-based event number.
   * @param out output stream.
   */
  static void WriteEventHeader(MAuint32 numEvent,std::ostream* out);
};


}

#endif
