/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2015, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   VisibleRecoFrame.cc
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

#include "SampleAnalyzer/Commons/RestFrames/VisibleRecoFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // VisibleRecoFrame class
  ///////////////////////////////////////////////
  VisibleRecoFrame::VisibleRecoFrame(const std::string& sname, 
				     const std::string& stitle) 
    : VisibleFrame<ReconstructionFrame>(sname, stitle) {}

  VisibleRecoFrame::VisibleRecoFrame() : VisibleFrame<ReconstructionFrame>() {}
 

  VisibleRecoFrame::~VisibleRecoFrame(){
   
  }

  VisibleRecoFrame& VisibleRecoFrame::Empty(){
    return VisibleRecoFrame::m_Empty;
  }

  VisibleRecoFrame VisibleRecoFrame::m_Empty;

}
