/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   InvisibleFrame.h
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

#ifndef InvisibleFrame_H
#define InvisibleFrame_H

#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"

namespace RestFrames {

  class RestFrame;

  ///////////////////////////////////////////////
  // InvisibleFrame class
  ///////////////////////////////////////////////
  template <class T>
  class InvisibleFrame : public T {
  public:
    //constructor and destructor
    InvisibleFrame(const std::string& sname, const std::string& stitle);
    virtual ~InvisibleFrame();

  protected:
    virtual bool IsSoundBody() const;

  };

}

#endif
