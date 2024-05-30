/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   CombinatoricGroup.cc
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

#include "SampleAnalyzer/Commons/RestFrames/CombinatoricGroup.h"
#include "SampleAnalyzer/Commons/RestFrames/CombinatoricState.h"
#include "SampleAnalyzer/Commons/RestFrames/ReconstructionFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // CombinatoricGroup class
  // a combinatoric collection of particles
  ///////////////////////////////////////////////

  CombinatoricGroup::CombinatoricGroup(const std::string& sname,
				       const std::string& stitle) : 
    Group(sname, stitle)
  {
    m_Type = kCombinatoricGroup;
  }

  CombinatoricGroup::CombinatoricGroup() : Group() {}

  CombinatoricGroup::~CombinatoricGroup() {}

  CombinatoricGroup& CombinatoricGroup::Empty(){
    return CombinatoricGroup::m_Empty;
  }

  void CombinatoricGroup::Clear(){
    m_Elements.Clear();
    m_NElementsForFrame.clear();
    m_NExclusiveElementsForFrame.clear(); 
    m_InitStates.Clear();
    Group::Clear();
  }

  void CombinatoricGroup::AddFrame(RestFrame& frame){
    if(!frame) return;
    if(!frame.IsVisibleFrame()) return;
    if(!frame.IsRecoFrame()) return;

    int N = GetNFrames();
    Group::AddFrame(frame);
    if(GetNFrames() == N) 
      return;
    
    m_NElementsForFrame[&frame] = 0;
    m_NExclusiveElementsForFrame[&frame] = true;
  }

  void CombinatoricGroup::RemoveFrame(RestFrame& frame){
    if(!ContainsFrame(frame)) 
      return;
    m_NElementsForFrame.erase(&frame);
    m_NExclusiveElementsForFrame.erase(&frame);
    Group::RemoveFrame(frame);
  }
  
  void CombinatoricGroup::SetNElementsForFrame(const RestFrame& frame, 
					       int N, bool exclusive_N){
    if(!ContainsFrame(frame)){ 
      N = 0;
      exclusive_N = true;
      return;
    }

    SetMind(false);

    m_NElementsForFrame[&frame] = std::max(0, N);
    m_NExclusiveElementsForFrame[&frame] = exclusive_N;
  }

  void CombinatoricGroup::GetNElementsForFrame(const RestFrame& frame, int& N, 
					       bool& exclusive_N) const {
    if(!ContainsFrame(frame)) return;
    
    N = m_NElementsForFrame[&frame];
    exclusive_N = m_NExclusiveElementsForFrame[&frame];
  }

  void CombinatoricGroup::AddJigsaw(Jigsaw& jigsaw){
    if(!jigsaw) return;
    if(!jigsaw.IsCombinatoricJigsaw()) return;
    Group::AddJigsaw(jigsaw);
  }

  CombinatoricState& CombinatoricGroup::InitializeParentState(){
    std::string name = GetName()+"_parent";
    CombinatoricState* statePtr = new CombinatoricState(name, name);
    AddDependent(statePtr);
    return *statePtr;
  }

  CombinatoricState& CombinatoricGroup::GetParentState() const {
    if(m_GroupStatePtr)
      return static_cast<CombinatoricState&>(*m_GroupStatePtr);
    else
      return CombinatoricState::Empty();
  }

  CombinatoricState& CombinatoricGroup::GetChildState(int i) const {
    if(!Group::GetChildState(i))
      return CombinatoricState::Empty();
    else
      return static_cast<CombinatoricState&>(Group::GetChildState(i));
  }

  // Event analysis functions
  bool CombinatoricGroup::ClearEvent(){
    m_Elements.Clear();
    return true;
  }
 
  bool CombinatoricGroup::AnalyzeEvent(){
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetSpirit(false);
    }
    
    GetParentState().ClearElements();
    GetParentState().AddElements(m_Elements);    

    return SetSpirit(true);
  }

  RFKey CombinatoricGroup::AddLabFrameFourVector(const MA5::MALorentzVector& V,
						 const RFCharge& charge){
    if(IsEmpty()) return RFKey(-1);
    
    MA5::MALorentzVector P = V;
    if(P.M() < 0.) P.SetVectM(V.Vect(),0.);
    
    VisibleState& state = GetNewElement();
    state.SetFourVector(P);
    state.SetCharge(charge);
    m_Elements.Add(state);
   
    return state.GetKey();
  }

  RFKey CombinatoricGroup::AddLabFrameFourVector(const MA5::MALorentzVector& V,
						 int charge){
    return AddLabFrameFourVector(V, RFCharge(charge));
  }
  
  RFKey CombinatoricGroup::AddLabFrameFourVector(const MA5::MALorentzVector& V,
						 int charge_num, 
						 int charge_den){
    return AddLabFrameFourVector(V, RFCharge(charge_num,charge_den));
  }

  RestFrame const& CombinatoricGroup::GetFrame(const RFKey& key) const {
    int N = GetNChildStates();
    for(int i = N-1; i >= 0; i--)
      if(GetChildState(i).ContainsElement(key))
	return GetChildState(i).GetListFrames()[0];
    return RestFrame::Empty();
  }

  MA5::MALorentzVector CombinatoricGroup::GetLabFrameFourVector(const RFKey& key) const {
    int N = GetNChildStates();
    for(int i = N-1; i >= 0; i--)
      if(GetChildState(i).ContainsElement(key))
  	return GetChildState(i).GetElement(key).GetFourVector();
    return MA5::MALorentzVector(0.,0.,0.,0.);
  }

  int CombinatoricGroup::GetNElementsInFrame(const RestFrame& frame) const {
    if(!ContainsFrame(frame)) return -1;
    return static_cast<CombinatoricState&>(Group::GetChildState(frame)).GetNElements();
  }

  VisibleState& CombinatoricGroup::GetNewElement(){
    if(m_Elements.GetN() < m_InitStates.GetN())
      return m_InitStates[m_Elements.GetN()];
    char strn[10];
    snprintf(strn, sizeof(strn), "%d", m_Elements.GetN() + 1);
    std::string name = GetName()+"_"+std::string(strn);
    VisibleState* statePtr = new VisibleState(name,name);
    AddDependent(statePtr);
    m_InitStates.Add(*statePtr);
    return *statePtr;
  }

  CombinatoricGroup CombinatoricGroup::m_Empty;
  
}
