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


#ifndef MAVector3_h
#define MAVector3_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class MAVector3
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
   
  MAdouble64 x_;
  MAdouble64 y_;
  MAdouble64 z_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  
  // Constructors ------------------------------------------------
  MAVector3()
  { Reset(); }

  MAVector3(MAdouble64 x, MAdouble64 y, MAdouble64 z)
  { x_=x; y_=y; z_=z; }
  
  MAVector3(const MAVector3 &p)
  { x_=p.x_; y_=p.y_; z_=p.z_; }

  
  // Destructor --------------------------------------------------
  virtual ~MAVector3()
  {}


  // Common methods-----------------------------------------------
  void Print() const
  {}

  void Reset()
  {x_=0.; y_=0.; z_=0.;}
  void clear()
  {Reset();}

  // Static methods ----------------------------------------------

  // returns phi angle in the interval [0,2*PI) 
  static MAdouble64 Phi_0_2pi(MAdouble64 x);

  // returns phi angle in the interval [-PI,PI)
  static MAdouble64 Phi_mpi_pi(MAdouble64 x);

  
  // Access operator (read-only mode) ----------------------------
  const MAdouble64& operator() (MAuint8) const;
  const MAdouble64& operator[] (MAuint8 i) const {return operator()(i);}

  
  // Access operator (read-write mode) ---------------------------
  MAdouble64& operator() (MAuint8);
  MAdouble64& operator[] (MAuint8 i) {return operator()(i);}

  
  // Simple accessors --------------------------------------------
  const MAdouble64& X()  const {return x_;}
  const MAdouble64& Y()  const {return y_;}
  const MAdouble64& Z()  const {return z_;}
  const MAdouble64& Px() const {return x_;}
  const MAdouble64& Py() const {return y_;}
  const MAdouble64& Pz() const {return z_;}

  
  // Simple mutators ---------------------------------------------
  void SetX(MAdouble64 x) {x_=x;}
  void SetY(MAdouble64 y) {y_=y;}
  void SetZ(MAdouble64 z) {z_=z;}
  void SetXYZ(MAdouble64 x, MAdouble64 y, MAdouble64 z)
  {x_=x; y_=y; z_=z;}

  
  // Sophisticated accessors -------------------------------------

  // The azimuth angle. returns phi from -pi to pi  
  MAdouble64 Phi() const
  { return x_ == 0.0 && y_ == 0.0 ? 0.0 : std::atan2(y_,x_); }

  // The polar angle.
  MAdouble64 Theta() const
  { return x_ == 0.0 && y_ == 0.0 && z_ == 0.0 ? 0.0 : std::atan2(Perp(),z_); }

  // Cosine of the polar angle.  
  MAdouble64 CosTheta() const
  {
    MAdouble64 ptot = Mag();
    return ptot == 0.0 ? 1.0 : z_/ptot;
  }
  
  // The magnitude squared (rho^2 in spherical coordinate system).  
  MAdouble64 Mag2() const
  { return x_*x_+ y_*y_ + z_*z_; }

    // The magnitude (rho in spherical coordinate system).
  MAdouble64 Mag() const
  { return std::sqrt(Mag2()); }

  // The transverse component squared (R^2 in cylindrical coordinate system).  
  MAdouble64 Perp2() const
  { return x_*x_+ y_*y_; }

  // The transverse component (R in cylindrical coordinate system).  
  MAdouble64 Pt() const
  { return std::sqrt(Perp2()); }

  MAdouble64 Perp() const
  { return Pt(); }

  // Returns the pseudo-rapidity, i.e. -ln(tan(theta/2))
  MAdouble64 PseudoRapidity() const;
  inline MAdouble64 Eta() const
  { return PseudoRapidity(); }
  
   
  // Sophisticated mutators ---------------------------------------

  // set all components
  void SetPtEtaPhi(MAdouble64 pt, MAdouble64 eta, MAdouble64 phi);
  void SetPtThetaPhi(MAdouble64 pt, MAdouble64 theta, MAdouble64 phi);
  void SetMagThetaPhi(MAdouble64 mag, MAdouble64 theta, MAdouble64 phi);

  // Set phi keeping mag and theta constant (BaBar).
  void SetPhi(MAdouble64);

  // Set theta keeping mag and phi constant (BaBar).
  void SetTheta(MAdouble64);

  // Set magnitude keeping theta and phi constant (BaBar).
  inline void SetMag(MAdouble64 ma)
  {
    MAdouble64 factor = Mag();
    if (factor == 0)
    {
      std::cout << "SetMag : zero vector can't be stretched" << std::endl;
    }
    else
    {
      factor = ma/factor;
      SetX(x_*factor);
      SetY(y_*factor);
      SetZ(z_*factor);
    }
  } 

  // Set the transverse component keeping phi and z constant.
  inline void SetPerp(MAdouble64 r)
  {
    MAdouble64 p = Perp();
    if (p != 0.0)
    {
      x_ *= r/p;
      y_ *= r/p;
    }
  }

  
  // Operations with another  -----------------------------------

   // DeltaPhi between 2 vectors
   MAdouble64 DeltaPhi(const MAVector3 &v) const
   { return Phi_mpi_pi(Phi()-v.Phi()); }

   // DeltaR between 2 vectors
   MAdouble64 DeltaR(const MAVector3& p) const;
  
   // Scalar product.
   MAdouble64 Dot(const MAVector3& p) const
   {
     return x_*p.x_ + y_*p.y_ + z_*p.z_;
   }

   // Cross product.
   MAVector3 Cross(const MAVector3& p) const
   {
     return MAVector3(y_*p.z_-p.y_*z_, z_*p.x_-p.z_*x_, x_*p.y_-p.x_*y_);
   }

   // The angle w.r.t. another 3-vector.
   MAdouble64 Angle(const MAVector3& ) const;

   
  // Producing new vector ------------------------------

  // Unit vector parallel to this.
  MAVector3 Unit() const;

  // Vector orthogonal to this
  MAVector3 Orthogonal() const
  {
    MAdouble64 xx = x_ < 0.0 ? -x_ : x_;
    MAdouble64 yy = y_ < 0.0 ? -y_ : y_;
    MAdouble64 zz = z_ < 0.0 ? -z_ : z_;
    if (xx < yy)
    {
      return xx < zz ? MAVector3(0,z_,-y_) : MAVector3(y_,-x_,0);
    }
    else
    {
      return yy < zz ? MAVector3(-z_,0,x_) : MAVector3(y_,-x_,0);
    }
  }

  
  // Operators -----------------------------------------
   
  // Assignment.
  MAVector3& operator = (const MAVector3& p)
  {
    x_ = p.x_;
    y_ = p.y_;
    z_ = p.z_;
    return *this;
  }
    
  // Addition
  MAVector3& operator += (const MAVector3& p)
  {
    x_ += p.x_;
    y_ += p.y_;
    z_ += p.z_;
    return *this;
  }

  // Subtraction.
  MAVector3 & operator -= (const MAVector3& p)
  {
    x_ -= p.x_;
    y_ -= p.y_;
    z_ -= p.z_;
    return *this;
  }

  // Unary minus.
  MAVector3 operator - () const
  { return MAVector3(-x_, -y_, -z_); }

  // Scaling with real numbers.
  MAVector3 & operator *= (MAdouble64 a)
  { x_ *= a; y_ *= a; z_ *= a; return *this; }

  // Addition of 3-vectors.
  MAVector3 operator + (const MAVector3& a) const
  { return MAVector3(X()+a.X(), Y()+a.Y() , Z()+a.Z() ); }

  MAVector3 operator - (const MAVector3& a) const
  { return MAVector3(X()-a.X(), Y()-a.Y() , Z()-a.Z() ); }

  MAVector3 operator * (MAdouble64 a) const
  { return MAVector3(a*X(), a*Y(), a*Z()); }

  MAdouble64 operator * (const MAVector3& a) const
  { return Dot(a); }
  
  
};


inline MAVector3 operator * (MAdouble64 a, const MAVector3& p)
{ return MAVector3(a*p.X(), a*p.Y(), a*p.Z()); }


}


#endif
