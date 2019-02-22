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


#ifndef MARotation3axis_h
#define MARotation3axis_h


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

class MARotation3axis
{

 public :

  enum AxisType{Xaxis=0,Yaxis=1,Zaxis=2};

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  MAdouble64 c_; // cos(angle)
  MAdouble64 s_; // sin(angle)
  AxisType   axis_;
  mutable MAdouble64 tmp_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  // Constructors
  MARotation3axis()
  {c_=0.; s_=0.; axis_=Zaxis;}

  MARotation3axis(MAdouble64 angle, AxisType axis)
  {setAngleAxis(angle,axis);}
  
  // Destructor
  ~MARotation3axis()
  {}

  // Setting the rotation angle
  void setAngleAxis(MAdouble64 angle,AxisType axis)
  {
    s_ = std::sin(angle);
    c_ = std::cos(angle);
    axis_ = axis;
  }
  
  // Rotate a MALorentzVector
  void rotate(MALorentzVector& q) const
  { rotate(q.Vect()); }

  // Rotate a MAVector3
  void rotate(MAVector3& p) const
  {
    if (axis_==Xaxis)
    {
      tmp_ = p.Y();
      p.SetY( c_*tmp_ - s_*p.Z() );
      p.SetZ( s_*tmp_ + c_*p.Z() );
    }
    else if (axis_==Yaxis)
    {
      tmp_ = p.Z();
      p.SetZ( c_*tmp_ - s_*p.X() );
      p.SetX( s_*tmp_ + c_*p.X() );
    }
    else if (axis_==Zaxis)
    {
      tmp_ = p.X();
      p.SetX( c_*tmp_ - s_*p.Y() );
      p.SetY( s_*tmp_ + c_*p.Y() );
    }
  }

  // Operator *
  MALorentzVector operator* (const MALorentzVector& q) const
  {
    if (axis_==Xaxis)
    {
      return MALorentzVector(q.X(),
             c_*q.Y() - s_*q.Z(),
             s_*q.Y() + c_*q.Z(),
             q.E());
    }
    else if (axis_==Yaxis)
    {
      return MALorentzVector(s_*q.Z() + c_*q.X(),
             q.Y(),
             c_*q.Z() - s_*q.X(),
             q.E());
    }
    else if (axis_==Zaxis)
    {
      return MALorentzVector(c_*q.X() - s_*q.Y(),
             s_*q.X() + c_*q.Y(),
             q.Z(),
             q.E());
    }

  }
  // Operator *
  MAVector3 operator* (const MAVector3& p) const
  {
    if (axis_==Xaxis)
    {
      return MAVector3(p.X(),
           c_*p.Y() - s_*p.Z(),
           s_*p.Y() + c_*p.Z());
    }
    else if (axis_==Yaxis)
    {
      return MAVector3(s_*p.Z() + c_*p.X(),
             p.Y(),
             c_*p.Z() - s_*p.X());
    }
    else if (axis_==Zaxis)
    {
      return MAVector3(c_*p.X() - s_*p.Y(),
           s_*p.X() + c_*p.Y(),
           p.Z());
    }
  }
  
};
 
}

#endif
