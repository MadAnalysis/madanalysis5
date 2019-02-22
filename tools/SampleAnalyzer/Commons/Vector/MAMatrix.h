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


#ifndef MAMatrix_h
#define MAMatrix_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class MAMatrix
{

 public :

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  std::vector<std::vector<MAdouble64> > m_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  // Constructors
  MAMatrix()
  {}

  MAMatrix(MAuint16 n, MAuint16 m)
  {setDim(n,m);}
  
  MAMatrix(MAuint16 n)
  {setDim(n,n);}

  // Destructor
  ~MAMatrix()
  {}

  // Setting the rotation angle
  void setDim(MAuint16 n, MAuint16 m)
  {
    m_.resize(n,std::vector<MAdouble64>(m,0.));
  }

  // Operator
  const std::vector<MAdouble64>& operator[] (MAuint16 i) const
  { return m_[i]; }

  std::vector<MAdouble64>& operator[] (MAuint16 i)
  { return m_[i]; }

  
};
 
}

#endif
