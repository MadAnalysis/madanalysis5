/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2015, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFKey.cc
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jul
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

#include "SampleAnalyzer/Commons/RestFrames/RFKey.h"

namespace RestFrames {

  RFKey::RFKey(int key){ m_Key = key; }
  RFKey::RFKey(const RFKey& key){ m_Key = key.GetKey(); }
  RFKey::~RFKey(){ }

  void RFKey::operator=(const RFKey& key){ m_Key = key.GetKey(); }

  bool RFKey::operator==(int key) const { return (m_Key == key); }

  bool RFKey::operator==(const RFKey& key) const { return (m_Key == key.GetKey()); }

  int RFKey::GetKey() const { return m_Key; }

  bool RFKey::IsSame(const RFKey& key) const {
    return m_Key == key.GetKey();
  }

}
