/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   InvisibleGroup.h
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

#ifndef InvisibleGroup_H
#define InvisibleGroup_H

#include "SampleAnalyzer/Commons/RestFrames/Group.h"
#include "SampleAnalyzer/Commons/RestFrames/InvisibleState.h"

namespace RestFrames {

  class InvisibleState;

  class InvisibleGroup : public Group {
  public:
    InvisibleGroup(const std::string& sname, const std::string& stitle);
    InvisibleGroup();
    virtual ~InvisibleGroup();
	
    virtual void Clear();

    virtual void AddFrame(RestFrame& frame);
    virtual void AddJigsaw(Jigsaw& jigsaw);
    
    // Event analysis functions
    virtual void SetMass(double M);
    virtual void SetLabFrameFourVector(const MA5::MALorentzVector& V);
    virtual void SetLabFrameThreeVector(const MA5::MAVector3& V);

    virtual MA5::MALorentzVector GetLabFrameFourVector() const;

    static InvisibleGroup& Empty();

  protected:
    virtual bool ClearEvent();
    virtual bool AnalyzeEvent();

    virtual InvisibleState& InitializeParentState();
    virtual InvisibleState& GetParentState() const;
    
    virtual InvisibleState& GetChildState(int i) const;

  private:
    MA5::MALorentzVector m_Lab_P;

    static InvisibleGroup m_Empty;

  };

}

#endif
