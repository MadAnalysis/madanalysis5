/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   VisibleFrame.cc
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

#include "SampleAnalyzer/Commons/RestFrames/VisibleFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/ReconstructionFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/GeneratorFrame.h"

namespace RestFrames {
  ///////////////////////////////////////////////
  // VisibleFrame class methods
  ///////////////////////////////////////////////
  template <class T> 
  VisibleFrame<T>::VisibleFrame(const std::string& sname, 
				const std::string& stitle) 
    : T(sname, stitle)
  {
    T::m_Type = kVisibleFrame;
    m_Charge = 0;
  }

  template <class T> 
  VisibleFrame<T>::VisibleFrame() : T() {}

  template <class T> 
  VisibleFrame<T>::~VisibleFrame() {}

  template <class T> 
  bool VisibleFrame<T>::IsSoundBody() const {
    if(RFBase::IsSoundBody()) 
      return true;
    if(!RestFrame::IsSoundBody())
      return T::SetBody(false);
    
    int Nchild = T::GetNChildren();
    if(Nchild > 0 || !T::GetParentFrame()){
      T::m_Log << LogWarning << "Problem with parent or child frames" << LogEnd;
      return T::SetBody(false);
    }
    return T::SetBody(true);
  }

  template <class T> 
  void VisibleFrame<T>::SetCharge(const RFCharge& charge){
    m_Charge = charge;
  }

  template <class T> 
  void VisibleFrame<T>::SetCharge(int charge){
    m_Charge = charge;
  }

  template <class T> 
  void VisibleFrame<T>::SetCharge(int charge_num, int charge_den){
    RFCharge charge(charge_num, charge_den);
    m_Charge = charge;
  }

  template <class T> 
  RFCharge VisibleFrame<T>::GetCharge() const {
    return m_Charge;
  }
  
  template <class T> 
  void VisibleFrame<T>::SetLabFrameFourVector(const MA5::MALorentzVector& V,
					      const RFCharge& charge){
    m_Lab_P.SetVectM(V.Vect(),V.M());
    SetCharge(charge);
  }
  
  template <class T> 
  void VisibleFrame<T>::SetLabFrameFourVector(const MA5::MALorentzVector& V,
					      int charge){
    SetLabFrameFourVector(V, RFCharge(charge));
  }
  
  template <class T> 
  void VisibleFrame<T>::SetLabFrameFourVector(const MA5::MALorentzVector& V,
					      int charge_num, 
					      int charge_den){
    SetLabFrameFourVector(V, RFCharge(charge_num,charge_den));
  }
  
  template <class T> 
  MA5::MALorentzVector VisibleFrame<T>::GetLabFrameFourVector() const {
    MA5::MALorentzVector V;
    V.SetVectM(m_Lab_P.Vect(),m_Lab_P.M());
    return V;
  }

  template class VisibleFrame<ReconstructionFrame>;
  template class VisibleFrame<GeneratorFrame>;

}
