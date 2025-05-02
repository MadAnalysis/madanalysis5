/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   LabGenFrame.cc
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

#include "SampleAnalyzer/Commons/RestFrames/LabGenFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // LabGenFrame class
  ///////////////////////////////////////////////
  LabGenFrame::LabGenFrame(const std::string& sname, 
			   const std::string& stitle) : 
    LabFrame<GeneratorFrame>(sname, stitle)
  {
    m_PT = 0.;
    m_PToM = -1.;
    m_PL = 0.;
    m_Phi = -1.;

    m_MaxM = -1.;

    m_NBurnInMCMC = 1000;
    m_NDiscardMCMC = 5;

    m_FailTolerance = 1000;
  }

  LabGenFrame::~LabGenFrame() {}

  void LabGenFrame::Clear(){
    GeneratorFrame::Clear();
  }

  void LabGenFrame::SetThreeVector(const MA5::MAVector3& P){
    m_PT = P.Pt();
    m_PL = P.Z();
    SetPhi(P.Phi());
  }

  void LabGenFrame::SetPToverM(double val){
    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "Unable to set transverse momentum ";
      m_Log << "to negative value: " << val << "*mass";
      m_Log << LogEnd;
    } else {
      m_PToM = val;
      m_PT = 0.;
    }
  }

  void LabGenFrame::SetTransverseMomentum(double val){
    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "Unable to set transverse momentum ";
      m_Log << "to negative value: " << val;
      m_Log << LogEnd;
    } else {
      m_PT = val;
      m_PToM = -1.;
    }
  }

  void LabGenFrame::SetLongitudinalMomentum(double val){
    m_PL = val;
  }

  void LabGenFrame::SetPhi(double val){
    while(val > acos(-1.)*2.) val -= acos(-1.)*2.;
    while(val < 0.) val += acos(-1.)*2.;
    m_Phi = val;
  }

  void LabGenFrame::ResetGenFrame(){
    SetSpirit(false);
    m_Phi = -1.;
  }

  bool LabGenFrame::InitializeAnalysis(){
    m_Log << LogVerbose << "Initializing this tree for analysis..." << LogEnd;
   
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return SetMind(false);
    }

    if(!InitializeAnalysisRecursive()){
      m_Log << LogWarning << "...Unable to recursively initialize analysis" << LogEnd;
      return SetMind(false);
    }

    for(int i = 0; i < m_NBurnInMCMC; i++)
      if(!IterateRecursiveMCMC()){
	m_Log << LogWarning << "...Unable to recursively initialize analysis" << LogEnd;
	return SetMind(false);
      }

    m_Log << LogVerbose << "...Done" << LogEnd;
    return SetMind(true);
  }
  
  void LabGenFrame::SetN_MCMCBurnIn(int N){
    SetMind(false);
    m_NBurnInMCMC = std::max(0,N);
  }

  void LabGenFrame::SetN_MCMCDiscard(int N){
    SetMind(false);
    m_NDiscardMCMC = std::max(1,N);
  }

  void LabGenFrame::SetFailTolerance(int Nfail){
    m_FailTolerance = Nfail;
  }

  bool LabGenFrame::InitializeGenAnalysis(){
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return SetMind(false);
    } 

    GeneratorFrame& child = GetChildFrame();
    if(child.IsVariableMassMCMC()){
      double ChildMass, ChildProb;
      child.GenerateMassMCMC(ChildMass, ChildProb, m_MaxM);
      m_ChildMassMCMC = ChildMass;
      m_ChildProbMCMC = ChildProb;
      SetMassMCMC(ChildMass, child);
    } else {
      m_ChildMassMCMC = child.GetMass();
      m_ChildProbMCMC = 1.;
    }

    return SetMind(true);
  }

  bool LabGenFrame::IterateMCMC(){
    GeneratorFrame& child = GetChildFrame();
    if(child.IsVariableMassMCMC()){
      double ChildMass, ChildProb = 0.;
      child.GenerateMassMCMC(ChildMass, ChildProb, m_MaxM);

      double probOld = GetProbMCMC(m_ChildMassMCMC)*
	child.GetProbMCMC(m_ChildMassMCMC)/m_ChildProbMCMC;

      double probNew = GetProbMCMC(ChildMass)*
	child.GetProbMCMC(ChildMass)/ChildProb;

      if(probNew/probOld > GetRandom()){
	m_ChildMassMCMC = ChildMass;
	m_ChildProbMCMC = ChildProb;
	SetMassMCMC(ChildMass, child);
      } else {
	SetMassMCMC(m_ChildMassMCMC, child);
      }
    }

    return SetMind(true);
  }

  bool LabGenFrame::GenerateFrame(){
    if(!IsSoundBody()) 
      return false;

    MA5::MALorentzVector P;
    double M = GetChildFrame().GetMass();
    if(m_Phi < 0.) m_Phi = 2.*acos(-1.)*GetRandom();

    if(m_PToM > 0.)
      P.SetPxPyPzE(m_PToM*M*cos(m_Phi), m_PToM*M*sin(m_Phi), m_PL, 
		   sqrt(m_PL*m_PL + M*M*(1. + m_PToM*m_PToM)));
    else 
      P.SetPxPyPzE(m_PT*cos(m_Phi), m_PT*sin(m_Phi), m_PL, 
		   sqrt(m_PT*m_PT + m_PL*m_PL + M*M));
    m_Phi = -1.;

    std::vector<MA5::MALorentzVector> ChildVector;
    ChildVector.push_back(P);
    SetChildren(ChildVector);
    
    return SetSpirit(true);
  }

  bool LabGenFrame::ClearEvent(){
    SetSpirit(false);
    if(!IsSoundBody()) 
      return false;
    return ClearEventRecursive();
  }

  bool LabGenFrame::AnalyzeEvent(){
    bool pass = false;
    int tries = 0;

    while(!pass){
      for(int i = 0; i < m_NDiscardMCMC; i++)
	if(!IterateRecursiveMCMC())
	  return SetSpirit(false);
      
      if(!AnalyzeEventRecursive()){
	return SetSpirit(false);
      }
      
      pass = EventInAcceptance();

      if(!pass){
	tries++;
	if(tries > m_FailTolerance &&
	   m_FailTolerance > 0){
	  m_Log << LogWarning;
	  m_Log << "Failed to generate event in ";
	  m_Log << "acceptance in " << tries;
	  m_Log << " tries. Giving up." << LogEnd;
	  return SetSpirit(false);
	}
      }
    }

    return SetSpirit(true);
  }

}
