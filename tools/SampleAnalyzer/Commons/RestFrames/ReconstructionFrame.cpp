/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   ReconstructionFrame.cc
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

#include "SampleAnalyzer/Commons/RestFrames/ReconstructionFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/LabRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/Group.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleState.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // ReconstructionFrame class methods
  ///////////////////////////////////////////////
  ReconstructionFrame::ReconstructionFrame(const std::string& sname, 
					   const std::string& stitle)
    : RestFrame(sname, stitle)
  {
    m_Ana = kRecoFrame;
    m_GroupPtr = nullptr;
  }

  ReconstructionFrame::ReconstructionFrame()
    : RestFrame() {}

  ReconstructionFrame::~ReconstructionFrame(){ 
    Clear(); 
  }

  void ReconstructionFrame::Clear(){
    SetGroup();
    m_ChildStates.clear();
    RestFrame::Clear();
  }

  ReconstructionFrame& ReconstructionFrame::Empty(){
    return VisibleRecoFrame::Empty();
  }
  
  void ReconstructionFrame::AddChildFrame(RestFrame& frame){
    if(!frame) return;
    if(!frame.IsRecoFrame()) return;
    RestFrame::AddChildFrame(frame);
  }

  void ReconstructionFrame::RemoveChildFrame(RestFrame& frame){
    SetBody(false);
    bool contains = m_ChildFrames.Contains(frame);
    m_ChildBoosts.erase(&frame);
    m_ChildStates.erase(&frame);
    m_ChildFrames.Remove(frame);
    if(contains)
      frame.SetParentFrame();
  }

  void ReconstructionFrame::RemoveChildFrames(){
    SetBody(false);
    while(GetNChildren() > 0)
      RemoveChildFrame(m_ChildFrames[0]);
    m_ChildFrames.Clear();
    m_ChildBoosts.clear();
    m_ChildStates.clear();
  }

  void ReconstructionFrame::SetParentFrame(RestFrame& frame){
    if(!frame) return;
    if(!frame.IsRecoFrame()) return;
    RestFrame::SetParentFrame(frame);
  }

  ReconstructionFrame const& ReconstructionFrame::GetParentFrame() const {
    const RestFrame& frame = RestFrame::GetParentFrame();
    if(!frame.IsEmpty())
      return static_cast<const ReconstructionFrame&>(frame);
    else 
      return ReconstructionFrame::Empty();
  }

  ReconstructionFrame& ReconstructionFrame::GetChildFrame(int i) const {
    RestFrame& frame = RestFrame::GetChildFrame(i);
    if(!frame.IsEmpty())
      return static_cast<ReconstructionFrame&>(frame);
    else 
      return ReconstructionFrame::Empty();
  }

  StateList const& ReconstructionFrame::GetChildStates(int i) const {
    return GetChildStates(GetChildFrame(i));
  }
  
  StateList const& ReconstructionFrame::GetChildStates(const RestFrame& child) const {
    if(m_ChildStates.count(&child) <= 0)
      return State::EmptyList();

    return m_ChildStates[&child];
  }

  void ReconstructionFrame::SetGroup(Group& group){
    if(IsEmpty()) return;
    
    SetMind(false);

    if(m_GroupPtr){
      if(*m_GroupPtr != group){
	Group* groupPtr = m_GroupPtr;
	m_GroupPtr = nullptr;
	groupPtr->RemoveFrame(*this);
      }
    }
    if(!group)
      m_GroupPtr = nullptr;
    else
      m_GroupPtr = &group;
  }

  Group& ReconstructionFrame::GetGroup() const { 
    if(m_GroupPtr)
      return *m_GroupPtr;
    else 
      return Group::Empty();
  }

  GroupList ReconstructionFrame::GetListGroups() const {
    GroupList groups;
    FillListGroupsRecursive(groups);
    return groups;
  }

  void ReconstructionFrame::FillListGroupsRecursive(GroupList& groups) const {
    if(m_GroupPtr) groups.Add(*m_GroupPtr);
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++)
      GetChildFrame(i).FillListGroupsRecursive(groups);
  }

  bool ReconstructionFrame::InitializeVisibleStates(){
    m_ChildStates.clear();
    if(!GetLabFrame())
      return false;
      
    const VisibleStateList& states = 
      static_cast<const LabRecoFrame&>(GetLabFrame()).GetTreeStates();
   
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++){
      RestFrame& child = GetChildFrame(i);
      m_ChildStates[&child] = StateList();
      RFList<ReconstructionFrame> frames = child.GetListVisibleFrames();
      int Nf = frames.GetN();
      for(int f = 0; f < Nf; f++)
	if(!frames[f].GetGroup())
	  if(!m_ChildStates[&child].Add(StateList(states).GetFrame(frames[f]))){
	    m_Log << LogWarning;
	    m_Log << "Unable to associate State with Group-less Frame:";
	    m_Log << Log(frames[f]) << LogEnd;
	    return false;
	  }
    }
    return true;
  }

  bool ReconstructionFrame::InitializeGroupStates(){
    GroupList groups = GetListGroups();
    int Ngroup = groups.GetN();
    int Nchild = GetNChildren();

    for(int c = 0; c < Nchild; c++){
      RestFrameList frames = 
	GetChildFrame(c).GetListVisibleFrames() +
	GetChildFrame(c).GetListInvisibleFrames();
      int Nframe = frames.GetN();
      for(int f = 0; f < Nframe; f++){
	for(int g = 0; g < Ngroup; g++){
	  if(groups[g].ContainsFrame(frames[f])){
	    State& state = groups[g].GetChildState(frames[f]);
	    if(!state){
	      m_Log << LogWarning;
	      m_Log << "Unable to get State associated with Group Frame: " << std::endl;
	      m_Log << " Frame:" << Log(frames[f]);
	      m_Log << " Group:" << Log(groups[g]) << LogEnd;
	      return false;
	    }
	    m_ChildStates[&GetChildFrame(c)].Add(state);
	    break;
	  }
	}
      }
    }  
    return true;
  }

  bool ReconstructionFrame::InitializeAnalysisRecursive(){
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return SetMind(false);
    }

    if(!InitializeVisibleStates())
      return SetMind(false);
    if(!InitializeGroupStates())
      return SetMind(false);
   
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++)
      if(!GetChildFrame(i).InitializeAnalysisRecursive()){
	m_Log << LogWarning;
	m_Log << "Unable to recursively initialize analysis for frame:";
	m_Log << Log(GetChildFrame(i)) << LogEnd;
	return SetMind(false);
      }

    return SetMind(true);
  }

  bool ReconstructionFrame::ResetRecoFrame(){
    return true;
  }

  bool ReconstructionFrame::ClearEventRecursive(){ 
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return false;
    }
   
    if(!ResetRecoFrame())
      return false;

    int Nf =  GetNChildren();
    for(int i = 0; i < Nf; i++)
      if(!GetChildFrame(i).ClearEventRecursive())
	return false;

    return true;
  }

  bool ReconstructionFrame::ReconstructFrame(){
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetSpirit(false);
    }

    MA5::MALorentzVector Ptot(0,0,0,0);
    MA5::MABoost Booster;

    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++){
      ReconstructionFrame& child = GetChildFrame(i);
      MA5::MALorentzVector P = m_ChildStates[&child].GetFourVector();
      if(P.M() > 0.)
      {
        Booster.setBoostVector(P);
        SetChildBoostVector(child, Booster.BoostVector());
      }
      else
	SetChildBoostVector(child, m_Empty3Vector);
      Ptot += P;
      child.SetFourVector(P,*this);

      if(child.IsVisibleFrame())
	static_cast<VisibleRecoFrame&>(child).
	  SetCharge(m_ChildStates[&child].GetCharge());
    }

    if(IsLabFrame()) SetFourVector(Ptot,*this);

    return SetSpirit(true);
  }

  bool ReconstructionFrame::AnalyzeEventRecursive(){
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetSpirit(false);
    }
    if(!ReconstructFrame()){
      m_Log << LogWarning;
      m_Log << "Unable to reconstruct event for this frame.";
      m_Log << LogEnd;
      return SetSpirit(false);
    }

    int Nchild = GetNChildren();
    MA5::MABoost Booster;
    for(int i = 0; i < Nchild; i++){
      ReconstructionFrame& child = GetChildFrame(i);

      bool terminal = child.IsVisibleFrame() || child.IsInvisibleFrame();
      if(!terminal)
        m_ChildStates[&child].Boost(-GetChildBoostVector(child));
    
      if(!child.AnalyzeEventRecursive())
	return SetSpirit(false);
     
      if(!terminal)
        m_ChildStates[&child].Boost(-GetChildBoostVector(child));
    }

    return SetSpirit(true);
  }

}
