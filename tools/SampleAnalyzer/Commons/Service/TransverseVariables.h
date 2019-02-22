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


#ifndef TRANSVERSE_VARIABLE_SERVICE_h
#define TRANSVERSE_VARIABLE_SERVICE_h


// STL headers
#include <iostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"


namespace MA5
{

class TransverseVariables
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:
    /// The MT2 variable requires 2 momenta + the missing energy + one test mass
    /// The MT2W variable requires a 3rd momentum
    MALorentzVector p1_, p2_, p3_;
    MAfloat64 pmx_, pmy_;
    MAfloat64 m_;
    MAfloat64 E2sq_;

    /// The w mass (useful for mt2w)
    MAfloat64 mw_, mw2_;

    /// Other usefull kinematical variables
    MAfloat64 msq_, pmtsq_, pmtm_, p1met_, plpb1_;

    /// Initialization with two momenta + met
    void InitializeMT2(const MALorentzVector&p1, const MALorentzVector&p2, const MALorentzVector&met,
      const MAfloat64 mass)
    {
      // Coefficients
      C1_.resize(0); C2_.resize(0);
      // Momenta
      if( p1.M() < p2.M() ) { p1_=p1; p2_=p2; }
      else                  { p1_=p2; p2_=p1; }
      // MET
      pmx_ = met.Px();
      pmy_ = met.Py();
      pmtsq_ = pow(met.Pt(),2.);
      // Test mass
      m_  = mass;
      msq_ = pow(mass,2.);
      // Other kinematical stuff
      pmtm_ = msq_ + pmtsq_;
      p1met_ = p1_.Px()*pmx_ + p1_.Py()*pmy_;
    }

    /// the MT2 method requires two series of 6 coefficients + associated fcns
    std::vector<MAfloat64> C1_, C2_;

    void InitC(const MALorentzVector &p, std::vector<MAfloat64> &C)
    {
      C.push_back( 1. - pow(p.Px(),2)/p.Mt2() );
      C.push_back( -p.Px()*p.Py()/p.Mt2() );
      C.push_back( 1. - pow(p.Py(),2)/p.Mt2() );
      C.push_back( 0. );
      C.push_back( 0. );
      C.push_back( 0. );
    }

    void UpdateC1(const MAfloat64 &del)
    {
      C1_[3] = -p2_.Px()*del;
      C1_[4] = -p2_.Py()*del;
      C1_[5] = msq_ - p2_.Mt2()*pow(del,2);
    }

    void UpdateC2(const MAfloat64 &del)
    {
      C2_[3] = -pmx_ + p1_.Px()*del;
      C2_[4] = -pmy_ + p1_.Py()*del;
      C2_[5] = pmtm_ - p1_.Mt2()*pow(del,2);
    }

    void InitCoefs()   { InitC(p2_,C1_); InitC(p1_,C2_); }

    /// Bissection method to extract MT2
    MAint32 Nsolutions(const MAfloat64&);
    MAint32 Nsolutions_massless(const MAfloat64&);
    MAbool FindHigh(MAfloat64 &dsqH);

    /// Special case: both particles are nearly massless
    MAfloat64 GetMT2_massless();

    /// Initialization with three momenta + met
    void InitializeMT2W(const MALorentzVector&p1, const MALorentzVector&p2, const MALorentzVector&p3,
      const MALorentzVector &met)
    {
      p1_=p1; p2_=p2; p3_=p3;
      E2sq_ = pow(p2_.E(),2.);
      // MET
      pmx_ = met.Px();
      pmy_ = met.Py();
      pmtsq_ = pow(met.Pt(),2.);
      // The w mass
      mw_ = 80.4;
      mw2_=pow(mw_,2.);
      // dot products
      plpb1_ = p1_.E()*p2_.E() -
       p1_.Px()*p2_.Px() -
       p1.Py()*p2_.Py() -
       p1.Pz()*p2_.Pz();
    }

    /// Core function for mt2w
    MAbool TestComp(const MAfloat64&);
    MAfloat64 GetMT2W(const ParticleBaseFormat*,const ParticleBaseFormat*,const ParticleBaseFormat*,
       const ParticleBaseFormat&);

  public:
    /// Constructor
    TransverseVariables() { }

    /// Destructor
    ~TransverseVariables() { }

    /// Compute the total transverse energy
    inline MAfloat64 EventTET(const MCEventFormat* event) const
    {
      return event->TET();
    }

    /// Compute the missing transverse energy
    inline MAfloat64 EventMET(const MCEventFormat* event) const
    {
      return event->MET().pt();
    }

    /// Compute the total hadronic transverse energy
    inline MAfloat64 EventTHT(const MCEventFormat* event) const
    {
      return event->THT();
    }

    /// Compute the total effective mass
    inline MAfloat64 EventMEFF(const MCEventFormat* event) const
    {
      return event->Meff();
    }

    /// Compute the missing hadronic transverse energy
    inline MAfloat64 EventMHT(const MCEventFormat* event) const
    {
      return event->MHT().pt();
    }

    /// Compute the total transverse energy
    inline MAfloat64 EventTET(const RecEventFormat* event) const
    {
      return event->TET();
    }

    /// Compute the missing transverse energy
    inline MAfloat64 EventMET(const RecEventFormat* event) const
    {
      return event->MET().pt();
    }

    /// Compute the total hadronic transverse energy
    inline MAfloat64 EventTHT(const RecEventFormat* event) const
    {
      return event->THT();
    }

    /// Compute the total effective mass
    inline MAfloat64 EventMEFF(const RecEventFormat* event) const
    {
      return event->Meff();
    }

    /// Compute the missing hadronic transverse energy
    inline MAfloat64 EventMHT(const RecEventFormat* event) const
    {
      return event->MHT().pt();
    }

    /// MT2 methods
    MAfloat64 MT2(const MALorentzVector* p1, const MALorentzVector* p2,
      const MALorentzVector& met, const MAfloat64 &mass)
    {
      InitializeMT2(*p1, *p2, met,mass);
      return GetMT2();
    }

    /// MT2 methods
    MAfloat64 MT2(const ParticleBaseFormat* p1, const ParticleBaseFormat* p2,
      const ParticleBaseFormat& met, const MAfloat64 &mass)
    {
      InitializeMT2(p1->momentum(), p2->momentum(), met.momentum(),mass);
      return GetMT2();
    }

    MAfloat64 GetMT2();

    /// MT2W method
    MAfloat64 MT2W(std::vector<const RecJetFormat*>,const RecLeptonFormat*,const ParticleBaseFormat&);
    MAfloat64 MT2W(std::vector<const MCParticleFormat*>,const MCParticleFormat*,const ParticleBaseFormat&);

  /// The Alpha_T variable
  MAfloat64 AlphaT(const MCEventFormat*);
  MAfloat64 AlphaT(const RecEventFormat*);


};

}

#endif
