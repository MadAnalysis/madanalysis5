////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef MARotationGeneral_h
#define MARotationGeneral_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class MARotationGeneral
{

 public :

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  MAdouble64 c_; // cos(angle)
  MAdouble64 s_; // sin(angle)
  MAVector3 axis_; // axis unit vector
  mutable MAVector3 tmp_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :
  // Constructors
  MARotationGeneral()
  {c_=0.; s_=0.; axis_=MAVector3(0.0,0.0,0.0);}

  MARotationGeneral(MAdouble64 angle, MAVector3 axis)
  {setAngleAxis(angle,axis);}

  // Destructor
  ~MARotationGeneral()
  {}

  // Setting the rotation angle
  void setAngleAxis(MAdouble64 angle, MAVector3 axis)
  {
    s_ = std::sin(angle);
    c_ = std::cos(angle);
    axis_ = axis.Unit();
  }

  // Rotate a MALorentzVector
  void rotate(MALorentzVector& q) const
  { rotate(q.Vect()); }

  // Rotate a MAVector3
  // p * costheta + (p x axis) sintheta + p (p.axis) (1-costheta)
  void rotate(MAVector3& p) const
  {
     tmp_ = p;
     p.SetX(tmp_.X()*c_ - tmp_.Cross(axis_).X()*s_ + axis_.X()*tmp_.Dot(axis_)*(1.0 - c_));
     p.SetY(tmp_.Y()*c_ - tmp_.Cross(axis_).Y()*s_ + axis_.Y()*tmp_.Dot(axis_)*(1.0 - c_));
     p.SetZ(tmp_.Z()*c_ - tmp_.Cross(axis_).Z()*s_ + axis_.Z()*tmp_.Dot(axis_)*(1.0 - c_));
  }

};
}

#endif
