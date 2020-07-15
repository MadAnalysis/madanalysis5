////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef SMEARERBASE_H
#define SMEARERBASE_H


// STL headers
#include <algorithm>
#include <cmath>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Commons/Service/RandomService.h"


namespace MA5
{

    class SmearerBase
    {
        //---------------------------------------------------------------------------------
        //                              private data members
        //---------------------------------------------------------------------------------
        private:
            // Set constants: speed of light & pi
            MAdouble64 c_;
            MAdouble64 pi_;

        //---------------------------------------------------------------------------------
        //                              public data members
        //---------------------------------------------------------------------------------
        public:

            // Creating a container for the smeared output
            MCParticleFormat output_;

            // Magnetic field along beam axis
            MAdouble64 Bz_;

            // Tracker cylinder radius
            MAdouble64 Radius_;

            // Tracker half length
            MAdouble64 HalfLength_;

            // To optimise the code running time
            MAbool MuonSmearer_;
            MAbool ElectronSmearer_;
            MAbool PhotonSmearer_;
            MAbool TauSmearer_;
            MAbool JetSmearer_;
            MAbool ParticlePropagator_;

            //---------------------------------------------------------------------------------
            //                                method members
            //---------------------------------------------------------------------------------
            /// Constructor without argument
            SmearerBase() { }

            /// Destructor
            virtual ~SmearerBase() {}

            /// Initialisation
            void Initialize(MAbool base=false)
            {
                if (!base) { PrintHeader(); }
                SetParameters();
                PrintDebug();
                output_.Reset();
                c_  = 2.99792458E+8; // [m/s]
                pi_ = 3.141592653589793;
            }

            /// Matching general method
            MCParticleFormat Execute(const MCParticleFormat * part, MAuint32 absid)
            {
                // Clearing the output vector
                output_.Reset();

                // Propagation
                MCParticleFormat propagated_part;
                if (isPropagatorOn() && ((absid==13)||(absid==11))) propagated_part = ParticlePropagator(part);
                else SetDefaultOutput(part, propagated_part);

                if      (absid == 21) output_ = JetSmearer(         dynamic_cast<const MCParticleFormat*>(&propagated_part));
                else if (absid == 15) output_ = TauSmearer(         dynamic_cast<const MCParticleFormat*>(&propagated_part));
                else if (absid == 13) output_ = MuonSmearer(        dynamic_cast<const MCParticleFormat*>(&propagated_part));
                else if (absid == 11) output_ = ElectronSmearer(    dynamic_cast<const MCParticleFormat*>(&propagated_part));
                else if (absid == 22) output_ = PhotonSmearer(      dynamic_cast<const MCParticleFormat*>(&propagated_part));
                else if (absid == 0)  output_ = ConstituentSmearer( dynamic_cast<const MCParticleFormat*>(&propagated_part));
                else
                {
                    WARNING << "Unknown smearing method" << endmsg;
                    WARNING << "Smearing skipped for PDG-ID : "<< absid << endmsg;
                    SetDefaultOutput(part,output_);
                }
                return output_;
            }

            // Copy part to output
            void SetDefaultOutput(const MCParticleFormat * part, MCParticleFormat & output)
            {
                output.Reset();
                output.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());

                if (isPropagatorOn()) // (code-efficiency-related)
                {
                    // Set position of the decay vertex
                    MALorentzVector pos;
                    pos.SetXYZT(part->decay_vertex().X(),
                                part->decay_vertex().Y(),
                                part->decay_vertex().Z(),
                                part->decay_vertex().T());
                    output.setPosition(pos);

                    output.setD0((part->decay_vertex().X() * part->py() - \
                                  part->decay_vertex().Y() * part->px()) / part->pt()); // in [mm]

                    output.setDZ(part->decay_vertex().Z() - (part->decay_vertex().X() * \
                                 part->px() + part->decay_vertex().Y() * part->py()) / part->pt() * sinh(part->eta())); // in [mm]
                }
            }

            // Set parameters
            virtual void SetParameters()
            {
                Bz_                 = 1.0e-9;
                Radius_             = 1.0e-9;
                HalfLength_         = 1.0e-9;
                ParticlePropagator_ = false;
                MuonSmearer_        = false;
                ElectronSmearer_    = false;
                PhotonSmearer_      = false;
                TauSmearer_         = false;
                JetSmearer_         = false;
            }

            // For all methods below, the only relevant part of the output object is the momentum
            // The reset allows to clear the left-over from the previous object

            // Electron smearing method
            virtual MCParticleFormat ElectronSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether electron smearing is on (code-efficiency-related)
            MAbool isElectronSmearerOn() {return ElectronSmearer_;}

            // Muon smearing method
            virtual MCParticleFormat MuonSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether muon smearing is on (code-efficiency-related)
            MAbool isMuonSmearerOn() {return MuonSmearer_;}

            // Hadronic Tau smearing method
            virtual MCParticleFormat TauSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether tau smearing is on (code-efficiency-related)
            MAbool isTauSmearerOn() {return TauSmearer_;}

            // Photon smearing method
            virtual MCParticleFormat PhotonSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }
            // Check whether photon smearing is on (code-efficiency-related)
            MAbool isPhotonSmearerOn() {return PhotonSmearer_;}

            // Jet smearing method
            virtual MCParticleFormat JetSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }

            // Check whether jet smearing is on (code-efficiency-related)
            MAbool isJetSmearerOn() {return JetSmearer_;}

            // Jet Constituent smearing method
            virtual MCParticleFormat ConstituentSmearer(const MCParticleFormat * part)
            {
                SetDefaultOutput(part,output_);
                return output_;
            }

            //================================//
            //   Particle Propagator Method   //
            //================================//

            // Check whether particle propagator is on (code-efficiency-related)
            MAbool isPropagatorOn() {return ParticlePropagator_;}

            // Particle propagator method
            MCParticleFormat ParticlePropagator(const MCParticleFormat * part)
            {
                MCParticleFormat tmp_part;
                tmp_part.Reset();
                SetDefaultOutput(part,tmp_part);
                tmp_part.setPdgid(part->pdgid());

                // Beamline collision position. 
                // This can be randomized in the future
                MAdouble64 bsx = 0., bsy = 0.;

                // Original particle properties
                MAdouble64 x      = part->decay_vertex().X() * 1.0E-3; // in [m]
                MAdouble64 y      = part->decay_vertex().Y() * 1.0E-3; // in [m]
                MAdouble64 z      = part->decay_vertex().Z() * 1.0E-3; // in [m]
                MAdouble64 theta  = part->theta();
                MAdouble64 e      = part->e();
                MAdouble64 px     = part->px();
                MAdouble64 py     = part->py();
                MAdouble64 pz     = part->pz();
                MAdouble64 pt     = part->pt();
                MAdouble64 pt2    = pt*pt;
                // Propagate according to mother's charge
                MAdouble64 q      = PDG->GetCharge(part->mothers()[0]->pdgid());

                // Check if the particle is in the range of the tracker and if there
                // isn't a valid tracker skip propagation
                if ((hypot(x,y) > Radius_ || fabs(z) > HalfLength_ || pt2 < 1e-9) || (Radius_<=1e-9 || HalfLength_<=1e-9))
                {
                    MALorentzVector pos;
                    pos.SetXYZT(x * 1.0E+3, y * 1.0E+3, z * 1.0E+3, part->decay_vertex().T());
                    tmp_part.setPosition(pos);
                    tmp_part.setD0((((x - bsx) * py - (y - bsy) * px) / pt) * 1.0E+3);
                    tmp_part.setDZ((z - ((x - bsx) * px + (y - bsy) * py) / pt * (pz / pt)) * 1.0E+3);
                    return tmp_part;
                }

                if (abs(q) < 1.0E-9 || fabs(Bz_) < 1.0E-9)
                {
                    // solve pt2*t^2 + 2*(px*x + py*y)*t - (Radius_^2 - x*x - y*y) = 0    (1)
                    MAdouble64 Lxy    = px * y - py * x;
                    MAdouble64 DiscR2 = pt2 * Radius_*Radius_ - Lxy*Lxy;

                    if (DiscR2 < 0.0 || Radius_ <= 1e-9) // no solution
                    {
                        MALorentzVector pos;
                        pos.SetXYZT(x * 1.0E+3, y * 1.0E+3, z * 1.0E+3, part->decay_vertex().T());
                        tmp_part.setPosition(pos);
                        tmp_part.setD0((((x - bsx) * py - (y - bsy) * px) / pt) * 1.0E+3);
                        tmp_part.setDZ((z - ((x - bsx) * px + (y - bsy) * py) / pt * (pz / pt)) * 1.0E+3);
                        return tmp_part;
                    }

                    MAdouble64 DiscR = sqrt(DiscR2);
                    // Two possible solutions to the Eq. (1)
                    MAdouble64 t1    = (-Lxy + DiscR) / pt2;
                    MAdouble64 t2    = (-Lxy - DiscR) / pt2;
                    MAdouble64 t     = (t1 < 0.0) ? t2 : t1;

                    MAdouble64 z_t = z + pz * t;
                    if(fabs(z_t) > HalfLength_)
                    {
                        MAdouble64 t3 = (HalfLength_  - z) / pz;
                        MAdouble64 t4 = (-HalfLength_ - z) / pz;
                        MAdouble64 t  = (t3 < 0.0) ? t4 : t3;
                    }

                    MAdouble64 x_t = x + px * t;
                    MAdouble64 y_t = y + py * t;
                    // new ctau = sqrt((x_t - x) * (x_t - x) + (y_t - y) * (y_t - y) + (z_t - z) * (z_t - z));
                    MALorentzVector pos;
                    pos.SetXYZT(x_t * 1.0E+3, y_t * 1.0E+3, z_t * 1.0E+3, part->decay_vertex().T() + t * e * 1.0E+3);
                    tmp_part.setPosition(pos);
                    tmp_part.setD0((((x - bsx) * py - (y - bsy) * px) / pt) * 1.0E+3);
                    tmp_part.setDZ((z - ((x - bsx) * px + (y - bsy) * py) / pt * (pz / pt)) * 1.0E+3);
                }
                else
                {
                    // 1.  initial transverse momentum p_{T0}: Part->pt
                    //     initial transverse momentum direction phi_0 = -atan(p_X0/p_Y0)
                    //     relativistic gamma: gamma = E/mc^2; gammam = gamma * m
                    //     gyration frequency omega = q/(gamma m) Bz
                    //     helix radius r = p_{T0} / (omega gamma m)
                    MAdouble64 gammam = e * 1.0E+9 / c_*c_;          // gammam [eV/c^2]      
                    MAdouble64 omega  = q * Bz_ / gammam;             // omega is here in [89875518/s]
                    MAdouble64 R      = pt / (q * Bz_) * 1.0E+9 / c_; // helix radius in [m]
                    MAdouble64 phi_0  = atan2(py,px);                // [rad] in [-pi, pi]

                    // Helix axis coordinates
                    MAdouble64 x_helix    = x+R*sin(phi_0);
                    MAdouble64 y_helix    = y-R*cos(phi_0);
                    MAdouble64 r_helix    = hypot(x_helix,y_helix);
                    MAdouble64 phi        = atan2(y_helix, x_helix) + (x_helix < 0.0)*pi_;

                    // calculate coordinates of closest approach to track circle
                    // in transverse plane xd, yd, zd
                    MAdouble64 xd = x_helix * x_helix * x_helix - x_helix * fabs(R) * r_helix + x_helix * y_helix * y_helix;
                    xd = (r_helix*r_helix > 0.0) ? xd / r_helix*r_helix : -999.;

                    MAdouble64 yd = y_helix * (-fabs(R) * r_helix + r_helix*r_helix);
                    yd = (r_helix*r_helix > 0.0) ? yd / r_helix*r_helix : -999.;

                    MAdouble64 zd = z + (hypot(xd,yd) - hypot(x,y)) * pz / pt;

                    MALorentzVector closest;
                    closest.SetXYZ(xd * 1.0E+3, yd * 1.0E+3, zd * 1.0E+3);
                    tmp_part.setClosestPoint(closest);

                    // use perigee momentum rather than original particle
                    // momentum, since the orignal particle momentum isn't known
                    MAdouble64 px_perigee  = copysign(1.0, R) * pt * (-y_helix / r_helix);
                    MAdouble64 py_perigee  = copysign(1.0, R) * pt * (x_helix / r_helix);
                    MAdouble64 phi_perigee = atan2(py,px);

                    tmp_part.momentum().SetPtEtaPhiE(pt, part->eta(), phi_perigee, e);

                    MAdouble64 d0 = (((x - bsx) * py_perigee - (y - bsy) * px_perigee) / pt);
                    // in [m] dont forget to multiply by 1.0E3
                    MAdouble64 dz = (z - ((x - bsx) * px_perigee + (y - bsy) * py_perigee) / pt * sinh(part->eta()));
                    // in [m] dont forget to multiply by 1.0E3

                    // 3. time evaluation t = min(t_r, t_z)
                    //    t_r : time to exit from the sides
                    //    t_z : time to exit from the front or the back
                    MAdouble64 t_r  = 0.0; // in [ns]
                    MAint32 sign_pz = (pz > 0.0) ? 1 : -1;
                    MAdouble64 t_z, t;
                    if(pz == 0.0) t_z = 1.0E+99;
                    else          t_z = gammam / (pz * 1.0E+9 / c_) * (-z + HalfLength_ * sign_pz);

                    // helix does not cross the cylinder sides
                    if (r_helix + fabs(R) < Radius_) t = t_z;
                    else
                    {
                        MAdouble64 asinrho = asin((Radius_*Radius_-r_helix*r_helix-R*R)/(2.*fabs(R)*r_helix));
                        MAdouble64 delta   = phi_0 - phi;

                        if(delta < -pi_) delta += 2 * pi_;
                        if(delta > pi_ ) delta -= 2 * pi_;

                        MAdouble64 t1 = (delta + asinrho) / omega;
                        MAdouble64 t2 = (delta + pi_ - asinrho) / omega;
                        MAdouble64 t3 = (delta + pi_ + asinrho) / omega;
                        MAdouble64 t4 = (delta - asinrho) / omega;
                        MAdouble64 t5 = (delta - pi_ - asinrho) / omega;
                        MAdouble64 t6 = (delta - pi_ + asinrho) / omega;

                        if(t1 < 0.0) t1 = 1.0E+99;
                        if(t2 < 0.0) t2 = 1.0E+99;
                        if(t3 < 0.0) t3 = 1.0E+99;
                        if(t4 < 0.0) t4 = 1.0E+99;
                        if(t5 < 0.0) t5 = 1.0E+99;
                        if(t6 < 0.0) t6 = 1.0E+99;

                        MAdouble64 t_ra = fmin(t1, fmin(t2, t3));
                        MAdouble64 t_rb = fmin(t4, fmin(t5, t6));
                        MAdouble64 t_r  = fmin(t_ra, t_rb);
                        t = fmin(t_r, t_z);
                    }

                    // 4. position in terms of x(t), y(t), z(t)
                    MAdouble64 x_t = x_helix + R * sin(omega * t - phi_0);
                    MAdouble64 y_t = y_helix + R * cos(omega * t - phi_0);
                    MAdouble64 z_t = z + pz * 1.0E+9 / c_ / gammam * t;
                    MAdouble64 r_t = hypot(x_t, y_t);

                    // compute path length for an helix
                    MAdouble64 alpha = pz * 1.0E+9 / c_ / gammam;
                    MAdouble64 L     = t * sqrt(alpha*alpha + R*R*omega*omega);

                    MALorentzVector pos;
                    pos.SetXYZT(x_t * 1.0E+3, y_t * 1.0E+3, z_t * 1.0E+3, L + t * e * 1.0E+3);
                    tmp_part.setPosition(pos);
                    tmp_part.setD0(d0 * 1.0E+3);
                    tmp_part.setDZ(dz * 1.0E+3);
                }
                return tmp_part;
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
            MAdouble64 Gaussian(MAdouble64 sigma, MAdouble64 property)
            {
              MAdouble64 kC1   = 1.448242853;        MAdouble64 kAs = 0.8853395638;
              MAdouble64 kC2   = 3.307147487;        MAdouble64 kBs = 0.2452635696;
              MAdouble64 kC3   = 1.46754004;         MAdouble64 kCs = 0.2770276848;
              MAdouble64 kD1   = 1.036467755;        MAdouble64 kB  = 0.5029324303;
              MAdouble64 kD2   = 5.295844968;        MAdouble64 kX0 = 0.4571828819;
              MAdouble64 kD3   = 3.631288474;        MAdouble64 kYm = 0.187308492 ;
              MAdouble64 kHm   = 0.483941449;        MAdouble64 kS  = 0.7270572718 ;
              MAdouble64 kZm   = 0.107981933;        MAdouble64 kT  = 0.03895759111;
              MAdouble64 kHp   = 4.132731354;        MAdouble64 kHp1 = 3.132731354;
              MAdouble64 kZp   = 18.52161694;        MAdouble64 kHzm = 0.375959516;
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
                  if ((kC1-y)*(kC3+fabs(z))<kC2) { result = z; break; }
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

            /// Smearer Cumulative distribution function TO BE TESTED
//            MAdouble64 Sigmoid(MAdouble64 sigma, MAdouble64 property)
//            {
//                MAdouble64 err = erf(property/(sigma*sqrt(2.)));
//                MAdouble64 r        = RANDOM->flat();
//                MAdouble64 sign     = (r >= 0.5) * 1.0 + (r < 0.5) * (-1.0);
//                return property + sign * RANDOM->flat() * (1. + err) * 0.5;
//            }

            virtual void PrintDebug()
            {
                DEBUG << "   -> Default Smearer." << endmsg;
                DEBUG << "   * Magnetic field [T] = " << Bz_ << endmsg;
                DEBUG << "   * Radius [m]         = " << Radius_ << endmsg;
                DEBUG << "   * Half Length [m]    = " << HalfLength_ << endmsg;
                DEBUG << "   * Propagator         = " << ParticlePropagator_ << endmsg;
                DEBUG << "   * Muon Smearer       = " << MuonSmearer_ << endmsg;
                DEBUG << "   * Electron Smearer   = " << ElectronSmearer_ << endmsg;
                DEBUG << "   * Photon Smearer     = " << PhotonSmearer_ << endmsg;
                DEBUG << "   * Tau Smearer        = " << TauSmearer_ << endmsg;
                DEBUG << "   * Jet Smearer        = " << JetSmearer_ << endmsg;
            }


            void PrintHeader()
            {
                INFO << "   <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
                INFO << "   <>                                                              <>" << endmsg;
                INFO << "   <>     Simplified Fast Detector Simulation in MadAnalysis 5     <>" << endmsg;
                INFO << "   <>            Please cite arXiv:2006.09387 [hep-ph]             <>" << endmsg;
                INFO << "   <>         https://madanalysis.irmp.ucl.ac.be/wiki/SFS          <>" << endmsg;
                INFO << "   <>                                                              <>" << endmsg;
                INFO << "   <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
            }
    };
}

#endif
