/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassDiffCombJigsaw.cc
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
#include "SampleAnalyzer/Commons/RestFrames/MinMassDiffCombJigsaw.h"

namespace RestFrames {

  MinMassDiffCombJigsaw::MinMassDiffCombJigsaw(const std::string& sname, 
					       const std::string& stitle,
					       int N_comb, int N_mass) 
    : CombinatoricJigsaw(sname, stitle, N_comb, N_mass),
      m_Ncomb(N_comb), m_Nmass(N_mass) { (void)m_Ncomb;}
  
  MinMassDiffCombJigsaw::~MinMassDiffCombJigsaw() {}

  bool MinMassDiffCombJigsaw::EvaluateMetric(double& metric) const {
    double diff = 0.;
    double m1, m2;
    
    for(int i = 0; i < m_Nmass-1; i++){
      m1 = GetDependancyStates(i).GetFourVector().M();
      for(int j = i+1; i < m_Nmass; j++){
	m2 = GetDependancyStates(j).GetFourVector().M();
	diff += (m1-m2)*(m1-m2);
      }
    }
    
    metric = diff;
    return true;
  }

}

