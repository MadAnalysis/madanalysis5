/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   State.cc
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

#include "SampleAnalyzer/Commons/RestFrames/State.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleState.h"
#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // State class
  ///////////////////////////////////////////////
  int State::m_class_key = 0;

  // constructor 
  State::State(const std::string& sname, 
	       const std::string& stitle)
    : RFBase(sname, stitle, State::m_class_key++) 
  {
    m_Log.SetSource("State "+GetName());
    m_Type = kVanillaState;
    m_ParentJigsawPtr = nullptr;
    m_ChildJigsawPtr = nullptr;
    m_P.SetPxPyPzE(0.,0.,0.,0.);
    m_Charge = 0;
  }

  State::State() : RFBase() { 
    m_Type = kVanillaState; 
    m_Log.SetSource("State "+GetName());
  }

  State::~State() {}

  void State::Clear(){
    m_ParentJigsawPtr = nullptr;
    m_ChildJigsawPtr = nullptr;
    m_P.SetPxPyPzE(0.,0.,0.,0.);
    m_Frames.Clear();
  }

  State& State::Empty(){
    return VisibleState::Empty();
  }

  StateList const& State::EmptyList(){
    return m_EmptyList;
  }

  /// \brief Returns State (*StateType*) type 
  StateType State::GetType() const {
    return m_Type;
  }
    
  /// \brief Is this a VisibleState? (yes/no)
  bool State::IsVisibleState() const {
    return m_Type == kVisibleState;
  }
    
  /// \brief Is this an InvisibleState? (yes/no)
  bool State::IsInvisibleState() const {
    return m_Type == kInvisibleState;
  }

  /// \brief Is this a CombinatoricState? (yes/no)
  bool State::IsCombinatoricState() const {
    return m_Type == kCombinatoricState;
  }

  void State::AddFrames(const ConstRestFrameList& frames){
    int N = frames.GetN();
    for(int i = 0; i < N; i++)
      AddFrame(frames[i]);
  }

  ConstRestFrameList const& State::GetListFrames() const {
    return m_Frames;
  }

  int State::GetNFrames() const {
    return m_Frames.GetN();
  }

  bool State::IsFrame(const RestFrame& frame) const {
    if(!frame) return false;
    if(m_Frames.GetN() != 1) return false;
    return m_Frames[0] == frame;
  }

  bool State::IsFrames(const ConstRestFrameList& frames) const {
    return m_Frames == frames;
  }

  void State::SetParentJigsaw(Jigsaw& jigsaw){
    if(IsEmpty()) return;
    
    if(!jigsaw)
      m_ParentJigsawPtr = nullptr;
    else
      m_ParentJigsawPtr = &jigsaw;
  }

  void State::SetChildJigsaw(Jigsaw& jigsaw){
    if(IsEmpty()) return;
    
    if(!jigsaw)
      m_ChildJigsawPtr = nullptr;
    else
      m_ChildJigsawPtr = &jigsaw;
  }

  Jigsaw& State::GetParentJigsaw() const { 
    if(m_ParentJigsawPtr)
      return *m_ParentJigsawPtr;
    else 
      return Jigsaw::Empty();
  }
  
  Jigsaw& State::GetChildJigsaw() const { 
    if(m_ChildJigsawPtr)
      return *m_ChildJigsawPtr;
    else
      return Jigsaw::Empty();
  }

  void State::Boost(const MA5::MAVector3& B){
    MA5::MABoost Booster;
    Booster.setBoostVector(B.X(), B.Y(), B.Z());
    Booster.boost(m_P);
  }

  void State::SetFourVector(const MA5::MALorentzVector& V){
    m_P = V;
  }

  MA5::MALorentzVector State::GetFourVector() const {
    return m_P;
  }

  RFCharge State::GetCharge() const {
    return m_Charge;
  }

  const StateList State::m_EmptyList;

}
