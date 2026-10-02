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
 * @file MAVector3.h
 * @brief Three-vector (ROOT TVector3-like interface).
 */

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

/** @brief Three-vector with an interface similar to ROOT's TVector3. */
class MAVector3
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
   
  /** @brief Cartesian components. */
  MAdouble64 x_;
  MAdouble64 y_;
  MAdouble64 z_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  
  // Constructors ------------------------------------------------
  /** @brief Constructor (null vector). */
  MAVector3()
  { Reset(); }

  /**
   * @brief Constructor from components.
   *
   * @param x x component.
   * @param y y component.
   * @param z z component.
   */
  MAVector3(MAdouble64 x, MAdouble64 y, MAdouble64 z)
  { x_=x; y_=y; z_=z; }
  
  /**
   * @brief Copy constructor.
   *
   * @param p vector to copy.
   */
  MAVector3(const MAVector3 &p)
  { x_=p.x_; y_=p.y_; z_=p.z_; }

  
  // Destructor --------------------------------------------------
  /** @brief Destructor. */
  virtual ~MAVector3()
  {}


  // Common methods-----------------------------------------------
  /** @brief Print (nothing is printed). */
  void Print() const
  {}

  /** @brief Set all the components to zero. */
  void Reset()
  {x_=0.; y_=0.; z_=0.;}
  /** @brief Set all the components to zero (alias of Reset()). */
  void clear()
  {Reset();}

  // Static methods ----------------------------------------------

  /**
   * @brief Map an angle to the interval [0, 2pi).
   *
   * @param x angle.
   * @return the mapped angle (NaN is returned unchanged).
   */
  static MAdouble64 Phi_0_2pi(MAdouble64 x);

  /**
   * @brief Map an angle to the interval [-pi, pi).
   *
   * @param x angle.
   * @return the mapped angle (NaN is returned unchanged).
   */
  static MAdouble64 Phi_mpi_pi(MAdouble64 x);

  
  /**
   * @brief Access a component by index (read-only).
   *
   * @param i index (0: x, 1: y, 2: z).
   * @return the component (x with an error for a bad index).
   */
  const MAdouble64& operator() (MAuint8) const;
  /**
   * @brief Access a component by index (read-only).
   *
   * @param i index (0: x, 1: y, 2: z).
   * @return the component.
   */
  const MAdouble64& operator[] (MAuint8 i) const {return operator()(i);}

  
  /**
   * @brief Access a component by index.
   *
   * @param i index (0: x, 1: y, 2: z).
   * @return the component (x with an error for a bad index).
   */
  MAdouble64& operator() (MAuint8);
  /**
   * @brief Access a component by index.
   *
   * @param i index (0: x, 1: y, 2: z).
   * @return the component.
   */
  MAdouble64& operator[] (MAuint8 i) {return operator()(i);}

  
  /**
   * @brief Accessor to the x component.
   *
   * @return the component.
   */
  const MAdouble64& X()  const {return x_;}
  /**
   * @brief Accessor to the y component.
   *
   * @return the component.
   */
  const MAdouble64& Y()  const {return y_;}
  /**
   * @brief Accessor to the z component.
   *
   * @return the component.
   */
  const MAdouble64& Z()  const {return z_;}
  /**
   * @brief Accessor to the x component.
   *
   * @return the component.
   */
  const MAdouble64& Px() const {return x_;}
  /**
   * @brief Accessor to the y component.
   *
   * @return the component.
   */
  const MAdouble64& Py() const {return y_;}
  /**
   * @brief Accessor to the z component.
   *
   * @return the component.
   */
  const MAdouble64& Pz() const {return z_;}

  
  /**
   * @brief Set the x component.
   *
   * @param x value.
   */
  void SetX(MAdouble64 x) {x_=x;}
  /**
   * @brief Set the y component.
   *
   * @param y value.
   */
  void SetY(MAdouble64 y) {y_=y;}
  /**
   * @brief Set the z component.
   *
   * @param z value.
   */
  void SetZ(MAdouble64 z) {z_=z;}
  /**
   * @brief Set all the components.
   *
   * @param x x component.
   * @param y y component.
   * @param z z component.
   */
  void SetXYZ(MAdouble64 x, MAdouble64 y, MAdouble64 z)
  {x_=x; y_=y; z_=z;}

  
  // Sophisticated accessors -------------------------------------

  /**
   * @brief Azimuthal angle in [-pi, pi] (0 for a vector along z).
   *
   * @return the angle.
   */
  MAdouble64 Phi() const
  { return x_ == 0.0 && y_ == 0.0 ? 0.0 : std::atan2(y_,x_); }

  /**
   * @brief Polar angle.
   *
   * @return the angle.
   */
  MAdouble64 Theta() const
  { return x_ == 0.0 && y_ == 0.0 && z_ == 0.0 ? 0.0 : std::atan2(Perp(),z_); }

  /**
   * @brief Cosine of the polar angle.
   *
   * @return the cosine (1 for a null vector).
   */
  MAdouble64 CosTheta() const
  {
    MAdouble64 ptot = Mag();
    return ptot == 0.0 ? 1.0 : z_/ptot;
  }
  
  /**
   * @brief Squared magnitude.
   *
   * @return the squared magnitude.
   */
  MAdouble64 Mag2() const
  { return x_*x_+ y_*y_ + z_*z_; }

  /**
   * @brief Magnitude.
   *
   * @return the magnitude.
   */
  MAdouble64 Mag() const
  { return std::sqrt(Mag2()); }

  /**
   * @brief Squared transverse component.
   *
   * @return the squared transverse component.
   */
  MAdouble64 Perp2() const
  { return x_*x_+ y_*y_; }

  /**
   * @brief Transverse component.
   *
   * @return the transverse component.
   */
  MAdouble64 Pt() const
  { return std::sqrt(Perp2()); }

  /**
   * @brief Transverse component (alias of Pt()).
   *
   * @return the transverse component.
   */
  MAdouble64 Perp() const
  { return Pt(); }

  /**
   * @brief Pseudorapidity -ln(tan(theta/2)).
   *
   * @return the pseudorapidity (+/-999 along the beam axis, 0 for a null vector).
   */
  MAdouble64 PseudoRapidity() const;
  /**
   * @brief Pseudorapidity (alias of PseudoRapidity()).
   *
   * @return the pseudorapidity.
   */
  inline MAdouble64 Eta() const
  { return PseudoRapidity(); }
  
   
  // Sophisticated mutators ---------------------------------------

  /**
   * @brief Set the vector from (pT, eta, phi).
   *
   * @param pt transverse component (absolute value used).
   * @param eta pseudorapidity.
   * @param phi azimuthal angle.
   */
  void SetPtEtaPhi(MAdouble64 pt, MAdouble64 eta, MAdouble64 phi);
  /**
   * @brief Set the vector from (pT, theta, phi).
   *
   * @param pt transverse component.
   * @param theta polar angle.
   * @param phi azimuthal angle.
   */
  void SetPtThetaPhi(MAdouble64 pt, MAdouble64 theta, MAdouble64 phi);
  /**
   * @brief Set the vector from (magnitude, theta, phi).
   *
   * @param mag magnitude (absolute value used).
   * @param theta polar angle.
   * @param phi azimuthal angle.
   */
  void SetMagThetaPhi(MAdouble64 mag, MAdouble64 theta, MAdouble64 phi);

  /**
   * @brief Set the azimuthal angle, keeping the magnitude and the polar angle.
   *
   * @param ph azimuthal angle.
   */
  void SetPhi(MAdouble64);

  /**
   * @brief Set the polar angle, keeping the magnitude and the azimuthal angle.
   *
   * @param th polar angle.
   */
  void SetTheta(MAdouble64);

  /**
   * @brief Set the magnitude, keeping the direction.
   *
   * @param ma magnitude.
   */
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

  /**
   * @brief Set the transverse component, keeping phi and z.
   *
   * @param r transverse component.
   */
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

   /**
    * @brief Azimuthal angle difference, in [-pi, pi).
    *
    * @param v other vector.
    * @return the difference.
    */
   MAdouble64 DeltaPhi(const MAVector3 &v) const
   { return Phi_mpi_pi(Phi()-v.Phi()); }

   /**
    * @brief Angular distance DeltaR = sqrt(Deta^2 + Dphi^2).
    *
    * @param p other vector.
    * @return DeltaR.
    */
   MAdouble64 DeltaR(const MAVector3& p) const;
  
   /**
    * @brief Scalar product.
    *
    * @param p other vector.
    * @return the scalar product.
    */
   MAdouble64 Dot(const MAVector3& p) const
   {
     return x_*p.x_ + y_*p.y_ + z_*p.z_;
   }

   /**
    * @brief Cross product.
    *
    * @param p other vector.
    * @return the cross product.
    */
   MAVector3 Cross(const MAVector3& p) const
   {
     return MAVector3(y_*p.z_-p.y_*z_, z_*p.x_-p.z_*x_, x_*p.y_-p.x_*y_);
   }

   /**
    * @brief Angle between two vectors.
    *
    * @param q other vector.
    * @return the angle in [0, pi] (0 if a vector is null).
    */
   MAdouble64 Angle(const MAVector3& ) const;

   
  // Producing new vector ------------------------------

  /**
   * @brief Unit vector parallel to this one.
   *
   * @return the unit vector (the null vector stays null).
   */
  MAVector3 Unit() const;

  /**
   * @brief Vector orthogonal to this one.
   *
   * @return an orthogonal vector.
   */
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
   
  /**
   * @brief Assignment.
   *
   * @param p vector.
   * @return this vector.
   */
  MAVector3& operator = (const MAVector3& p)
  {
    x_ = p.x_;
    y_ = p.y_;
    z_ = p.z_;
    return *this;
  }
    
  /**
   * @brief Addition.
   *
   * @param p vector.
   * @return this vector.
   */
  MAVector3& operator += (const MAVector3& p)
  {
    x_ += p.x_;
    y_ += p.y_;
    z_ += p.z_;
    return *this;
  }

  /**
   * @brief Subtraction.
   *
   * @param p vector.
   * @return this vector.
   */
  MAVector3 & operator -= (const MAVector3& p)
  {
    x_ -= p.x_;
    y_ -= p.y_;
    z_ -= p.z_;
    return *this;
  }

  /**
   * @brief Opposite vector.
   *
   * @return -this.
   */
  MAVector3 operator - () const
  { return MAVector3(-x_, -y_, -z_); }

  /**
   * @brief Scaling.
   *
   * @param a factor.
   * @return this vector.
   */
  MAVector3 & operator *= (MAdouble64 a)
  { x_ *= a; y_ *= a; z_ *= a; return *this; }

  /**
   * @brief Sum of two vectors.
   *
   * @param a vector.
   * @return the sum.
   */
  MAVector3 operator + (const MAVector3& a) const
  { return MAVector3(X()+a.X(), Y()+a.Y() , Z()+a.Z() ); }

  /**
   * @brief Difference of two vectors.
   *
   * @param a vector.
   * @return the difference.
   */
  MAVector3 operator - (const MAVector3& a) const
  { return MAVector3(X()-a.X(), Y()-a.Y() , Z()-a.Z() ); }

  /**
   * @brief Scaled vector.
   *
   * @param a factor.
   * @return the scaled vector.
   */
  MAVector3 operator * (MAdouble64 a) const
  { return MAVector3(a*X(), a*Y(), a*Z()); }

  /**
   * @brief Scalar product.
   *
   * @param a vector.
   * @return the scalar product.
   */
  MAdouble64 operator * (const MAVector3& a) const
  { return Dot(a); }
  
  
};


/**
 * @brief Scaled vector (factor on the left).
 *
 * @param a factor.
 * @param p vector.
 * @return the scaled vector.
 */
inline MAVector3 operator * (MAdouble64 a, const MAVector3& p)
{ return MAVector3(a*p.X(), a*p.Y(), a*p.Z()); }


}


#endif
