/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   ResonanceGenFrame.h
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

#ifndef ResonanceGenFrame_H
#define ResonanceGenFrame_H

#include "SampleAnalyzer/Commons/RestFrames/DecayGenFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // ResonanceGenFrame class
  ///////////////////////////////////////////////
  class ResonanceGenFrame : public DecayGenFrame {
  public:
    ResonanceGenFrame(const std::string& sname, const std::string& stitle);
    ResonanceGenFrame();
    virtual ~ResonanceGenFrame();

    static ResonanceGenFrame& Empty();

    virtual void SetMass(double mass);
    virtual void SetWidth(double w);

    virtual void SetVariableMass(bool varymass = true);

    virtual double GetPoleMass() const;
    virtual double GetWidth() const;
   
    virtual double GetProbMCMC(double mass = -1.) const;
    virtual void GenerateMassMCMC(double& mass, double& prob, 
				  double max = -1.) const;

  private:
    double m_PoleMass;
    double m_Width;

    static ResonanceGenFrame m_Empty;
  };

}

#endif
