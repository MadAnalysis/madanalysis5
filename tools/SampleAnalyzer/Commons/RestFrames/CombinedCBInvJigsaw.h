/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   CombinedCBInvJigsaw.h
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

#ifndef CombinedCBInvJigsaw_H
#define CombinedCBInvJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/InvisibleJigsaw.h"

namespace RestFrames {

  class ContraBoostInvJigsaw;

  class CombinedCBInvJigsaw : public InvisibleJigsaw {
  public:
    CombinedCBInvJigsaw(const std::string& sname, 
			const std::string& stitle,
			int N_CBjigsaw);
    ~CombinedCBInvJigsaw();

    virtual std::string GetLabel() const {
      return "Combined Contra-boost Inv.";
    }

    void AddJigsaw(const ContraBoostInvJigsaw& jigsaw, int ijigsaw);
    
    virtual double GetMinimumMass() const;
    
    virtual bool AnalyzeEvent();

  private:
    const int m_NCB;
    double GetCBMinimumMass(int i) const;

  };

}

#endif
