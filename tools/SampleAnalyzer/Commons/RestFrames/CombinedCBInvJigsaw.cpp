/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   CombinedCBInvJigsaw.cc
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

#include "SampleAnalyzer/Commons/RestFrames/CombinedCBInvJigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/ContraBoostInvJigsaw.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  CombinedCBInvJigsaw::CombinedCBInvJigsaw(const std::string& sname,
					     const std::string& stitle,
					     int N_CBjigsaw)
    : InvisibleJigsaw(sname, stitle, 2*N_CBjigsaw, 2*N_CBjigsaw),
      m_NCB(N_CBjigsaw)
  {
    m_InvMassDependancy = true;
  }
 
  CombinedCBInvJigsaw::~CombinedCBInvJigsaw() {}
  
  void CombinedCBInvJigsaw::AddJigsaw(const ContraBoostInvJigsaw& jigsaw, int ijigsaw){
    if(!jigsaw) return;
    if(ijigsaw < 0 || ijigsaw >= m_NCB) return;
    
    AddInvisibleFrames(jigsaw.GetChildFrames(0), 2*ijigsaw+0);
    AddInvisibleFrames(jigsaw.GetChildFrames(1), 2*ijigsaw+1);
    AddVisibleFrames(jigsaw.GetDependancyFrames(0), 2*ijigsaw+0);
    AddVisibleFrames(jigsaw.GetDependancyFrames(1), 2*ijigsaw+1);
  }
  
  double CombinedCBInvJigsaw::GetMinimumMass() const {
    if(m_NCB < 1) return 0.;
    if(m_NCB == 1) return GetCBMinimumMass(0);
    
    double Mmin2 = 0.;
    
    std::vector<double> Minv2;
    for(int i = 0; i < m_NCB; i++){
      Minv2.push_back(GetCBMinimumMass(i));
      Minv2[i] *= Minv2[i];
      Mmin2 += Minv2[i];
    }

    std::vector<MA5::MALorentzVector> Pvis;
    std::vector<double> Mvis2;
    for(int i = 0; i < m_NCB; i++){
      Pvis.push_back(GetDependancyStates(2*i+0).GetFourVector()+
		     GetDependancyStates(2*i+1).GetFourVector());
      Mvis2.push_back(Pvis[i].M2());
    }

    for(int i = 0; i < m_NCB-1; i++){
      for(int j = i+1; j < m_NCB; j++){
	if(Minv2[i] > Mvis2[i] || Minv2[j] > Mvis2[j]){
	  Mmin2 += std::max(0.,(Pvis[i]+Pvis[j]).M2()-Mvis2[i]-Mvis2[j])*
	    std::max(((Minv2[i] > 0. && Mvis2[i] <= 0.) ? 1. : Minv2[i]/Mvis2[i]),
		     ((Minv2[j] > 0. && Mvis2[j] <= 0.) ? 1. : Minv2[j]/Mvis2[j]));
	    
	} else {
	  Mmin2 += std::max(0.,(Pvis[i]+Pvis[j]).M2()-Mvis2[i]-Mvis2[j]);
	  Mmin2 += std::max(Minv2[i] - Mvis2[i], Minv2[j] - Mvis2[j])*2.;
	}
      }
    }
    
    return sqrt(std::max(0., Mmin2));
  }
  
  double CombinedCBInvJigsaw::GetCBMinimumMass(int i) const {
    if(!IsSoundMind())
      return 0.;

    double Minv1 = GetChildState(2*i+0).GetMinimumMass();
    double Minv2 = GetChildState(2*i+1).GetMinimumMass();
    MA5::MALorentzVector Pvis1 = GetDependancyStates(2*i+0).GetFourVector();
    MA5::MALorentzVector Pvis2 = GetDependancyStates(2*i+1).GetFourVector();
    double Mvis1 = std::max(Pvis1.M(), 0.);
    double Mvis2 = std::max(Pvis2.M(), 0.);
     double M12 = (Pvis1+Pvis2).M();

    if(Minv1 < -0.5 && Minv2 < -0.5) // children can go tachyonic
      return 2.*GetP(M12,Mvis1,Mvis2);

    Minv1 = std::max(Minv1,0.);
    Minv2 = std::max(Minv2,0.);

    double Minvmax = std::max(0.,std::max(Minv1,Minv2));
    
    double Mvismin = std::min(Mvis1,Mvis2);
    double Mvismax = std::max(Mvis1,Mvis2);

    if(Minv1 < Mvis2 && Minv2 < Mvis1){
      if(Minvmax <= Mvismin)
	return sqrt( M12*M12 + 4.*(Minvmax-Mvismin)*(Minvmax+Mvismax) );
      return M12;
    } 
    
    if(Mvismin <= 0.0 && Minvmax > 0.)
      return M12;
    
    return M12*(1.+sqrt(std::max(Minv1*Minv1-Mvis2*Mvis2,
				 Minv2*Minv2-Mvis1*Mvis1))/Mvismin); 
  }

  bool CombinedCBInvJigsaw::AnalyzeEvent(){
    if(!IsSoundMind())
      return SetSpirit(false);

    if(m_NCB < 1) 
      return SetSpirit(false);

    MA5::MALorentzVector INV = GetParentState().GetFourVector();
    MA5::MALorentzVector VIS(0.,0.,0.,0.);
    
    std::vector<MA5::MALorentzVector> Pvis[2];
    for(int i = 0; i < m_NCB; i++){
      Pvis[0].push_back( GetDependancyStates(2*i+0).GetFourVector() );
      Pvis[1].push_back( GetDependancyStates(2*i+1).GetFourVector() );
      VIS += Pvis[0][i]+Pvis[1][i];
    }

    // go to the rest frame of (VIS+INV system)
    MA5::MABoost Booster;
    Booster.setBoostVector(-VIS-INV);
    for(int i = 0; i < m_NCB; i++){
      Booster.boost(Pvis[0][i]);
      Booster.boost(Pvis[1][i]);
    }

    std::vector<double> c[2];
    std::vector<double> E[2];
    std::vector<double> mvis[2];
    std::vector<double> minv[2];
    std::vector<double> Minv2;
    double mvismin, minvmax;
    double m1, m2, k1, k2;
    double MC2, Xbar, K;
    double N, sumE, sumcE;
    for(int i = 0; i < m_NCB; i++){
      E[0].push_back(Pvis[0][i].E());
      E[1].push_back(Pvis[1][i].E());
      mvis[0].push_back(std::max(0.,Pvis[0][i].M()));
      mvis[1].push_back(std::max(0.,Pvis[1][i].M()));
      minv[0].push_back( GetChildState(2*i+0).GetMinimumMass() );
      minv[1].push_back( GetChildState(2*i+1).GetMinimumMass() );
      Minv2.push_back( GetCBMinimumMass(i) );
      Minv2[i] *= Minv2[i];

      minvmax = std::max(0.,std::max(minv[0][i],minv[1][i]));
      mvismin = std::min(mvis[0][i],mvis[1][i]);

      c[0].push_back(1.);
      c[1].push_back(1.);
      if(minvmax < mvismin){
	m1 = mvis[0][i];
	m2 = mvis[1][i];
	MC2 = 2.*(E[0][i]*E[1][i] + Pvis[0][i].Vect().Dot(Pvis[1][i].Vect()));
	k1 =  (m1+m2)*(m1-m2)*(1.-minvmax/mvismin) + MC2-2*m1*m2 +
	  (m1+m2)*fabs(m1-m2)*minvmax/mvismin;
	k2 = -(m1+m2)*(m1-m2)*(1.-minvmax/mvismin) + MC2-2*m1*m2 +
	  (m1+m2)*fabs(m1-m2)*minvmax/mvismin;
	Xbar = sqrt( (k1+k2)*(k1+k2)*(MC2*MC2-4*m1*m1*m2*m2) +
			    16.*minvmax*minvmax*(k1*k1*m1*m1 + k2*k2*m2*m2 + k1*k2*MC2) );
	K = ( fabs(k1*m1*m1-k2*m2*m2) - 0.5*fabs(k2-k1)*MC2 + 0.5*Xbar )/
	  (k1*k1*m1*m1 + k2*k2*m2*m2 + k1*k2*MC2);
	c[0][i] = 0.5*(1.+K*k1);
	c[1][i] = 0.5*(1.+K*k2);
      }
      
      sumE  = E[0][i] + E[1][i];
      sumcE = c[0][i]*E[0][i] + c[1][i]*E[1][i];

      N = (sqrt(sumE*sumE+Minv2[i]-(Pvis[0][i]+Pvis[1][i]).M2())+sumE)/sumcE/2.;
      //N = sumkE > 0. ? sqrt(std::max(0.,Minv2[i]-(Pvis[0][i]+Pvis[1][i]).M2()+sumE*sumE))/sumkE : 0.;

      c[0][i] *= N;
      c[1][i] *= N;
    }

    sumE  = 0.;
    sumcE = 0.;
    for(int i = 0; i < m_NCB; i++){
      sumE  += E[0][i] + E[1][i];
      sumcE += c[0][i]*E[0][i] + c[1][i]*E[1][i];
    }

    N = (sqrt(sumE*sumE + INV.M2() - VIS.M2())+sumE)/sumcE/2.;
    //N = sumkE > 0. ? sqrt(std::max(0.,INV.M2()-VIS.M2()+sumE*sumE))/sumkE : 0.;

    MA5::MALorentzVector INV1, INV2;
    double Einv1, Einv2;
    MA5::MAVector3 Pinv1, Pinv2;
    for(int i = 0; i < m_NCB; i++){
      c[0][i] *= N;
      c[1][i] *= N;

      Einv1 = (c[0][i]-1.)*E[0][i] + c[1][i]*E[1][i];
      Einv2 = (c[1][i]-1.)*E[1][i] + c[0][i]*E[0][i];
      Pinv1 = (c[0][i]-1.)*Pvis[0][i].Vect() - c[1][i]*Pvis[1][i].Vect();
      Pinv2 = (c[1][i]-1.)*Pvis[1][i].Vect() - c[0][i]*Pvis[0][i].Vect();

      INV1.SetPxPyPzE(Pinv1.X(),Pinv1.Y(),Pinv1.Z(),Einv1);
      INV2.SetPxPyPzE(Pinv2.X(),Pinv2.Y(),Pinv2.Z(),Einv2);

      if(minv[0][i] >= 0. && INV1.M() < minv[0][i])
	INV1.SetVectM(Pinv1, minv[0][i]);
      if(minv[1][i] >= 0. && INV2.M() < minv[1][i])
	INV2.SetVectM(Pinv2, minv[1][i]);

      Booster.setBoostVector(VIS+INV);
      Booster.boost(INV1);
      Booster.boost(INV2);

      GetChildState(2*i+0).SetFourVector(INV1);
      GetChildState(2*i+1).SetFourVector(INV2);
    }
    
    return SetSpirit(true);
  }

}
