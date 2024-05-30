/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   SetRapidityInvJigsaw.cc
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

#include "SampleAnalyzer/Commons/RestFrames/SetRapidityInvJigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  SetRapidityInvJigsaw::SetRapidityInvJigsaw(const std::string& sname, 
					     const std::string& stitle) : 
    InvisibleJigsaw(sname, stitle, 1, 1)
  {
    m_Axis = RestFrame::GetAxis();
  }

  SetRapidityInvJigsaw::SetRapidityInvJigsaw() : InvisibleJigsaw() {}
 
  SetRapidityInvJigsaw::~SetRapidityInvJigsaw() {}

  void SetRapidityInvJigsaw::SetAxis(const MA5::MAVector3& axis){
    m_Axis = axis;
  }

  bool SetRapidityInvJigsaw::AnalyzeEvent(){
    if(!IsSoundMind()) 
      return SetSpirit(false);
    
    MA5::MALorentzVector inv_P = GetParentState().GetFourVector();
    MA5::MALorentzVector vis_P = GetDependancyStates(0).GetFourVector();

    MA5::MABoost Booster;
    Booster.setBoostVector(vis_P);
    MA5::MAVector3 boost_para = Booster.BoostVector().Dot(m_Axis.Unit())*m_Axis.Unit();
    Booster.setBoostVector(boost_para.X(), boost_para.Y(), boost_para.Z());

    //inv_P.SetZ(0.0);
    inv_P.SetPz(0.0);
    Booster.boost(inv_P);

    GetChildState(0).SetFourVector(inv_P);
    
    return SetSpirit(true);
  }

}
