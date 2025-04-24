/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   InvisibleJigsaw.h
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

#ifndef InvisibleJigsaw_H
#define InvisibleJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/InvisibleGroup.h"
#include "SampleAnalyzer/Commons/RestFrames/InvisibleState.h"

namespace RestFrames {

  class InvisibleJigsaw : public Jigsaw {
  public:
    InvisibleJigsaw(const std::string& sname, 
		    const std::string& stitle, 
		    int Ninvisible, int Nvisible);
    InvisibleJigsaw();
    virtual ~InvisibleJigsaw();

    virtual void Clear();

    void SetGroup(Group& group = Group::Empty());
    InvisibleGroup& GetGroup() const;

    void AddVisibleFrame(const RestFrame& frame, int i = 0);
    void AddVisibleFrames(const ConstRestFrameList& frames, int i = 0);
    
    void AddInvisibleFrame(const RestFrame& frame, int i = 0);
    void AddInvisibleFrames(const ConstRestFrameList& frames, int i = 0);
    
    void AddMassFrame(const RestFrame& frame, int i = 0);
    void AddMassFrames(const ConstRestFrameList& frames, int i = 0);

    virtual double GetMinimumMass() const;

    static InvisibleJigsaw& Empty();

  protected:
    bool m_InvMassDependancy;

    virtual bool IsSoundBody() const;
    InvisibleState& GetNewChildState();

    void SetParentState(State& state = State::Empty());
    InvisibleState const& GetParentState() const;
    
    InvisibleState& GetChildState(int i) const;

    virtual bool InitializeAnalysis();
    bool InitializeDependancyJigsaws();
    bool InitializeJigsawExecutionList(JigsawList& exec_jigsaws);

    virtual void FillInvisibleMassJigsawDependancies(JigsawList& jigsaws) const;

  private:
    const int m_Nvis;
    const int m_Ninv;  
  
  };

}

#endif
