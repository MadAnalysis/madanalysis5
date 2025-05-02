/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   Jigsaw.cc
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

#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/InvisibleJigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/LabRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/State.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // Jigsaw class methods
  ///////////////////////////////////////////////
  int Jigsaw::m_class_key = 0;

  Jigsaw::Jigsaw(const std::string& sname, 
		 const std::string& stitle,
		 int Nchild, int Ndependancy)
    : RFBase(sname, stitle, Jigsaw::m_class_key++),
      m_Nchild(Nchild), m_Ndeps(Ndependancy)
  {
    m_Log.SetSource("Jigsaw "+GetName());
    m_GroupPtr = nullptr;
    m_ParentStatePtr = nullptr;
    for(int i = 0; i < m_Nchild; i++){
      m_ChildFrames.push_back(ConstRestFrameList());
    }
    
    for(int i = 0; i < m_Ndeps; i++){
      m_DependancyFrames.push_back(ConstRestFrameList());
      m_DependancyStates.push_back(StateList());
    }
  }

  Jigsaw::Jigsaw() : RFBase(), m_Nchild(0), m_Ndeps(0){ 
    m_Type = kVanillaJigsaw; 
    m_Log.SetSource("Jigsaw "+GetName());
  }

  Jigsaw::~Jigsaw(){
    Clear();
  }

  Jigsaw& Jigsaw::Empty(){
    return InvisibleJigsaw::Empty();
  }

  void Jigsaw::Clear(){
    SetBody(false);
    SetGroup();
    SetParentState();

    for(int i = 0; i < m_Nchild; i++){
      m_ChildFrames[i].Clear();
      m_ChildStates[i].Clear();
    }

    for(int i = 0; i < m_Ndeps; i++){
      m_DependancyStates[i].Clear();
      m_DependancyFrames[i].Clear();
    }

    m_DependancyJigsaws.Clear();
  }

  bool Jigsaw::IsInvisibleJigsaw() const {
    return m_Type == kInvisibleJigsaw;
  }

  bool Jigsaw::IsCombinatoricJigsaw() const {
    return m_Type == kCombinatoricJigsaw;
  }

  std::string Jigsaw::PrintString(LogType type) const {
    std::string output = RFBase::PrintString(type);
    if(IsInvisibleJigsaw())
      output += "   Type: Invisible \n";
    if(IsCombinatoricJigsaw())
      output += "   Type: Combinatoric \n";
    return output;
  }

  void Jigsaw::SetGroup(Group& group){
    if(IsEmpty()) return;
    
    SetBody(false);
    SetParentState();
    
    if(m_GroupPtr){
      if(*m_GroupPtr == group){
	return;
      } else {
	Group* groupPtr = m_GroupPtr;
	m_GroupPtr = nullptr;
	groupPtr->RemoveJigsaw(*this);
      }
    }
    if(!group)
      m_GroupPtr = nullptr;
    else
      m_GroupPtr = &group;
  }

  Group& Jigsaw::GetGroup() const {
    if(m_GroupPtr)
      return *m_GroupPtr;
    else 
      return Group::Empty();
  }

  bool Jigsaw::CanResolve(const ConstRestFrameList& frames) const {
    if(!IsSoundBody())
      return SetBody(false);

    return GetParentFrames() == frames;
  }

  bool Jigsaw::CanResolve(const State& state) const {
    if(!state) return false;
    return CanResolve(state.GetListFrames());
  }

  void Jigsaw::SetParentState(){
    SetParentState(State::Empty());
  }
  
  void Jigsaw::SetParentState(State& state){
    if(IsEmpty()) return;
    
    SetBody(false);
    if(m_ParentStatePtr){
      if(*m_ParentStatePtr == state)
	return;
      else {
	m_ParentStatePtr = nullptr;
      } 
    }
    if(!state)
      m_ParentStatePtr = nullptr;
    else {
      m_ParentStatePtr = &state;
      state.SetChildJigsaw(*this);
    } 
  }

  State const& Jigsaw::GetParentState() const {
    if(m_ParentStatePtr)
      return *m_ParentStatePtr;
    else
      return State::Empty();
  }

  int Jigsaw::GetNChildren() const {
    return m_Nchild;
  }

  int Jigsaw::GetNDependancyStates() const {
    return m_Ndeps;
  }

  State& Jigsaw::GetChildState(int i) const {
    return m_ChildStates[i];
  }

  StateList const& Jigsaw::GetChildStates() const {
    return m_ChildStates;
  }

  StateList const& Jigsaw::GetDependancyStates(int i) const {
    if(i < 0 || i >= m_Ndeps)
      return State::EmptyList();
    return m_DependancyStates[i];
  }

  ConstRestFrameList Jigsaw::GetParentFrames() const {
    ConstRestFrameList frames;
    for(int i = 0; i < m_Nchild; i++)
      frames += GetChildFrames(i);
    return frames;
  }

  ConstRestFrameList const& Jigsaw::GetChildFrames(int i) const {
    if(i < 0 || i >= m_Nchild) 
      return RestFrame::EmptyList();
    return m_ChildFrames[i];
  }

  ConstRestFrameList const& Jigsaw::GetDependancyFrames(int i) const {
    if(i < 0 || i >= m_Ndeps) 
      return RestFrame::EmptyList();
    return m_DependancyFrames[i];
  }

  bool Jigsaw::InitializeTree(){
    if(!IsSoundBody())
      return SetMind(false);

    if(!GetParentState()){
      m_Log << LogWarning;
      m_Log << "Unable to initialize Jigsaw. ";
      m_Log << "No parent State set." << LogEnd;
      return SetMind(false);
    }

    if(!CanResolve(GetParentState())){
      m_Log << LogWarning;
      m_Log << "Unable to resolve input parent State. ";
      m_Log << "  Frames (capable): " << Log(GetParentFrames()) << std::endl;
      m_Log << "  Frames (requested): " << Log(GetParentState().GetListFrames());
      m_Log << LogEnd;
      return SetMind(false);
    }
    
    for(int i = 0; i < m_Nchild; i++){
      m_ChildStates[i].Clear();
      m_ChildStates[i].SetParentJigsaw(*this);
      m_ChildStates[i].AddFrames(GetChildFrames(i));
    }
 
    return SetMind(true);
  }
  
  bool Jigsaw::InitializeAnalysis(){
    if(!IsSoundMind())
      return SetMind(false);

    if(!GetGroup())
      return SetMind(false);

    // get list of states and groups from lab frame
    const LabRecoFrame& lab_frame = 
      static_cast<const LabRecoFrame&>(GetGroup().GetLabFrame());
    StateList states = lab_frame.GetTreeStates();
    GroupList groups = lab_frame.GetListGroups();

    int Ngroup = groups.GetN();
    std::vector<ConstRestFrameList> group_frames;
    for(int i = 0; i < Ngroup; i++)
      group_frames.push_back(ConstRestFrameList());

    for(int d = 0; d < m_Ndeps; d++){
      m_DependancyStates[d].Clear();

      for(int i = 0; i < Ngroup; i++)
	group_frames[i].Clear();
     
      int Nf = m_DependancyFrames[d].GetN();
      for(int f = 0; f < Nf; f++){
	const RestFrame& frame = m_DependancyFrames[d][f];

	bool no_group = true;
	for(int g = 0; g < Ngroup; g++){
	  if(groups[g].ContainsFrame(frame)){
	    group_frames[g].Add(frame);
	    no_group = false;
	    break;
	  }
	}

	if(no_group){
	  State& state = states.GetFrame(frame);
	  if(!state){
	    m_Log << LogWarning;
	    m_Log << "Cannot find State corresponding to frame: " << std::endl;
	    m_Log << Log(frame) << " " << Log(states) << LogEnd;
	    return SetMind(false);
	  }
	  m_Log << LogVerbose;
	  m_Log << "Adding dependancy State for index " << d;
	  m_Log << " corresponding to frame:";
	  m_Log << Log(frame) << LogEnd;
	  m_DependancyStates[d] += state;
	}
      }
      for(int g = 0; g < Ngroup; g++){
	if(group_frames[g].GetN() == 0) continue;
	StateList group_states = groups[g].GetChildStates(group_frames[g]);
	if(group_states.GetN() == 0){
	  m_Log << "Cannot find States in Group:" << std::endl;
	  m_Log << " Frames:" << std::endl << "   ";
	  m_Log << Log(group_frames[g]) << std::endl;
	  m_Log << " Group:" << std::endl;
	  m_Log << Log(groups.Get(g)) << LogEnd;
	  return SetMind(false);
	}
	m_Log << LogVerbose;
	m_Log << "Sucessfully found dependancy States for index " << d << std::endl;
	int Ns = group_states.GetN();
	m_Log << " Frames:" << std::endl << "   ";
	m_Log << Log(group_frames[g]) << std::endl;
	m_Log << " States:" << std::endl;
	for(int s = 0; s < Ns; s++){
	  m_Log << "   state " << s << ": ";
	  m_Log << Log(group_states[s].GetListFrames()) << std::endl;
	}
	m_Log << LogEnd;
	m_DependancyStates[d] += group_states;
      }
    }
    
    return SetMind(true);
  } 

  bool Jigsaw::InitializeDependancyJigsaws(){
    if(!IsSoundMind()) 
      return false;

    m_DependancyJigsaws.Clear();

    JigsawList jigsaws;
    FillStateJigsawDependancies(jigsaws);
    jigsaws -= *this;
    m_DependancyJigsaws.Add(jigsaws);

    jigsaws.Clear();
    FillGroupJigsawDependancies(jigsaws);
    jigsaws -= *this;
    m_DependancyJigsaws += jigsaws;

    return true;
  }

  void Jigsaw::AddChildFrame(const RestFrame& frame, int i){
    if(!frame) return;
    if(!frame.IsRecoFrame()) return;
    if(i < 0 || i >= m_Nchild) return;
    
    SetBody(false);
   
    m_ChildFrames[i] += frame;
  }

  void Jigsaw::AddDependancyFrame(const RestFrame& frame, int i){
    if(!frame) return;
    if(!frame.IsRecoFrame()) return;
    if(i < 0 || i >= m_Ndeps) return;

    SetBody(false);

    m_DependancyFrames[i] += frame;
  }

  void Jigsaw::RemoveFrame(const RestFrame& frame){
    if(!frame) return;

    SetBody(false);

    for(int i = 0; i < m_Nchild; i++)
      m_ChildFrames[i].Remove(frame);
    
    for(int i = 0; i < m_Ndeps; i++)
      m_DependancyFrames[i].Remove(frame);
  }

  void Jigsaw::RemoveFrames(const ConstRestFrameList& frames){
    int N = frames.GetN();
    for(int i = 0; i < N; i++)
      RemoveFrame(frames[i]);
  }

  bool Jigsaw::IsSoundBody() const {
    if(RFBase::IsSoundBody()) 
      return true;

    for(int i = 0; i < m_Nchild-1; i++){
      for(int j = i+1; j < m_Nchild; j++){
	if(m_ChildFrames[i].Intersection(m_ChildFrames[j]).GetN() > 0){
	  m_Log << LogWarning;
	  m_Log << "Child frames are repeated between ";
	  m_Log << "more than one output index: ";
	  m_Log << Log(m_ChildFrames[i].Intersection(m_ChildFrames[j]));
	  m_Log << LogEnd;
	  return SetBody(false);
	}
      }
    }
    for(int i = 0; i < m_Nchild; i++){
      if(m_ChildFrames[i].GetN() == 0){
	m_Log << LogWarning;
	m_Log << "No child frames at index ";
	m_Log << i << LogEnd;
	return SetBody(false);
      }
    }

    return SetBody(true);
  }

  bool Jigsaw::DependsOnJigsaw(const Jigsaw& jigsaw) const {
    return m_DependancyJigsaws.Contains(jigsaw);
  }

  void Jigsaw::FillGroupJigsawDependancies(JigsawList& jigsaws) const {
    if(jigsaws.Contains(*this)) return;
    jigsaws.Add((Jigsaw&)(*m_This));
    if(m_ParentStatePtr)
      m_ParentStatePtr->GetParentJigsaw().FillGroupJigsawDependancies(jigsaws);
  }

  void Jigsaw::FillStateJigsawDependancies(JigsawList& jigsaws) const {
    if(jigsaws.Contains(*this)) return;
    jigsaws.Add((Jigsaw&)(*m_This));

    for(int i = 0; i < m_Ndeps; i++){
      int M = m_DependancyStates[i].GetN();
      for(int j = 0; j < M; j++){
	m_DependancyStates[i][j].GetParentJigsaw().
	  FillStateJigsawDependancies(jigsaws);
      }
    } 
  }

}
