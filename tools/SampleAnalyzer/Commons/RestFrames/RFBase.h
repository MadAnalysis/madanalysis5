/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2018, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFBase.h
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jun
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

#ifndef RFBase_H
#define RFBase_H

#include <iostream>
#include <string>
#include <algorithm>
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"

#include "SampleAnalyzer/Commons/RestFrames/RFKey.h"
#include "SampleAnalyzer/Commons/RestFrames/RFLog.h"
#include "SampleAnalyzer/Commons/RestFrames/RFList.h"

namespace RestFrames {

  static const double RF_tol = 1e-6;

  ////////////////////////////////////////////////////////////////////
  /// \brief Base class for all RestFrame package objects
  ///
  /// Abstract base class from which all RestFrame package objects
  /// inherit. 
  ////////////////////////////////////////////////////////////////////
  class RFBase {
  
  public:
    
    ////////////////////////////////////////////////////////////////////
    /// \brief Standard constructor
    /// 
    /// \param sname    class instance name used for log statements
    /// \param stitle   class instance title used in figures
    ////////////////////////////////////////////////////////////////////
    RFBase(const std::string& sname, const std::string& stitle, int key);

    RFBase();
    
    virtual ~RFBase();

    friend void SetWarningTolerance(int NMAX);

    /// \brief Clears RFBase of all connections to other objects
    virtual void Clear();

    /// \brief Checks whether this is default (empty) instance of class
    bool IsEmpty() const;

    /// \brief Tests whether key is the same as this
    bool operator !() const { return IsEmpty(); }

    ////////////////////////////////////////////////////////////////////
    /// \name RFBase identity/comparison methods
    /// \brief RFBase identity query member functions
    /// 
    /// Member functions for identifying/comparing class instances
    ////////////////////////////////////////////////////////////////////
    ///@{

    /// \brief gets object identification key
    RFKey GetKey() const;

    /// \brief Returns object name 
    std::string GetName() const;
    
    /// \brief Returns object title 
    std::string GetTitle() const;
    
    /// \brief Tests whether key is the same as this
    bool IsSame(const RFKey& key) const;

    /// \brief Tests whether *obj* is the same as this
    bool IsSame(const RFBase& obj) const;
    
    /// \brief Tests whether key is the same as this
    bool operator == (const RFKey& key) const { return IsSame(key); }

    /// \brief Tests whether *obj* is the same as this
    bool operator == (const RFBase& obj) const { return IsSame(obj); }

    /// \brief Tests whether key is the same as this
    bool operator != (const RFKey& key) const { return !IsSame(key); }

    /// \brief Tests whether *obj* is the same as this
    bool operator != (const RFBase& obj) const { return !IsSame(obj); }

    ///@} // end identity/comparison methods

    /// \brief Print information associated with object
    void Print(LogType type) const;

    /// \brief String of information associated with object
    virtual std::string PrintString(LogType type) const;

    static RFBase& Empty();
    
  protected:      
    mutable RFLog m_Log;

    bool SetBody(bool body) const;
    bool SetMind(bool mind) const;
    bool SetSpirit(bool spirit) const;

    virtual bool IsSoundBody() const;
    virtual bool IsSoundMind() const;
    virtual bool IsSoundSpirit() const;

    void UnSoundBody(const std::string& function) const;
    void UnSoundMind(const std::string& function) const;
    void UnSoundSpirit(const std::string& function) const;

    /// \brief pointer to RFBase object owned by this one
    void AddDependent(RFBase* dep);
    
    RFBase* m_This;

    static const MA5::MAVector3       m_Empty3Vector;
    static const MA5::MALorentzVector m_Empty4Vector;

  private:
    std::string m_Name;
    std::string m_Title;
    RFKey m_Key;
    
    mutable bool m_Body;       
    mutable bool m_Mind;       
    mutable bool m_Spirit;
    
    std::vector<RFBase*> m_Owns;

    static RFBase m_Empty;
    
    static int m_BodyCount;
    static int m_MindCount;
    static int m_SpiritCount;

    static int m_WarningTolerance;
  };

  double GetP(double Mp, double Mc1, double Mc2);

  // adapted from boost/current_function.hpp
  #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
  # define RF_FUNCTION __PRETTY_FUNCTION__
  #elif defined(__DMC__) && (__DMC__ >= 0x810)
  # define RF_FUNCTION __PRETTY_FUNCTION__
  #elif defined(__FUNCSIG__)
  # define RF_FUNCTION __FUNCSIG__
  #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
  # define RF_FUNCTION __FUNCTION__
  #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
  # define RF_FUNCTION __FUNC__
  #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
  # define RF_FUNCTION __func__
  #elif defined(__cplusplus) && (__cplusplus >= 201103)
  # define RF_FUNCTION __func__
  #else
  # define RF_FUNCTION "(unknown)"
  #endif

  ////////////////////////////////////////////////////////////////////
  /// \brief Set the tolerance for number of RestFrames warnings
  /// 
  /// \param NMAX  Number of allowed warnings of a given type
  ///
  /// Function sets the tolerance for the number of RestFrames 
  /// warnings. Numbers less than 1 indicate infinite tolerance.
  ////////////////////////////////////////////////////////////////////
  void SetWarningTolerance(int NMAX = -1);
  void TooManyBodies(const RFBase& obj);
  void TooManyMinds(const RFBase& obj);
  void TooManySpirits(const RFBase& obj);

}

#endif
