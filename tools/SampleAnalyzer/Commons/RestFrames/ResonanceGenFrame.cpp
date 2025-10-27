/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   ResonanceGenFrame.cc
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
#include "SampleAnalyzer/Commons/RestFrames/ResonanceGenFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // ResonanceGenFrame class
  ///////////////////////////////////////////////

  ResonanceGenFrame::ResonanceGenFrame(const std::string& sname, 
				       const std::string& stitle)
    : DecayGenFrame(sname,stitle)
  {
    m_PoleMass = 0.;
    m_Width = 0.;
  }

  ResonanceGenFrame::ResonanceGenFrame() : DecayGenFrame()
  {
    m_PoleMass = 0.;
    m_Width = 0.;
  }

  ResonanceGenFrame::~ResonanceGenFrame() {}

  ResonanceGenFrame& ResonanceGenFrame::Empty(){
    return ResonanceGenFrame::m_Empty;
  }

  void ResonanceGenFrame::SetMass(double val){
    SetMind(false);

    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "Unable to set mass to negative value ";
      m_Log << val << ". Setting to zero." << LogEnd;
      m_PoleMass = 0.;
      m_Mass = 0.;
    } else {
      m_PoleMass = val;
      m_Mass = val;
    }
  }

  void ResonanceGenFrame::SetWidth(double val){
    SetMind(false);
    
    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "Unable to set width to negative value ";
      m_Log << val << ". Setting to zero." << LogEnd;
      m_Width = 0.;
      SetVariableMassMCMC(false);
    } else {
      m_Width = val;
      SetVariableMassMCMC(true);
    }
  }

  void ResonanceGenFrame::SetVariableMass(bool varymass) {
    SetMind(false);

    if(varymass){
      if(m_Width > 0.){
	SetVariableMassMCMC(true);
      }
      else {
	m_Log << LogWarning;
	m_Log << "Unable to set variable mass. ";
	m_Log << "Resonance width is set to zero. " << LogEnd;
      }
    } else {
      m_Mass = m_PoleMass;
      SetVariableMassMCMC(false);
    }
  }


  double ResonanceGenFrame::GetPoleMass() const {
    return m_PoleMass;
  }

  double ResonanceGenFrame::GetWidth() const {
    return m_Width;
  }

  double ResonanceGenFrame::GetProbMCMC(double mass) const {
    if(mass < 0)
      mass = GetMass();

    double den = mass*mass-m_PoleMass*m_PoleMass;
    den *= den;
    den += m_PoleMass*m_PoleMass*m_Width*m_Width;

    if(den > 0)
      return (DecayGenFrame::GetProbMCMC(mass)*mass*mass)/den;
    else
      return 0.;
  }

  void ResonanceGenFrame::GenerateMassMCMC(double& mass, double& 
					   prob, double max) const {
    double min = 0.;
    int N = GetNChildren();
    for(int i = 0; i < N; i++)
      min += GetChildFrame(i).GetMass();

    if(((max < min) && (max > 0)) || m_Width <= 0.){
      mass = 0.;
      prob = 1.;
      return;
    }
    
    if(min <= 0)
      min = 0.;
    
    double M2 = m_PoleMass*m_PoleMass;
    double MW = m_PoleMass*m_Width;
    double Imin = atan((min*min-M2)/MW);
    double Imax;
    if(max <= 0) 
      //Imax = TMath::Pi()/2.;
      Imax = M_PI/2.;
    else 
      Imax = atan((max*max-M2)/MW);
    
    mass = sqrt(M2 + MW*tan(Imin+GetRandom()*(Imax-Imin)));

    double den = mass*mass-m_PoleMass*m_PoleMass;
    den *= den;
    den += m_PoleMass*m_PoleMass*m_Width*m_Width;
    if(den > 0)
      prob = 1./den;
    else
      prob =  0.;
  }

  ResonanceGenFrame ResonanceGenFrame::m_Empty;
}
