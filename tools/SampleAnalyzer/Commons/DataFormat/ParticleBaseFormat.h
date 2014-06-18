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


#ifndef ParticleBaseFormat_h
#define ParticleBaseFormat_h

// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// ROOT headers
#include <TLorentzVector.h>

// SampleAnalyzer
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class STDHEPreader;
class HEPMCReader;
class ROOTReader;

class ParticleBaseFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class STDHEPreader;
  friend class HEPMCReader;
  friend class ROOTReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
   
  /// Quadrivector of particle (E, px,py,pz)
  TLorentzVector 	momentum_;  


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument
  ParticleBaseFormat()
  { }

  /// Constructor without argument
  ParticleBaseFormat(Double_t px, Double_t py, Double_t pz, Double_t e)
  { momentum_.SetPxPyPzE(px,py,pz,e); }

  /// Constructor without argument
  ParticleBaseFormat(const TLorentzVector& p)
  { momentum_.SetPxPyPzE(p.Px(),p.Py(),p.Pz(),p.E()); }

  /// Destructor
  virtual ~ParticleBaseFormat()
  { }

  /// Clear all information
  virtual void Reset()
  {
    momentum_.SetPxPyPzE(0.,0.,0.,0.);
  }

  /// Print particle informations
  virtual void Print() const
  {
    INFO << "momentum=(";
//    INFO << std::setw(8);
    INFO << std::left << momentum_.Px()
         << ", "<</*std::setw(8)*/"" << std::left << momentum_.Py()  
         << ", "<</*std::setw(8)*/"" << std::left << momentum_.Pz() 
         << ", "<</*std::setw(8)*/"" << std::left << momentum_.E() << ") - ";
  }
			
  /// Accessor to 4-vector momentum (read-only)
  const TLorentzVector& momentum() const {return momentum_;}

  /// Accessor to 4-vector momentum
  TLorentzVector& momentum() {return momentum_;}

  /// Set the 4-vector momentum
  void setMomentum(const TLorentzVector& v) {momentum_=v;}

  /// Accessor to the particle energy
  const Float_t e()       const {return momentum_.E();       }

  /// Accessor to the particle mass
  const Float_t m()       const {return momentum_.M();       }

  /// Accessor to the particle momentum magnitude
  const Float_t p()       const {return momentum_.P();       }

  /// Accessor to the particle transverse mass
  // WARNING: ROOT native formula is not the good one
  const Float_t mt()      const 
  { 
    //    return momentum_.Mt();
    Float_t tmp = momentum_.Et2() - momentum_.Pt()*momentum_.Pt();
    if (tmp<0) return 0.; else return sqrt(tmp);
  }

  const Float_t mt_met(const TLorentzVector& MET) const 
  { 
    // Computing ET sum
    double ETsum = sqrt( momentum_.M()*momentum_.M() +
                         momentum_.Pt()*momentum_.Pt() )  + MET.Pt();

    // Computing PT sum
    TLorentzVector pt = momentum_ + MET;

    double value = ETsum*ETsum - pt.Pt()*pt.Pt();
    if (value<0) return 0;
    else return sqrt(value);
  }

  /// Accessor to the particle transverse energy
  const Float_t et()      const {return momentum_.Et();      }

  /// Accessor to the particle transverse momentum magnitude
  const Float_t pt()      const {return momentum_.Perp();    }

  /// Accessor to the particle x-component momentum
  const Float_t px()      const {return momentum_.Px();      }

  /// Accessor to the particle y-component momentum
  const Float_t py()      const {return momentum_.Py();      }

  /// Accessor to the particle z-component momentum
  const Float_t pz()      const {return momentum_.Pz();      }

  /// Accessor to the particle pseudo-rapidity
  const Float_t eta()     const {return momentum_.Eta();     }

  /// Accessor to the particle polar angle
  const Float_t theta()   const {return momentum_.Theta();   }

  /// Accessor to the particle azimuthal angle
  const Float_t phi()     const {return momentum_.Phi();     }

  /// Accessor to the delta Phi (given in [0, pi] with another particle direction
  const Float_t dphi_0_pi(const ParticleBaseFormat* p) const 
  {
    double dphi = fabs(momentum_.Phi() - p->momentum().Phi());
    if(dphi>3.14159265) dphi=2.*3.14159265-dphi;
    return dphi;
  }

  /// Accessor to the delta Phi (given in [0, pi] with another particle direction
  const Float_t dphi_0_pi(const ParticleBaseFormat& p) const 
  {
    double dphi = fabs(momentum_.Phi() - p.momentum().Phi());
    if(dphi>3.14159265) dphi=2.*3.14159265-dphi;
    return dphi;
  }

  /// Accessor to the delta Phi (given in [0, 2pi] with another particle direction
  const Float_t dphi_0_2pi(const ParticleBaseFormat* p) const 
  {
    double dphi =momentum_.Phi() - p->momentum().Phi();
    if(dphi<0.) dphi+=2.*3.14159265;
    return dphi;
  }

  /// Accessor to the delta Phi (given in [0, 2pi] with another particle direction
  const Float_t dphi_0_2pi(const ParticleBaseFormat& p) const 
  {
    double dphi = momentum_.Phi() - p.momentum().Phi();
    if(dphi<0.) dphi+=2.*3.14159265;
    return dphi;
  }

  /// Accessor to the particle rapidity
  const Float_t y()       const {return momentum_.Rapidity();}

  /// Accessor to the relativist beta parameter
  const Float_t beta()    const {return momentum_.Beta();    }

  /// Accessor to the relativist gamma parameter
  const Float_t gamma()   const {return momentum_.Gamma();   }

  /// Accessor to the polar radius
  const Float_t r()       const
  { 
    return sqrt(momentum_.Eta()*momentum_.Eta() + \
                momentum_.Phi()*momentum_.Phi() ); 
  }

  /// Accessor to the delta R with another particle direction
  const Float_t dr(const ParticleBaseFormat& p) const 
  { return momentum_.DeltaR(p.momentum()); }

  /// Accessor to the delta R with another particle direction
  const Float_t dr(const ParticleBaseFormat* p) const 
  { return momentum_.DeltaR(p->momentum()); }

  /// Accessor to the angle with another particle direction
  const Float_t angle(const ParticleBaseFormat& p) const 
  { return momentum_.Angle(p.momentum().Vect()); }

  /// Accessor to the angle with another particle direction
  const Float_t angle(const ParticleBaseFormat* p) const 
  { return momentum_.Angle(p->momentum().Vect()); }

  /// operator * (scalar)
  ParticleBaseFormat operator * (Double_t a) const
  { return ParticleBaseFormat(a*momentum_); }

  /// operator * (momentum)
  Double_t operator * (const ParticleBaseFormat& p) const
  { return momentum_.Dot(p.momentum_); }

  /// operator + (momentum)
  ParticleBaseFormat operator + (const ParticleBaseFormat& p) const 
  { return ParticleBaseFormat(momentum_+p.momentum_); }

  /// operator - (momentum)
  ParticleBaseFormat operator - (const ParticleBaseFormat& p) const
  { return ParticleBaseFormat(momentum_-p.momentum_); }

  /// operator + (momentum)
  ParticleBaseFormat operator + (const TLorentzVector& p) const 
  { return ParticleBaseFormat(momentum_+p); }

  /// operator - (momentum)
  ParticleBaseFormat operator - (const TLorentzVector& p) const
  { return ParticleBaseFormat(momentum_-p); }

  /// operator += (momentum)
  ParticleBaseFormat& operator += (const ParticleBaseFormat& p)
  { this->momentum_ += p.momentum_;
    return *this; }

  /// operator -= (momentum)
  ParticleBaseFormat& operator -= (const ParticleBaseFormat& p)
  { this->momentum_ -= p.momentum_;
    return *this; }

};

}

#endif
