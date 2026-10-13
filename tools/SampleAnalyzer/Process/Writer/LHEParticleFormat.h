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
 * @file LHEParticleFormat.h
 * @brief Particle line of an LHE file.
 */

#ifndef LHE_PARTICLE_FORMAT_h
#define LHE_PARTICLE_FORMAT_h


// STL headers
#include <iostream>
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


namespace MA5
{

/** @brief Content of a particle line of an LHE event. */
class LHEParticleFormat
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public:

  /** @brief PDG code, status, mothers, colours, momentum, mass, c*tau and spin (HEPEUP convention). */
  MAint32   id;
  MAint32   status;
  MAuint32  mother1;
  MAuint32  mother2;
  MAuint32  color1;
  MAuint32  color2;
  MAfloat32 px;
  MAfloat32 py;
  MAfloat32 pz;
  MAfloat32 e;
  MAfloat32 m;
  MAfloat32 ctau;
  MAfloat32 spin;

  /**
   * @brief Format a number in the Fortran style (mantissa and two-digit exponent).
   *
   * @param value number.
   * @param precision number of digits.
   * @return the formatted number.
   */
  static std::string FortranFormat_SimplePrecision(MAfloat32 value,
                                                   MAuint32 precision=7); 
  /**
   * @brief Format a number in the Fortran style (mantissa and two-digit exponent).
   *
   * @param value number.
   * @param precision number of digits.
   * @return the formatted number.
   */
  static std::string FortranFormat_DoublePrecision(MAfloat64 value,
                                                   MAuint32 precision=11); 

  /**
   * @brief Write the particle line.
   *
   * @param num index of the particle (unused).
   * @param out output stream.
   */
  void Print(MAuint32 num, std::ostream* out);
};


}

#endif
