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


#ifndef LHCO_PARTICLE_FORMAT_h
#define LHCO_PARTICLE_FORMAT_h

// STL headers
#include <iostream>
#include <string>

// ROOT headers
#include <Rtypes.h>

namespace MA5
{

class LHCOParticleFormat
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public:

  UInt_t id;
  Float_t eta;
  Float_t phi;
  Float_t pt;
  Float_t jmass;
  Float_t ntrk;
  Float_t btag;
  Float_t hadem;

  static const std::string header;

  void Print(UInt_t num, std::ostream* out);
  static void WriteEventHeader(unsigned int numEvent,std::ostream* out);
};


}

#endif
