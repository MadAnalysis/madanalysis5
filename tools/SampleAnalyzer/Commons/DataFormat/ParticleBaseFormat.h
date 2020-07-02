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


#ifndef ParticleBaseFormat_h
#define ParticleBaseFormat_h


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
  MALorentzVector momentum_;  


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument
  ParticleBaseFormat()
  { }

  /// Constructor without argument
  ParticleBaseFormat(MAfloat64 px, MAfloat64 py, MAfloat64 pz, MAfloat64 e)
  { momentum_.SetPxPyPzE(px,py,pz,e); }

  /// Constructor without argument
  ParticleBaseFormat(const MALorentzVector& p)
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
  const MALorentzVector& momentum() const {return momentum_;}

  /// Accessor to 4-vector momentum
  MALorentzVector& momentum() {return momentum_;}

  /// Set the 4-vector momentum
  void setMomentum(const MALorentzVector& v) {momentum_=v;}

  /// Accessor to the particle energy
  const MAfloat32 e()       const {return momentum_.E();       }

  /// Accessor to the particle mass
  const MAfloat32 m()       const {return momentum_.M();       }

  /// Accessor to the particle momentum magnitude
  const MAfloat32 p()       const {return momentum_.P();       }

  /// Accessor to the particle transverse mass
  // WARNING: ROOT native formula is not the good one
  const MAfloat32 mt()      const 
  { 
    //    return momentum_.Mt();
    MAfloat32 tmp = momentum_.Et2() - momentum_.Pt()*momentum_.Pt();
    if (tmp<0) return 0.; else return sqrt(tmp);
  }

  const MAfloat32 mt_met(const MALorentzVector& MET) const 
  { 
    // Computing ET sum
    MAfloat64 ETsum = sqrt( momentum_.M()*momentum_.M() +
                         momentum_.Pt()*momentum_.Pt() )  + MET.Pt();

    // Computing PT sum
    MALorentzVector pt = momentum_ + MET;

    MAfloat64 value = ETsum*ETsum - pt.Pt()*pt.Pt();
    if (value<0) return 0;
    else return sqrt(value);
  }

  /// Accessor to the particle transverse energy
  const MAfloat32 et()      const {return momentum_.Et();      }

  /// Accessor to the particle transverse momentum magnitude
  const MAfloat32 pt()      const {return momentum_.Perp();    }

  /// Accessor to the particle x-component momentum
  const MAfloat32 px()      const {return momentum_.Px();      }

  /// Accessor to the particle y-component momentum
  const MAfloat32 py()      const {return momentum_.Py();      }

  /// Accessor to the particle z-component momentum
  const MAfloat32 pz()      const {return momentum_.Pz();      }

  /// Accessor to the particle pseudo-rapidity
  const MAfloat32 eta()     const {return momentum_.Eta();     }

  /// Accessor to the particle pseudo-rapidity
  const MAfloat32 abseta()     const {return std::abs(momentum_.Eta());     }

  /// Accessor to the particle polar angle
  const MAfloat32 theta()   const {return momentum_.Theta();   }

  /// Accessor to the particle azimuthal angle
  const MAfloat32 phi()     const {return momentum_.Phi();     }

  /// Accessor to the delta Phi (given in [0, pi] with another particle direction
  const MAfloat32 dphi_0_pi(const ParticleBaseFormat* p) const 
  {
    MAfloat64 dphi = std::abs(momentum_.Phi() - p->momentum().Phi());
    if(dphi>3.14159265) dphi=2.*3.14159265-dphi;
    return dphi;
  }

  /// Accessor to the delta Phi (given in [0, pi] with another particle direction
  const MAfloat32 dphi_0_pi(const ParticleBaseFormat& p) const 
  {
    MAfloat64 dphi = std::abs(momentum_.Phi() - p.momentum().Phi());
    if(dphi>3.14159265) dphi=2.*3.14159265-dphi;
    return dphi;
  }

  /// Accessor to the recoil class (computed wrt another particle)
  const MAfloat32 recoil(const ParticleBaseFormat* p) const
    { return std::abs((momentum_ - p->momentum()).M()); }

  /// Accessor to the recoil class (computed wrt another particle)
  const MAfloat32 recoil(const ParticleBaseFormat& p) const
    { return std::abs((momentum_ - p.momentum()).M()); }

  /// Accessor to the delta Phi (given in [0, 2pi] with another particle direction
  const MAfloat32 dphi_0_2pi(const ParticleBaseFormat* p) const 
  {
    MAfloat64 dphi =momentum_.Phi() - p->momentum().Phi();
    if(dphi<0.) dphi+=2.*3.14159265;
    return dphi;
  }

  /// Accessor to the delta Phi (given in [0, 2pi] with another particle direction
  const MAfloat32 dphi_0_2pi(const ParticleBaseFormat& p) const 
  {
    MAfloat64 dphi = momentum_.Phi() - p.momentum().Phi();
    if(dphi<0.) dphi+=2.*3.14159265;
    return dphi;
  }

  /// Accessor to the particle rapidity
  const MAfloat32 y()       const {return momentum_.Rapidity();}

  /// Accessor to the relativist beta parameter
  const MAfloat32 beta()    const {return momentum_.Beta();    }

  /// Accessor to the relativist gamma parameter
  const MAfloat32 gamma()   const {return momentum_.Gamma();   }

  /// Accessor to the polar radius
  const MAfloat32 r()       const
  { 
    return sqrt(momentum_.Eta()*momentum_.Eta() + \
                momentum_.Phi()*momentum_.Phi() ); 
  }

  /// Accessor to the delta R with another particle direction
  const MAfloat32 dr(const ParticleBaseFormat& p) const 
  { return momentum_.DeltaR(p.momentum()); }

  /// Accessor to the delta R with another particle direction
  const MAfloat32 dr(const ParticleBaseFormat* p) const 
  { return momentum_.DeltaR(p->momentum()); }

  /// Accessor to the angle with another particle direction
  const MAfloat32 angle(const ParticleBaseFormat& p) const 
  { return momentum_.Angle(p.momentum()); }

  /// Accessor to the angle with another particle direction
  const MAfloat32 angle(const ParticleBaseFormat* p) const 
  { return momentum_.Angle(p->momentum()); }

  /// operator * (scalar)
  ParticleBaseFormat operator * (MAfloat64 a) const
  { return ParticleBaseFormat(a*momentum_); }

  /// operator * (momentum)
  MAfloat64 operator * (const ParticleBaseFormat& p) const
  { return momentum_.Dot(p.momentum_); }

  /// operator + (momentum)
  ParticleBaseFormat operator + (const ParticleBaseFormat& p) const 
  { return ParticleBaseFormat(momentum_+p.momentum_); }

  /// operator - (momentum)
  ParticleBaseFormat operator - (const ParticleBaseFormat& p) const
  { return ParticleBaseFormat(momentum_-p.momentum_); }

  /// operator + (momentum)
  ParticleBaseFormat operator + (const MALorentzVector& p) const 
  { return ParticleBaseFormat(momentum_+p); }

  /// operator - (momentum)
  ParticleBaseFormat operator - (const MALorentzVector& p) const
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
