/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   LabRecoFrame.h
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

#ifndef LabRecoFrame_H
#define LabRecoFrame_H

#include "SampleAnalyzer/Commons/RestFrames/ReconstructionFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/LabFrame.h"

namespace RestFrames {

  class Jigsaw;
  class VisibleState;

  ///////////////////////////////////////////////
  // LabRecoFrame class
  ///////////////////////////////////////////////
  class LabRecoFrame : public LabFrame<ReconstructionFrame> {
  public:
    LabRecoFrame(const std::string& sname, const std::string& stitle);
    LabRecoFrame();
    virtual ~LabRecoFrame();
    
    virtual void Clear();

    virtual bool InitializeAnalysis();
    virtual bool ClearEvent();
    virtual bool AnalyzeEvent();

    void AddTreeState(VisibleState& state) const;
    void AddTreeStates(const VisibleStateList& states) const;
    void RemoveTreeState(const VisibleState& state) const;
    void RemoveTreeStates(const VisibleStateList& states) const;
    VisibleStateList const& GetTreeStates() const;

  protected:
    GroupList  m_TreeGroups;
    JigsawList m_TreeJigsaws;
    mutable VisibleStateList m_TreeStates;
  
    bool InitializeTreeStates();
    bool InitializeTreeGroups();
    bool InitializeTreeJigsaws();

    bool ExecuteJigsaws();

  private:
    VisibleState& GetNewVisibleState();
    VisibleStateList m_InitStates;

  };

}

#endif
