/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassDiffCombJigsaw.h
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

#ifndef MinMassesSqCombJigsaw_H
#define MinMassesSqCombJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/CombinatoricJigsaw.h"

namespace RestFrames {

  class MinMassesSqCombJigsaw : public CombinatoricJigsaw {
  public:
    MinMassesSqCombJigsaw(const std::string& sname, 
			  const std::string& stitle,
			  int N_comb, int N_mass);
    virtual ~MinMassesSqCombJigsaw();

    virtual std::string GetLabel() const { return "Min #Sigma M_{i}^{2}"; }

  protected:
    virtual bool EvaluateMetric(double& metric) const;

  private:
    const int m_Ncomb;
    const int m_Nmass;

  };

}

#endif
