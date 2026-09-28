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
 * @file MALorentzVector.h
 * @brief Four-vector (ROOT TLorentzVector-like interface).
 */

#ifndef MALorentzVector_h
#define MALorentzVector_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Vector/MAVector3.h"


namespace MA5
{

/** @brief Four-vector (x, y, z, t) = (px, py, pz, E) with an interface similar to ROOT's TLorentzVector. */
class MALorentzVector
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
   
  /** @brief Spatial part. */
  MAVector3  p_;
  /** @brief Time component (energy). */
  MAdouble64 e_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  
  // Constructors ------------------------------------------------
  /** @brief Constructor (null vector). */
  MALorentzVector();
  /**
   * @brief Constructor from components.
   *
   * @param x x component.
   * @param y y component.
   * @param z z component.
   * @param t time component.
   */
  MALorentzVector(MAdouble64 x, MAdouble64 y, MAdouble64 z, MAdouble64 t);
  /**
   * @brief Constructor from a three-vector and a time component.
   *
   * @param p spatial part.
   * @param e time component.
   */
  MALorentzVector(const MAVector3& p, MAdouble64 e);
  /**
   * @brief Copy constructor.
   *
   * @param q vector to copy.
   */
  MALorentzVector(const MALorentzVector &);

  
  // Destructor --------------------------------------------------
  /** @brief Destructor. */
  virtual ~MALorentzVector();


  // Common methods-----------------------------------------------
  /** @brief Print (nothing is printed). */
  void Print() const
  {}

  /** @brief Set all the components to zero. */
  void Reset()
  {e_=0.; p_.Reset();}
  /** @brief Set all the components to zero (alias of Reset()). */
  void clear()
  { Reset(); }
  
  // Access operator (read-only mode) ----------------------------
  /**
   * @brief Access a component by index (read-only).
   *
   * @param i index (0-2: x, y, z; 3: t).
   * @return the component (t with an error for a bad index).
   */
  const MAdouble64& operator () (MAuint8) const;
  /**
   * @brief Access a component by index (read-only).
   *
   * @param i index (0-2: x, y, z; 3: t).
   * @return the component.
   */
  const MAdouble64& operator [] (MAuint8 i) const {return operator()(i);}

  
  // Access operator (read-write mode) ---------------------------
  /**
   * @brief Access a component by index.
   *
   * @param i index (0-2: x, y, z; 3: t).
   * @return the component (t with an error for a bad index).
   */
  MAdouble64 & operator () (MAuint8);
  /**
   * @brief Access a component by index.
   *
   * @param i index (0-2: x, y, z; 3: t).
   * @return the component.
   */
  MAdouble64 & operator [] (MAuint8 i) {return operator()(i);}

  
  // Simple accessors --------------------------------------------
  /**
   * @brief Accessor to the x component.
   *
   * @return the component.
   */
  const MAdouble64& X()      const {return p_.X();}
  /**
   * @brief Accessor to the y component.
   *
   * @return the component.
   */
  const MAdouble64& Y()      const {return p_.Y();}
  /**
   * @brief Accessor to the z component.
   *
   * @return the component.
   */
  const MAdouble64& Z()      const {return p_.Z();}
  /**
   * @brief Accessor to the time component.
   *
   * @return the component.
   */
  const MAdouble64& T()      const {return e_;    }
  /**
   * @brief Accessor to the x component of the momentum.
   *
   * @return the component.
   */
  const MAdouble64& Px()     const {return p_.X();}
  /**
   * @brief Accessor to the y component of the momentum.
   *
   * @return the component.
   */
  const MAdouble64& Py()     const {return p_.Y();}
  /**
   * @brief Accessor to the z component of the momentum.
   *
   * @return the component.
   */
  const MAdouble64& Pz()     const {return p_.Z();}
  /**
   * @brief Accessor to the energy.
   *
   * @return the component.
   */
  const MAdouble64& E()      const {return e_;    }
  /**
   * @brief Accessor to the energy.
   *
   * @return the component.
   */
  const MAdouble64& Energy() const {return e_;    }

  /**
   * @brief Accessor to the spatial part (read-only).
   *
   * @return the three-vector.
   */
  const MAVector3& Vect() const {return p_;}
  /**
   * @brief Accessor to the spatial part.
   *
   * @return the three-vector.
   */
  MAVector3&       Vect()       {return p_;}

  
  // Simple mutators ---------------------------------------------
  /**
   * @brief Set the x component.
   *
   * @param x value.
   */
  void SetX (MAdouble64 x) {p_.SetX(x);}
  /**
   * @brief Set the y component.
   *
   * @param y value.
   */
  void SetY (MAdouble64 y) {p_.SetY(y);}
  /**
   * @brief Set the z component.
   *
   * @param z value.
   */
  void SetZ (MAdouble64 z) {p_.SetZ(z);}
  /**
   * @brief Set the time component.
   *
   * @param e value.
   */
  void SetT (MAdouble64 e) {e_=e;      }
  /**
   * @brief Set the x component of the momentum.
   *
   * @param x value.
   */
  void SetPx(MAdouble64 x) {p_.SetX(x);}
  /**
   * @brief Set the y component of the momentum.
   *
   * @param y value.
   */
  void SetPy(MAdouble64 y) {p_.SetY(y);}
  /**
   * @brief Set the z component of the momentum.
   *
   * @param z value.
   */
  void SetPz(MAdouble64 z) {p_.SetZ(z);}
  /**
   * @brief Set the energy.
   *
   * @param e value.
   */
  void SetE (MAdouble64 e) {e_=e;      }

  /**
   * @brief Set the spatial part.
   *
   * @param p three-vector.
   */
  void SetVect(const MAVector3& p)
  {p_=p;}
  
  /**
   * @brief Set the spatial part.
   *
   * @param x x component.
   * @param y y component.
   * @param z z component.
   */
  void SetXYZ(MAdouble64 x, MAdouble64 y, MAdouble64 z)
  {p_.SetXYZ(x,y,z);}

  /**
   * @brief Set all the components.
   *
   * @param x x component.
   * @param y y component.
   * @param z z component.
   * @param t time component.
   */
  void SetXYZT(MAdouble64 x, MAdouble64 y, MAdouble64 z, MAdouble64 t)
  {p_.SetXYZ(x,y,z); e_=t;}

  /**
   * @brief Set the momentum and the mass (the energy is computed; a negative mass gives E^2 = p^2 - m^2).
   *
   * @param x x component.
   * @param y y component.
   * @param z z component.
   * @param m mass.
   */
  void SetXYZM(MAdouble64  x, MAdouble64  y, MAdouble64  z, MAdouble64 m)
  {
    if (m>=0) SetXYZT( x, y, z, std::sqrt(x*x+y*y+z*z+m*m) );
    else SetXYZT( x, y, z, std::sqrt( std::max((x*x+y*y+z*z-m*m), 0. ) ) );
  }
  
  /**
   * @brief Set the momentum and the energy.
   *
   * @param x x component.
   * @param y y component.
   * @param z z component.
   * @param e energy.
   */
  void SetPxPyPzE(MAdouble64 x, MAdouble64 y, MAdouble64 z, MAdouble64 e)
  {p_.SetXYZ(x,y,z); e_=e;}

  
  // Shortcut to MAvector3 sophisticated accessors -------------------------

  /**
   * @brief Azimuthal angle, in [-pi, pi].
   *
   * @return the angle.
   */
  MAdouble64 Phi() const {return p_.Phi(); }

  /**
   * @brief Polar angle.
   *
   * @return the angle.
   */
  MAdouble64 Theta() const {return p_.Theta(); }

  /**
   * @brief Cosine of the polar angle.
   *
   * @return the cosine.
   */
  MAdouble64 CosTheta() const {return p_.CosTheta();}

  /**
   * @brief Squared transverse component (despite the name, not the spherical radius).
   *
   * @return pT^2.
   */
  MAdouble64 Rho2() const {return p_.Perp2();}

  /**
   * @brief Transverse component (despite the name, not the spherical radius).
   *
   * @return pT.
   */
  MAdouble64 Rho() const {return p_.Perp();}

  /**
   * @brief Magnitude of the momentum.
   *
   * @return |p|.
   */
  MAdouble64 P() const {return p_.Mag();}

  /**
   * @brief Squared magnitude of the momentum.
   *
   * @return |p|^2.
   */
  MAdouble64 P2() const {return p_.Mag2();}

  /**
   * @brief Squared transverse momentum.
   *
   * @return pT^2.
   */
  MAdouble64 Perp2() const {return p_.Perp2();}

  /**
   * @brief Transverse momentum.
   *
   * @return pT.
   */
  MAdouble64 Pt()   const {return p_.Pt();}
  /**
   * @brief Transverse momentum (alias of Pt()).
   *
   * @return pT.
   */
  MAdouble64 Perp() const {return p_.Pt();}

  /**
   * @brief Pseudorapidity.
   *
   * @return the pseudorapidity.
   */
  MAdouble64 PseudoRapidity() const {return p_.PseudoRapidity();}
  /**
   * @brief Pseudorapidity (alias of PseudoRapidity()).
   *
   * @return the pseudorapidity.
   */
  MAdouble64 Eta()            const {return p_.PseudoRapidity();}
  

  // Specific sophisticated accessors ------------------------------
  
  /**
   * @brief Squared invariant mass.
   *
   * @return E^2 - |p|^2.
   */
  MAdouble64 Mag2() const
  { return T()*T() - p_.Mag2(); }

  /**
   * @brief Invariant mass (negative if the squared mass is negative).
   *
   * @return the mass.
   */
  inline MAdouble64 Mag() const
  {
    MAdouble64 mm = Mag2();
    return mm < 0.0 ? - std::sqrt(-mm) : std::sqrt(mm);
  }
  
  /**
   * @brief Squared invariant mass (alias of Mag2()).
   *
   * @return E^2 - |p|^2.
   */
  inline MAdouble64 M2() const
  { return Mag2(); }
    
  /**
   * @brief Invariant mass (alias of Mag()).
   *
   * @return the mass.
   */
  inline MAdouble64 M() const
  { return Mag(); }
    
  /**
   * @brief Squared transverse mass E^2 - pz^2 (= m^2 + pT^2).
   *
   * @return the squared transverse mass.
   */
  inline MAdouble64 Mt2() const
  { return E()*E() - Z()*Z(); }
    
  /**
   * @brief Transverse mass (negative if the squared value is negative).
   *
   * @return the transverse mass.
   */
  inline MAdouble64 Mt() const
  {
    MAdouble64 mm = Mt2();
    return mm < 0.0 ? - std::sqrt(-mm) : std::sqrt(mm);
  }
    
  /**
   * @brief Squared transverse energy E^2 pT^2 / |p|^2.
   *
   * @return the squared transverse energy.
   */
  inline MAdouble64 Et2() const
  {
    MAdouble64 pt2 = p_.Perp2();
    return pt2 == 0 ? 0 : E()*E() * pt2/(pt2+Z()*Z());
  }

  /**
   * @brief Transverse energy (with the sign of E).
   *
   * @return the transverse energy.
   */
  inline MAdouble64 Et() const
  {
    MAdouble64 etet = Et2();
    return e_ < 0.0 ? -std::sqrt(etet) : std::sqrt(etet);
  }

  /**
   * @brief Velocity |p|/E.
   *
   * @return beta.
   */
  inline MAdouble64 Beta() const
  { return p_.Mag()/e_; }
    
  /**
   * @brief Lorentz factor.
   *
   * @return gamma.
   */
  inline MAdouble64 Gamma() const
  {
    MAdouble64 b = Beta();
    return 1.0/std::sqrt(1- b*b);
  }
    
  /**
   * @brief Rapidity 0.5 ln((E+pz)/(E-pz)).
   *
   * @return the rapidity.
   */
  inline MAdouble64 Rapidity() const
  {
    return 0.5 * std::log ( (E()+Pz()) / (E()-Pz()) );
  }    

  
  // Sophisticated mutators ---------------------------------------

  /**
   * @brief Set the momentum and the mass.
   *
   * @param spatial momentum.
   * @param magnitude mass.
   */
  inline void SetVectMag(const MAVector3& spatial, MAdouble64 magnitude)
    {SetXYZM(spatial.X(), spatial.Y(), spatial.Z(), magnitude);}
  /**
   * @brief Set the momentum and the mass.
   *
   * @param spatial momentum.
   * @param mass mass.
   */
  inline void SetVectM  (const MAVector3& spatial, MAdouble64 mass)
    {SetVectMag(spatial, mass);}
  /**
   * @brief Set the momentum and the energy (declared but never defined).
   *
   * @param spatial momentum.
   * @param energy energy.
   */
  // NOTE: no definition exists: using this method gives a link error.
  inline void SetVectE  (const MAVector3& spatial, MAdouble64 energy);

  /**
   * @brief Set the vector from (pT, eta, phi, m).
   *
   * @param pt transverse momentum.
   * @param eta pseudorapidity.
   * @param phi azimuthal angle.
   * @param m mass.
   */
  void SetPtEtaPhiM   (MAdouble64 pt, MAdouble64 eta, MAdouble64 phi, MAdouble64 m)
  { SetXYZM(pt*std::cos(phi), pt*std::sin(phi), pt*std::sinh(eta) ,m); }
  
  /**
   * @brief Set the vector from (pT, eta, phi, E).
   *
   * @param pt transverse momentum.
   * @param eta pseudorapidity.
   * @param phi azimuthal angle.
   * @param e energy.
   */
  void SetPtEtaPhiE(MAdouble64 pt, MAdouble64 eta, MAdouble64 phi, MAdouble64 e)
  {p_.SetPtEtaPhi(pt,eta,phi); e_=e;}

   
  // Operations with another  -----------------------------------

  /**
   * @brief Azimuthal angle difference, in [-pi, pi).
   *
   * @param q other vector.
   * @return the difference.
   */
  inline MAdouble64 DeltaPhi(const MALorentzVector& q) const
  { return p_.DeltaPhi(q.Vect()); }

  /**
   * @brief Angular distance DeltaR = sqrt(Deta^2 + Dphi^2).
   *
   * @param q other vector.
   * @return DeltaR.
   */
  MAdouble64 DeltaR (const MALorentzVector& q) const
  { return p_.DeltaR(q.Vect()); }
  
  /**
   * @brief Angle between the spatial parts.
   *
   * @param q other vector.
   * @return the angle.
   */
  MAdouble64 Angle(const MALorentzVector& q) const
  { return p_.Angle(q.Vect()); }

  /**
   * @brief Minkowski scalar product.
   *
   * @param q other vector.
   * @return the scalar product.
   */
  MAdouble64 Dot(const MALorentzVector& q) const
  { return T()*q.T() - Z()*q.Z() - Y()*q.Y() - X()*q.X(); }

  
  // Operators -----------------------------------------
   
   /**
    * @brief Assignment.
    *
    * @param q vector.
    * @return this vector.
    */
   inline MALorentzVector & operator = (const MALorentzVector &);
   // Assignment.

   /**
    * @brief Sum of two four-vectors.
    *
    * @param q vector.
    * @return the sum.
    */
   inline MALorentzVector   operator +  (const MALorentzVector &) const;
   /**
    * @brief Add a four-vector.
    *
    * @param q vector.
    * @return this vector.
    */
   inline MALorentzVector & operator += (const MALorentzVector &);
   // Additions.

   /**
    * @brief Difference of two four-vectors.
    *
    * @param q vector.
    * @return the difference.
    */
   inline MALorentzVector   operator -  (const MALorentzVector &) const;
   /**
    * @brief Subtract a four-vector.
    *
    * @param q vector.
    * @return this vector.
    */
   inline MALorentzVector & operator -= (const MALorentzVector &);
   // Subtractions.

   /**
    * @brief Opposite four-vector.
    *
    * @return -this.
    */
   inline MALorentzVector operator - () const;
   // Unary minus.

   /**
    * @brief Scaled four-vector.
    *
    * @param a factor.
    * @return the scaled vector.
    */
   inline MALorentzVector operator * (MAdouble64 a) const;
   /**
    * @brief Scale the four-vector.
    *
    * @param a factor.
    * @return this vector.
    */
   inline MALorentzVector & operator *= (MAdouble64 a);
   /**
    * @brief Minkowski scalar product.
    *
    * @param q vector.
    * @return the scalar product.
    */
   MAdouble64 operator * (const MALorentzVector& q) const
   { return Dot(q); }



};


inline MALorentzVector &MALorentzVector::operator = (const MALorentzVector & q)
{
  p_ = q.Vect();
  e_ = q.T();
  return *this;
}

inline MALorentzVector MALorentzVector::operator + (const MALorentzVector & q) const
{ return MALorentzVector(p_+q.Vect(), e_+q.E()); }

inline MALorentzVector &MALorentzVector::operator += (const MALorentzVector & q)
{
   p_ += q.Vect();
   e_ += q.E();
   return *this;
}

inline MALorentzVector MALorentzVector::operator - (const MALorentzVector & q) const
{ return MALorentzVector(p_-q.Vect(), e_-q.E()); }

inline MALorentzVector &MALorentzVector::operator -= (const MALorentzVector & q)
{
   p_ -= q.Vect();
   e_ -= q.E();
   return *this;
}

inline MALorentzVector MALorentzVector::operator - () const
{ return MALorentzVector(-X(), -Y(), -Z(), -e_); }

inline MALorentzVector& MALorentzVector::operator *= (MAdouble64 a)
{
   p_ *= a;
   e_ *= a;
   return *this;
}

inline MALorentzVector MALorentzVector::operator * (MAdouble64 a) const
{ return MALorentzVector(a*X(), a*Y(), a*Z(), a*e_); }


/**
 * @brief Scaled four-vector (factor on the left).
 *
 * @param a factor.
 * @param p vector.
 * @return the scaled vector.
 */
inline MALorentzVector operator * (MAdouble64 a, const MALorentzVector& p)
{ return MALorentzVector(a*p.X(), a*p.Y(), a*p.Z(), a*p.T()); }
 
}

#endif
