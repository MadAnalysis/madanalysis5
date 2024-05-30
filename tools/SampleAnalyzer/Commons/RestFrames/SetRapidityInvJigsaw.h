/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   SetRapidityInvJigsaw.h
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

#ifndef SetRapidityInvJigsaw_H
#define SetRapidityInvJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/InvisibleJigsaw.h"

namespace RestFrames {

  class SetRapidityInvJigsaw : public InvisibleJigsaw {
  public:
    SetRapidityInvJigsaw(const std::string& sname, const std::string& stitle);
    SetRapidityInvJigsaw();
    virtual ~SetRapidityInvJigsaw();

    virtual std::string GetLabel() const { return "Set Invisible Rapidity"; }

    virtual void SetAxis(const MA5::MAVector3& axis);

    virtual bool AnalyzeEvent();

  protected:
    MA5::MAVector3 m_Axis;
  
  };

}

#endif
