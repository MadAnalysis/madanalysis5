/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassesInvSqJigsaw.h
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 Feb
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

#ifndef MinMassesSqInvJigsaw_H
#define MinMassesSqInvJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/InvisibleJigsaw.h"

namespace RestFrames {

  class MinMassesSqInvJigsaw : public InvisibleJigsaw {
  public:
    MinMassesSqInvJigsaw(const std::string& sname, 
		       const std::string& stitle,
		       int N_vis_inv_pair);
    MinMassesSqInvJigsaw();
    ~MinMassesSqInvJigsaw();

    virtual std::string GetLabel() const { return "Min Masses Sq."; }

    virtual double GetMinimumMass() const;
    
    virtual bool AnalyzeEvent();

    static MinMassesSqInvJigsaw& Empty();


  private:
    const int m_Npair;
    mutable std::vector<MA5::MALorentzVector> m_Pvis;
    mutable std::vector<MA5::MALorentzVector> m_Pinv;
    std::vector<double>        m_Minv;

    double GetPScale(double Minv);
    void ApplyOptimalRotation();

    static MinMassesSqInvJigsaw m_Empty;
  };

}

#endif
