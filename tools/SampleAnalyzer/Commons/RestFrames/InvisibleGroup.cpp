/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   InvisibleGroup.cc
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

#include "SampleAnalyzer/Commons/RestFrames/InvisibleGroup.h"
#include "SampleAnalyzer/Commons/RestFrames/InvisibleState.h"
#include "SampleAnalyzer/Commons/RestFrames/ReconstructionFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // InvisibleGroup class
  ///////////////////////////////////////////////

  InvisibleGroup::InvisibleGroup(const std::string& sname, 
				 const std::string& stitle) : 
    Group(sname, stitle)
  {
    m_Type = kInvisibleGroup;
  }

  InvisibleGroup::InvisibleGroup() : Group() {}
 

  InvisibleGroup::~InvisibleGroup() {}

  InvisibleGroup& InvisibleGroup::Empty(){
    return InvisibleGroup::m_Empty;
  }

  void InvisibleGroup::Clear(){
    Group::Clear();
  }

  void InvisibleGroup::AddFrame(RestFrame& frame){
    if(!frame) return;
    if(!frame.IsInvisibleFrame()) return;
    if(!frame.IsRecoFrame()) return;
    Group::AddFrame(frame);
  }

  void InvisibleGroup::AddJigsaw(Jigsaw& jigsaw){
    if(!jigsaw) return;
    if(!jigsaw.IsInvisibleJigsaw()) return;
    Group::AddJigsaw(jigsaw);
  }

  InvisibleState& InvisibleGroup::InitializeParentState(){
    std::string name = GetName()+"_parent";
    InvisibleState* statePtr = new InvisibleState(name, name);
    AddDependent(statePtr);
    return *statePtr;
  }

  InvisibleState& InvisibleGroup::GetParentState() const {
    if(m_GroupStatePtr)
      return static_cast<InvisibleState&>(*m_GroupStatePtr);
    else
      return InvisibleState::Empty();
  }

  InvisibleState& InvisibleGroup::GetChildState(int i) const {
    if(!Group::GetChildState(i))
      return InvisibleState::Empty();
    else
      return static_cast<InvisibleState&>(Group::GetChildState(i));
  }

  bool InvisibleGroup::ClearEvent(){
    SetSpirit(false);
    if(!IsSoundMind()) 
      return SetSpirit(false);
    m_Lab_P.SetPxPyPzE(0.,0.,0.,0.);
    return true;
  }
  
  void InvisibleGroup::SetMass(double M){
    m_Lab_P.SetVectM(m_Lab_P.Vect(), std::max(0., M));
  }

  void InvisibleGroup::SetLabFrameFourVector(const MA5::MALorentzVector& V){
    m_Lab_P.SetVectM(V.Vect(), std::max(0., V.M()));
  }

  void InvisibleGroup::SetLabFrameThreeVector(const MA5::MAVector3& V){
    m_Lab_P.SetVectM(V, m_Lab_P.M());
  }

  MA5::MALorentzVector InvisibleGroup::GetLabFrameFourVector() const {
    return m_Lab_P;
  }

  bool InvisibleGroup::AnalyzeEvent(){
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetSpirit(false);
    }
    m_GroupStatePtr->SetFourVector(m_Lab_P);
    return SetSpirit(true);
  }
  
  InvisibleGroup InvisibleGroup::m_Empty;

}
