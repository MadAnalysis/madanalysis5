/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   InvisibleJigsaw.cc
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

#include "SampleAnalyzer/Commons/RestFrames/InvisibleJigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/SetMassInvJigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // InvisibleJigsaw class
  ///////////////////////////////////////////////

  InvisibleJigsaw::InvisibleJigsaw(const std::string& sname, 
				   const std::string& stitle, 
				   int Ninvisible, int Nvisible) : 
    Jigsaw(sname, stitle, Ninvisible, 2*Nvisible),
    m_Nvis(Nvisible), m_Ninv(Ninvisible)
  {
    m_Type = kInvisibleJigsaw;
    m_InvMassDependancy = false;
    for(int i = 0; i < m_Ninv; i++){
      m_ChildStates += GetNewChildState();
    }
  }

  InvisibleJigsaw::InvisibleJigsaw() 
    : Jigsaw(), m_Nvis(0), m_Ninv(0) {}

  InvisibleJigsaw::~InvisibleJigsaw() {}

  void InvisibleJigsaw::Clear(){
    Jigsaw::Clear();
  }

  InvisibleJigsaw& InvisibleJigsaw::Empty(){
    return SetMassInvJigsaw::Empty();
  }

  void InvisibleJigsaw::SetGroup(Group& group){
    if(!group) return;
    if(!group.IsInvisibleGroup()) return;
    Jigsaw::SetGroup(group);
  }

  InvisibleGroup& InvisibleJigsaw::GetGroup() const {
    if(!Jigsaw::GetGroup())
      return InvisibleGroup::Empty();
    else
      return static_cast<InvisibleGroup&>(Jigsaw::GetGroup());
  }

  void InvisibleJigsaw::SetParentState(State& state){
    if(!state) return;
    if(!state.IsInvisibleState()) return;
    Jigsaw::SetParentState(state);
  }

  InvisibleState const& InvisibleJigsaw::GetParentState() const {
    if(!Jigsaw::GetParentState())
      return InvisibleState::Empty();
    else
      return static_cast<const InvisibleState&>(Jigsaw::GetParentState());
  }

  InvisibleState& InvisibleJigsaw::GetChildState(int i) const{
    if(!Jigsaw::GetChildState(i)) 
      return InvisibleState::Empty();
    else
      return static_cast<InvisibleState&>(Jigsaw::GetChildState(i));
  }

  void InvisibleJigsaw::AddVisibleFrame(const RestFrame& frame, int i){
    if(!frame) return;
    if(i < 0 || i >= m_Nvis) return;

    ConstRestFrameList frames = frame.GetListVisibleFrames();
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddDependancyFrame(frames[f], i);
  }

  void InvisibleJigsaw::AddVisibleFrames(const ConstRestFrameList& frames, int i){
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddVisibleFrame(frames[f], i);
  }

  void InvisibleJigsaw::AddMassFrame(const RestFrame& frame, int i){
    if(!frame) return;
    if(i < 0 || i >= m_Nvis) return;
   
    ConstRestFrameList frames = frame.GetListVisibleFrames();
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddDependancyFrame(frames[f], m_Nvis + i);
  }

  void InvisibleJigsaw::AddMassFrames(const ConstRestFrameList& frames, int i){
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddMassFrame(frames[f], i);
  }

  void InvisibleJigsaw::AddInvisibleFrame(const RestFrame& frame, int i){
    if(!frame) return;
    if(i < 0 || i >= m_Ninv) return;
   
    ConstRestFrameList frames = frame.GetListInvisibleFrames();
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddChildFrame(frames[f], i);
  }

  void InvisibleJigsaw::AddInvisibleFrames(const ConstRestFrameList& frames, int i){
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddInvisibleFrame(frames[f], i);
  }

  InvisibleState& InvisibleJigsaw::GetNewChildState(){
    char strn[10];
    snprintf(strn, sizeof(strn), "%d",GetNChildren());
    std::string name = GetName()+"_"+std::string(strn);
    InvisibleState* statePtr = new InvisibleState(name, name);
    AddDependent(statePtr);
    return *statePtr;
  }

  double InvisibleJigsaw::GetMinimumMass() const {
    if(!IsSoundMind()) return 0.;

    int N = GetNChildren();
    double M = 0.;
    for(int i = 0; i < N; i++)
      M += std::max(0., GetChildState(i).GetMinimumMass());
    return M;
  }

   bool InvisibleJigsaw::InitializeAnalysis(){
    if(!IsSoundMind())
      return SetMind(false);

    for(int v = 0; v < m_Nvis; v++)
      if(GetDependancyFrames(m_Nvis + v).GetN() == 0)
	AddMassFrames(GetDependancyFrames(v), v);

    SetMind(true);
    return Jigsaw::InitializeAnalysis();
  }

  void InvisibleJigsaw::FillInvisibleMassJigsawDependancies(JigsawList& jigsaws) const {
    if(m_InvMassDependancy){
      for(int i = m_Nvis; i < 2*m_Nvis; i++){
	int M = GetDependancyStates(i).GetN();
	for(int j = 0; j < M; j++){
	  GetDependancyStates(i)[j].GetParentJigsaw().
	    FillGroupJigsawDependancies(jigsaws);
	}
      }
    }
    for(int i = 0; i < m_Ninv; i++)
      static_cast<const InvisibleJigsaw&>(GetChildState(i).GetChildJigsaw()).
	FillInvisibleMassJigsawDependancies(jigsaws);
  }

  bool InvisibleJigsaw::InitializeDependancyJigsaws(){
    if(!IsSoundMind()) return false;
    
    m_DependancyJigsaws.Clear();

    JigsawList jigsaws; 
    FillStateJigsawDependancies(jigsaws);
    jigsaws -= *this;
    m_DependancyJigsaws += jigsaws;

    jigsaws.Clear();
    FillInvisibleMassJigsawDependancies(jigsaws);
    jigsaws -= *this;
    m_DependancyJigsaws += jigsaws;

    jigsaws.Clear();
    FillGroupJigsawDependancies(jigsaws);
    jigsaws -= *this;
    m_DependancyJigsaws += jigsaws;

    return true;
  }

  bool InvisibleJigsaw::InitializeJigsawExecutionList(JigsawList& exec_jigsaws){
    if(!IsSoundMind()) return false;
    if(exec_jigsaws.Contains(*this)) return true;

    // Add group dependancy jigsaws first
    JigsawList group_jigsaws;
    FillGroupJigsawDependancies(group_jigsaws);
    group_jigsaws -= *this;

    int Ngroup = group_jigsaws.GetN();
    for(int i = Ngroup-1; i >= 0; i--){
      Jigsaw& jigsaw = group_jigsaws[i];
      if(!exec_jigsaws.Contains(jigsaw))
	if(!jigsaw.InitializeJigsawExecutionList(exec_jigsaws))
	  return SetMind(false);
      m_DependancyJigsaws -= jigsaw;
    }
    // Satisfy dependancy jigsaws
    while(m_DependancyJigsaws.GetN() > 0){
      Jigsaw& jigsaw = m_DependancyJigsaws[m_DependancyJigsaws.GetN()-1];
      if(exec_jigsaws.Contains(jigsaw)){
	m_DependancyJigsaws -= jigsaw;
	continue;
      }
      if(jigsaw.DependsOnJigsaw(*this)) 
	m_DependancyJigsaws -= jigsaw;
      if(!jigsaw.InitializeJigsawExecutionList(exec_jigsaws))
	return SetMind(false);
      m_DependancyJigsaws -= jigsaw;
    }

    exec_jigsaws += *this;
    return true;
  }

  bool InvisibleJigsaw::IsSoundBody() const {
    if(RFBase::IsSoundBody())
      return true;

    if(!Jigsaw::IsSoundBody())
      return SetBody(false);
    
    for(int i = 0; i < m_Nvis; i++){
      if(GetDependancyFrames(i).GetN() <= 0){
	m_Log << LogWarning;
	m_Log << "Empty collection of visible frames: " << i;
	m_Log << LogEnd;
	return SetBody(false);
      }
    }

    return SetBody(true);
  }

}
