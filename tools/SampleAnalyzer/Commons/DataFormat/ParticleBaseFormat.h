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
 * @file ParticleBaseFormat.h
 * @brief Base class of all particle and object formats (four-momentum and displacement).
 */

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

/**
 * @brief Base class of the Monte Carlo particles and reconstructed objects.
 *
 * It stores the four-momentum and the displacement observables (point of closest
 * approach to the beam axis, transverse and longitudinal impact parameters d0 and
 * dz, exact and in the straight-line approximation), and provides the usual
 * kinematic accessors. Lengths are in mm.
 */
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
   
  /** @brief Four-momentum (px, py, pz, E). */
  MALorentzVector momentum_;  

  /** @brief Point of closest approach to the beam axis, impact parameters d0/dz and their straight-line approximations. */
  MAVector3 closest_approach_;
  MAdouble64 d0_;
  MAdouble64 dz_;
  MAdouble64 d0_approx_;
  MAdouble64 dz_approx_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /** @brief Constructor without argument (the members are not reset). */
  ParticleBaseFormat()
  // FIXME: d0_, dz_, d0_approx_ and dz_approx_ are left uninitialised by this constructor.
  { }

  /**
   * @brief Constructor from the momentum components.
   *
   * @param px x component.
   * @param py y component.
   * @param pz z component.
   * @param e energy.
   */
  ParticleBaseFormat(MAfloat64 px, MAfloat64 py, MAfloat64 pz, MAfloat64 e)
  { Reset(); momentum_.SetPxPyPzE(px,py,pz,e); }

  /**
   * @brief Constructor from a four-vector.
   *
   * @param p four-momentum.
   */
  ParticleBaseFormat(const MALorentzVector& p)
  { Reset(); momentum_.SetPxPyPzE(p.Px(),p.Py(),p.Pz(),p.E()); }

  /** @brief Destructor. */
  virtual ~ParticleBaseFormat()
  { }

  /** @brief Reset the momentum and the displacement observables. */
  virtual void Reset()
  {
    momentum_.SetPxPyPzE(0.,0.,0.,0.);
    closest_approach_.SetXYZ(0.,0.,0.);
    d0_        = 0.; dz_        = 0.;
    d0_approx_ = 0.; dz_approx_ = 0.;
  }

  /** @brief Print the four-momentum. */
  virtual void Print() const
  {
    INFO << "momentum=(";
//    INFO << std::setw(8);
    INFO << std::left << momentum_.Px()
         << ", "<</*std::setw(8)*/"" << std::left << momentum_.Py()  
         << ", "<</*std::setw(8)*/"" << std::left << momentum_.Pz() 
         << ", "<</*std::setw(8)*/"" << std::left << momentum_.E() << ") - ";
  }

  /**
   * @brief Accessor to the four-momentum (read-only).
   *
   * @return the four-momentum.
   */
  const MALorentzVector& momentum() const {return momentum_;}

  /**
   * @brief Accessor to the four-momentum.
   *
   * @return the four-momentum (modifiable).
   */
  MALorentzVector& momentum() {return momentum_;}

  /**
   * @brief Set the four-momentum.
   *
   * @param v four-momentum.
   */
  void setMomentum(const MALorentzVector& v) {momentum_=v;}

  /**
   * @brief Accessor to the energy.
   *
   * @return the energy.
   */
  const MAfloat32 e()       const {return momentum_.E();       }

  /**
   * @brief Accessor to the invariant mass.
   *
   * @return the invariant mass.
   */
  const MAfloat32 m()       const {return momentum_.M();       }

  /**
   * @brief Accessor to the magnitude of the momentum.
   *
   * @return the magnitude of the momentum.
   */
  const MAfloat32 p()       const {return momentum_.P();       }

  /// Accessor to the particle transverse mass
  /**
   * @brief Accessor to the transverse mass, computed as sqrt(Et^2 - pT^2) (0 if negative).
   *
   * @return the transverse mass.
   */
  const MAfloat32 mt()      const 
  { 
    //    return momentum_.Mt();
    MAfloat32 tmp = momentum_.Et2() - momentum_.Pt()*momentum_.Pt();
    if (tmp<0) return 0.; else return sqrt(tmp);
  }

  /**
   * @brief Transverse mass of the system made of the particle and the missing transverse momentum.
   *
   * @param MET missing transverse momentum.
   * @return the transverse mass (0 if the squared mass is negative).
   */
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

  /**
   * @brief Accessor to the transverse energy.
   *
   * @return the transverse energy.
   */
  const MAfloat32 et()      const {return momentum_.Et();      }

  /**
   * @brief Accessor to the transverse momentum.
   *
   * @return the transverse momentum.
   */
  const MAfloat32 pt()      const {return momentum_.Perp();    }

  /**
   * @brief Accessor to the x component of the momentum.
   *
   * @return the x component of the momentum.
   */
  const MAfloat32 px()      const {return momentum_.Px();      }

  /**
   * @brief Accessor to the y component of the momentum.
   *
   * @return the y component of the momentum.
   */
  const MAfloat32 py()      const {return momentum_.Py();      }

  /**
   * @brief Accessor to the z component of the momentum.
   *
   * @return the z component of the momentum.
   */
  const MAfloat32 pz()      const {return momentum_.Pz();      }

  /**
   * @brief Accessor to the pseudorapidity.
   *
   * @return the pseudorapidity.
   */
  const MAfloat32 eta()     const {return momentum_.Eta();     }

  /**
   * @brief Accessor to the absolute value of the pseudorapidity.
   *
   * @return the absolute value of the pseudorapidity.
   */
  const MAfloat32 abseta()     const {return std::abs(momentum_.Eta());     }

  /**
   * @brief Accessor to the polar angle.
   *
   * @return the polar angle.
   */
  const MAfloat32 theta()   const {return momentum_.Theta();   }

  /**
   * @brief Accessor to the azimuthal angle.
   *
   * @return the azimuthal angle.
   */
  const MAfloat32 phi()     const {return momentum_.Phi();     }

  /**
   * @brief Azimuthal angle difference with another particle, in [0, pi].
   *
   * @param p other particle.
   * @return the angle difference.
   */
  const MAfloat32 dphi_0_pi(const ParticleBaseFormat* p) const 
  {
    MAfloat64 dphi = std::abs(momentum_.Phi() - p->momentum().Phi());
    if(dphi>3.14159265) dphi=2.*3.14159265-dphi;
    return dphi;
  }

  /**
   * @brief Azimuthal angle difference with another particle, in [0, pi].
   *
   * @param p other particle.
   * @return the angle difference.
   */
  const MAfloat32 dphi_0_pi(const ParticleBaseFormat& p) const 
  {
    MAfloat64 dphi = std::abs(momentum_.Phi() - p.momentum().Phi());
    if(dphi>3.14159265) dphi=2.*3.14159265-dphi;
    return dphi;
  }

  /**
   * @brief Recoil mass with respect to another particle, |M(p_this - p_other)|.
   *
   * @param p other particle.
   * @return the recoil mass.
   */
  const MAfloat32 recoil(const ParticleBaseFormat* p) const
    { return std::abs((momentum_ - p->momentum()).M()); }

  /**
   * @brief Recoil mass with respect to another particle, |M(p_this - p_other)|.
   *
   * @param p other particle.
   * @return the recoil mass.
   */
  const MAfloat32 recoil(const ParticleBaseFormat& p) const
    { return std::abs((momentum_ - p.momentum()).M()); }

  /**
   * @brief Azimuthal angle difference with another particle, in [0, 2pi].
   *
   * @param p other particle.
   * @return the angle difference.
   */
  const MAfloat32 dphi_0_2pi(const ParticleBaseFormat* p) const 
  {
    MAfloat64 dphi =momentum_.Phi() - p->momentum().Phi();
    if(dphi<0.) dphi+=2.*3.14159265;
    return dphi;
  }

  /**
   * @brief Azimuthal angle difference with another particle, in [0, 2pi].
   *
   * @param p other particle.
   * @return the angle difference.
   */
  const MAfloat32 dphi_0_2pi(const ParticleBaseFormat& p) const 
  {
    MAfloat64 dphi = momentum_.Phi() - p.momentum().Phi();
    if(dphi<0.) dphi+=2.*3.14159265;
    return dphi;
  }

  /**
   * @brief Accessor to the rapidity.
   *
   * @return the rapidity.
   */
  const MAfloat32 y()       const {return momentum_.Rapidity();}

  /**
   * @brief Accessor to the velocity beta = p/E.
   *
   * @return the velocity beta = p/E.
   */
  const MAfloat32 beta()    const {return momentum_.Beta();    }

  /**
   * @brief Accessor to the Lorentz factor gamma.
   *
   * @return the Lorentz factor gamma.
   */
  const MAfloat32 gamma()   const {return momentum_.Gamma();   }

  /**
   * @brief Accessor to sqrt(eta^2 + phi^2) (distance to the origin of the (eta, phi) plane).
   *
   * @return the value.
   */
  const MAfloat32 r()       const
  { 
    return sqrt(momentum_.Eta()*momentum_.Eta() + \
                momentum_.Phi()*momentum_.Phi() ); 
  }

  /**
   * @brief Angular distance DeltaR = sqrt(Deta^2 + Dphi^2) with another particle.
   *
   * @param p other particle.
   * @return DeltaR.
   */
  const MAfloat32 dr(const ParticleBaseFormat& p) const 
  { return momentum_.DeltaR(p.momentum()); }

  /**
   * @brief Angular distance DeltaR = sqrt(Deta^2 + Dphi^2) with another particle.
   *
   * @param p other particle.
   * @return DeltaR.
   */
  const MAfloat32 dr(const ParticleBaseFormat* p) const 
  { return momentum_.DeltaR(p->momentum()); }

  /**
   * @brief Angle between the momenta of two particles.
   *
   * @param p other particle.
   * @return the angle in radians.
   */
  const MAfloat32 angle(const ParticleBaseFormat& p) const 
  { return momentum_.Angle(p.momentum()); }

  /**
   * @brief Angle between the momenta of two particles.
   *
   * @param p other particle.
   * @return the angle in radians.
   */
  const MAfloat32 angle(const ParticleBaseFormat* p) const 
  { return momentum_.Angle(p->momentum()); }

  /**
   * @brief Scale the four-momentum.
   *
   * @param a scale factor.
   * @return a particle with the scaled four-momentum.
   */
  ParticleBaseFormat operator * (MAfloat64 a) const
  { return ParticleBaseFormat(a*momentum_); }

  /**
   * @brief Minkowski scalar product of two four-momenta.
   *
   * @param p other particle.
   * @return the scalar product.
   */
  MAfloat64 operator * (const ParticleBaseFormat& p) const
  { return momentum_.Dot(p.momentum_); }

  /**
   * @brief Sum of two four-momenta.
   *
   * @param p other particle.
   * @return a particle with the summed four-momentum.
   */
  ParticleBaseFormat operator + (const ParticleBaseFormat& p) const 
  { return ParticleBaseFormat(momentum_+p.momentum_); }

  /**
   * @brief Difference of two four-momenta.
   *
   * @param p other particle.
   * @return a particle with the difference.
   */
  ParticleBaseFormat operator - (const ParticleBaseFormat& p) const
  { return ParticleBaseFormat(momentum_-p.momentum_); }

  /**
   * @brief Add a four-vector.
   *
   * @param p four-vector.
   * @return a particle with the summed four-momentum.
   */
  ParticleBaseFormat operator + (const MALorentzVector& p) const 
  { return ParticleBaseFormat(momentum_+p); }

  /**
   * @brief Subtract a four-vector.
   *
   * @param p four-vector.
   * @return a particle with the difference.
   */
  ParticleBaseFormat operator - (const MALorentzVector& p) const
  { return ParticleBaseFormat(momentum_-p); }

  /**
   * @brief Add the four-momentum of another particle.
   *
   * @param p other particle.
   * @return this particle.
   */
  ParticleBaseFormat& operator += (const ParticleBaseFormat& p)
  { this->momentum_ += p.momentum_;
    return *this; }

  /**
   * @brief Subtract the four-momentum of another particle.
   *
   * @param p other particle.
   * @return this particle.
   */
  ParticleBaseFormat& operator -= (const ParticleBaseFormat& p)
  { this->momentum_ -= p.momentum_;
    return *this; }

  /**
   * @brief Accessor to the point of closest approach to the beam axis.
   *
   * @return the point [mm].
   */
  const MAVector3& closest_approach() const {return closest_approach_;}
  /**
   * @brief Accessor to the transverse impact parameter.
   *
   * @return d0 [mm].
   */
  const MAdouble64& d0() const {return d0_;}
  /**
   * @brief Accessor to the longitudinal impact parameter.
   *
   * @return dz [mm].
   */
  const MAdouble64& dz() const {return dz_;}

  /**
   * @brief Accessor to d0 in the straight-line approximation.
   *
   * @return d0 [mm].
   */
  const MAdouble64& d0_approx() const {return d0_approx_;}
  /**
   * @brief Accessor to dz in the straight-line approximation.
   *
   * @return dz [mm].
   */
  const MAdouble64& dz_approx() const {return dz_approx_;}

  /**
   * @brief Set d0.
   *
   * @param v value [mm].
   */
  void setD0(MAdouble64 v)        {d0_=v;}
  /**
   * @brief Set dz.
   *
   * @param v value [mm].
   */
  void setDZ(MAdouble64 v)        {dz_=v;}
  /**
   * @brief Set d0 in the straight-line approximation.
   *
   * @param v value [mm].
   */
  void setD0Approx(MAdouble64 v)  {d0_approx_=v;}
  /**
   * @brief Set dz in the straight-line approximation.
   *
   * @param v value [mm].
   */
  void setDZApprox(MAdouble64 v)  {dz_approx_=v;}
  /**
   * @brief Set the point of closest approach.
   *
   * @param v point [mm].
   */
  void setClosestApproach(const MAVector3& v)  {closest_approach_=v;}



};

}

#endif
