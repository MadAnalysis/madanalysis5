/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFCharge.h
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 May
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

#ifndef RFCharge_H
#define RFCharge_H

#include <cstdlib>

namespace RestFrames {

  ///////////////////////////////////////////////
  // RFCharge class
  ///////////////////////////////////////////////
  class RFCharge {

  public:
    RFCharge(const RFCharge& charge);
    RFCharge(int charge = 0);
    RFCharge(int charge_num, int charge_den);
    ~RFCharge();

    int GetNumerator() const;
    int GetDenominator() const;

    bool operator == (const RFCharge& val) const;
    bool operator == (int val) const;

    bool operator != (const RFCharge& val) const;
    bool operator != (int val) const;

    void operator = (const RFCharge& val);
    void operator = (int val);

    RFCharge operator + (const RFCharge& val) const;
    RFCharge operator + (int val) const;

    RFCharge operator - (const RFCharge& val) const;
    RFCharge operator - (int val) const;

    RFCharge operator * (const RFCharge& val) const;
    RFCharge operator * (int val) const;

    RFCharge operator / (const RFCharge& val) const;
    RFCharge operator / (int val) const;

    RFCharge& operator += (const RFCharge& val);
    RFCharge& operator += (int val);

    RFCharge& operator -= (const RFCharge& val);
    RFCharge& operator -= (int val);

    RFCharge& operator *= (const RFCharge& val);
    RFCharge& operator *= (int val);

    RFCharge& operator /= (const RFCharge& val);
    RFCharge& operator /= (int val);

    RFCharge operator-();

    operator double() const;

  private:
    bool m_Sign;
    int m_ChargeNum;
    int m_ChargeDen;

    void Simplify();

  };

  RFCharge operator * (int val1, const RFCharge& val2);
  RFCharge operator / (int val1, const RFCharge& val2);

  int gcd(int x, int y);
}

#endif
