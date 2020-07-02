////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
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


// ShortCut to access to ConvertService
#define CONVERT MA5::ConvertService::GetInstance()   

namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class ConvertService contains static methods used for converting
/// all types into string type.
///
/// ConvertService is a singleton-pattern-based class : only one instance.
/// Getting the only one instance : ConvertService::GetInstance()
//////////////////////////////////////////////////////////////////////////////
class ConvertService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /// Pointer to the unique instance of ConvertService
  static ConvertService* Service_;

  /// Streamer used for the conversion
  std::stringstream Converter_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /// Constructor without argument
  ConvertService() 
  {}

  /// Destructor
  ~ConvertService()
  {}

  /// (Re)initialzing the streamer
  void Initialize()
  { Converter_.str(""); }

 public:

  /// Getting the unique instance of ConvertService
  static ConvertService* GetInstance()
  {
    if (Service_==0) Service_ = new ConvertService;
    return Service_;
  }

  /// Deleting the unique instance of Convert Service
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }

  /// Conversion function to std::string
  template <class T> 
  const std::string ToString(const T& value)
  {
    Initialize(); 
    Converter_ << value;
    return Converter_.str();
  }

  /// Conversion function to MAint32
  template <class T> 
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

  /// Conversion function to lower case
  const std::string ToLower(const std::string& value) const
  {
    std::string result=value;
    std::transform(value.begin(), value.end(), result.begin(),
                   (MAint32(*)(MAint32)) std::tolower);
    return result;
  }

  /// Conversion function to upper case
  const std::string ToUpper(const std::string& value) const
  {
    std::string result=value;
    std::transform(value.begin(), value.end(), result.begin(),
                   (MAint32(*)(MAint32)) std::toupper);
    return result;
  }

  /// Test the success of the last conversion
  MAbool Success()
  {
    return true;
  }

  /// Test the failure of the last conversion
  MAbool Fail()
  {
    return true;
  }


};

}

#endif
