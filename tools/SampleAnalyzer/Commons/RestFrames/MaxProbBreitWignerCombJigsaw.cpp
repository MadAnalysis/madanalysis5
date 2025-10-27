/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MaxProbBreitWignerCombJigsaw.cc
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
#include "SampleAnalyzer/Commons/RestFrames/MaxProbBreitWignerCombJigsaw.h"

namespace RestFrames {

  MaxProbBreitWignerCombJigsaw::MaxProbBreitWignerCombJigsaw(const std::string& sname, 
							     const std::string& stitle,
							     int N_comb, int N_object) 
    : CombinatoricJigsaw(sname, stitle, N_comb, N_object),
      m_Ncomb(N_comb), m_Nobj(N_object)
  {
    (void)m_Ncomb;
    for(int i = 0; i < m_Nobj; i++){
      m_Mass.push_back(0.);
      m_Width.push_back(-1.);
    }
  }
  
  MaxProbBreitWignerCombJigsaw::~MaxProbBreitWignerCombJigsaw() {}

  void MaxProbBreitWignerCombJigsaw::SetPoleMass(double mass, int i){
    if(i < 0 || i >= m_Nobj)
      return;

    m_Mass[i] = std::max(mass, 0.);
  }

  void MaxProbBreitWignerCombJigsaw::SetWidth(double width, int i){
    if(i < 0 || i >= m_Nobj)
      return;
    if(width <= 0.)
       m_Width[i] = -1.;
    else
      m_Width[i] = width;
  }

  bool MaxProbBreitWignerCombJigsaw::EvaluateMetric(double& metric) const {
    std::vector<MA5::MALorentzVector> P;
    for(int i = 0; i < m_Nobj; i++)
      P.push_back(GetDependancyStates(i).GetFourVector());
    
    double prob = 1.;
    MA5::MALorentzVector SUM(0.,0.,0.,0.);
    for(int i = 0; i < m_Nobj; i++)
      SUM += P[i];
    double M = SUM.M();
    for(int i = 0; i < m_Nobj-1; i++){
      SUM -= P[i];
      prob *= GetP((P[i]+SUM).M(), P[i].M(), SUM.M())/M;
    }

    double den;
    for(int i = 0; i < m_Nobj; i++){
      if(m_Width[i] > 0.){
	den = P[i].M2()-m_Mass[i]*m_Mass[i];
	den *= den;
	den += m_Mass[i]*m_Mass[i]*m_Width[i]*m_Width[i];
	if(den > 0.)
	  prob /= den;
      }
    }

    if(prob <= 0.)
      return false;

    return 1./prob;
    return true;
  }

}

