/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2015, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   VisibleGenFrame.cc
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

#include "SampleAnalyzer/Commons/RestFrames/VisibleGenFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // VisibleGenFrame class
  ///////////////////////////////////////////////
  VisibleGenFrame::VisibleGenFrame(const std::string& sname, 
				   const std::string& stitle) : 
    VisibleFrame<GeneratorFrame>(sname, stitle) {}

  VisibleGenFrame::VisibleGenFrame() : VisibleFrame<GeneratorFrame>() {}

  VisibleGenFrame::~VisibleGenFrame() {}

  /// \brief Returns empty instance of class
  VisibleGenFrame& VisibleGenFrame::Empty(){
    return VisibleGenFrame::m_Empty;
  }

  void VisibleGenFrame::SetMass(double val){
    if(val < 0.){
      m_Log << LogWarning;
      m_Log << "Unable to set mass to negative value ";
      m_Log << val << ". Setting to zero." << LogEnd;
      m_Mass = 0.;
    } else {
      m_Mass = val;
    }
  }

  void VisibleGenFrame::ResetGenFrame() {}

  bool VisibleGenFrame::GenerateFrame(){ 
    return true;
  }

  VisibleGenFrame VisibleGenFrame::m_Empty;

}
