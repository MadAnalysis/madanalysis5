/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   DecayGenFrame.h
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

#ifndef DecayGenFrame_H
#define DecayGenFrame_H

#include "SampleAnalyzer/Commons/RestFrames/DecayFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/GeneratorFrame.h"

namespace RestFrames {

  class ResonanceGenFrame;

  ///////////////////////////////////////////////
  // DecayGenFrame class
  ///////////////////////////////////////////////
  class DecayGenFrame : public DecayFrame<GeneratorFrame> {
  public:
    DecayGenFrame(const std::string& sname, const std::string& stitle);
    DecayGenFrame();
    virtual ~DecayGenFrame();

    virtual void SetMass(double val);
    virtual void SetVariableMass(bool varymass = true);

    // For two-body decays
    virtual void SetCosDecayAngle(double val);
    virtual void SetDeltaPhiDecayPlane(double val);

    virtual double GetProbMCMC(double mass = -1.) const;
    virtual void GenerateMassMCMC(double& mass, double& prob, 
				  double max = -1.) const;

  protected:
    double m_CosDecayAngle;
    double m_DeltaPhiDecayPlane;

    std::vector<int>    m_ChildIndexMCMC;
    std::vector<double> m_ChildMassMCMC;
    std::vector<double> m_ChildProbMCMC;
    std::vector<double> m_InterMassFracMCMC;

    virtual bool IsSoundBody() const;

    virtual void ResetGenFrame();
    virtual bool GenerateFrame();
   
    void GenerateTwoBodyRecursive(const std::vector<double>& M_parent, 
				  const std::vector<double>& M_child,
				  const MA5::MAVector3& axis_par, 
				  const MA5::MAVector3& axis_perp,
				  std::vector<MA5::MALorentzVector>& P_child);

    virtual bool InitializeGenAnalysis();
    virtual bool IterateMCMC();

  };

  int DoubleMax(const void *a, const void *b);

}

#endif
