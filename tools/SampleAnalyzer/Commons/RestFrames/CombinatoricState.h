/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   CombinatoricState.h
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

#ifndef CombinatoricState_H
#define CombinatoricState_H

#include "SampleAnalyzer/Commons/RestFrames/State.h"

namespace RestFrames {

  class VisibleState;
  class Jigsaw;

  ///////////////////////////////////////////////
  // CombinatoricState class
  ///////////////////////////////////////////////
  class CombinatoricState : public State {
  public:
    CombinatoricState(const std::string& sname, const std::string& stitle);
    CombinatoricState();
    virtual ~CombinatoricState();
	
    static CombinatoricState& Empty();

    virtual void Clear();

    virtual void AddFrame(const RestFrame& frame);

    virtual void SetParentJigsaw(Jigsaw& jigsaw);
    virtual void SetChildJigsaw(Jigsaw& jigsaw);

    virtual void Boost(const MA5::MAVector3& B);
    virtual MA5::MALorentzVector GetFourVector() const;
    virtual RFCharge GetCharge() const;

    void ClearElements();
    void AddElement(VisibleState& state);
    void AddElements(const VisibleStateList& states);
    VisibleStateList const& GetElements() const;
    int GetNElements() const;

    bool ContainsElement(const State& state) const;
    bool ContainsElement(const RFKey& key) const;
    VisibleState const& GetElement(const RFKey& key) const;

  protected:
    VisibleStateList m_Elements;
  
  private:
    static CombinatoricState m_Empty;
  
  };

}

#endif
