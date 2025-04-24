/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassesSqCombJigsaw.cc
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
#include "SampleAnalyzer/Commons/RestFrames/MinMassesSqCombJigsaw.h"

namespace RestFrames {

  MinMassesSqCombJigsaw::MinMassesSqCombJigsaw(const std::string& sname, 
					       const std::string& stitle,
					       int N_comb, int N_mass) 
    : CombinatoricJigsaw(sname, stitle, N_comb, N_mass),
      m_Ncomb(N_comb), m_Nmass(N_mass) { (void)m_Ncomb; }
  
  MinMassesSqCombJigsaw::~MinMassesSqCombJigsaw() {}

  bool MinMassesSqCombJigsaw::EvaluateMetric(double& metric) const {
    double sum = 0.;

    for(int i = 0; i < m_Nmass; i++)
      sum += GetDependancyStates(i).GetFourVector().M2();
    
    metric = sum;
    return true;
  }

}

