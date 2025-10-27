/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   LabRecoFrame.cc
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

#include "SampleAnalyzer/Commons/RestFrames/LabRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleState.h"
#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/Group.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleRecoFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // LabRecoFrame class
  ///////////////////////////////////////////////
  LabRecoFrame::LabRecoFrame(const std::string& sname, 
			     const std::string& stitle)
    : LabFrame<ReconstructionFrame>(sname, stitle) {}

  LabRecoFrame::LabRecoFrame() : LabFrame<ReconstructionFrame>() {}

  LabRecoFrame::~LabRecoFrame(){
    Clear();
  }

  void LabRecoFrame::Clear(){
    m_TreeGroups.Clear();
    m_TreeJigsaws.Clear();
    m_TreeStates.Clear();
    ReconstructionFrame::Clear();
  }
  
  void LabRecoFrame::AddTreeState(VisibleState& state) const {
    m_TreeStates += state;
  }

  void LabRecoFrame::AddTreeStates(const VisibleStateList& states) const {
    m_TreeStates += states;
  }

  void LabRecoFrame::RemoveTreeState(const VisibleState& state) const {
    m_TreeStates -= state;
  }
  
  void LabRecoFrame::RemoveTreeStates(const VisibleStateList& states) const {
    m_TreeStates -= states;
  }

  VisibleStateList const& LabRecoFrame::GetTreeStates() const {
    return m_TreeStates;
  }

  bool LabRecoFrame::InitializeAnalysis(){
    m_Log << LogVerbose << "Initializing this tree for analysis..." << LogEnd;
    
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return SetMind(false);
    }

    if(!InitializeTreeGroups()){
      m_Log << LogWarning;
      m_Log << "Unable to intialize Groups for analysis" << LogEnd;
      return SetMind(false);
    }
    
    if(!InitializeTreeStates()){
      m_Log << LogWarning;
      m_Log << "Unable to intialize States for analysis" << LogEnd;
      return SetMind(false);
    }
    
    if(!InitializeTreeJigsaws()){
      m_Log << LogWarning;
      m_Log << "Unable to intialize Jigsaws for analysis" << LogEnd;
      return SetMind(false);
    }
    
    if(!InitializeAnalysisRecursive()){
      m_Log << LogWarning;
      m_Log << "Unable to recursively initialize tree for analysis";
      m_Log << LogEnd;
      return SetMind(false);
    }
    
    m_Log << LogVerbose << "...Done initializing tree for analysis" << LogEnd;
    return SetMind(true);
  }

  bool LabRecoFrame::InitializeTreeGroups(){
     m_Log << LogVerbose << "Initializing Groups for analysis..." << LogEnd;
    
    m_TreeGroups.Clear();
    m_TreeGroups += GetListGroups();

    int Ngroup = m_TreeGroups.GetN();
    for(int i = 0; i < Ngroup; i++){
      if(!m_TreeGroups[i].InitializeAnalysis()){
	m_Log << LogWarning;
	m_Log << "Unable to initialize analysis for Group ";
	m_Log << Log(m_TreeGroups[i]) << LogEnd;
	return false;
      }
    }
    m_Log << LogVerbose << "...Done initializing Groups for analysis" << LogEnd;
    return true;
  }

  bool LabRecoFrame::InitializeTreeStates(){
    m_Log << LogVerbose << "Initializing States for analysis..." << LogEnd;
    
    m_TreeStates.Clear();
    int Ng = m_TreeGroups.GetN();

    RFList<ReconstructionFrame> frames = GetListVisibleFrames();
    int Nf = frames.GetN();
    for(int f = 0; f < Nf; f++){
      bool has_group = false;
      for(int g = 0; g < Ng; g++){
	if(m_TreeGroups[g].ContainsFrame(frames[f])){
	  has_group = true;
	  break;
	}
      }
      if(!has_group) {
	VisibleState& state = GetNewVisibleState();
	state.AddFrame(frames[f]);
	state.SetCharge(frames[f].GetCharge());
	m_TreeStates += state;
      }
    }

    frames = GetListInvisibleFrames();
    Nf = frames.GetN();
    for(int f = 0; f < Nf; f++){
      bool has_group = false;
      for(int g = 0; g < Ng; g++){
	if(m_TreeGroups[g].ContainsFrame(frames[f])){
	  has_group = true;
	  break;
	}
      }
      if(!has_group){
	m_Log << LogWarning;
	m_Log << "Found InvisibleRecoFrame without an assigned group: " << std::endl;
	m_Log << Log(frames[f]) << LogEnd;
	return false;
      }
    }
    m_Log << LogVerbose << "...Done initializing States" << LogEnd;
    return true;
  }

  bool LabRecoFrame::InitializeTreeJigsaws(){
    m_Log << LogVerbose << "Initializing Jigsaws for analysis..." << LogEnd;
    
    m_TreeJigsaws.Clear();
    
    int Ng = m_TreeGroups.GetN();
    
    // Initialize Dependancy States in Jigsaws
    for(int g = 0; g < Ng; g++){
      JigsawList jigsaws = m_TreeGroups[g].GetListJigsaws();
      int Nj = jigsaws.GetN();
      for(int j = 0; j < Nj; j++)
	if(!jigsaws[j].InitializeAnalysis()){
	  m_Log << LogWarning;
	  m_Log << "Unable to initialize Jigsaw for analysis:" << std::endl;
	  m_Log << Log(jigsaws[j]) << LogEnd;
	  return false;
	}
    }
    // Initialize Dependancy Jigsaw lists inside jigsaws
    for(int g = 0; g < Ng; g++){
      JigsawList jigsaws = m_TreeGroups[g].GetListJigsaws();
      int Nj = jigsaws.GetN();
      for(int j = 0; j < Nj; j++){
	Jigsaw& jigsaw = jigsaws[j];
	if(!jigsaw.InitializeDependancyJigsaws()){
	  m_Log << LogWarning;
	  m_Log << "Unable to initialize dependancy Jigsaw list for Jigsaw:" << std::endl;
	  m_Log << Log(jigsaw) << LogEnd;
	  return false;
	}
	m_TreeJigsaws.Add(jigsaw);
      }
    }
    // Initialize Jigsaw execution list
    JigsawList exec_jigsaws;
    int Nj = m_TreeJigsaws.GetN();
    for(int j = 0; j < Nj; j++){
      if(!m_TreeJigsaws[j].InitializeJigsawExecutionList(exec_jigsaws)){
	m_TreeJigsaws.Clear();
	m_Log << LogWarning;
	m_Log << "Unable to initialize Jigsaw execution list in Jigsaw:" << std::endl;
	m_Log << Log(m_TreeJigsaws[j]) << LogEnd;
	return false;
      }
    }  
    m_TreeJigsaws.Clear();
    m_TreeJigsaws += exec_jigsaws;

    m_Log << LogVerbose << "...Done initializing Jigsaws" << LogEnd;
    return true;
  }

  bool LabRecoFrame::ClearEvent(){
    SetSpirit(false);
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetMind(false);
    }
    
    int Ng = m_TreeGroups.GetN();
    for(int i = 0; i < Ng; i++)
      if(!m_TreeGroups[i].ClearEvent())
	return SetMind(false);
    
    if(!ClearEventRecursive())
      return SetMind(false);

    return SetMind(true);
  }

  bool LabRecoFrame::AnalyzeEvent(){
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetSpirit(false);
    }

    int Ns = m_TreeStates.GetN();
    for(int i = 0; i < Ns; i++){
      m_TreeStates[i].SetLabFrameFourVector();
    }
      
    int Ng = m_TreeGroups.GetN();
    for(int i = 0; i < Ng; i++){
      if(!m_TreeGroups[i].AnalyzeEvent()){
	m_Log << LogWarning;
	m_Log << "Unable to analyze event for Group: ";
	m_Log << Log(m_TreeGroups[i]) << LogEnd;
	return SetSpirit(false);
      }
    }
    int Nj = m_TreeJigsaws.GetN();
    for(int i = 0; i < Nj; i++){
      if(!m_TreeJigsaws[i].AnalyzeEvent()){
	m_Log << LogWarning;
	m_Log << "Unable to analyze event for Jigsaw: ";
	m_Log << Log(m_TreeJigsaws[i]) << LogEnd;
	return SetSpirit(false);
      }
    }
    if(!AnalyzeEventRecursive()){
      m_Log << LogWarning;
      m_Log << "Unable to recursively analyze event" << LogEnd;
      return SetSpirit(false);
    }
    return SetSpirit(true);
  }

  VisibleState& LabRecoFrame::GetNewVisibleState(){
    if(m_TreeStates.GetN() < m_InitStates.GetN())
      return m_InitStates[m_TreeStates.GetN()];
    char strn[12];
    snprintf(strn, sizeof(strn), "%d",m_TreeStates.GetN()+1);
    std::string name = GetName()+"_"+std::string(strn);
    VisibleState* statePtr = new VisibleState(name, name);
    AddDependent(statePtr);
    m_InitStates.Add(*statePtr);
    return *statePtr;
  }

}
