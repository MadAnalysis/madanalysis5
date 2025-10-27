/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassChi2CombJigsaw.cc
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 Jun
///
//   This file is part of RestFrames.
//
//   RestFrames is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   RestFrames is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with RestFrames. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////

#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/MinMassChi2CombJigsaw.h"

namespace RestFrames {

  MinMassChi2CombJigsaw::MinMassChi2CombJigsaw(const std::string& sname, 
					       const std::string& stitle,
					       int N_comb, int N_mass) 
    : CombinatoricJigsaw(sname, stitle, N_comb, N_mass),
      m_Ncomb(N_comb), m_Nmass(N_mass)
  {
    (void)m_Ncomb;
    for(int i = 0; i < m_Nmass; i++){
      m_Mass.push_back(0.);
      m_Sigma.push_back(1.);
    }
  }
  
  MinMassChi2CombJigsaw::~MinMassChi2CombJigsaw() {}

  void MinMassChi2CombJigsaw::SetMass(double mass, int i){
    if(i < 0 || i >= m_Nmass)
      return;

    m_Mass[i] = std::max(mass, 0.);
  }

  void MinMassChi2CombJigsaw::SetSigma(double sigma, int i){
    if(i < 0 || i >= m_Nmass)
      return;
    if(sigma <= 0.)
      return;

    m_Sigma[i] = sigma;
  }

  bool MinMassChi2CombJigsaw::EvaluateMetric(double& metric) const {
    double mass;
    double chi2 = 0.;
    
    for(int i = 0; i < m_Nmass; i++){
      mass = GetDependancyStates(i).GetFourVector().M();
      chi2 += (mass-m_Mass[i])*(mass-m_Mass[i])/m_Sigma[i]/m_Sigma[i];
    }
    
    metric = chi2;
    return true;
  }

}

