/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MaxProbBreitWignerCombJigsaw.h
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 Jun
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

#ifndef MaxProbBreitWignerCombJigsaw_H
#define MaxProbBreitWignerCombJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/CombinatoricJigsaw.h"

namespace RestFrames {

  class MaxProbBreitWignerCombJigsaw : public CombinatoricJigsaw {
  public:
    MaxProbBreitWignerCombJigsaw(const std::string& sname, 
				 const std::string& stitle,
				 int N_comb, int N_object);
    virtual ~MaxProbBreitWignerCombJigsaw();

    virtual std::string GetLabel() const { return "Max Prob Breit-Wigner"; }
    
    virtual void SetPoleMass(double mass, int i = 0);
    virtual void SetWidth(double width, int i = 0);

  protected:
    virtual bool EvaluateMetric(double& metric) const;

  private:
    const int m_Ncomb;
    const int m_Nobj;

    std::vector<double> m_Mass;
    std::vector<double> m_Width;

  };

}

#endif
