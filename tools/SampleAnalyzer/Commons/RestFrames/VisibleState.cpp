/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   VisibleState.cc
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

#include "SampleAnalyzer/Commons/RestFrames/VisibleState.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"

namespace RestFrames {

  VisibleState::VisibleState(const std::string& sname, 
			     const std::string& stitle)
    : State(sname, stitle) 
  {
    m_Type = kVisibleState;
    m_FramePtr = nullptr;
  }

  VisibleState::VisibleState() : State() {}

  VisibleState::~VisibleState() {}

  void VisibleState::Clear(){
    m_FramePtr = nullptr;
    State::Clear();
  }

  VisibleState& VisibleState::Empty(){
    return VisibleState::m_Empty;
  }

  void VisibleState::AddFrame(const RestFrame& frame){
    if(IsEmpty()) return;
    
    if(!frame) return;
    if(!frame.IsVisibleFrame() || !frame.IsRecoFrame()) return;
    m_FramePtr = static_cast<const VisibleRecoFrame*>(&frame);
    m_Frames.Clear();
    m_Frames += frame;
  }

  bool VisibleState::IsFrame(const RestFrame& frame) const {
    if(!frame) return false;
    if(!m_FramePtr) return false;
    return *m_FramePtr == frame;
  }

  bool VisibleState::IsFrames(const ConstRestFrameList& frames) const {
    return IsFrame(frames[0]);
  }

  RestFrame const& VisibleState::GetFrame() const {
    if(m_FramePtr) 
      return *m_FramePtr;
    else
      return RestFrame::Empty();
  }

  void VisibleState::SetLabFrameFourVector(){
    if(!m_FramePtr) return;
    SetFourVector(m_FramePtr->GetLabFrameFourVector());
    SetCharge(m_FramePtr->GetCharge());
  }

  void VisibleState::SetCharge(const RFCharge& charge){
    m_Charge = charge;
  }
  
  void VisibleState::SetCharge(int charge){
    m_Charge = charge;
  }
  
  void VisibleState::SetCharge(int charge_num, int charge_den){
    m_Charge = RFCharge(charge_num, charge_den);
  }

  VisibleState VisibleState::m_Empty;
}
