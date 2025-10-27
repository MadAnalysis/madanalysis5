/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   DecayGenFrame.cc
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

#include <stdlib.h>
#include "SampleAnalyzer/Commons/RestFrames/DecayGenFrame.h"
#include "SampleAnalyzer/Commons/Vector/MARotationGeneral.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // DecayGenFrame class
  ///////////////////////////////////////////////

  DecayGenFrame::DecayGenFrame(const std::string& sname, 
			       const std::string& stitle) 
    : DecayFrame<GeneratorFrame>(sname,stitle)
  {
    m_CosDecayAngle = -2.;
    m_DeltaPhiDecayPlane = -2.;
  }

  DecayGenFrame::DecayGenFrame() : DecayFrame<GeneratorFrame>() {}
  
  DecayGenFrame::~DecayGenFrame() {}

  bool DecayGenFrame::IsSoundBody() const{
    if(RFBase::IsSoundBody())
      return true;
    if(!RestFrame::IsSoundBody())
      return false;
    int Nchild = GetNChildren();
    if(Nchild < 2 || GetParentFrame().IsEmpty()) 
      return SetBody(false);
    return SetBody(true);
  }

  void DecayGenFrame::SetMass(double val){
    SetMind(false);
    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "Unable to set mass to negative value ";
      m_Log << val << ". Setting to zero." << LogEnd;
      m_Mass = 0.;
    } else {
      m_Mass = val;
    }
 
  }

  void DecayGenFrame::SetCosDecayAngle(double val){
    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "CosDecay angle must be in [-1, 1]: ";
      m_Log << val << ". Setting to random." << LogEnd;
      m_CosDecayAngle = -2.;
    } else {
      m_CosDecayAngle = val;
    }
  }

  void DecayGenFrame::SetDeltaPhiDecayPlane(double val){
    while(val > acos(-1.)*2.) val -= acos(-1.)*2.;
    while(val < 0.) val += acos(-1.)*2.;
    m_DeltaPhiDecayPlane = val;
  }

  void DecayGenFrame::ResetGenFrame(){
    SetSpirit(false);
    m_CosDecayAngle = -2.;
    m_DeltaPhiDecayPlane = -2.;
  }

  void DecayGenFrame::SetVariableMass(bool varymass) {
    SetMind(false);
    SetVariableMassMCMC(varymass);
  }

  bool DecayGenFrame::InitializeGenAnalysis(){
    if(!IsSoundBody())
      return SetMind(false);

    double min_mass = GetMinimumMassMCMC();
    if(m_Mass < min_mass && !IsVariableMassMCMC()){
      m_Log << LogWarning;
      m_Log << "Unable to initialize analysis: ";
      m_Log << "decay frame mass (" << m_Mass << ") ";
      m_Log << "is less than required child masses (";
      m_Log << min_mass << ")" << LogEnd;
      return SetMind(false);
    }

    m_ChildIndexMCMC.clear();
    m_ChildMassMCMC.clear();
    m_ChildProbMCMC.clear();
    double Mass = GetMass();
    m_Log << LogInfo;
    int N = GetNChildren();
    for(int i = 0; i < N; i++){
      double cmass = 0.;
      double cprob = 1.;
      GeneratorFrame& child = GetChildFrame(i);
      if(child.IsVariableMassMCMC()){
	child.GenerateMassMCMC(cmass, cprob, Mass);
	SetMassMCMC(cmass, child);
	m_ChildIndexMCMC.push_back(i);
	m_ChildMassMCMC.push_back(cmass);
	m_ChildProbMCMC.push_back(cprob);
      } else {
	cmass = child.GetMass();
      }
      Mass -= cmass;
    }

    m_InterMassFracMCMC.clear();
    m_InterMassFracMCMC.push_back(0.);
    for(int i = 1; i < N-1; i++) 
      m_InterMassFracMCMC.push_back(GetRandom());
    qsort((double*)(&m_InterMassFracMCMC[0])+1,N-2,sizeof(double),DoubleMax);
    m_InterMassFracMCMC.push_back(1.);

    return SetMind(true);
  }

  bool DecayGenFrame::IterateMCMC(){
    int N = GetNChildren();

    std::vector<double> InterMassFrac;
    InterMassFrac.push_back(0.);
    for(int i = 1; i < N-1; i++) 
      InterMassFrac.push_back(GetRandom());
    qsort((double*)(&InterMassFrac[0])+1,N-2,sizeof(double),DoubleMax);
    InterMassFrac.push_back(1.);
    
    double probOld = GetProbMCMC();

    std::vector<double> InterMassFracOld = m_InterMassFracMCMC;
    m_InterMassFracMCMC = InterMassFrac;

    double probNew = GetProbMCMC();

    if(probOld > 0.)
      if(probNew/probOld < GetRandom())
    	m_InterMassFracMCMC = InterMassFracOld;

    int Nvar = m_ChildIndexMCMC.size();
    for(int v = 0; v < Nvar; v++){
      int index = m_ChildIndexMCMC[v];
      GeneratorFrame& child = GetChildFrame(index);
      
      double massMax = GetMass();
      for(int i = 0; i < N; i++)
	if(i != index)
	  massMax -= GetChildFrame(i).GetMass();

      double ChildMass = 0.;
      double ChildProb = 0.;
      child.GenerateMassMCMC(ChildMass, ChildProb, massMax);
      double probOld = child.GetProbMCMC(m_ChildMassMCMC[v]);
      double probNew = child.GetProbMCMC(ChildMass);
      probOld /= m_ChildProbMCMC[v];
      probNew /= ChildProb;

      probOld *= GetProbMCMC();
      SetMassMCMC(ChildMass, child);
      probNew *= GetProbMCMC();
      
      if(probOld > 0){
	if(probNew/probOld < GetRandom()){
	  SetMassMCMC(m_ChildMassMCMC[v], child);
	} else {
	  m_ChildMassMCMC[v] = ChildMass;
	  m_ChildProbMCMC[v] = ChildProb;
	}
      }	 
    }
    
    return SetMind(true);
  }

  double DecayGenFrame::GetProbMCMC(double mass) const {
    if(mass < 0.)
      mass = GetMass();

    double SumChildMass = 0.;
    int N = GetNChildren();
    for(int i = 0; i < N; i++)
      SumChildMass += GetChildFrame(i).GetMass();

    if(mass < SumChildMass)
      return 0.;

    double ETOT = mass - SumChildMass;
    std::vector<double> InterMass;
    for(int i = 0; i < N; i++){
      InterMass.push_back(m_InterMassFracMCMC[N-1-i]*ETOT + SumChildMass);
      SumChildMass -= GetChildFrame(i).GetMass();
    }

    double prob = 1.;
    for(int i = 0; i < N-1; i++)
      prob *= GetP(InterMass[i], InterMass[i+1], GetChildFrame(i).GetMass())/mass;
    
    prob /= mass*mass;

    return prob;
  }

  void DecayGenFrame::GenerateMassMCMC(double& mass, double& prob, 
				       double max) const {
    int N = GetNChildren();
    double SumMinChildMass = 0.;
    double SumChildMass = 0.;
    for(int i = 0; i < N; i++){
      GeneratorFrame& child = GetChildFrame(i);
      SumChildMass += child.GetMass();
      if(!child.IsVariableMassMCMC())
	SumMinChildMass += child.GetMass();
      else 
	SumMinChildMass += child.GetMinimumMassMCMC();
    }

    if(SumMinChildMass > max && max > 0){
      mass = max;
      prob = 0;
      return;
    }

    double T = SumChildMass;
    double min = SumMinChildMass;
    double SL = (T > 0 ? T/10. : 10.);
    double SU = (T > 0 ? T : 100.); 
    SU = (max > 0 ? std::max(max/100.,T) : 100.);
    double IL = SL*(1.-exp(-(T-min)/SL));
    double IU = (max > 0 ? SU*(1.-exp(-(max-T)/SU)) : 1.);

    double R = GetRandom();
    if(R > IL/(IL+IU)){
      R = R*(IL+IU)/IU - IL/IU;
      mass = T - log(1-R*IU/SU)*SU;
      prob = exp(-(mass-T)/SU);
    } else {
      R = R*(IL+IU)/IL;
      mass = T + log(1-R*IL/SL)*SL;
      prob = exp((mass-T)/SL);
    }
    
  }

  bool DecayGenFrame::GenerateFrame(){
    if(!IsSoundMind()){ 
      m_Log << LogWarning;
      m_Log << "Unable to generate event for frame";
      m_Log << LogEnd;
      return SetSpirit(false);
    }

    std::vector<double> ChildMasses;
    double SumChildMass = 0.;
    double cmass;
    int N = GetNChildren();
    for(int i = 0; i < N; i++){
      cmass = GetChildFrame(i).GetMass();
      ChildMasses.push_back(cmass);
      SumChildMass += cmass;
    }
   
    double ETOT = GetMass() - SumChildMass;
    std::vector<double> InterMass;
    for(int i = 0; i < N; i++){
      InterMass.push_back(m_InterMassFracMCMC[N-1-i]*ETOT + SumChildMass);
      SumChildMass -= ChildMasses[i];
    }

    

    SetSpirit(true);

    MA5::MAVector3 n_par = GetParentBoostVector();
    MA5::MAVector3 n_perp = GetParentFrame().GetDecayPlaneNormalVector(*this);

    if(n_par.Cross(n_perp).Mag() <= 0.){
      n_par.SetXYZ(1.,0.,0.);
      n_perp.SetXYZ(0.,1.,0.);
    }

    std::vector<MA5::MALorentzVector> ChildVectors;
    GenerateTwoBodyRecursive(InterMass, ChildMasses,
			     n_par, n_perp, ChildVectors);
   
    SetChildren(ChildVectors);

    return SetSpirit(true);
  }

  void DecayGenFrame::GenerateTwoBodyRecursive(const std::vector<double>& M_p, 
					       const std::vector<double>& M_c,
					       const MA5::MAVector3& axis_par, 
					       const MA5::MAVector3& axis_perp,
					       std::vector<MA5::MALorentzVector>& P_c) {
    MA5::MAVector3 n_par = axis_par.Unit();
    MA5::MAVector3 n_perp = n_par.Cross(axis_perp.Cross(n_par)).Unit();

    int N_c = M_c.size();

    double m[2], Mp = M_p[0];
    m[0] = M_c[0];
    m[1] = M_p[1];
    MA5::MAVector3 V_c[2];

    double Pcm = GetP(Mp, m[0], m[1]);

    V_c[0] = Pcm*n_par;
    V_c[1] = -Pcm*n_par;

    if(m_CosDecayAngle < -1.) m_CosDecayAngle = 1.-2.*GetRandom();
    if(m_DeltaPhiDecayPlane < 0.) m_DeltaPhiDecayPlane = 2.*acos(-1.)*GetRandom();

    MA5::MARotationGeneral Rotator_perp(-acos(m_CosDecayAngle), n_perp);
    MA5::MARotationGeneral Rotator_para(-m_DeltaPhiDecayPlane,n_par);

    for(int i = 0; i < 2; i++) Rotator_perp.rotate(V_c[i]);
    for(int i = 0; i < 2; i++) Rotator_para.rotate(V_c[i]);
    m_CosDecayAngle = -2.;
    m_DeltaPhiDecayPlane = -2.;

    MA5::MALorentzVector P_child[2];
    for(int i = 0; i < 2; i++) 
      P_child[i].SetVectM(V_c[i], m[i]);
    P_c.push_back(P_child[0]);
     
    if(N_c == 2){
      P_c.push_back(P_child[1]);
      return;
    }

    // Recursively generate other two-body decays for N > 2
    std::vector<double> M_pR;
    std::vector<double> M_cR;
    for(int i = 1; i < N_c; i++) M_pR.push_back(M_p[i]);
    for(int i = 1; i < N_c; i++) M_cR.push_back(M_c[i]);
    MA5::MABoost Booster;
    Booster.setBoostVector(P_child[1]);
    std::vector<MA5::MALorentzVector> P_cR;
    GenerateTwoBodyRecursive(M_pR, M_cR, Booster.BoostVector(), V_c[0].Cross(axis_par), P_cR);
    for(int i = 0; i < N_c-1; i++) Booster.boost(P_cR[i]);
    for(int i = 0; i < N_c-1; i++) P_c.push_back(P_cR[i]);
  }

  int DoubleMax(const void *a, const void *b){
    double aa = *((double*)a);
    double bb = *((double*)b);
    if (aa > bb) return  1;
    if (aa < bb) return -1;
    return 0; 
  }

}
