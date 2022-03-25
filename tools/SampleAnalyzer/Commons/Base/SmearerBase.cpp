////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


// STL headers
#include <algorithm>
#include <cmath>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"
#include "SampleAnalyzer/Commons/Vector/MARotation3axis.h"


using namespace MA5;

//================================================//
//   Setting the LLP particle properties in the   //
//   general case (neutral and charged particles, //
//   zero or non-zero magnetic field).            //
//================================================//
void SmearerBase::ParticlePropagator(MCParticleFormat * part)
{
  // Rotation undergone by the mother's momentum
  //  --> to be applied to the daughter
  MAdouble64 RotAngle = part->mothers()[0]->momentum_rotation();
  if (RotAngle!=0.)
  {
    MARotation3axis rotator(RotAngle,MARotation3axis::Zaxis);
    rotator.rotate(part->momentum());
  }

  // Propagation time (times c)
  // For a FS particle, ctau = 0. We are thus setting it to a very large number
  // Note that this large number should be the distance between the particle's
  // radial position and the border of the tracker cylinder. But to keep things
  // simple we eliminated the use of detector boundaries.
  MAdouble64 prop_ctime = PHYSICS->Id->IsFinalState(part) ? 1e10 : (part->ctau() - part->mothers()[0]->ctau());

  // Setting d0/dz in the approximate straight line limit
  // Exact for neutral particles, particles at rest or vanishing magnetic field
  MAdouble64 xd = part->mothers()[0]->decay_vertex().X() - part->px()/part->pt() * ( part->mothers()[0]->decay_vertex().X()*part->px() + part->mothers()[0]->decay_vertex().Y()*part->py() ) / part->pt();
  MAdouble64 yd = part->mothers()[0]->decay_vertex().Y() - part->py()/part->pt() * ( part->mothers()[0]->decay_vertex().X()*part->px() + part->mothers()[0]->decay_vertex().Y()*part->py() ) / part->pt();
  MAdouble64 zd = part->mothers()[0]->decay_vertex().Z() - part->pz()/part->pt() * ( part->mothers()[0]->decay_vertex().X()*part->px() + part->mothers()[0]->decay_vertex().Y()*part->py() ) / part->pt();
  part->setClosestApproach(MAVector3(xd,yd,zd));
  part->setD0( (part->mothers()[0]->decay_vertex().X() * part->py() - part->mothers()[0]->decay_vertex().Y() * part->px()) / part->pt());
  part->setDZ(zd);
  part->setDZApprox(part->dz());
  part->setD0Approx(part->d0());

  // Neutral particles, particles at rest or vanishing magnetic field
  if (std::abs(PDG->GetCharge(part->pdgid())) < 1e-9 || std::fabs(Bz()) < 1e-9 || part->pt() < 1e-9)
  {
    // Updating the rotation angle of the momentum
    part->setMomentumRotation(RotAngle);

    // Straight propagation of the particle to its new decay vertex
    if (!PHYSICS->Id->IsFinalState(part))
      part->setDecayVertex(MALorentzVector(
        part->mothers()[0]->decay_vertex().X() + part->px()/part->e() * prop_ctime,
        part->mothers()[0]->decay_vertex().Y() + part->py()/part->e() * prop_ctime,
        part->mothers()[0]->decay_vertex().Z() + part->pz()/part->e() * prop_ctime,
        part->ctau()));
    return;
  }

  // Charged particle evolving in a magnetic field
  // Definition of the helix centre and radius [mm]
  MAdouble64 R       = part->pt()/( (PDG->GetCharge(part->pdgid())/3.)*Bz())*1.0e+12/c_;
  MAdouble64 x_helix = part->mothers()[0]->decay_vertex().X() + R * sin(part->phi());
  MAdouble64 y_helix = part->mothers()[0]->decay_vertex().Y() - R * cos(part->phi());
  MAdouble64 r_helix = hypot(x_helix,y_helix);

  // Computation of the closest approach
  if (r_helix>0.)
  {
    xd = x_helix * (1 - std::abs(R)/r_helix);
    yd = y_helix * (1 - std::abs(R)/r_helix);
    zd = part->mothers()[0]->decay_vertex().Z()
     + part->pz()/part->pt() * R * atan2(-copysign(1.0,R)*(part->px()*x_helix+part->py()*y_helix),
                                         -copysign(1.0,R)*(part->px()*y_helix-part->py()*x_helix));

    part->setClosestApproach(MAVector3(xd,yd,zd));
    part->setD0(copysign(1.0, R)*(r_helix-std::abs(R)));
    part->setDZ(zd);
  }

  // Updating the decay vertex information for non-final-state particles
  if (!PHYSICS->Id->IsFinalState(part))
  {
    // Properties of the propagated momentum in the transverse plane
    MAdouble64 omegat = prop_ctime * PDG->GetCharge(part->pdgid())/3. * Bz()/part->e()*1.0e-12*c_;
    MAdouble64 px_new = part->px() * cos(omegat) + part->py() * sin(omegat);
    MAdouble64 py_new = part->py() * cos(omegat) - part->px() * sin(omegat);
    RotAngle += atan2(py_new, px_new) - part->phi() + ((RotAngle < 0.) - (RotAngle >= 2.*pi_)) * 2.*pi_;

    // Updating the rotation angle of the momentum
    part->setMomentumRotation(RotAngle);

    // Calculating the new decay vertex position
    if (prop_ctime == 0.)
    {
        part->setDecayVertex(part->decay_vertex());
    } else
    {
        part->setDecayVertex(MALorentzVector(
          x_helix + R/part->pt() * (part->px()*sin(omegat) - part->py()*cos(omegat)),
          y_helix + R/part->pt() * (part->py()*sin(omegat) + part->px()*cos(omegat)),
          part->mothers()[0]->decay_vertex().Z() + part->pz()/part->e()*prop_ctime,
          part->ctau()));
    }
  }
}


//================================================//
//   Setting the LLP particle properties when B=0 //
//   or for neutral particles                     //
//================================================//
void SmearerBase::SetDisplacementObservables(const MCParticleFormat* part, MCParticleFormat &output)
{
    // Point of closest approach
    MAdouble64 xd = part->mothers()[0]->decay_vertex().X() - part->px()/part->pt() * ( part->mothers()[0]->decay_vertex().X()*part->px() + part->mothers()[0]->decay_vertex().Y()*part->py() ) / part->pt();
    MAdouble64 yd = part->mothers()[0]->decay_vertex().Y() - part->py()/part->pt() * ( part->mothers()[0]->decay_vertex().X()*part->px() + part->mothers()[0]->decay_vertex().Y()*part->py() ) / part->pt();
    MAdouble64 zd = part->mothers()[0]->decay_vertex().Z() - part->pz()/part->pt() * ( part->mothers()[0]->decay_vertex().X()*part->px() + part->mothers()[0]->decay_vertex().Y()*part->py() ) / part->pt();

    // Determine d0, dz and closest point
    output.setClosestApproach(MAVector3(xd,yd,zd));

    output.setD0( (part->mothers()[0]->decay_vertex().X() * part->py() - part->mothers()[0]->decay_vertex().Y() * part->px()) / part->pt());
    output.setDZ(zd);

    // Approximated d0/dz (that are the same here)
    output.setDZApprox(output.dz());
    output.setD0Approx(output.d0());
}


/// Smearer Gaussian function 
/// (one smears the quantity 'property' with a Gaussian of variance 'sigma')
///
/// REFERENCE:  - W. Hoermann and G. Derflinger (1990):
///              The ACR Method for generating normal random variables,
///              OR Spektrum 12 (1990), 181-185.
///
/// Implementation taken from
/// UNURAN (c) 2000  W. Hoermann & J. Leydold, Institut f. Statistik, WU Wien
/// Taken from ROOT: https://root.cern.ch/doc/master/TRandom_8cxx_source.html#l00263
MAdouble64 SmearerBase::Gaussian(MAdouble64 sigma, MAdouble64 property)
{
  MAdouble64 kC1   = 1.448242853;        MAdouble64 kAs   = 0.8853395638;
  MAdouble64 kC2   = 3.307147487;        MAdouble64 kBs   = 0.2452635696;
  MAdouble64 kC3   = 1.46754004;         MAdouble64 kCs   = 0.2770276848;
  MAdouble64 kD1   = 1.036467755;        MAdouble64 kB    = 0.5029324303;
  MAdouble64 kD2   = 5.295844968;        MAdouble64 kX0   = 0.4571828819;
  MAdouble64 kD3   = 3.631288474;        MAdouble64 kYm   = 0.187308492 ;
  MAdouble64 kHm   = 0.483941449;        MAdouble64 kS    = 0.7270572718 ;
  MAdouble64 kZm   = 0.107981933;        MAdouble64 kT    = 0.03895759111;
  MAdouble64 kHp   = 4.132731354;        MAdouble64 kHp1  = 3.132731354;
  MAdouble64 kZp   = 18.52161694;        MAdouble64 kHzm  = 0.375959516;
  MAdouble64 kPhln = 0.4515827053;       MAdouble64 kHzmp = 0.591923442;
  MAdouble64 kHm1  = 0.516058551;

  MAdouble64 result, rn,x,y,z;
  do
  {
    y = RANDOM->flat();
    if (y>kHm1) { result = kHp*y-kHp1; break; }
    else if (y<kZm) { rn = kZp*y-1; result = (rn>0) ? (1+rn) : (-1+rn); break; }
    else if (y<kHm)
    {
      rn = RANDOM->flat();
      rn = rn-1+rn;
      z = (rn>0) ? 2-rn : -2-rn;
      if ((kC1-y)*(kC3+std::fabs(z))<kC2) { result = z; break; }
      else
      {
        x = rn*rn;
        if ((y+kD1)*(kD3+x)<kD2) { result = rn; break; }
        else if (kHzmp-y<exp(-(z*z+kPhln)/2)) { result = z; break; }
        else if (y+kHzm<exp(-(x+kPhln)/2)) { result = rn; break; }
      }
    }
    while (1)
    {
      x = RANDOM->flat();
      y = kYm * RANDOM->flat();
      z = kX0 - kS*x - y;
      if (z>0) rn = 2+y/x;
      else { x = 1-x;y = kYm-y;rn = -(2+y/x); }
      if ((y-kAs+x)*(kCs+x)+kBs<0) { result = rn; break; }
      else if (y<x+kT)
        if (rn*rn<4*(kB-log(x))) { result = rn; break; }
    }
  } while(0);
  return property + sigma * result;
}
