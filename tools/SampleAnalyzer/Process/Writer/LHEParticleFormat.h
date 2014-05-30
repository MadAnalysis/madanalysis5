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


#ifndef LHE_PARTICLE_FORMAT_h
#define LHE_PARTICLE_FORMAT_h

// STL headers
#include <iostream>
#include <string>

// ROOT headers
#include <Rtypes.h>

namespace MA5
{

class LHEParticleFormat
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public:

  Int_t   id;
  Int_t   status;
  UInt_t  mother1;
  UInt_t  mother2;
  UInt_t  color1;
  UInt_t  color2;
  Float_t px;
  Float_t py;
  Float_t pz;
  Float_t e;
  Float_t m;
  Float_t ctau;
  Float_t spin;

  static std::string FortranFormat_SimplePrecision(Float_t value,
                                                   UInt_t precision=7); 
  static std::string FortranFormat_DoublePrecision(Double_t value,
                                                   UInt_t precision=11); 

  void Print(UInt_t num, std::ostream* out);
};


}

#endif
