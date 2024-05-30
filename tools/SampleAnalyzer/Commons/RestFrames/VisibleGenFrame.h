/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   VisibleGenFrame.h
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

#ifndef VisibleGenFrame_H
#define VisibleGenFrame_H

#include "SampleAnalyzer/Commons/RestFrames/VisibleFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/GeneratorFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // VisibleGenFrame class
  ///////////////////////////////////////////////
  class VisibleGenFrame : public VisibleFrame<GeneratorFrame> {
  public:
    VisibleGenFrame(const std::string& sname, const std::string& stitle);
    VisibleGenFrame();
    virtual ~VisibleGenFrame();

    static VisibleGenFrame& Empty();

    virtual void SetMass(double val);

  protected:
    virtual void ResetGenFrame();
    virtual bool GenerateFrame();

  private:
    static VisibleGenFrame m_Empty;
    void Init();

  };

}

#endif
