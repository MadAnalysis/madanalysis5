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
 * @file TransverseVariables.h
 * @brief Transverse variables: MET, HT, Meff, MT2, MT2W and alphaT (PHYSICS->Transverse).
 */

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

/**
 * @brief Toolbox computing transverse variables.
 *
 * MT2 is computed with the bisection method of Cheng and Han (arXiv:0810.5178), MT2W
 * following Bai, Cheng, Gallicchio and Gu (arXiv:1203.4813), and alphaT following
 * CMS (the minimal HT difference between two pseudo-jets).
 */
class TransverseVariables
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:
    /** @brief Visible momenta (two for MT2, three for MT2W). */
    MALorentzVector p1_, p2_, p3_;
    /** @brief Components of the missing transverse momentum. */
    MAfloat64 pmx_, pmy_;
    /** @brief Test mass of the invisible particles. */
    MAfloat64 m_;
    /** @brief Square of the energy of the second visible particle (MT2W). */
    MAfloat64 E2sq_;

    /** @brief W mass and its square (MT2W). */
    MAfloat64 mw_, mw2_;

    /** @brief Intermediate kinematic quantities. */
    MAfloat64 msq_, pmtsq_, pmtm_, p1met_, plpb1_;

    /**
     * @brief Initialise the MT2 computation (the lighter visible particle is stored in p1_).
     *
     * @param p1 first visible momentum.
     * @param p2 second visible momentum.
     * @param met missing transverse momentum.
     * @param mass test mass of the invisible particles.
     */
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

    /** @brief Coefficients of the two ellipses of the MT2 computation. */
    std::vector<MAfloat64> C1_, C2_;

    /**
     * @brief Initialise the constant coefficients of an ellipse.
     *
     * @param p visible momentum.
     * @param C coefficients to fill.
     */
    void InitC(const MALorentzVector &p, std::vector<MAfloat64> &C)
    {
      C.push_back( 1. - pow(p.Px(),2)/p.Mt2() );
      C.push_back( -p.Px()*p.Py()/p.Mt2() );
      C.push_back( 1. - pow(p.Py(),2)/p.Mt2() );
      C.push_back( 0. );
      C.push_back( 0. );
      C.push_back( 0. );
    }

    /**
     * @brief Update the coefficients of the first ellipse.
     *
     * @param del scaled trial MT2.
     */
    void UpdateC1(const MAfloat64 &del)
    {
      C1_[3] = -p2_.Px()*del;
      C1_[4] = -p2_.Py()*del;
      C1_[5] = msq_ - p2_.Mt2()*pow(del,2);
    }

    /**
     * @brief Update the coefficients of the second ellipse.
     *
     * @param del scaled trial MT2.
     */
    void UpdateC2(const MAfloat64 &del)
    {
      C2_[3] = -pmx_ + p1_.Px()*del;
      C2_[4] = -pmy_ + p1_.Py()*del;
      C2_[5] = pmtm_ - p1_.Mt2()*pow(del,2);
    }

    /** @brief Initialise the coefficients of both ellipses. */
    void InitCoefs()   { InitC(p2_,C1_); InitC(p1_,C2_); }

    /**
     * @brief Number of intersections of the two ellipses (Sturm sequence).
     *
     * @param E energy scale used to make the coefficients dimensionless.
     * @return the number of real solutions.
     */
    MAint32 Nsolutions(const MAfloat64&);
    /**
     * @brief Number of intersections of the two parabolas (massless case).
     *
     * @param dsq trial MT2 squared.
     * @return the number of real solutions.
     */
    MAint32 Nsolutions_massless(const MAfloat64&);
    /**
     * @brief Lower the upper bound of the bisection until the ellipses intersect.
     *
     * @param dsqH upper bound (modified).
     * @return true if a valid upper bound is found.
     */
    MAbool FindHigh(MAfloat64 &dsqH);

    /**
     * @brief MT2 when both visible particles are (nearly) massless.
     *
     * @return MT2.
     */
    MAfloat64 GetMT2_massless();

    /**
     * @brief Initialise the MT2W computation.
     *
     * @param p1 lepton.
     * @param p2 first b-jet candidate.
     * @param p3 second b-jet candidate.
     * @param met missing transverse momentum.
     */
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

    /**
     * @brief Is a trial top mass compatible with the event kinematics (MT2W)?
     *
     * @param mt trial top mass.
     * @return true if compatible.
     */
    MAbool TestComp(const MAfloat64&);
    /**
     * @brief MT2W for one lepton/b-jet assignment (bisection between mW+mb and 500 GeV).
     *
     * @param lep lepton.
     * @param j1 first jet.
     * @param j2 second jet.
     * @param met missing transverse momentum.
     * @return MT2W (499 GeV if no compatible mass below 500 GeV).
     */
    MAfloat64 GetMT2W(const ParticleBaseFormat*,const ParticleBaseFormat*,const ParticleBaseFormat*,
       const ParticleBaseFormat&);

  public:
    /** @brief Constructor. */
    TransverseVariables() { }

    /** @brief Destructor. */
    ~TransverseVariables() { }

    /**
     * @brief Accessor to the scalar sum of the transverse energies of a Monte Carlo event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventTET(const MCEventFormat* event) const
    {
      return event->TET();
    }

    /**
     * @brief Accessor to the missing transverse energy of a Monte Carlo event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventMET(const MCEventFormat* event) const
    {
      return event->MET().pt();
    }

    /**
     * @brief Accessor to the scalar sum of the hadronic transverse energies of a Monte Carlo event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventTHT(const MCEventFormat* event) const
    {
      return event->THT();
    }

    /**
     * @brief Accessor to the effective mass of a Monte Carlo event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventMEFF(const MCEventFormat* event) const
    {
      return event->Meff();
    }

    /**
     * @brief Accessor to the missing hadronic transverse energy of a Monte Carlo event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventMHT(const MCEventFormat* event) const
    {
      return event->MHT().pt();
    }

    /**
     * @brief Accessor to the scalar sum of the transverse energies of a reconstructed event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventTET(const RecEventFormat* event) const
    {
      return event->TET();
    }

    /**
     * @brief Accessor to the missing transverse energy of a reconstructed event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventMET(const RecEventFormat* event) const
    {
      return event->MET().pt();
    }

    /**
     * @brief Accessor to the scalar sum of the hadronic transverse energies of a reconstructed event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventTHT(const RecEventFormat* event) const
    {
      return event->THT();
    }

    /**
     * @brief Accessor to the effective mass of a reconstructed event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventMEFF(const RecEventFormat* event) const
    {
      return event->Meff();
    }

    /**
     * @brief Accessor to the missing hadronic transverse energy of a reconstructed event.
     *
     * @param event event.
     * @return the value.
     */
    inline MAfloat64 EventMHT(const RecEventFormat* event) const
    {
      return event->MHT().pt();
    }

    /**
     * @brief Compute MT2.
     *
     * @param p1 first visible momentum.
     * @param p2 second visible momentum.
     * @param met missing transverse momentum.
     * @param mass test mass of the invisible particles.
     * @return MT2.
     */
    MAfloat64 MT2(const MALorentzVector* p1, const MALorentzVector* p2,
      const MALorentzVector& met, const MAfloat64 &mass)
    {
      InitializeMT2(*p1, *p2, met,mass);
      return GetMT2();
    }

    /**
     * @brief Compute MT2.
     *
     * @param p1 first visible particle.
     * @param p2 second visible particle.
     * @param met missing transverse momentum.
     * @param mass test mass of the invisible particles.
     * @return MT2.
     */
    MAfloat64 MT2(const ParticleBaseFormat* p1, const ParticleBaseFormat* p2,
      const ParticleBaseFormat& met, const MAfloat64 &mass)
    {
      InitializeMT2(p1->momentum(), p2->momentum(), met.momentum(),mass);
      return GetMT2();
    }

    /**
     * @brief Compute MT2 for the current initialisation.
     *
     * @return MT2.
     */
    MAfloat64 GetMT2();

    /**
     * @brief Compute MT2W for reconstructed objects.
     *
     * The minimum over the lepton/b-jet assignments is taken; b-tagged jets are preferred
     * (at most three jets are considered).
     *
     * @param jets jets (at least two).
     * @param lep lepton.
     * @param met missing transverse momentum.
     * @return MT2W (0 with less than two jets).
     */
    MAfloat64 MT2W(std::vector<const RecJetFormat*>,const RecLeptonFormat*,const ParticleBaseFormat&);
    /**
     * @brief Compute MT2W for Monte Carlo particles (b quarks play the role of the b-jets).
     *
     * @param jets jets (at least two).
     * @param lep lepton.
     * @param met missing transverse momentum.
     * @return MT2W.
     */
    MAfloat64 MT2W(std::vector<const MCParticleFormat*>,const MCParticleFormat*,const ParticleBaseFormat&);

  /**
   * @brief Compute alphaT from the final-state partons (quarks and gluons).
   *
   * @param event Monte Carlo event.
   * @return alphaT (0 with less than two jets, -1 if undefined).
   */
  MAfloat64 AlphaT(const MCEventFormat*);
  /**
   * @brief Compute alphaT from the reconstructed jets.
   *
   * @param event reconstructed event.
   * @return alphaT (0 with less than two jets, -1 if undefined).
   */
  MAfloat64 AlphaT(const RecEventFormat*);
  /**
   * @brief Compute alphaT from the final-state partons (reference implementation).
   *
   * @param event Monte Carlo event.
   * @return alphaT.
   */
  MAfloat64 SlowAlphaT(const MCEventFormat*);
  /**
   * @brief Compute alphaT from the reconstructed jets (reference implementation).
   *
   * @param event reconstructed event.
   * @return alphaT.
   */
  MAfloat64 SlowAlphaT(const RecEventFormat*);


};

}

#endif
