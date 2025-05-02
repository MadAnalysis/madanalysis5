/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   CombinatoricJigsaw.cc
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

#include "SampleAnalyzer/Commons/RestFrames/CombinatoricJigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/MinMassesCombJigsaw.h"

namespace RestFrames {

  CombinatoricJigsaw::CombinatoricJigsaw(const std::string& sname,
					 const std::string& stitle,
					 int Ncomb, int Nobject)
    : Jigsaw(sname, stitle, Ncomb, Nobject), 
      m_Ncomb(Ncomb), m_Nobj(Nobject)
  {
    m_Type = kCombinatoricJigsaw;
    for(int i = 0; i < m_Ncomb; i++){
      m_ChildStates += GetNewChildState();
    }
  }
  
  CombinatoricJigsaw::CombinatoricJigsaw()
    : Jigsaw(), m_Ncomb(0), m_Nobj(0) {}

  CombinatoricJigsaw::~CombinatoricJigsaw() {}

  void CombinatoricJigsaw::Clear(){
    Jigsaw::Clear();
  }

  CombinatoricJigsaw& CombinatoricJigsaw::Empty(){
    return MinMassesCombJigsaw::Empty();
  }

  void CombinatoricJigsaw::SetGroup(Group& group){
    if(!group) return;
    if(!group.IsCombinatoricGroup()) return;
    Jigsaw::SetGroup(group);
  }

  CombinatoricGroup& CombinatoricJigsaw::GetGroup() const {
    if(!Jigsaw::GetGroup())
      return CombinatoricGroup::Empty();
    else
      return static_cast<CombinatoricGroup&>(Jigsaw::GetGroup());
  }

  void CombinatoricJigsaw::SetParentState(State& state){
    if(!state) return;
    if(!state.IsCombinatoricState()) return;
    Jigsaw::SetParentState(state);
  }

  CombinatoricState const& CombinatoricJigsaw::GetParentState() const {
    if(!Jigsaw::GetParentState())
      return CombinatoricState::Empty();
    else
      return static_cast<const CombinatoricState&>(Jigsaw::GetParentState());
  }

  CombinatoricState& CombinatoricJigsaw::GetChildState(int i) const {
    if(!Jigsaw::GetChildState(i)) 
      return CombinatoricState::Empty();
    else
      return static_cast<CombinatoricState&>(Jigsaw::GetChildState(i));
  }

  void CombinatoricJigsaw::AddCombFrame(const RestFrame& frame, int i){
    if(!frame) return;

    ConstRestFrameList frames = 
      frame.GetListVisibleFrames();
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddChildFrame(frames[f], i);
  }

  void CombinatoricJigsaw::AddCombFrames(const ConstRestFrameList& frames, int i){
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddCombFrame(frames[f], i);
  }
  
  void CombinatoricJigsaw::AddObjectFrame(const RestFrame& frame, int i){
    if(!frame) return;

    ConstRestFrameList frames = 
      frame.GetListVisibleFrames()+
      frame.GetListInvisibleFrames();
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddDependancyFrame(frames[f], i);
  }
  
  void CombinatoricJigsaw::AddObjectFrames(const ConstRestFrameList& frames, int i){
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddObjectFrame(frames[f], i);
  }

  void CombinatoricJigsaw::SetCombCharge(const RFCharge& charge, int i){
    if(i < 0 || i >= m_Ncomb)
      return;

    m_ChargeForChild[i] = charge;
  }

  void CombinatoricJigsaw::SetCombCharge(int charge, int i){
    SetCombCharge(RFCharge(charge), i);
  }

  void CombinatoricJigsaw::SetCombCharge(int charge_num, int charge_den, int i){
    SetCombCharge(RFCharge(charge_num, charge_den), i);
  }

  void CombinatoricJigsaw::UnsetCombCharge(int i){
    if(m_ChargeForChild.count(i) > 0)
      m_ChargeForChild.erase(i);
  }

  void CombinatoricJigsaw::SetObjectCharge(const RFCharge& charge, int i){
    if(i < 0 || i >= m_Nobj)
      return;

    m_ChargeForObject[i] = charge;
  }

  void CombinatoricJigsaw::SetObjectCharge(int charge, int i){
    SetObjectCharge(RFCharge(charge), i);
  }

  void CombinatoricJigsaw::SetObjectCharge(int charge_num, int charge_den, int i){
    SetObjectCharge(RFCharge(charge_num, charge_den), i);
  }

  void CombinatoricJigsaw::UnsetObjectCharge(int i){
    if(m_ChargeForObject.count(i) > 0)
      m_ChargeForObject.erase(i);
  }
  
  bool CombinatoricJigsaw::InitializeJigsawExecutionList(JigsawList& exec_jigsaws){
    if(!IsSoundMind()) return false;
    if(exec_jigsaws.Contains(*this)) return true;

    m_ExecuteJigsaws.Clear();

    // Add group dependancy jigsaws first
    JigsawList group_jigsaws; 
    FillGroupJigsawDependancies(group_jigsaws);
    group_jigsaws -= *this;

    int Ngroup = group_jigsaws.GetN();
    for(int i = Ngroup-1; i >= 0; i--){
      Jigsaw& jigsaw = group_jigsaws[i];
      m_DependancyJigsaws -= jigsaw;
      if(!exec_jigsaws.Contains(jigsaw))
	if(!jigsaw.InitializeJigsawExecutionList(exec_jigsaws))
	  return SetMind(false);
    }
    // Satisfy dependancy jigsaws
    while(m_DependancyJigsaws.GetN() > 0){
      Jigsaw& jigsaw = m_DependancyJigsaws[m_DependancyJigsaws.GetN()-1];
      if(exec_jigsaws.Contains(jigsaw)){
	m_DependancyJigsaws -= jigsaw;
	continue;
      }
      if(!jigsaw.DependsOnJigsaw(*this)){
	if(!jigsaw.InitializeJigsawExecutionList(exec_jigsaws)){
	  return SetMind(false);
	}
	m_DependancyJigsaws -= jigsaw;
	continue;
      }
      JigsawList temp_exec_jigsaws = exec_jigsaws;
      temp_exec_jigsaws += m_ExecuteJigsaws;
      temp_exec_jigsaws += *this;
      if(!jigsaw.InitializeJigsawExecutionList(temp_exec_jigsaws))
	return SetMind(false);
      temp_exec_jigsaws -= *this;
      temp_exec_jigsaws -= exec_jigsaws;

      m_DependancyJigsaws -= temp_exec_jigsaws;
      m_ExecuteJigsaws    += temp_exec_jigsaws;
    }

    exec_jigsaws += *this;
    exec_jigsaws += m_ExecuteJigsaws;
    return true;
  }

  CombinatoricState& CombinatoricJigsaw::GetNewChildState(){
    char strn[10];
    snprintf(strn, sizeof(strn), "%d", GetNChildren());
    std::string name = GetName()+"_"+std::string(strn);
    CombinatoricState* statePtr = new CombinatoricState(name, name);
    AddDependent(statePtr);
    return *statePtr;
  }

  bool CombinatoricJigsaw::ExecuteDependancyJigsaws(){
    int N = m_ExecuteJigsaws.GetN();
    for(int i = 0; i < N; i++)
      if(!m_ExecuteJigsaws[i].AnalyzeEvent()) 
	return false;
    return true;
  }

  bool CombinatoricJigsaw::AnalyzeEvent(){
    if(!IsSoundMind() || !GetGroup())
      return SetSpirit(false);

    if(!InitializeCombinatoric()){
      m_Log << LogWarning;
      m_Log << "Problem initializing event info" << LogEnd;
      return SetSpirit(false);
    }

    if(!LoopCombinatoric()){
      m_Log << LogWarning;
      m_Log << "Problem looping over combinatorics" << LogEnd;
      return SetSpirit(false);
    }

    return SetSpirit(true);
  }

  bool CombinatoricJigsaw::InitializeAnalysis(){
    if(!Jigsaw::InitializeAnalysis())
      return SetMind(false);

    CombinatoricGroup& group = GetGroup();

    m_NForChild.clear();
    m_NExclusive.clear();

    m_NinputTOT = 0;
    m_NExclusiveTOT = true;

    for(int i = 0; i < m_Ncomb; i++){
      RestFrameList const& frames = GetChildState(i).GetListFrames();
      int Nf = frames.GetN();
      int NTOT = 0;
      bool exclTOT = true;
      for(int f = 0; f < Nf; f++){
	int N = -1;
	bool excl = false;
	group.GetNElementsForFrame(frames[f], N, excl);
	if(N < 0) return SetMind(false);
	NTOT += N;
	exclTOT = exclTOT && excl;
      }
      m_NForChild.push_back(NTOT);
      m_NExclusive.push_back(exclTOT);
      m_NinputTOT += NTOT;
      m_NExclusiveTOT = m_NExclusiveTOT && exclTOT;
    }

    return SetMind(true);
  }

  bool CombinatoricJigsaw::InitializeCombinatoric(){
    if(!IsSoundMind()) 
      return SetSpirit(false);

    if(!GetGroup())
      return SetSpirit(false);

    if(!GetParentState())
      return SetSpirit(false);

    m_InputStates.Clear();
    m_InputStates = GetParentState().GetElements();

    if(m_InputStates.GetN() < m_NinputTOT){
      m_Log << LogWarning;
      m_Log << "Unable to execute Jigsaw. ";
      m_Log << "Insufficienct number of inputs: ";
      m_Log << m_NinputTOT << " (required) != ";
      m_Log << m_InputStates.GetN() << " (provided)";
      m_Log << LogEnd;
      return SetSpirit(false);
    }

    if(m_NExclusiveTOT &&
       (m_InputStates.GetN() != m_NinputTOT)){
      m_Log << LogWarning;
      m_Log << "Unable to execute Jigsaw. ";
      m_Log << "Incorrect number of exclusive inputs: ";
      m_Log << m_NinputTOT << " (required) != ";
      m_Log << m_InputStates.GetN() << " (provided)";
      m_Log << LogEnd;
      return SetSpirit(false);
    }

    return SetSpirit(true);
  }

  int CombinatoricJigsaw::GetNInputStates() const {
    return m_InputStates.GetN();
  }

  VisibleState& CombinatoricJigsaw::GetInputState(int i) const {
    return m_InputStates[i];
  }

  int CombinatoricJigsaw::GetNinputForChild(int i) const {
    if(i < 0 || i >= m_Ncomb)
      return 0;
    return m_NForChild[i];
  }

  bool CombinatoricJigsaw::IsNinputExclForChild(int i) const {
    if(i < 0 || i >= m_Ncomb)
      return false;
    return m_NExclusive[i];
  }

  bool CombinatoricJigsaw::IsChargeSetForChild(int i) const {
    return !(m_ChargeForChild.count(i) == 0);
  }
  
  RFCharge CombinatoricJigsaw::GetChargeForChild(int i) const {
    if(IsChargeSetForChild(i))
      return m_ChargeForChild[i];
    else 
      return RFCharge();
  }

  bool CombinatoricJigsaw::IsChargeSetForObject(int i) const {
    return !(m_ChargeForObject.count(i) == 0);
  }

  RFCharge CombinatoricJigsaw::GetChargeForObject(int i) const {
    if(IsChargeSetForObject(i))
      return m_ChargeForObject[i];
    else 
      return RFCharge();
  }

  bool CombinatoricJigsaw::LoopCombinatoric(){
    int Ninput = m_InputStates.GetN();	

    int N_comb = 1;
    for(int i = 0; i < Ninput; i++) N_comb *= m_Ncomb;
    
    int c_min = -1;
    double metric_min = -1; 
   
    for(int c = 0; c < N_comb; c++){
      int key = c;
      for(int i = 0; i < m_Ncomb; i++)
	GetChildState(i).ClearElements();

      // set output states for combinatoric;
      for(int i = 0; i < Ninput; i++){
	int ihem = key%m_Ncomb;
	key /= m_Ncomb;
	GetChildState(ihem).AddElement(m_InputStates[i]);
      }

      // check validity of combinatoric
      bool valid = true;
      for(int i = 0; i < m_Ncomb; i++){
	if(IsNinputExclForChild(i)){
	  if(GetChildState(i).GetNElements() != GetNinputForChild(i)){
	    valid = false;
	    break;
	  }
	} else {
	  if(GetChildState(i).GetNElements() < GetNinputForChild(i)){
	    valid = false;
	    break;
	  }
	}
	if(IsChargeSetForChild(i)){
	  if(GetChildState(i).GetCharge() != GetChargeForChild(i)){
	    valid = false;
	    break;
	  }
	}
      }
      if(!valid)
	continue;

      // Execute depedancy Jigsaws for this combintoric
      if(!ExecuteDependancyJigsaws())
	continue;

      // check validity of objects
      for(int i = 0; i < m_Nobj; i++){
	if(IsChargeSetForObject(i)){
	  if(GetDependancyStates(i).GetCharge() != GetChargeForObject(i)){
	    valid = false;
	    break;
	  }
	}
      }
      if(!valid)
	continue;

      // Evaluate metric for this combinatoric
      double metric;
      if(!EvaluateMetric(metric))
	continue;

      if(metric < metric_min || c_min < 0){
	metric_min = metric;
	c_min = c;
      }

      if((metric < metric_min && metric >= 0.) || c_min < 0){
	metric_min = metric;
	c_min = c;
      }
    }
   
    if(c_min < 0){
      m_Log << LogWarning;
      m_Log << "Unable to find combinatoric satisfying ";
      m_Log << "requested conditions." << LogEnd;
      return SetSpirit(false);
    }
     
    // Set outputs to best combinatoric
    for(int i = 0; i < m_Ncomb; i++) 
      GetChildState(i).ClearElements();
    int key = c_min;
    for(int i = 0; i < Ninput; i++){
      int ihem = key%m_Ncomb;
      key /= m_Ncomb;
      GetChildState(ihem).AddElement(m_InputStates[i]);
    }
  
    // Execute depedancy Jigsaws
    if(!ExecuteDependancyJigsaws())
      return SetSpirit(false);
      
    return SetSpirit(true);
  }

  bool CombinatoricJigsaw::IsSoundBody() const {
    if(RFBase::IsSoundBody())
      return true;

    if(!Jigsaw::IsSoundBody())
      return SetBody(false);
    
    for(int i = 0; i < m_Nobj; i++){
      if(GetDependancyFrames(i).GetN() <= 0){
	m_Log << LogWarning;
	m_Log << "Empty collection of object frames: " << i;
	m_Log << LogEnd;
	return SetBody(false);
      }
    }

    return SetBody(true);
  }

}
