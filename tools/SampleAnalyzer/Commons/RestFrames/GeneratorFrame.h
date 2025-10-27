/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   GeneratorFrame.h
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

#ifndef GeneratorFrame_H
#define GeneratorFrame_H

#include <random>
#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // GeneratorFrame class
  ///////////////////////////////////////////////
  class GeneratorFrame : public RestFrame {
  public:
    GeneratorFrame(const std::string& sname, const std::string& stitle);
    GeneratorFrame();
    virtual ~GeneratorFrame();

    /// \brief Clears GeneratorFrame of all connections to other objects
    virtual void Clear();

    /// \brief Add a child RestFrame to this frame
    ///
    /// \param frame    RestFrame to be added as child
    ///
    /// Method for adding a RestFrame *frame* as a child 
    /// of this frame. *frame* will not be added as a child
    /// if it is already listed as a child.
    virtual void AddChildFrame(RestFrame& frame);

    /// \brief Set the parent frame for this frame
    ///
    /// \param frame     parent frame
    ///
    /// Method for connecting a child frame to its parent frame
    /// Empty default sets parent frame to none
    virtual void SetParentFrame(RestFrame& frame = 
				RestFrame::Empty());

    /// \brief Returns the parent of this frame
    ///
    /// Returns the parent frame of this frame.
    /// If the parent frame is not set, an empty
    /// frame is returned.
    virtual GeneratorFrame const& GetParentFrame() const;

    /// \brief Get the frame of the *i* th child
    virtual GeneratorFrame& GetChildFrame(int i = 0) const;

    void SetPCut(double cut);
    void SetPtCut(double cut);
    void SetEtaCut(double cut);
    void SetMassWindowCut(double min, double max);

    void RemovePCut();
    void RemovePtCut();
    void RemoveEtaCut();
    void RemoveMassWindowCut();

    /// \brief Print generator efficiency information
    void PrintGeneratorEfficiency() const;
    
    /// \brief Get the mass of this frame
    virtual double GetMass() const;

    /// \brief Frame is capable having a variable mass? (true/false)
    bool IsVariableMassMCMC() const;

    virtual double GetMinimumMassMCMC() const;
    virtual void GenerateMassMCMC(double& mass, double& prob, 
				  double max = -1.) const;
    virtual double GetProbMCMC(double mass = -1.) const;

    double GetRandom() const;
    double GetGaus(double mu, double sig) const;

    static GeneratorFrame& Empty();

  protected:
    double m_Mass;

    bool InitializeAnalysisRecursive();
    bool AnalyzeEventRecursive();
    bool ClearEventRecursive();

    virtual void ResetGenFrame() = 0;
    virtual bool GenerateFrame() = 0;

    void SetChildren(const std::vector<MA5::MALorentzVector>& P_children);
    virtual bool InitializeGenAnalysis();

    virtual bool IterateMCMC();
    bool IterateRecursiveMCMC();

    void SetVariableMassMCMC(bool var = true);
    virtual void SetMassMCMC(double mass);
    void SetMassMCMC(double mass, GeneratorFrame& frame) const;

    bool EventInAcceptance() const;

  private:
    std::mt19937 *m_Random;
    
    bool m_VarMassMCMC;

    mutable long m_Ngen;
    mutable long m_Npass;

    double m_PCut;
    double m_PtCut;
    double m_EtaCut;
    double m_minMassCut;
    double m_maxMassCut;

    bool m_doCuts;
    bool m_doPCut;
    bool m_doPtCut;
    bool m_doEtaCut;
    bool m_dominMassCut;
    bool m_domaxMassCut;
   
  };

}

#endif
