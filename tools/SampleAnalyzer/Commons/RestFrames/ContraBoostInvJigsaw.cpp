/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   ContraBoostInvJigsaw.cc
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jan
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

#include "SampleAnalyzer/Commons/RestFrames/ContraBoostInvJigsaw.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  ContraBoostInvJigsaw::ContraBoostInvJigsaw(const std::string& sname,
					     const std::string& stitle) : 
    InvisibleJigsaw(sname, stitle, 2, 2)
  {
    m_InvMassDependancy = true;
  }

  ContraBoostInvJigsaw::ContraBoostInvJigsaw() : InvisibleJigsaw() {}
 
  ContraBoostInvJigsaw::~ContraBoostInvJigsaw() {}

  ContraBoostInvJigsaw& ContraBoostInvJigsaw::Empty(){
    return ContraBoostInvJigsaw::m_Empty;
  }

  double ContraBoostInvJigsaw::GetMinimumMass() const {
    if(!IsSoundMind())
      return 0.;

    double Minv1 = GetChildState(0).GetMinimumMass();
    double Minv2 = GetChildState(1).GetMinimumMass();
    MA5::MALorentzVector Pvis1 = GetDependancyStates(0).GetFourVector();
    MA5::MALorentzVector Pvis2 = GetDependancyStates(1).GetFourVector();
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

  bool ContraBoostInvJigsaw::AnalyzeEvent(){
    if(!IsSoundMind())
      return SetSpirit(false);

    MA5::MALorentzVector Pvis1 = GetDependancyStates(0).GetFourVector();
    MA5::MALorentzVector Pvis2 = GetDependancyStates(1).GetFourVector();
    MA5::MALorentzVector INV = GetParentState().GetFourVector();

    // go to the rest frame of (Pvis1+Pvis2+INV system)
    MA5::MABoost Booster;
    Booster.setBoostVector(-Pvis1-Pvis2-INV);
    Booster.boost(Pvis1);
    Booster.boost(Pvis2);
    Booster.boost(INV);

    double E1 = Pvis1.E();
    double E2 = Pvis2.E();
    double m1 = std::max(0.,Pvis1.M());
    double m2 = std::max(0.,Pvis2.M());
    MA5::MAVector3 P1 = Pvis1.Vect();
    MA5::MAVector3 P2 = Pvis2.Vect();

    double Minv1 = GetChildState(0).GetMinimumMass();
    double Minv2 = GetChildState(1).GetMinimumMass();
    double Minv = std::max(0.,std::max(Minv1,Minv2));
    double Mvis = std::min(m1,m2);

    double c1 = 1.;
    double c2 = 1.;
    if(Minv < Mvis){
      double MC2 = 2.*(E1*E2 + P1.Dot(P2));
      double k1 =  (m1+m2)*(m1-m2)*(1.-Minv/Mvis) + MC2-2*m1*m2 + (m1+m2)*fabs(m1-m2)*Minv/Mvis;
      double k2 = -(m1+m2)*(m1-m2)*(1.-Minv/Mvis) + MC2-2*m1*m2 + (m1+m2)*fabs(m1-m2)*Minv/Mvis;
      double Xbar = sqrt( (k1+k2)*(k1+k2)*(MC2*MC2-4*m1*m1*m2*m2) +
			  16.*Minv*Minv*(k1*k1*m1*m1 + k2*k2*m2*m2 + k1*k2*MC2) );
      double K = ( fabs(k1*m1*m1-k2*m2*m2) - 0.5*fabs(k2-k1)*MC2 + 0.5*Xbar )/
	(k1*k1*m1*m1 + k2*k2*m2*m2 + k1*k2*MC2);
      c1 = 0.5*(1.+K*k1);
      c2 = 0.5*(1.+K*k2);
    }

    double sumE  = E1+E2;
    double sumcE = c1*E1+c2*E2;

    double N = (sqrt(sumE*sumE-(Pvis1+Pvis2).M2()+INV.M2())+sumE)/sumcE/2.;

    c1 *= N;
    c2 *= N;

    MA5::MALorentzVector INV1,INV2;
    double Einv1 = (c1-1.)*E1 + c2*E2;
    double Einv2 = c1*E1 + (c2-1.)*E2;
    MA5::MAVector3 Pinv1 = (c1-1.)*P1 - c2*P2;
    MA5::MAVector3 Pinv2 = (c2-1.)*P2 - c1*P1;

    INV1.SetPxPyPzE(Pinv1.X(),Pinv1.Y(),Pinv1.Z(),Einv1);
    INV2.SetPxPyPzE(Pinv2.X(),Pinv2.Y(),Pinv2.Z(),Einv2);

    if(Minv1 >= 0. && INV1.M() < Minv1)
      INV1.SetVectM(Pinv1,Minv1);
    if(Minv2 >= 0. && INV2.M() < Minv2)
      INV2.SetVectM(Pinv2,Minv2);

    Booster.setBoostVector(Pvis1+Pvis2+INV);
    Booster.boost(INV1);
    Booster.boost(INV2);

    GetChildState(0).SetFourVector(INV1);
    GetChildState(1).SetFourVector(INV2);
    
    return SetSpirit(true);
  }

  ContraBoostInvJigsaw ContraBoostInvJigsaw::m_Empty;

}
