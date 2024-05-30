/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassesCombJigsaw.h
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

#ifndef MinMassesCombJigsaw_H
#define MinMassesCombJigsaw_H

#include "SampleAnalyzer/Commons/RestFrames/CombinatoricJigsaw.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // MinMassesCombJigsaw class
  ///////////////////////////////////////////////
  class MinMassesCombJigsaw : public CombinatoricJigsaw {
  public:
    MinMassesCombJigsaw(const std::string& sname, const std::string& stitle);
    MinMassesCombJigsaw();
    virtual ~MinMassesCombJigsaw();

    virtual std::string GetLabel() const { return "Minimize Masses"; }
    
    virtual void AddFrame(const RestFrame& frame, int i = 0);
    virtual void AddFrames(const ConstRestFrameList& frames, int i = 0);

    static MinMassesCombJigsaw& Empty();

  protected:
    virtual bool EvaluateMetric(double& metric) const;
    virtual bool AnalyzeEvent();

  private:
    static MinMassesCombJigsaw m_Empty;

  };

}

#endif
