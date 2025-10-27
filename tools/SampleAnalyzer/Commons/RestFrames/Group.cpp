/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   Group.cc
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

#include "SampleAnalyzer/Commons/RestFrames/Group.h"
#include "SampleAnalyzer/Commons/RestFrames/InvisibleGroup.h"
#include "SampleAnalyzer/Commons/RestFrames/ReconstructionFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/State.h"

namespace RestFrames {

  int Group::m_class_key = 0;

  Group::Group(const std::string& sname, 
	       const std::string& stitle)
    : RFBase(sname, stitle, Group::m_class_key++)
  {
    m_Type = kVanillaGroup;
    m_GroupStatePtr = nullptr;
    m_Log.SetSource("Group "+GetName());
  }

  Group::Group() : RFBase() { 
    m_Type = kVanillaGroup; 
    m_Log.SetSource("Group "+GetName());
  }

  Group::~Group(){
    Clear();
  }

  Group& Group::Empty(){
    return InvisibleGroup::Empty();
  }

  void Group::Clear(){
    SetBody(false);
    
    m_GroupStatePtr = nullptr;
    
    RemoveFrames();
    RemoveJigsaws();

    m_States.Clear();
    m_StatesToResolve.Clear();
    m_JigsawsToUse.Clear(); 
  }

  bool Group::IsInvisibleGroup() const{
    return m_Type == kInvisibleGroup;
  }
  
  bool Group::IsCombinatoricGroup() const{
    return m_Type == kCombinatoricGroup;
  }

  void Group::AddFrame(RestFrame& frame){
    if(IsEmpty()) return;
    if(!frame) return;
    if(!frame.IsRecoFrame()) return;
    SetBody(false);

    static_cast<ReconstructionFrame&>(frame).SetGroup(*this);
    m_Frames.Add(frame);
  }

  void Group::AddFrames(const RestFrameList& frames){
    int N = frames.GetN();
    for(int i = 0; i < N; i++)
      AddFrame(frames[i]);
  }

  void Group::AddJigsaw(Jigsaw& jigsaw){
    if(IsEmpty()) return;
    if(!jigsaw) return;

    if(!jigsaw.GetGroup().IsEmpty()){
      if(jigsaw.GetGroup() == *this) 
	return;
      Group& group = jigsaw.GetGroup();
      jigsaw.SetGroup();
      group.RemoveJigsaw(jigsaw);
    } 
      
    SetBody(false);

    if(m_JigsawsToUse.Add(jigsaw))
      jigsaw.SetGroup(*this);
  }

  void Group::RemoveFrame(RestFrame& frame){
    if(!m_Frames.Contains(frame)) 
      return;
   
    SetBody(false);

    static_cast<ReconstructionFrame&>(frame).SetGroup();
    m_Frames.Remove(frame);
  }

  void Group::RemoveFrames(){
    int N = m_Frames.GetN();
    for(int i = N-1; i >= 0; i--){
      RemoveFrame(m_Frames[i]);
    }
  }

  void Group::RemoveJigsaw(Jigsaw& jigsaw){
    if(!m_Jigsaws.Contains(jigsaw) &&
       !m_JigsawsToUse.Contains(jigsaw))
      return;
      
    SetBody(false);

    jigsaw.SetGroup();
    m_Jigsaws.Remove(jigsaw);
    m_JigsawsToUse.Remove(jigsaw);
  }

  void Group::RemoveJigsaws(){
    int N = m_Jigsaws.GetN();
    for(int i = N-1; i >= 0; i--){
      RemoveJigsaw(m_Jigsaws[i]);
    }
    N = m_JigsawsToUse.GetN();
    for(int i = N-1; i >= 0; i--){
      RemoveJigsaw(m_JigsawsToUse[i]);
    }
  }

  bool Group::ContainsFrame(const RestFrame& frame) const {
    return m_Frames.Contains(frame);
  }

  int Group::GetNFrames() const {
    return m_Frames.GetN();
  }

  const RestFrameList& Group::GetListFrames() const {
    return m_Frames;
  }

  const JigsawList& Group::GetListJigsaws() const {
    return m_Jigsaws;
  }

  State& Group::GetParentState() const {
    if(m_GroupStatePtr)
      return *m_GroupStatePtr;
    else
      return State::Empty();
  }

  bool Group::InitializeAnalysis(){
    m_Log << LogVerbose;
    m_Log << "Initializing Group for analysis...";
    m_Log << LogEnd;

    m_GroupStatePtr = &InitializeParentState();
    m_GroupStatePtr->AddFrames(m_Frames);

    if(!ResolveUnknowns()){
      m_Log << LogWarning;
      m_Log << "Unable to resolve unknowns associated with ";
      m_Log << "Frames in this Group with available Jigsaws";
      m_Log << LogEnd;
      return SetBody(false);
    }

    m_Log << LogVerbose;
    m_Log << "...Done initializing group for analysis" << LogEnd;
    SetBody(true);
    return SetMind(true);
  }

  bool Group::ResolveUnknowns(){
    m_JigsawsToUse += m_Jigsaws;
    m_Jigsaws.Clear();

    int Njigsaw = m_JigsawsToUse.GetN();
    for(int i = 0; i < Njigsaw; i++){
      if(m_JigsawsToUse[i].GetNChildren() == 1){
	m_JigsawsToUse[i].RemoveFrames(m_Frames);
	int N = GetNFrames();
	for(int j = 0; j < N; j++)
	  m_JigsawsToUse[i].AddChildFrame(m_Frames[j]);
      }
    }

    m_States.Clear();
    m_StatesToResolve.Clear();
    m_States.Add(*m_GroupStatePtr);
    m_StatesToResolve.Add(*m_GroupStatePtr);

    while(m_StatesToResolve.GetN() > 0){
      State& state = m_StatesToResolve.Get(0);
      if(!ResolveState(state)){
	if(state.GetNFrames() != 1){
	  m_Log << LogWarning;
	  m_Log << "Cannot find Jigsaw to Resolve State for frames:";
	  m_Log << std::endl << "   " << Log(state.GetListFrames());
	  m_Log << LogEnd;
	  return false; 
	}
	m_StatesToResolve.Remove(state);
      }
    }
    return true;
  }

  bool Group::ResolveState(const State& state){
    Jigsaw* jigsawSolutionPtr = nullptr;

    int N = m_JigsawsToUse.GetN();
    for(int i = 0; i < N; i++){
      Jigsaw* jigsawPtr = &m_JigsawsToUse[i];
      if(jigsawPtr->CanResolve(state)){
	if(!jigsawSolutionPtr){
	  jigsawSolutionPtr = jigsawPtr;
	  continue;
	}
	if(jigsawPtr->GetNChildren() <= jigsawSolutionPtr->GetNChildren())
	  if((jigsawPtr->GetNChildren() < jigsawSolutionPtr->GetNChildren()) ||
	     (jigsawPtr->GetNDependancyStates() < jigsawSolutionPtr->GetNDependancyStates())) 
	    jigsawSolutionPtr = jigsawPtr;
      }
    }
    if(!jigsawSolutionPtr)
      return false;
    
    m_Log << LogVerbose;
    m_Log << "Found Jigsaw to resolve State:" << std::endl; 
    m_Log << " Frames:" << std::endl << "   ";
    m_Log << Log(state.GetListFrames()) << std::endl;
    m_Log << " Jigsaw:" << Log(jigsawSolutionPtr);
    m_Log << LogEnd;
    
    if(!InitializeJigsaw(*jigsawSolutionPtr))
      return false;

    m_JigsawsToUse.Remove(*jigsawSolutionPtr);
    return true;
  }

  bool Group::InitializeJigsaw(Jigsaw& jigsaw){
    State& state = m_StatesToResolve[0];
    jigsaw.SetParentState(state);

    if(!jigsaw.InitializeTree()){
      m_Log << LogWarning;
      m_Log << "Unable to initialize Jigsaw:";
      m_Log << Log(jigsaw) << LogEnd;
      return false;
    }

    m_States += jigsaw.GetChildStates();
    m_StatesToResolve -= state;
    m_StatesToResolve += jigsaw.GetChildStates();
    m_Jigsaws += jigsaw;

    return true;
  }
 
  int Group::GetNChildStates() const {
    return m_States.GetN();
  }

  State& Group::GetChildState(int i) const {
    if(i < 0 || i > GetNChildStates()-1)
      return State::Empty();
    else 
      return m_States[i];
  }

  State& Group::GetChildState(const RestFrame& frame) const {
    if(!frame) return State::Empty();
    int Ns = GetNChildStates();
    for(int i = Ns-1; i >= 0; i--)
      if(m_States[i].IsFrame(frame))
	return m_States[i];
    SetMind(false);
    return State::Empty();
  }
  
  StateList Group::GetChildStates(const RestFrameList& frames) const {
    // Find States that correspond to these frames, giving
    // preference to States that include more frames (less dependancies)
    StateList states;
    int Ns = m_States.GetN();
    for(int i = 0; i < Ns; i++){
      RestFrameList iframes = m_States[i].GetListFrames();
      if(frames.Contains(iframes)){
	int Nsol = states.GetN();
	bool isnew = true;
	for(int j = 0; j < Nsol; j++){
	  // if new copy of existing frame list appears, discard old
	  RestFrameList jframes = states[j].GetListFrames();
	  if(iframes.Contains(jframes)){
	    states.Remove(states[j]);
	    break;
	  }
	  // if superset of existing frame list appears, discard new
	  if(jframes.Contains(iframes)){
	    isnew = false;
	    break;
	  }
	}
	if(isnew) states.Add(m_States[i]);
      }
    }

    RestFrameList match_frames;
    Ns = states.GetN();
    for(int i = 0; i < Ns; i++)
      match_frames += states[i].GetListFrames();
    
    if(!(frames == match_frames)){
      m_Log << LogWarning;
      m_Log << "Unable to find States corresponding to Frames: " << std::endl;
      m_Log << Log(frames) << LogEnd;
      SetMind(false);
      return StateList();
    }
    return states;
  }

  RestFrame const& Group::GetLabFrame() const {
    if(m_Frames.GetN() < 1)
      return RestFrame::Empty();
    else
      return m_Frames[0].GetLabFrame();
  }

}
