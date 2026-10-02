////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


/**
 * @file ConvertService.h
 * @brief Singleton converting values to and from strings (CONVERT).
 */

#ifndef CONVERT_SERVICE_H
#define CONVERT_SERVICE_H


// STL headers 
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype> // std::tolower

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


/** @brief Shortcut to the ConvertService singleton. */
#define CONVERT MA5::ConvertService::GetInstance()   

namespace MA5
{

/** @brief Singleton converting values to and from strings through a std::stringstream. */
class ConvertService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /** @brief Unique instance. */
  static ConvertService* Service_;

  /** @brief Stream used for the conversions. */
  std::stringstream Converter_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /** @brief Constructor. */
  ConvertService() 
  {}

  /** @brief Destructor. */
  ~ConvertService()
  {}

  /** @brief Clear the content of the stream. */
  void Initialize()
  // FIXME: the error flags of the stream are not cleared: after a failed conversion, all the
  // following conversions fail as well.
  { Converter_.str(""); }

 public:

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static ConvertService* GetInstance()
  {
    if (Service_==0) Service_ = new ConvertService;
    return Service_;
  }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }

  /// Conversion function to std::string
  template <class T> 
  /**
   * @brief Convert a value to a string.
   *
   * @tparam T type of the value (must support operator<<).
   * @param value value.
   * @return the string.
   */
  const std::string ToString(const T& value)
  {
    Initialize(); 
    Converter_ << value;
    return Converter_.str();
  }

  /// Conversion function to MAint32
  template <class T> 
  /**
   * @brief Convert a value to a 32-bit integer.
   *
   * @tparam T type of the value.
   * @param value value.
   * @return the integer (0 with an error if the conversion fails).
   */
  const MAint32 ToMAint32(const T& value)
  {
    Initialize(); 
    Converter_ << value;
    MAint32 convert=0;
    Converter_ >> convert;

    try
    {
      if (Converter_.fail()) throw EXCEPTION_ERROR("Impossible to convert a string to a int","word="+ToString(value),0);
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    

    return convert;
  }

  /// Conversion function to MAuint32
  template <class T> 
  /**
   * @brief Convert a value to an unsigned 32-bit integer.
   *
   * @tparam T type of the value.
   * @param value value.
   * @return the integer (0 with an error if the conversion fails).
   */
  const MAuint32 ToMAuint32(const T& value)
  {
    Initialize(); 
    Converter_ << value;
    MAuint32 convert=0;
    Converter_ >> convert;

    try
    {
      if (Converter_.fail()) throw EXCEPTION_ERROR("Impossible to convert a string to a int","word="+ToString(value),0);
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    

    return convert;
  }

  /// Conversion function to MAfloat32
  template <class T> 
  /**
   * @brief Convert a value to a single-precision float.
   *
   * @tparam T type of the value.
   * @param value value.
   * @return the float (0 with an error if the conversion fails).
   */
  const MAfloat32 ToFloat(const T& value)
  {
    Initialize(); 
    Converter_ << value;
    MAfloat32 convert=0;
    Converter_ >> convert;

    try
    {
      if (Converter_.fail()) throw EXCEPTION_ERROR("Impossible to convert a string to a float","word="+ToString(value),0);
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    

    return convert;
  }

  /**
   * @brief Convert a string to lower case.
   *
   * @param value string.
   * @return the lower-case string.
   */
  const std::string ToLower(const std::string& value) const
  {
    std::string result=value;
    std::transform(value.begin(), value.end(), result.begin(),
                   (MAint32(*)(MAint32)) std::tolower);
    return result;
  }

  /**
   * @brief Convert a string to upper case.
   *
   * @param value string.
   * @return the upper-case string.
   */
  const std::string ToUpper(const std::string& value) const
  {
    std::string result=value;
    std::transform(value.begin(), value.end(), result.begin(),
                   (MAint32(*)(MAint32)) std::toupper);
    return result;
  }

  /**
   * @brief Test the success of the last conversion (not implemented).
   *
   * @return always true.
   */
  MAbool Success()
  {
    return true;
  }

  /**
   * @brief Test the failure of the last conversion (not implemented).
   *
   * @return always true.
   */
  MAbool Fail()
  // FIXME: Success() and Fail() both always return true.
  {
    return true;
  }


};

}

#endif
