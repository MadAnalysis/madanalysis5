/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   GeneratorFrame.cc
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

#include <math.h>
#include <ctime>
#include "SampleAnalyzer/Commons/RestFrames/GeneratorFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/VisibleGenFrame.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // GeneratorFrame class methods
  ///////////////////////////////////////////////
  GeneratorFrame::GeneratorFrame(const std::string& sname, 
				 const std::string& stitle) 
    : RestFrame(sname, stitle)
  {
    m_Ana = kGenFrame;
    m_VarMassMCMC = false;
    m_Mass = 0.;
  
    m_Ngen = 0;
    m_Npass = 0;

    m_PCut = -1.;
    m_PtCut = -1.;
    m_EtaCut = -1.;
    m_minMassCut = -1.;
    m_maxMassCut = -1.;
    m_doCuts = false;
    m_doPCut = false;
    m_doPtCut = false;
    m_doEtaCut = false;
    m_dominMassCut = false;
    m_domaxMassCut = false;
    
   // TDatime now;
   // int today = now.GetDate();
   // int clock = now.GetTime();
   int key   = GetKey().GetKey();
   int clock=std::time(nullptr);
   int seed= clock+key;
  // int seed = today+clock+key;
    //m_Random = new TRandom3(seed);
    m_Random = new std::mt19937(seed);
  }

  GeneratorFrame::GeneratorFrame() 
    : RestFrame() { }

  GeneratorFrame::~GeneratorFrame(){
    if(m_Random) delete m_Random;
  }

  void GeneratorFrame::Clear(){
    RestFrame::Clear();
  }

  /// \brief Returns empty instance of class
  GeneratorFrame& GeneratorFrame::Empty(){
    return VisibleGenFrame::Empty();
  }

  double GeneratorFrame::GetMass() const {
    return std::max(m_Mass, 0.);
  }

  void GeneratorFrame::AddChildFrame(RestFrame& frame){
    if(!frame) return;
    if(!frame.IsGenFrame()) return;
    RestFrame::AddChildFrame(frame);
  }

  void GeneratorFrame::SetParentFrame(RestFrame& frame){
    if(!frame) return;
    if(!frame.IsGenFrame()) return;
    RestFrame::SetParentFrame(frame);
  }

  GeneratorFrame const& GeneratorFrame::GetParentFrame() const {
    const RestFrame& frame = RestFrame::GetParentFrame();
    if(!frame.IsEmpty())
      return static_cast<const GeneratorFrame&>(frame);
    else 
      return GeneratorFrame::Empty();
  }

  GeneratorFrame& GeneratorFrame::GetChildFrame(int i) const {
    RestFrame& frame = RestFrame::GetChildFrame(i);
    if(!frame.IsEmpty())
      return static_cast<GeneratorFrame&>(frame);
    else 
      return GeneratorFrame::Empty();
  }

  bool GeneratorFrame::ClearEventRecursive(){ 
    ResetGenFrame();
    if(!IsSoundMind())
      return false;
    
    int Nf =  GetNChildren();
    for(int i = 0; i < Nf; i++)
      if(!GetChildFrame(i).ClearEventRecursive())
	return false;

    return true;
  }

  bool GeneratorFrame::AnalyzeEventRecursive(){
    if(!IsSoundMind()){
      UnSoundMind(RF_FUNCTION);
      return SetSpirit(false);
    }
    if(!GenerateFrame()){
      m_Log << LogWarning;
      m_Log << "Unable to generate event for this frame.";
      m_Log << LogEnd;
      return SetSpirit(false);
    }

    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++)
      if(!GetChildFrame(i).AnalyzeEventRecursive()){
	return SetSpirit(false);
      }

    return SetSpirit(true);
  }

  void GeneratorFrame::SetChildren(const std::vector<MA5::MALorentzVector>& P_children){
    int N = P_children.size();
    MA5::MABoost Booster;
    for(int i = 0; i < N; i++)
    {
      MA5::MALorentzVector P = P_children[i];
      Booster.setBoostVector(P);
      SetChildBoostVector(GetChildFrame(i), Booster.BoostVector());
      GetChildFrame(i).SetFourVector(P,*this);
    }
  }

  bool GeneratorFrame::InitializeGenAnalysis(){
    return SetMind(true);
  }

  bool GeneratorFrame::InitializeAnalysisRecursive(){
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return SetMind(false);
    }

    if(!InitializeGenAnalysis())
      return SetMind(false);

    m_Ngen = 0;
    m_Npass = 0;

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

  double GeneratorFrame::GetRandom() const {
    static std::uniform_real_distribution<double> rd;
    return rd(*m_Random);
    //return m_Random->Rndm();
  }

  double GeneratorFrame::GetGaus(double mu, double sig) const {
    std::normal_distribution<double> gau(mu,sig);
    return gau(*m_Random);
    //return m_Random->Gaus(mu,sig);
  }

  bool GeneratorFrame::IterateMCMC(){
    return true;
  }

  bool GeneratorFrame::IterateRecursiveMCMC(){
     if(!IsSoundMind()){
       UnSoundMind(RF_FUNCTION);
       return SetMind(false);
     }
    
     if(!IterateMCMC())
       return SetMind(false);

     int N = GetNChildren();
     for(int i = 0; i < N; i++)
       if(!GetChildFrame(i).IterateRecursiveMCMC())
	 return SetMind(false);

     return SetMind(true);
  }

  void GeneratorFrame::SetVariableMassMCMC(bool var){ 
    m_VarMassMCMC = var; 
  }

  bool GeneratorFrame::IsVariableMassMCMC() const { 
    return m_VarMassMCMC; 
  }

  double GeneratorFrame::GetMinimumMassMCMC() const {
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return SetBody(false);
    }
    
    double mass = 0.;
    int N = GetNChildren();
    for(int i = 0; i < N; i++)
      mass += GetChildFrame(i).GetMinimumMassMCMC();

    if(!IsVariableMassMCMC())
      mass = std::max(GetMass(),mass);

    return mass;
  }

  void GeneratorFrame::GenerateMassMCMC(double& mass, double& prob, 
					double max) const {
    mass = 0.;
    prob = 1.;
  }

  void GeneratorFrame::SetMassMCMC(double val){
    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "Unable to set mass to negative value ";
      m_Log << val << ". Setting to zero." << LogEnd;
      m_Mass = 0.;
    } else {
      m_Mass = val;
    }
  }

  void GeneratorFrame::SetMassMCMC(double mass, 
				   GeneratorFrame& frame) const {
    frame.SetMassMCMC(mass);
  }

  double GeneratorFrame::GetProbMCMC(double mass) const {
    return 1.;
  }

  void GeneratorFrame::PrintGeneratorEfficiency() const {
    if(IsLabFrame()){
      m_Log << LogInfo << std::endl;
      m_Log << "Total events generated: " << m_Ngen << std::endl;
      m_Log << "Events in acceptance:   " << m_Npass << std::endl;
      m_Log << "Generator efficiency:   ";
      m_Log << 100.*double(m_Npass)/double(m_Ngen) << " %";
      m_Log << std::endl << LogEnd;
    } 

    if(m_doCuts){
      m_Log << LogInfo;
      m_Log << "Acceptance cuts for frame:" << std::endl;
      if(m_dominMassCut || m_domaxMassCut){
	m_Log << "   ";
	if(m_dominMassCut)
	  m_Log << m_minMassCut << " < ";
	m_Log << "mass";
	if(m_domaxMassCut)
	  m_Log << " < " << m_maxMassCut;
	m_Log << std::endl;
      }
      if(m_doPCut){
	m_Log << "   P > " << m_PCut << std::endl; 
      }
      if(m_doPtCut){
	m_Log << "   Pt > " << m_PtCut << std::endl; 
      }
      if(m_doEtaCut){
	m_Log << "   |Eta| < " << m_EtaCut << std::endl; 
      }
      m_Log << "Acceptance efficiency = ";
      m_Log << 100.*double(m_Npass)/double(m_Ngen) << " %";
      m_Log << std::endl << LogEnd;
    }

    int N = GetNChildren();
    for(int i = 0; i < N; i++)
      GetChildFrame(i).PrintGeneratorEfficiency();
  }
  
  bool GeneratorFrame::EventInAcceptance() const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return SetSpirit(false);
    }
    
    bool pass = true;
    if(m_doCuts){
      MA5::MALorentzVector P = GetFourVector();
      if(m_doPCut)
	if(P.Mag() < (1.-1e-10)*m_PCut)
	  pass = false;
      if(m_doPtCut)
	if(P.Pt() < (1.-1e-10)*m_PtCut)
	  pass = false;
      if(m_doEtaCut)
	if(fabs(P.Eta()) > (1.+1e-10)*m_EtaCut)
	  pass = false;
      if(m_dominMassCut)
	if(m_Mass < (1.-1e-10)*m_minMassCut)
	  pass = false;
      if(m_domaxMassCut)
	if(m_Mass > (1.+1e-10)*m_maxMassCut)
	  pass = false;
    }

    bool evt_pass = pass;
    bool pass_c;
    int N = GetNChildren();
    for(int i = 0; i < N; i++){
      pass_c = GetChildFrame(i).EventInAcceptance();
      evt_pass = evt_pass && pass_c;
    }

    if(IsLabFrame())
      pass = evt_pass;

    m_Ngen++;
    if(pass)
      m_Npass++;
    
    return evt_pass;
  }
  
  void GeneratorFrame::SetPCut(double cut){
    if(cut <= 0.) return;
    m_PCut = cut;
    m_doPCut = true;
    m_doCuts = true;
  }
  
  void GeneratorFrame::SetPtCut(double cut){
    if(cut <= 0.) return;
    m_PtCut = cut;
    m_doPtCut = true;
    m_doCuts = true;
  }
  
  void GeneratorFrame::SetEtaCut(double cut){
    m_EtaCut = fabs(cut);
    m_doEtaCut = true;
    m_doCuts = true;
  }
  
  void GeneratorFrame::SetMassWindowCut(double min, double max){
    if(min > 0 || max > 0)
      m_doCuts = true;
    else
      return;

    if(min > 0){
      m_dominMassCut = true;
      m_minMassCut = min;
    }

    if(max > 0){
      m_domaxMassCut = true;
      m_maxMassCut = max;
    }  
  }

  void GeneratorFrame::RemovePCut(){
    m_doPCut = false;
    m_doCuts = m_doPCut || m_doPtCut || m_doEtaCut ||
      m_dominMassCut || m_domaxMassCut;
  }
  
  void GeneratorFrame::RemovePtCut(){
    m_doPtCut = false;
    m_doCuts = m_doPCut || m_doPtCut || m_doEtaCut ||
      m_dominMassCut || m_domaxMassCut;
  }
  
  void GeneratorFrame::RemoveEtaCut(){
    m_doEtaCut = false;
    m_doCuts = m_doPCut || m_doPtCut || m_doEtaCut ||
      m_dominMassCut || m_domaxMassCut;
  }

  void GeneratorFrame::RemoveMassWindowCut(){
    m_dominMassCut = false;
    m_domaxMassCut = false;
    m_doCuts = m_doPCut || m_doPtCut || m_doEtaCut ||
      m_dominMassCut || m_domaxMassCut;
  }

}
