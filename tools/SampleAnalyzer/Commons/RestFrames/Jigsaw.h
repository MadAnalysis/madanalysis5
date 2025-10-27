/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   Jigsaw.h
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

#ifndef Jigsaw_H
#define Jigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/RFBase.h"
#include "SampleAnalyzer/Commons/RestFrames/Group.h"

namespace RestFrames {

  enum JigsawType { kVanillaJigsaw, kInvisibleJigsaw, 
		    kCombinatoricJigsaw };

  ///////////////////////////////////////////////
  // Jigsaw class
  ///////////////////////////////////////////////
  class Jigsaw : public RFBase {
  public:
    Jigsaw(const std::string& sname, 
	   const std::string& stitle,
	   int Nchild, int Ndependancy);
    Jigsaw();
    
    virtual ~Jigsaw();

    /// \brief Clears Jigsaw of all connections to other objects
    virtual void Clear();

    JigsawType GetType() const { return m_Type; }

    virtual std::string GetLabel() const { return "Jigsaw"; }
  
    /// \brief String of information associated with Jigsaw
    virtual std::string PrintString(LogType type) const;

    bool IsInvisibleJigsaw() const;
    bool IsCombinatoricJigsaw() const;

    virtual void SetGroup(Group& group = Group::Empty());
    virtual Group& GetGroup() const;

    virtual ConstRestFrameList GetParentFrames() const;
    virtual ConstRestFrameList const& GetChildFrames(int i) const;
    virtual ConstRestFrameList const& GetDependancyFrames(int i) const;
    
    void RemoveFrame(const RestFrame& frame);
    void RemoveFrames(const ConstRestFrameList& frames);

    static Jigsaw& Empty();
    
  protected:
    JigsawType m_Type;
    virtual bool IsSoundBody() const;

    virtual bool AnalyzeEvent() = 0;

    bool CanResolve(const State& state) const;
    bool CanResolve(const ConstRestFrameList& frames) const;
    bool DependsOnJigsaw(const Jigsaw& jigsaw) const;

    virtual bool InitializeTree();
    virtual bool InitializeAnalysis();

    virtual bool InitializeDependancyJigsaws();
    virtual bool InitializeJigsawExecutionList(JigsawList& exec_jigsaws) = 0;
    
    void AddChildFrame(const RestFrame& frame, int i = 0);
    void AddDependancyFrame(const RestFrame& frame, int i = 0);

    virtual int GetNChildren() const;
    virtual State& GetChildState(int i) const;
    virtual StateList const& GetChildStates() const;

    int GetNDependancyStates() const;
    virtual StateList const& GetDependancyStates(int i) const;

    void SetParentState();
    virtual void SetParentState(State& state);
    virtual State const& GetParentState() const;

    virtual State& GetNewChildState() = 0;

    virtual void FillGroupJigsawDependancies(JigsawList& jigsaws) const;
    virtual void FillStateJigsawDependancies(JigsawList& jigsaws) const;

  private:
    Group* m_GroupPtr;
    State* m_ParentStatePtr;

    const int m_Nchild; 
    const int m_Ndeps;

    StateList     m_ChildStates;
    StateListList m_DependancyStates;

    std::vector<ConstRestFrameList> m_ChildFrames;
    std::vector<ConstRestFrameList> m_DependancyFrames;

    JigsawList m_DependancyJigsaws;

    static int m_class_key;

    friend class TreePlot;
    friend class Group;
    friend class LabRecoFrame;
    friend class InvisibleJigsaw;
    friend class CombinatoricJigsaw;

  };

}

#endif
