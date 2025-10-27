/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFCharge.cc
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

#include "SampleAnalyzer/Commons/RestFrames/RFCharge.h"

namespace RestFrames {

  RFCharge::RFCharge(const RFCharge& charge){
    int num = charge.GetNumerator();
    int den = charge.GetDenominator();

    if(den == 0){
      m_ChargeNum = 0;
      m_ChargeDen = 1;
      m_Sign = true;
      return;
    }

    if(num*den < 0)
      m_Sign = false;
    else 
      m_Sign = true;

    m_ChargeNum = std::abs(num);
    m_ChargeDen = std::abs(den);

    if(m_ChargeDen > 1)
      Simplify();
  }

  RFCharge::RFCharge(int charge){
    if(charge < 0)
      m_Sign = false;
    else
      m_Sign = true;

    m_ChargeNum = std::abs(charge);
    m_ChargeDen = 1;
  }

  RFCharge::RFCharge(int num, int den){
    if(den == 0){
      m_ChargeNum = 0;
      m_ChargeDen = 1;
      m_Sign = true;
      return;
    }
    
    if(num*den < 0)
      m_Sign = false;
    else 
      m_Sign = true;
    
    m_ChargeNum = std::abs(num);
    m_ChargeDen = std::abs(den);
    
    if(m_ChargeDen > 1)
      Simplify();
  }
  
  RFCharge::~RFCharge() {}

  int RFCharge::GetNumerator() const {
    if(m_Sign)
      return m_ChargeNum;
    else
      return -m_ChargeNum;
  }
  
  int RFCharge::GetDenominator() const {
    return m_ChargeDen;
  }

  void RFCharge::Simplify(){
    int f = gcd(m_ChargeNum, m_ChargeDen);
    if(f > 1){
      m_ChargeNum /= f;
      m_ChargeDen /= f;
    }
  }

  bool RFCharge::operator==(const RFCharge& val) const {
    if(m_Sign)
      return (m_ChargeNum == val.GetNumerator()) 
	&& (m_ChargeDen == val.GetDenominator());
    else
      return (-m_ChargeNum == val.GetNumerator()) 
	&& (m_ChargeDen == val.GetDenominator());
  }

  bool RFCharge::operator==(int val) const {
    if(m_ChargeDen != 1)
      return false;

    if(m_Sign)
      return (m_ChargeNum == val);
    else
      return (-m_ChargeNum == val);
  }

  bool RFCharge::operator!=(const RFCharge& val) const {
    if(m_Sign)
      return (m_ChargeNum != val.GetNumerator()) 
	|| (m_ChargeDen != val.GetDenominator());
    else
      return (-m_ChargeNum != val.GetNumerator()) 
	|| (m_ChargeDen != val.GetDenominator());
  }

  bool RFCharge::operator!=(int val) const{
    if(m_ChargeDen != 1)
      return true;

    if(m_Sign)
      return (m_ChargeNum != val);
    else
      return (-m_ChargeNum != val);
  }

  void RFCharge::operator=(const RFCharge& val){
    int num = val.GetNumerator();

    if(num < 0)
      m_Sign = false;
    else 
      m_Sign = true;

    m_ChargeNum = std::abs(num);
    m_ChargeDen = val.GetDenominator();
  }

  void RFCharge::operator=(int val){
    if(val < 0)
      m_Sign = false;
    else 
      m_Sign = true;

    m_ChargeNum = std::abs(val);
    m_ChargeDen = 1;
  }

  RFCharge RFCharge::operator+(const RFCharge& val) const {
    int den  = val.GetDenominator();

    return RFCharge(GetNumerator()*den+val.GetNumerator()*m_ChargeDen, 
		    den*m_ChargeDen);
  }

  RFCharge RFCharge::operator+(int val) const {
    return RFCharge(GetNumerator()+val*m_ChargeDen, m_ChargeDen);
  }

  RFCharge RFCharge::operator-(const RFCharge& val) const {
    int den  = val.GetDenominator();

    return RFCharge(GetNumerator()*den-val.GetNumerator()*m_ChargeDen, 
		    den*m_ChargeDen);
  }

  RFCharge RFCharge::operator-(int val) const {
    return RFCharge(GetNumerator()-val*m_ChargeDen, m_ChargeDen);
  }

  RFCharge RFCharge::operator*(const RFCharge& val) const {
    return RFCharge(GetNumerator()*val.GetNumerator(), 
		    val.GetDenominator()*m_ChargeDen);
  }

  RFCharge RFCharge::operator*(int val) const {
    return RFCharge(GetNumerator()*val, m_ChargeDen);
  }

  RFCharge RFCharge::operator/(const RFCharge& val) const {
    return RFCharge(GetNumerator()*val.GetDenominator(), 
		    val.GetNumerator()*m_ChargeDen);
  }

  RFCharge RFCharge::operator/(int val) const {
    return RFCharge(GetNumerator(), m_ChargeDen*val);
  }

  RFCharge& RFCharge::operator+=(const RFCharge& val){
    int den  = val.GetDenominator();
    int mnum = GetNumerator()*den + val.GetNumerator()*m_ChargeDen;

    if(mnum < 0)
      m_Sign = false;
    else
      m_Sign = true;

    m_ChargeNum = std::abs(mnum);
    m_ChargeDen *= den;

    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge& RFCharge::operator+=(int val){
    int mnum = GetNumerator() + val*m_ChargeDen;

    if(mnum < 0)
      m_Sign = false;
    else
      m_Sign = true;

    m_ChargeNum = std::abs(mnum);
    
    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge& RFCharge::operator-=(const RFCharge& val){
    int den  = val.GetDenominator();
    int mnum = GetNumerator()*den - val.GetNumerator()*m_ChargeDen;

    if(mnum < 0)
      m_Sign = false;
    else
      m_Sign = true;

    m_ChargeNum = std::abs(mnum);
    m_ChargeDen *= den;

    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge& RFCharge::operator-=(int val){
    int mnum = GetNumerator() - val*m_ChargeDen;

    if(mnum < 0)
      m_Sign = false;
    else
      m_Sign = true;

    m_ChargeNum = std::abs(mnum);
    
    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge& RFCharge::operator*=(const RFCharge& val){
    int mnum = GetNumerator()*val.GetNumerator();

    if(mnum == 0){
      m_ChargeNum = 0;
      m_ChargeDen = 1;
      m_Sign = true;
      return *this;
    }

    if(mnum < 0)
      m_Sign = false;
    else
      m_Sign = true;

    m_ChargeNum = std::abs(mnum);
    m_ChargeDen *= val.GetDenominator();

    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge& RFCharge::operator*=(int val){
    int mnum = GetNumerator()*val;

    if(mnum == 0){
      m_ChargeNum = 0;
      m_ChargeDen = 1;
      m_Sign = true;
      return *this;
    }

    if(mnum < 0)
      m_Sign = false;
    else
      m_Sign = true;

    m_ChargeNum = std::abs(mnum);

    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge& RFCharge::operator/=(const RFCharge& val){
    int num = val.GetNumerator();
    
    if(num == 0){
      m_ChargeNum = 0;
      m_ChargeDen = 1;
      m_Sign = true;
      return *this;
    }

    if(num < 0)
      m_Sign = !m_Sign;

    m_ChargeDen *= std::abs(num);
    m_ChargeNum *= val.GetDenominator();

    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge& RFCharge::operator/=(int val){
    if(val == 0){
      m_ChargeNum = 0;
      m_ChargeDen = 1;
      m_Sign = true;
      return *this;
    }

    if(val < 0)
      m_Sign = !m_Sign;

    m_ChargeDen *= std::abs(val);

    if(m_ChargeDen > 1)
      Simplify();

    return *this;
  }

  RFCharge RFCharge::operator-() {
    return (*this)*(-1);
  }

  RFCharge::operator double() const {
    return double(GetNumerator())/
      double(m_ChargeDen);
  }

  RFCharge operator*(int val1, const RFCharge& val2){
    return val2*val1;
  }
  
  RFCharge operator/(int val1, const RFCharge& val2){
    return RFCharge(val1)/val2;
  }

  int gcd(int x, int y){
    if(x < y)
      return gcd( y, x );
    
    int f = x % y;
    if(f == 0)
      return y;
    else
      return gcd( y, f );
  }

}
