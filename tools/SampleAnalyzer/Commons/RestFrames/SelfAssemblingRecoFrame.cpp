/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   SelfAssemblingRecoFrame.cc
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

#include "SampleAnalyzer/Commons/RestFrames/SelfAssemblingRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/LabRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleState.h"
#include "SampleAnalyzer/Commons/RestFrames/CombinatoricState.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // SelfAssemblingRecoFrame class
  ///////////////////////////////////////////////
  SelfAssemblingRecoFrame::SelfAssemblingRecoFrame(const std::string& sname, 
						   const std::string& stitle)
    : DecayRecoFrame(sname,stitle)
  {
    m_RType = RDSelfAssembling;
    m_IsAssembled = false;
    m_Nvisible = 0;
    m_Ndecay = 0;
    m_NewEvent = true;
  }
  
  SelfAssemblingRecoFrame::~SelfAssemblingRecoFrame() {}

  void SelfAssemblingRecoFrame::Clear(){
    if(m_IsAssembled) Disassemble();
    m_VisibleFrames.Clear();
    m_DecayFrames.Clear();
    ReconstructionFrame::Clear();
  }

  bool SelfAssemblingRecoFrame::ResetRecoFrame(){
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetSpirit(false);
    }
    if(m_IsAssembled) Disassemble();
    m_NewEvent = true;
    return SetMind(true);
  }

  void SelfAssemblingRecoFrame::RemoveChildFrame(RestFrame& frame){
    m_ChildFrames_UnAssembled.Remove(frame);
    ReconstructionFrame::RemoveChildFrame(frame);
  }

  void SelfAssemblingRecoFrame::Disassemble(){
    m_Nvisible = 0;
    m_Ndecay = 0;
  
    // replace frames with unassembled ones
    const LabRecoFrame& lab_frame = static_cast<const LabRecoFrame&>(GetLabFrame());
    lab_frame.RemoveTreeStates(m_VisibleStates);
    RestFrameList ChildFrames = m_ChildFrames_UnAssembled;
    RemoveChildFrames();
    m_ChildFrames_UnAssembled = ChildFrames;
    ClearNewFrames();
    AddChildFrames(m_ChildFrames_UnAssembled);
    
    if(!InitializeTreeRecursive()){
      m_Log << LogWarning;
      m_Log << "Problem with recursive tree after disassembly";
      m_Log << LogEnd;
      SetBody(false);
      return;
    } else 
      SetBody(true);

    if(!InitializeAnalysisRecursive()){
      m_Log << LogWarning;
      m_Log << "Problem connecting states after disassembly";
      m_Log << LogEnd;
      SetMind(false);
      return;
    } else 
      SetMind(true);

    SetSpirit(false);
    m_IsAssembled = false;
  }

  void SelfAssemblingRecoFrame::Assemble(){
    if(m_IsAssembled) Disassemble();
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return;
    }

    // new Visible States
    m_VisibleStates.Clear();
    // new Frames associated with States
    std::vector<RestFrame*> frames;
    // States' four-vector
    std::vector<MA5::MALorentzVector> Ps; 

    // clear unassembled lists
    m_ChildFrames_UnAssembled.Clear();
    m_ChildFrames_UnAssembled += GetChildFrames();
    
    int N = GetNChildren();
    for(int i = 0; i < N; i++){
      ReconstructionFrame& frame = GetChildFrame(i);
      bool expand_frame = false;
      if(GetChildStates(frame).GetN() == 1 && frame.IsVisibleFrame())
	if(GetChildStates(frame)[0].IsCombinatoricState()){
	  expand_frame = true;
	  VisibleStateList const& elements = 
	    static_cast<CombinatoricState&>(GetChildStates(frame)[0]).GetElements();
	  int Nelement = elements.GetN();
	  for(int e = 0; e < Nelement; e++){
	    VisibleState& element = elements[e];
	    VisibleRecoFrame& new_frame =
	      GetNewVisibleFrame(frame.GetName(),frame.GetTitle());
	    new_frame.SetCharge(element.GetCharge());
	    element.AddFrame(new_frame);
	    frames.push_back(&new_frame);
	    MA5::MALorentzVector V = element.GetFourVector();
	    if(V.M() < 0.) V.SetVectM(V.Vect(),0.);
	    Ps.push_back(V);
	    m_VisibleStates += element;
	  }
	  if(Nelement < 1){
	    expand_frame = false;
	  }
	}
      if(!expand_frame){
	MA5::MALorentzVector V = GetChildStates(frame).GetFourVector();
	if(V.M() < 0.) V.SetVectM(V.Vect(),0.);
	Ps.push_back(V);
	frames.push_back(&frame);
      }
    }

    RestFrameList ChildFrames = m_ChildFrames_UnAssembled;
    RemoveChildFrames();
    m_ChildFrames_UnAssembled = ChildFrames;
    AssembleRecursive(*this, frames, Ps); 
    if(!InitializeTreeRecursive()){
      m_Log << LogWarning;
      m_Log << "Problem with recursive tree after assembly";
      m_Log << LogEnd;
      SetBody(false);
      return;
    }
    
    SetMind(true);
    const LabRecoFrame& lab_frame = static_cast<const LabRecoFrame&>(GetLabFrame());
    lab_frame.AddTreeStates(m_VisibleStates);
    if(!InitializeAnalysisRecursive()){
      m_Log << LogWarning;
      m_Log << "Problem connecting states after assembly";
      m_Log << LogEnd;
      SetMind(false);
      return;
    }

    m_IsAssembled = true;
  }

  void SelfAssemblingRecoFrame::AssembleRecursive(RestFrame& frame, 
						  std::vector<RestFrame*>& frames, 
						  std::vector<MA5::MALorentzVector>& Ps){
    int Ninput = frames.size();
    if(Ninput <= 2){
      for(int i = 0; i < Ninput; i++) frame.AddChildFrame(*frames[i]);
      return;
    }

    MA5::MALorentzVector TOT(0.,0.,0.,0.);
    for(int i = 0; i < Ninput; i++) TOT += Ps[i];
    MA5::MABoost Booster;
    Booster.setBoostVector(-TOT);
    if(Booster.BoostVector().Mag() > 1.)
    {
      MA5::MAVector3 boost_prm = Booster.BoostVector();
      boost_prm.SetMagThetaPhi(1.0,boost_prm.Theta(),boost_prm.Phi());
      // Surely this line is wrong anyway????? should be 1???
      //boost.SetMagThetaPhi(0.,boost.Theta(),boost.Phi());
      Booster.setBoostVector(boost_prm.X(), boost_prm.Y(), boost_prm.Z());
    }
    for(int i = 0; i < Ninput; i++){
      if(Ps[i].M() < 0.)
	Ps[i].SetVectM(Ps[i].Vect(),Ps[i].M());
      Booster.boost(Ps[i]);
    }

    int ip_max[2];
    int jp_max[2];
    for(int i = 0; i < 2; i++) ip_max[i] = -1;
    for(int i = 0; i < 2; i++) jp_max[i] = -1;
    double val_max = -1.;
    // Loop over all 2-jet seed probes
    int ip[2], jp[2];
    for(ip[0] = 0; ip[0] < Ninput-1; ip[0]++){
      for(ip[1] = ip[0]+1; ip[1] < Ninput; ip[1]++){
	MA5::MAVector3 nRef = Ps[ip[0]].Vect().Cross(Ps[ip[1]].Vect());
	int Nhem[2];
	MA5::MALorentzVector hem[2];
	for(int i = 0; i < 2; i++){
	  Nhem[i] = 0;
	  hem[i].SetPxPyPzE(0.,0.,0.,0.);
	}
	// Loop over all jets
	for(int i = 0; i < Ninput; i++){
	  if((i == ip[0]) || (i == ip[1])) continue;
	  int ihem = int(Ps[i].Vect().Dot(nRef) > 0.);
	  Nhem[ihem]++;
	  hem[ihem] += Ps[i];
	}
	// assign 2 probes
	for(jp[0] = 0; jp[0] < 2; jp[0]++){
	  for(jp[1] = 0; jp[1] < 2; jp[1]++){
	    if(jp[0] == jp[1] && Nhem[(jp[0]+1)%2] == 0) continue;
	    MA5::MALorentzVector hem_probes[2];
	    for(int i = 0; i < 2; i++) hem_probes[i] = hem[i];
	    for(int i = 0; i < 2; i++) hem_probes[jp[i]] += Ps[ip[i]];
	    double val = hem_probes[0].P() + hem_probes[1].P();
	    if(val > val_max){
	      val_max = val;
	      for(int i = 0; i < 2; i++) ip_max[i] = ip[i];
	      for(int i = 0; i < 2; i++) jp_max[i] = jp[i];
	    }
	  }
	}
      }
    }

    std::vector<RestFrame*> child_frames[2];
    std::vector<MA5::MALorentzVector> child_Ps[2];
    MA5::MALorentzVector hem[2];
    for(int i = 0; i < 2; i++){
      hem[i].SetPxPyPzE(0.,0.,0.,0.);
    }
   
    for(int i = 0; i < 2; i++){
      child_frames[jp_max[i]].push_back(frames[ip_max[i]]);
      child_Ps[jp_max[i]].push_back(Ps[ip_max[i]]);
      hem[jp_max[i]] += Ps[ip_max[i]];
    }

    MA5::MAVector3 nRef = Ps[ip_max[0]].Vect().Cross(Ps[ip_max[1]].Vect());
    for(int i = 0; i < Ninput; i++){
      if((i == ip_max[0]) || (i == ip_max[1])) continue;
      int ihem = int(Ps[i].Vect().Dot(nRef) > 0.);
      child_frames[ihem].push_back(frames[i]);
      child_Ps[ihem].push_back(Ps[i]);
      hem[ihem] += Ps[i];
    }
   
    int flip = int(hem[1].M() > hem[0].M());
    for(int i = 0; i < 2; i++){
      int j = (i+flip)%2;
      if(child_frames[j].size() == 1){
	frame.AddChildFrame(*child_frames[j][0]);
      } else {
	RestFrame& new_frame = GetNewDecayFrame(GetName(),GetTitle());
	frame.AddChildFrame(new_frame);
	AssembleRecursive(new_frame, child_frames[j], child_Ps[j]);
      }
    }
  }

  bool SelfAssemblingRecoFrame::ReconstructFrame(){
    if(m_NewEvent){
      m_NewEvent = false;
      if(m_IsAssembled) Disassemble();
      if(!AnalyzeEventRecursive()){
	m_Log << LogWarning;
	m_Log << "Unable to recursively analyze event with ";
	m_Log << "disassembled SelfAssemblingRecoFrame" << LogEnd;
	return SetSpirit(false);
      }
      Assemble();
    }

    return ReconstructionFrame::ReconstructFrame();
  }

  void SelfAssemblingRecoFrame::ClearNewFrames(){
    int N = m_DecayFrames.GetN();
    for(int i = 0; i < N; i++) m_DecayFrames[i].Clear();
    N = m_VisibleFrames.GetN();
    for(int i = 0; i < N; i++) m_VisibleFrames[i].Clear();
  }

  DecayRecoFrame& SelfAssemblingRecoFrame::GetNewDecayFrame(const std::string& sname, 
							    const std::string& stitle){
    if(m_Ndecay < m_DecayFrames.GetN()){
      m_Ndecay++;
      return m_DecayFrames.Get(m_Ndecay-1);
    }
    char strn[10];
    snprintf(strn,sizeof(strn),"%d",m_Ndecay+1);
    std::string name  = sname+"_"+std::string(strn);
    std::string title = "#left("+stitle+"#right)_{"+std::string(strn)+"}";
    DecayRecoFrame* framePtr = new DecayRecoFrame(name, title);
    
    m_DecayFrames.Add(*framePtr);
    AddDependent(framePtr);
    m_Ndecay++;
    return *framePtr;
  }

  VisibleRecoFrame& SelfAssemblingRecoFrame::GetNewVisibleFrame(const std::string& sname, 
								const std::string& stitle){
    if(m_Nvisible < m_VisibleFrames.GetN()){
      m_Nvisible++;
      return m_VisibleFrames.Get(m_Nvisible-1);
    }
    char strn[10];
    snprintf(strn,sizeof(strn),"%d",m_Nvisible+1);
    std::string name  = sname+"_"+std::string(strn);
    std::string title = "#left("+stitle+"#right)_{"+std::string(strn)+"}";
    VisibleRecoFrame* framePtr = new VisibleRecoFrame(name, title);
    
    m_VisibleFrames.Add(*framePtr);
    AddDependent(framePtr);
    m_Nvisible++;
    return *framePtr;
  }

  RestFrame const& SelfAssemblingRecoFrame::GetFrame(const RFKey& key) const {
    if(!m_IsAssembled)
      return RestFrame::Empty();
    
    int N = GetNChildren();
    for(int i = 0; i < N; i++){
      if(GetChildStates(i).Contains(key))
	return GetChildStates(i).Get(key).GetListFrames()[0];
    }

    return RestFrame::Empty();
  }

}
