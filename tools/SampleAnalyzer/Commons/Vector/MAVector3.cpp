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


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Vector/MAVector3.h"
using namespace MA5;

MAdouble64 const  PI        = M_PI;
MAdouble64 const  TWOPI     = 2.*M_PI;


//______________________________________________________________________________
MAdouble64 MAVector3::Phi_0_2pi(MAdouble64 x)
{
   // (static function) returns phi angle in the interval [0,2*PI)
  if(std::isnan(x))
  {
    std::cout << "TVector2::Phi_0_2pi : function called with NaN" << std::endl;
     return x;
  }
  while (x >= TWOPI) x -= TWOPI;
  while (x <    0.)  x += TWOPI;
  return x;
}

//______________________________________________________________________________
MAdouble64 MAVector3::Phi_mpi_pi(MAdouble64 x)
{
   // (static function) returns phi angle in the interval [-PI,PI)
  if(std::isnan(x))
  {
    std::cout << "TVector2::Phi_mpi_pi : function called with NaN" << std::endl;
    return x;
  }
  while (x >= M_PI) x -= TWOPI;
  while (x < -M_PI) x += TWOPI;
  return x;
}

//______________________________________________________________________________
const MAdouble64& MAVector3::operator() (MAuint8 i) const
{
  if (i==0) return x_;
  else if (i==1) return y_;
  else if (i==2) return z_;
  else
  {
    std::cout << "ERROR: bad index = " << static_cast<int>(i) << std::endl;
  }
  return x_;
}

//______________________________________________________________________________
MAdouble64 & MAVector3::operator() (MAuint8 i)
{
  if (i==0) return x_;
  else if (i==1) return y_;
  else if (i==2) return z_;
  else
  {
    std::cout << "ERROR: bad index = " << static_cast<int>(i) << std::endl;
  }
  return x_;
}


//______________________________________________________________________________
MAdouble64 MAVector3::Angle(const MAVector3 & q) const 
{
  // return the angle w.r.t. another 3-vector
  MAdouble64 ptot2 = Mag2()*q.Mag2();
  if(ptot2 <= 0)
  {
     return 0.0;
  }
  else
  {
    MAdouble64 arg = Dot(q)/std::sqrt(ptot2);
    if(arg >  1.0) arg =  1.0;
    if(arg < -1.0) arg = -1.0;
    return std::acos(arg);
  }
}




//______________________________________________________________________________
MAVector3 MAVector3::Unit() const 
{
   // return unit vector parallel to this.
   MAdouble64  tot2 = Mag2();
   MAdouble64 tot = (tot2 > 0) ?  1.0/std::sqrt(tot2) : 1.0;
   MAVector3 p(x_*tot,y_*tot,z_*tot);
   return p;
}


//______________________________________________________________________________
MAdouble64 MAVector3::PseudoRapidity() const
{
   //MAdouble64 m = Mag();
   //return 0.5*log( (m+z_)/(m-z_) );
   // guard against Pt=0
   MAfloat64 cosTheta = CosTheta();
   if (cosTheta*cosTheta< 1) return -0.5* std::log( (1.0-cosTheta)/(1.0+cosTheta) );
   if (z_ == 0) return 0;
   if (z_ > 0) return  999;
   else        return -999;
}

//______________________________________________________________________________
void MAVector3::SetPtEtaPhi(MAdouble64 pt, MAdouble64 eta, MAdouble64 phi)
{
   //set Pt, Eta and Phi
   MAdouble64 apt = std::abs(pt);
   SetXYZ(apt*std::cos(phi),
   apt*std::sin(phi),
   apt/std::tan(2.0*std::atan(std::exp(-eta))) );
}

//______________________________________________________________________________
void MAVector3::SetPtThetaPhi(MAdouble64 pt, MAdouble64 theta, MAdouble64 phi)
{
   //set Pt, Theta and Phi
   x_ = pt * std::cos(phi);
   y_ = pt * std::sin(phi); 
   MAdouble64 tanTheta = std::tan(theta);
   z_ = tanTheta ? pt / tanTheta : 0;
}

//______________________________________________________________________________
void MAVector3::SetTheta(MAdouble64 th) 
{
   // set theta keeping mag and phi constant (BaBar).
   MAdouble64 ma   = Mag();
   MAdouble64 ph   = Phi();
   SetX(ma*std::sin(th)*std::cos(ph));
   SetY(ma*std::sin(th)*std::sin(ph));
   SetZ(ma*std::cos(th));
}

//______________________________________________________________________________
void MAVector3::SetPhi(MAdouble64 ph) 
{
   // set phi keeping mag and theta constant (BaBar).
   MAdouble64 xy   = Perp();
   SetX(xy*std::cos(ph));
   SetY(xy*std::sin(ph));
}

//______________________________________________________________________________
MAdouble64 MAVector3::DeltaR(const MAVector3 & v) const 
{
   //return deltaR with respect to v
   MAdouble64 deta = Eta()-v.Eta();
   MAdouble64 dphi = Phi_mpi_pi(Phi()-v.Phi());
   return std::sqrt( deta*deta+dphi*dphi );
}

//______________________________________________________________________________
void MAVector3::SetMagThetaPhi(MAdouble64 mag, MAdouble64 theta, MAdouble64 phi) 
{
   //setter with mag, theta, phi
   MAdouble64 amag = std::abs(mag);
   x_ = amag * std::sin(theta) * std::cos(phi);
   y_ = amag * std::sin(theta) * std::sin(phi);
   z_ = amag * std::cos(theta);
}

//______________________________________________________________________________

