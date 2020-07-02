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

class MALorentzVector
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
   
  MAVector3  p_;
  MAdouble64 e_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  
  // Constructors ------------------------------------------------
  MALorentzVector();
  MALorentzVector(MAdouble64 x, MAdouble64 y, MAdouble64 z, MAdouble64 t);
  MALorentzVector(const MAVector3& p, MAdouble64 e);
  MALorentzVector(const MALorentzVector &);

  
  // Destructor --------------------------------------------------
  virtual ~MALorentzVector();


  // Common methods-----------------------------------------------
  void Print() const
  {}

  void Reset()
  {e_=0.; p_.Reset();}
  void clear()
  { Reset(); }
  
  // Access operator (read-only mode) ----------------------------
  const MAdouble64& operator () (MAuint8) const;
  const MAdouble64& operator [] (MAuint8 i) const {return operator()(i);}

  
  // Access operator (read-write mode) ---------------------------
  MAdouble64 & operator () (MAuint8);
  MAdouble64 & operator [] (MAuint8 i) {return operator()(i);}

  
  // Simple accessors --------------------------------------------
  const MAdouble64& X()      const {return p_.X();}
  const MAdouble64& Y()      const {return p_.Y();}
  const MAdouble64& Z()      const {return p_.Z();}
  const MAdouble64& T()      const {return e_;    }
  const MAdouble64& Px()     const {return p_.X();}
  const MAdouble64& Py()     const {return p_.Y();}
  const MAdouble64& Pz()     const {return p_.Z();}
  const MAdouble64& E()      const {return e_;    }
  const MAdouble64& Energy() const {return e_;    }

  const MAVector3& Vect() const {return p_;}
  MAVector3&       Vect()       {return p_;}

  
  // Simple mutators ---------------------------------------------
  void SetX (MAdouble64 x) {p_.SetX(x);}
  void SetY (MAdouble64 y) {p_.SetY(y);}
  void SetZ (MAdouble64 z) {p_.SetZ(z);}
  void SetT (MAdouble64 e) {e_=e;      }
  void SetPx(MAdouble64 x) {p_.SetX(x);}
  void SetPy(MAdouble64 y) {p_.SetY(y);}
  void SetPz(MAdouble64 z) {p_.SetZ(z);}
  void SetE (MAdouble64 e) {e_=e;      }

  void SetVect(const MAVector3& p)
  {p_=p;}
  
  void SetXYZ(MAdouble64 x, MAdouble64 y, MAdouble64 z)
  {p_.SetXYZ(x,y,z);}

  void SetXYZT(MAdouble64 x, MAdouble64 y, MAdouble64 z, MAdouble64 t)
  {p_.SetXYZ(x,y,z); e_=t;}

  void SetXYZM(MAdouble64  x, MAdouble64  y, MAdouble64  z, MAdouble64 m)
  {
    if (m>=0) SetXYZT( x, y, z, std::sqrt(x*x+y*y+z*z+m*m) );
    else SetXYZT( x, y, z, std::sqrt( std::max((x*x+y*y+z*z-m*m), 0. ) ) );
  }
  
  void SetPxPyPzE(MAdouble64 x, MAdouble64 y, MAdouble64 z, MAdouble64 e)
  {p_.SetXYZ(x,y,z); e_=e;}

  
  // Shortcut to MAvector3 sophisticated accessors -------------------------

  // The azimuth angle. returns phi from -pi to pi  
  MAdouble64 Phi() const {return p_.Phi(); }

  // The polar angle.
  MAdouble64 Theta() const {return p_.Theta(); }

  // Cosine of the polar angle.  
  MAdouble64 CosTheta() const {return p_.CosTheta();}

  // Rho*Rho in spherical coordinate system).
  MAdouble64 Rho2() const {return p_.Perp2();}

  // Rho in spherical coordinate system).
  MAdouble64 Rho() const {return p_.Perp();}

  // Impulsion
  MAdouble64 P() const {return p_.Mag();}

  // Impulsion square
  MAdouble64 P2() const {return p_.Mag2();}

  // The transverse component squared (R^2 in cylindrical coordinate system).  
  MAdouble64 Perp2() const {return p_.Perp2();}

  // The transverse component (R in cylindrical coordinate system).  
  MAdouble64 Pt()   const {return p_.Pt();}
  MAdouble64 Perp() const {return p_.Pt();}

  // Returns the pseudo-rapidity, i.e. -ln(tan(theta/2))
  MAdouble64 PseudoRapidity() const {return p_.PseudoRapidity();}
  MAdouble64 Eta()            const {return p_.PseudoRapidity();}
  

  // Specific sophisticated accessors ------------------------------
  
  // Invariant mass squared.
  MAdouble64 Mag2() const
  { return T()*T() - p_.Mag2(); }

  inline MAdouble64 Mag() const
  {
    MAdouble64 mm = Mag2();
    return mm < 0.0 ? - std::sqrt(-mm) : std::sqrt(mm);
  }
  
  inline MAdouble64 M2() const
  { return Mag2(); }
    
  inline MAdouble64 M() const
  { return Mag(); }
    
  inline MAdouble64 Mt2() const
  { return E()*E() - Z()*Z(); }
    
  inline MAdouble64 Mt() const
  {
    MAdouble64 mm = Mt2();
    return mm < 0.0 ? - std::sqrt(-mm) : std::sqrt(mm);
  }
    
  inline MAdouble64 Et2() const
  {
    MAdouble64 pt2 = p_.Perp2();
    return pt2 == 0 ? 0 : E()*E() * pt2/(pt2+Z()*Z());
  }

  inline MAdouble64 Et() const
  {
    MAdouble64 etet = Et2();
    return e_ < 0.0 ? -std::sqrt(etet) : std::sqrt(etet);
  }

  inline MAdouble64 Beta() const
  { return p_.Mag()/e_; }
    
  inline MAdouble64 Gamma() const
  {
    MAdouble64 b = Beta();
    return 1.0/std::sqrt(1- b*b);
  }
    
  inline MAdouble64 Rapidity() const
  {
    return 0.5 * std::log ( (E()+Pz()) / (E()-Pz()) );
  }    

  
  // Sophisticated mutators ---------------------------------------

  inline void SetVectMag(const MAVector3& spatial, MAdouble64 magnitude);
  inline void SetVectM  (const MAVector3& spatial, MAdouble64 mass);
  inline void SetVectE  (const MAVector3& spatial, MAdouble64 energy);

   // Set all components
  void SetPtEtaPhiM   (MAdouble64 pt, MAdouble64 eta, MAdouble64 phi, MAdouble64 m)
  { SetXYZM(pt*std::cos(phi), pt*std::sin(phi), pt*std::sinh(eta) ,m); }
  
  void SetPtEtaPhiE(MAdouble64 pt, MAdouble64 eta, MAdouble64 phi, MAdouble64 e)
  {p_.SetPtEtaPhi(pt,eta,phi); e_=e;}

   
  // Operations with another  -----------------------------------

  // DeltaPhi between 2 vectors
  inline MAdouble64 DeltaPhi(const MALorentzVector& q) const
  { return p_.DeltaPhi(q.Vect()); }

  // DeltaR between 2 vectors
  MAdouble64 DeltaR (const MALorentzVector& q) const
  { return p_.DeltaR(q.Vect()); }
  
  // The angle w.r.t. another 3-vector.
  MAdouble64 Angle(const MALorentzVector& q) const
  { return p_.Angle(q.Vect()); }

  // Scalar product.
  MAdouble64 Dot(const MALorentzVector& q) const
  { return T()*q.T() - Z()*q.Z() - Y()*q.Y() - X()*q.X(); }

  
  // Operators -----------------------------------------
   
   inline MALorentzVector & operator = (const MALorentzVector &);
   // Assignment.

   inline MALorentzVector   operator +  (const MALorentzVector &) const;
   inline MALorentzVector & operator += (const MALorentzVector &);
   // Additions.

   inline MALorentzVector   operator -  (const MALorentzVector &) const;
   inline MALorentzVector & operator -= (const MALorentzVector &);
   // Subtractions.

   inline MALorentzVector operator - () const;
   // Unary minus.

   inline MALorentzVector operator * (MAdouble64 a) const;
   inline MALorentzVector & operator *= (MAdouble64 a);
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


inline MALorentzVector operator * (MAdouble64 a, const MALorentzVector& p)
{ return MALorentzVector(a*p.X(), a*p.Y(), a*p.Z(), a*p.T()); }
 
}

#endif
