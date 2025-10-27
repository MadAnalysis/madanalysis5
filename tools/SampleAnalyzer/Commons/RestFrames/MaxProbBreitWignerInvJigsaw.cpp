/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MaxProbBreitWignerInvJigsaw.cc
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 Jun
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

//#include "Math/Factory.h"

#include "SampleAnalyzer/Commons/RestFrames/MaxProbBreitWignerInvJigsaw.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"
#include "SampleAnalyzer/Commons/Vector/MARotationGeneral.h"

namespace RestFrames {

  MaxProbBreitWignerInvJigsaw::MaxProbBreitWignerInvJigsaw(const std::string& sname, 
							   const std::string& stitle,
							   int N_vis_inv_pair) : 
    InvisibleJigsaw(sname, stitle, N_vis_inv_pair, N_vis_inv_pair), 
    m_Npair(N_vis_inv_pair)
  {
    //m_minimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Combined");
    m_minimizer = new NELDERMEAD();
    m_minimizer->SetMaxFunctionCalls(10000000);
    m_minimizer->SetMaxIterations(100000);
    m_minimizer->SetTolerance(0.001);
    m_minimizer->SetPrintLevel(0);

    //m_functor = new ROOT::Math::Functor(this, &MaxProbBreitWignerInvJigsaw::EvaluateMetric, 3);
    //m_minimizer->SetFunction(*m_functor);

    m_minimizer->SetVariable(0, "phi1", 0., 0.001);
    m_minimizer->SetVariable(1, "phi2", 0., 0.001);
    m_minimizer->SetVariable(2, "phi3", 0., 0.001);

    for(int i = 0; i < m_Npair; i++){
      m_Mass.push_back(0.);
      m_Width.push_back(-1.);
    }
  }
 
  MaxProbBreitWignerInvJigsaw::~MaxProbBreitWignerInvJigsaw(){
    delete m_minimizer;
    //delete m_functor;
  }

  double MaxProbBreitWignerInvJigsaw::GetMinimumMass() const {
    if(!IsSoundMind())
      return 0.;

    if(m_Npair <= 0) 
      return 0.;
    if(m_Npair == 1) 
      return std::max(0.,GetChildState(0).GetMinimumMass());
    
    MA5::MALorentzVector PvisTOT(0.,0.,0.,0.);
    m_Pvis.clear();
    for(int i = 0; i < m_Npair; i++){
      m_Pvis.push_back(GetDependancyStates(i+m_Npair).GetFourVector());
      PvisTOT += m_Pvis[i];
    }

    double ECM = 0.;
    double Minv;
    MA5::MABoost Booster;
    Booster.setBoostVector(-PvisTOT);
    for(int i = 0; i < m_Npair; i++){
      Booster.boost(m_Pvis[i]);
      Minv = std::max(0.,GetChildState(i).GetMinimumMass());
      ECM += sqrt(m_Pvis[i].Mag()*m_Pvis[i].Mag() + Minv*Minv);
    }

    return ECM;
  }

  bool MaxProbBreitWignerInvJigsaw::AnalyzeEvent(){
    if(!IsSoundMind())
      return SetSpirit(false);

    if(m_Npair <= 1) 
      return false;
    
    MA5::MALorentzVector INV = GetParentState().GetFourVector();
    double Minv = INV.M();

    MA5::MALorentzVector VIS(0.,0.,0.,0.);
    m_Pvis.clear();
    for(int i = 0; i < m_Npair; i++){
      m_Pvis.push_back(GetDependancyStates(i).GetFourVector());
      VIS += m_Pvis[i];
    }

    // go to INV+VIS CM frame
    MA5::MABoost BoosterCM;
    BoosterCM.setBoostVector(-VIS-INV);

    BoosterCM.boost(INV);
    BoosterCM.boost(VIS);
    for(int i = 0; i < m_Npair; i++)
      BoosterCM.boost(m_Pvis[i]);

    // INV states defined in the VIS frame
    MA5::MABoost BoosterVIS;
    BoosterVIS.setBoostVector(-VIS);
    m_Pinv.clear();
    m_Minv.clear();
    for(int i = 0; i < m_Npair; i++){
      m_Minv.push_back(std::max(0.,GetChildState(i).GetMinimumMass()));
      m_Pinv.push_back(m_Pvis[i]);
      BoosterVIS.boost(m_Pinv[i]);
      m_Pinv[i].SetVectM(m_Pinv[i].Vect(), m_Minv[i]);
    }

    // VIS states in INV frame
    MA5::MABoost BoosterINV;
    BoosterINV.setBoostVector(-INV);
    for(int i = 0; i < m_Npair; i++)
      BoosterINV.boost(m_Pvis[i]);

    if(m_Npair == 2){
      MA5::MAVector3 Vdiff = (m_Pvis[0].Vect()-m_Pvis[1].Vect()).Unit();
      double pinv = GetP(Minv, m_Minv[0], m_Minv[1]);
      m_Pinv[0].SetVectM( pinv*Vdiff, m_Minv[0]);
      m_Pinv[1].SetVectM(-pinv*Vdiff, m_Minv[1]);
    } else {
      double k = GetPScale(Minv);
      for(int i = 0; i < m_Npair; i++)
	m_Pinv[i].SetVectM(k*m_Pinv[i].Vect(), m_Minv[i]);
    }

    ApplyOptimalRotation();
    
    // return to original frame
    BoosterINV.setBoostVector(INV);
    BoosterCM.setBoostVector(VIS+INV);
    for(int i = 0; i < m_Npair; i++){
      BoosterINV.boost(m_Pinv[i]);
      BoosterCM.boost(m_Pinv[i]);
      GetChildState(i).SetFourVector(m_Pinv[i]);
    }
    
    return SetSpirit(true);
  }

  // Based on Newton-Raphson root finding
  double MaxProbBreitWignerInvJigsaw::GetPScale(double Minv){
    std::vector<double> Pinv2;
    double Ek  = 0.;
    double fk  = 0.;
    double dfk = 0.;
    for(int i = 0; i < m_Npair; i++){
      Pinv2.push_back(m_Pinv[i].P()*m_Pinv[i].P());
      Ek += sqrt(m_Minv[i]*m_Minv[i]+Pinv2[i]);
      fk += m_Minv[i];
    }
    if(fk > Minv || Ek <= 0.) return 0.;
    
    double k2 = Minv/Ek;
    k2 *= k2;
    double dk2 = k2;
    int count = 0;
    while(fabs(dk2) >= 1e-10 && count < 100){
      fk = -Minv;
      dfk = 0.;
      for(int i = 0; i < m_Npair; i++){
	Ek = sqrt(m_Minv[i]*m_Minv[i]+k2*Pinv2[i]);
	fk  += Ek;
	dfk += Ek > 0 ? Pinv2[i]/Ek : 0.;
      }
      dk2 = 2.*fk/dfk;
      k2 -= dk2;
      count++;
    }
    
    return sqrt(std::max(0.,k2));
  }

  // Optimal rotation found using Minuit2 ROOT interface
  void MaxProbBreitWignerInvJigsaw::ApplyOptimalRotation(){
    // first, check dimensionality of points
    m_Z.SetXYZ(0.,0.,0.);
    int index = 0;

    while(m_Z.Mag() <= 0. && index < m_Npair){
      m_Z = m_Pinv[index].Vect().Cross(m_Pvis[index].Vect());
      index++;
    }
    m_Z = m_Z.Unit();

    m_2D = true;
    if(m_Npair > 2){
      for(int i = 0; i < m_Npair; i++){
	if(m_Z.Dot(m_Pvis[i].Vect().Unit()) > 1e-8){
	  m_2D = false;
	  break;
	}
	if(m_Z.Dot(m_Pinv[i].Vect().Unit()) > 1e-8){
	  m_2D = false;
	  break;
	}
      }
    }
    
    m_minimizer->SetVariableValue(0, 0.);
    m_minimizer->SetVariableValue(1, 0.);
    m_minimizer->SetVariableValue(2, 0.);

    if(m_2D){
      m_minimizer->FixVariable(1);
      m_minimizer->FixVariable(2);
    } else {
      m_minimizer->ReleaseVariable(1);
      m_minimizer->ReleaseVariable(2);
      m_X.SetXYZ(0.,0.,0.);
      for(int i = 0; i < m_Npair; i++)
	m_X += m_Pvis[i].Vect() - m_Pinv[i].Vect();
      
      if(m_X.Mag() <= 0)
	m_X = m_Pvis[0].Vect().Cross(m_Z);
      
      m_X = m_X.Unit();
      m_Y = m_Z.Cross(m_X).Unit();
    }

    auto minf = std::bind(&MaxProbBreitWignerInvJigsaw::EvaluateMetric, this,std::placeholders::_1);
    m_minimizer->Minimize(minf);
    if(m_minimizer->Status() > 0 && !m_2D){
      MA5::MAVector3 temp = m_Z;
      m_Z = m_X;
      m_X = m_Y;
      m_Y = temp;
      m_minimizer->SetVariableValue(0, 0.);
      m_minimizer->SetVariableValue(1, 0.);
      m_minimizer->SetVariableValue(2, 0.);
      m_minimizer->Minimize(minf);
    }

    const double* PHIs = m_minimizer->X();
    MA5::MARotationGeneral RotatorY(PHIs[1], m_Y);
    MA5::MARotationGeneral RotatorX(PHIs[2], m_X);
    MA5::MARotationGeneral RotatorZ(PHIs[0], m_Z);

    for(int i = 0; i < m_Npair; i++){
      RotatorZ.rotate(m_Pinv[i]);
      if(!m_2D)
      {
        RotatorY.rotate(m_Pinv[i]);
        RotatorX.rotate(m_Pinv[i]);
      }
    }

    return;
  }

  double MaxProbBreitWignerInvJigsaw::EvaluateMetric(const double* param){
    std::vector<MA5::MALorentzVector> Pinv;
    MA5::MARotationGeneral RotatorZ(param[0], m_Z);
    MA5::MARotationGeneral RotatorY(param[1], m_Y);
    MA5::MARotationGeneral RotatorX(param[2], m_X);
    for(int i = 0; i < m_Npair; i++){
      Pinv.push_back(m_Pinv[i]);
      RotatorZ.rotate(Pinv[i]);
      if(!m_2D)
      {
        RotatorY.rotate(Pinv[i]);
        RotatorX.rotate(Pinv[i]);
      }
      Pinv[i] += m_Pvis[i];
    }

    double prob = 1.;
    MA5::MALorentzVector SUM(0.,0.,0.,0.);
    for(int i = 0; i < m_Npair; i++)
      SUM += Pinv[i];
    double M = SUM.M();
    for(int i = 0; i < m_Npair-1; i++){
      SUM -= Pinv[i];
      prob *= GetP((Pinv[i]+SUM).M(), Pinv[i].M(), SUM.M())/M;
    }

    double den;
    for(int i = 0; i < m_Npair; i++){
      if(m_Width[i] > 0.){
	den = Pinv[i].M2()-m_Mass[i]*m_Mass[i];
	den *= den;
	den += m_Mass[i]*m_Mass[i]*m_Width[i]*m_Width[i];
	if(den > 0.)
	  prob /= den;
      }
    }

    if(prob <= 0.)
      return 1e10;

    return 1./prob;
  }

  void MaxProbBreitWignerInvJigsaw::SetPoleMass(double mass, int i){
    if(i < 0 || i >= m_Npair)
      return;

    m_Mass[i] = std::max(mass, 0.);
  }

  void MaxProbBreitWignerInvJigsaw::SetWidth(double width, int i){
    if(i < 0 || i >= m_Npair)
      return;
    if(width <= 0.)
       m_Width[i] = -1.;
    else
      m_Width[i] = width;
  }

}
