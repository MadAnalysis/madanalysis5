/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   VisibleFrame.h
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

#ifndef VisibleFrame_H
#define VisibleFrame_H

#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"

namespace RestFrames {

  class RestFrame;

  ///////////////////////////////////////////////
  // VisibleFrame class
  ///////////////////////////////////////////////
  template <class T>
  class VisibleFrame : public T {
  public:
    //constructor and destructor
    VisibleFrame(const std::string& sname,
		 const std::string& stitle);
    VisibleFrame();
    virtual ~VisibleFrame();

    virtual void SetCharge(const RFCharge& charge);
    virtual void SetCharge(int charge = 0);
    virtual void SetCharge(int charge_num, int charge_den);
    virtual RFCharge GetCharge() const;

    virtual void SetLabFrameFourVector(const MA5::MALorentzVector& V,
				       const RFCharge& charge = RFCharge());
    void SetLabFrameFourVector(const MA5::MALorentzVector& V, int charge);
    void SetLabFrameFourVector(const MA5::MALorentzVector& V,
			       int charge_num, int charge_den);
    
    
    virtual MA5::MALorentzVector GetLabFrameFourVector() const;

  protected:
    virtual bool IsSoundBody() const;

  private:
    MA5::MALorentzVector m_Lab_P;
    RFCharge m_Charge;

  };

}

#endif
