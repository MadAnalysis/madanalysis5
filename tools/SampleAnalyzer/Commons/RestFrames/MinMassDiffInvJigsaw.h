/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassDiffInvJigsaw.h
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

#ifndef MinMassDiffInvJigsaw_H
#define MinMassDiffInvJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/neldermead.h"
#include "SampleAnalyzer/Commons/RestFrames/InvisibleJigsaw.h"

namespace RestFrames {

  class MinMassDiffInvJigsaw : public InvisibleJigsaw {
  public:
    MinMassDiffInvJigsaw(const std::string& sname, 
			 const std::string& stitle,
			 int N_vis_inv_pair);
    ~MinMassDiffInvJigsaw();

    virtual std::string GetLabel() const { return "Min #Sigma M(vis , inv)_{i}^{2}"; }

    virtual double GetMinimumMass() const;
    
    virtual bool AnalyzeEvent();

  private:
    const int m_Npair;
    mutable std::vector<MA5::MALorentzVector> m_Pvis;
    mutable std::vector<MA5::MALorentzVector> m_Pinv;
    std::vector<double> m_Minv;
    
    bool m_2D;
    MA5::MAVector3 m_X;
    MA5::MAVector3 m_Y;
    MA5::MAVector3 m_Z;

    double GetPScale(double Minv);
    void ApplyOptimalRotation();

    NELDERMEAD* m_minimizer;
    double EvaluateMetric(const double* param);

  };

}

#endif
