/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   VisibleState.h
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

#ifndef VisibleState_H
#define VisibleState_H

#include "SampleAnalyzer/Commons/RestFrames/State.h"

namespace RestFrames {

  class VisibleRecoFrame;

  class VisibleState : public State {
  public:
    VisibleState(const std::string& sname,
		 const std::string& stitle);
    VisibleState();
    virtual ~VisibleState();

    virtual void Clear();

    static VisibleState& Empty();

    virtual void AddFrame(const RestFrame& frame);

    virtual RestFrame const& GetFrame() const;

    virtual bool IsFrame(const RestFrame& frame) const;
    virtual bool IsFrames(const ConstRestFrameList& frames) const;
	
    virtual void SetLabFrameFourVector();
    virtual void SetCharge(const RFCharge& charge);
    virtual void SetCharge(int charge = 0);
    virtual void SetCharge(int charge_num, int charge_den);

  protected:
    const VisibleRecoFrame* m_FramePtr;

  private:
    static VisibleState m_Empty;

  };

}

#endif
