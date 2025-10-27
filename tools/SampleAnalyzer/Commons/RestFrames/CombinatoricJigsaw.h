/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   CombinatoricJigsaw.h
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

#ifndef CombinatoricJigsaw_H
#define CombinatoricJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/Jigsaw.h"
#include "SampleAnalyzer/Commons/RestFrames/CombinatoricGroup.h"
#include "SampleAnalyzer/Commons/RestFrames/CombinatoricState.h"

namespace RestFrames {

  class CombinatoricJigsaw : public Jigsaw {
  public:
    //constructor and destructor
    CombinatoricJigsaw(const std::string& sname, 
		       const std::string& stitle,
		       int Ncomb, int Nobject);
    CombinatoricJigsaw();
    virtual ~CombinatoricJigsaw();

    virtual void Clear();

    static CombinatoricJigsaw& Empty();

    virtual void SetGroup(Group& group = Group::Empty());
    virtual CombinatoricGroup& GetGroup() const;

    virtual void AddCombFrame(const RestFrame& frame, int i = 0);
    void AddCombFrames(const ConstRestFrameList& frames, int i = 0);

    virtual void AddObjectFrame(const RestFrame& frame, int i = 0);
    void AddObjectFrames(const ConstRestFrameList& frames, int i = 0);

    void SetCombCharge(const RFCharge& charge, int i);
    void SetCombCharge(int charge, int i);
    void SetCombCharge(int charge_num, int charge_den, int i);
    void UnsetCombCharge(int i);

    void SetObjectCharge(const RFCharge& charge, int i);
    void SetObjectCharge(int charge, int i);
    void SetObjectCharge(int charge_num, int charge_den, int i);
    void UnsetObjectCharge(int i);

  protected:
    virtual bool IsSoundBody() const;
    CombinatoricState& GetNewChildState();
    
    virtual bool InitializeAnalysis();

    virtual bool InitializeCombinatoric();
    virtual bool LoopCombinatoric();

    virtual bool EvaluateMetric(double& metric) const = 0;

    virtual bool AnalyzeEvent();
    
    int GetNInputStates() const; 
    VisibleState& GetInputState(int i = 0) const;

    int GetNinputForChild(int i = 0) const;
    bool IsNinputExclForChild(int i = 0) const;

    bool IsChargeSetForChild(int i = 0) const;
    RFCharge GetChargeForChild(int i = 0) const;
    bool IsChargeSetForObject(int i = 0) const;
    RFCharge GetChargeForObject(int i = 0) const;

    void SetParentState(State& state = State::Empty());
    CombinatoricState const& GetParentState() const;
    
    CombinatoricState& GetChildState(int i) const;
 
    bool InitializeJigsawExecutionList(JigsawList& exec_jigsaws);

    JigsawList m_ExecuteJigsaws;
    bool ExecuteDependancyJigsaws();

  private:
    const int m_Ncomb;
    const int m_Nobj;

    VisibleStateList m_InputStates;

    int m_NinputTOT;
    bool m_NExclusiveTOT;
    std::vector<int> m_NForChild;
    std::vector<int> m_NExclusive;
    mutable std::map<int, RFCharge> m_ChargeForChild;
    mutable std::map<int, RFCharge> m_ChargeForObject;
  };

}

#endif
