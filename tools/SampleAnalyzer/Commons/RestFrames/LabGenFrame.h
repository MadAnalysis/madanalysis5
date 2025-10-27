/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   LabGenFrame.h
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

#ifndef LabGenFrame_H
#define LabGenFrame_H

#include "SampleAnalyzer/Commons/RestFrames/LabFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/GeneratorFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // LabGenFrame class
  ///////////////////////////////////////////////
  class LabGenFrame : public LabFrame<GeneratorFrame> {
  public:
    LabGenFrame(const std::string& sname, const std::string& stitle);
    virtual ~LabGenFrame();
  
    virtual void Clear();

    virtual bool InitializeAnalysis();
    virtual bool ClearEvent();
    virtual bool AnalyzeEvent();

    virtual void SetThreeVector(const MA5::MAVector3& P);
    virtual void SetTransverseMomentum(double val);
    virtual void SetLongitudinalMomentum(double val);
    virtual void SetPhi(double val);

    virtual void SetPToverM(double val);

    void SetN_MCMCBurnIn(int N);
    void SetN_MCMCDiscard(int N);

    void SetFailTolerance(int N);
    
  protected:
    double m_PT;
    double m_PL;
    double m_Phi;
    double m_PToM;

    double m_MaxM;

    virtual bool InitializeGenAnalysis();

    virtual void ResetGenFrame();
    virtual bool GenerateFrame();

    double m_ChildMassMCMC;
    double m_ChildProbMCMC;

    virtual bool IterateMCMC();
    
    int m_NBurnInMCMC;
    int m_NDiscardMCMC;

    int m_FailTolerance;
    
    long m_Ngenerated;
    long m_Npassed;

  private:
    void Init();
    
  };

}

#endif
