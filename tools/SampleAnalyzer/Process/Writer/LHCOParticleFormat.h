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


#ifndef LHCO_PARTICLE_FORMAT_h
#define LHCO_PARTICLE_FORMAT_h


// STL headers
#include <iostream>
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


namespace MA5
{

class LHCOParticleFormat
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public:

  MAuint32 id;
  MAfloat32 eta;
  MAfloat32 phi;
  MAfloat32 pt;
  MAfloat32 jmass;
  MAfloat32 ntrk;
  MAfloat32 btag;
  MAfloat32 hadem;

  static const std::string header;

  void Print(MAuint32 num, std::ostream* out);
  static void WriteEventHeader(MAuint32 numEvent,std::ostream* out);
};


}

#endif
