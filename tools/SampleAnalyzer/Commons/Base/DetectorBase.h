////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


#ifndef DETECTOR_BASE_H
#define DETECTOR_BASE_H

//SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"

//STL headers
#include <map>
#include <string>
#include <algorithm>
#include <locale>


namespace MA5
{

  class DetectorBase
  {
    //--------------------------------------------------------------------------
    //                              data members
    //--------------------------------------------------------------------------
  protected :

    std::string configFile_;

    //--------------------------------------------------------------------------
    //                              method members
    //--------------------------------------------------------------------------
  public :

    /// Constructor without argument
    DetectorBase () 
    { }

    /// Destructor
    virtual ~DetectorBase()
    { }

    /// Jet clustering
    virtual bool Execute(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Initialization
    virtual bool Initialize(const std::string& configFile, const std::map<std::string,std::string>& options)=0;

    /// Finalization
    virtual void Finalize()=0;

    /// Print parameters
    virtual void PrintParam()=0;

    /// Accessor to the detector name
    virtual std::string GetName()=0;

    /// Accessor to the detector parameters
    virtual std::string GetParameters()=0;

    /// Config File
    const std::string& GetConfigFile() const
    { return configFile_; }

    /// Putting the string in lower case
    static std::string Lower(const std::string& word)
    {
      std::string result;
      std::transform(word.begin(), word.end(), 
                     std::back_inserter(result), 
                     (int (*)(int))std::tolower);
      return result;
    }

  protected:


  };

}

#endif
