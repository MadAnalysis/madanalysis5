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


#ifndef MARotation3euler_h
#define MARotation3euler_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MAMatrix.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class MARotation3euler
{

 public :

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  MAMatrix m_; 


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  // Constructors
  MARotation3euler()
  {}

  MARotation3euler(MAdouble64 phi, MAdouble64 theta, MAdouble64 psi)
  { setAngles(phi,theta,psi); }
  
  // Destructor
  ~MARotation3euler()
  {}

  // Setting the rotation angle
  void setAngles(MAdouble64 phi, MAdouble64 theta, MAdouble64 psi)
  {
    MAdouble64 cphi   = std::cos(phi);
    MAdouble64 sphi   = std::sin(phi);
    MAdouble64 ctheta = std::cos(theta);
    MAdouble64 stheta = std::sin(theta);
    MAdouble64 cpsi   = std::cos(psi);
    MAdouble64 spsi   = std::sin(psi);
    m_.setDim(3,3);
    m_[0][0] =  cpsi   * cphi    - spsi * sphi * ctheta;
    m_[0][1] =  cpsi   * sphi    + spsi * ctheta * cphi; 
    m_[0][2] =  spsi   * stheta;
    m_[1][0] = -spsi   * cphi    - cpsi * ctheta * sphi;
    m_[2][1] = -spsi   * sphi    + cpsi * ctheta * cphi;
    m_[1][2] =  cpsi   * stheta;
    m_[2][0] =  stheta * sphi;
    m_[2][1] = -stheta * cphi;
    m_[2][2] =  ctheta;
  }
  
  // Rotate a MALorentzVector
  void rotate(MALorentzVector& q) const
  { rotate(q.Vect()); }

  // Rotate a MAVector3
  void rotate(MAVector3& p) const
  {
    p = operator*(p);
  }

  // Operator *
  MALorentzVector operator* (const MALorentzVector& q) const
  {
    return MALorentzVector(operator*(q.Vect()),q.E());
  }
  
  // Operator *
  MAVector3 operator* (const MAVector3& p) const
  {
    MAVector3 result;
    for (MAuint32 i=0;i<3;i++)
      for (MAuint32 j=0;j<3;j++)
      {
        result[i] = m_[i][j]*p[j];
      }
    return result;
  }
  
};
 
}

#endif
