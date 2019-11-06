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


// STL headers
#include <algorithm>
#include <fstream>
#include <locale>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


using namespace MA5;


// -----------------------------------------------------------------------------
// Initializing static data members
// -----------------------------------------------------------------------------
// DO NOT TOUCH THESE LINES
const std::string Configuration::sampleanalyzer_version_ = "1.8.31";
const std::string Configuration::sampleanalyzer_date_    = "2019/11/06";
// DO NOT TOUCH THESE LINES

// -----------------------------------------------------------------------------
// PrintSyntax
// -----------------------------------------------------------------------------
void Configuration::PrintSyntax()
{
  INFO << endmsg;
  INFO << "Syntax : SampleAnalyzer [option] <filelist>" << endmsg;
  INFO << " with <filelist> = txt file containing all sample file names"
       << endmsg;
  INFO << " with [option] = " << endmsg;
  INFO << "   --check_event      : check the compliance of the event file"
       << endmsg;
  INFO << "   --no_event_weight  : the event weights are not used"
       << endmsg;
  INFO << endmsg;
}


// -----------------------------------------------------------------------------
// Lower
// -----------------------------------------------------------------------------
void Configuration::Lower(std::string& word)
{
  std::transform(word.begin(), word.end(), 
                 word.begin(), 
                 (MAint32 (*)(MAint32))std::tolower);
}



// -----------------------------------------------------------------------------
// DecodeMA5version
// -----------------------------------------------------------------------------
void Configuration::DecodeMA5version(const std::string& option)
{
  std::string stamp = option.substr(14,std::string::npos);
  std::size_t result = stamp.find(";");
  try
  {
    // check the syntax
    if (result==std::string::npos) throw EXCEPTION_WARNING("MA5 version '"+stamp+"' is not valid.","",0);

    // version
    pythoninterface_version_ = stamp.substr(0,result);
    if (pythoninterface_version_.find("\"")==0)
      pythoninterface_version_ = pythoninterface_version_.substr(1,std::string::npos);
    if (pythoninterface_version_.size()>=2) 
      if (pythoninterface_version_.find("\"")==(pythoninterface_version_.size()-1))
        pythoninterface_version_ = pythoninterface_version_.substr(0,(pythoninterface_version_.size()-1));

    // date
    pythoninterface_date_ = stamp.substr(result+1,std::string::npos);
    if (pythoninterface_date_.find("\"")==0)
      pythoninterface_date_ = pythoninterface_date_.substr(1,std::string::npos);
    if (pythoninterface_date_.size()>=2) 
      if (pythoninterface_date_.find("\"")==(pythoninterface_date_.size()-1))
        pythoninterface_date_ = pythoninterface_date_.substr(0,(pythoninterface_date_.size()-1));
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    
}


// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
MAbool Configuration::Initialize(MAint32 &argc, MAchar *argv[])
{
  // Checking number of arguments
  // <filelist> is compulsory
  if (argc<2)
  {
    ERROR << "number of arguments is incorrect." << endmsg;
    PrintSyntax();
    return false;
  }

  // Decoding arguments
  for (MAuint32 i=1;i<static_cast<MAuint32>(argc);i++)
  {
    // converting const characters into string
    std::string argument = std::string(argv[i]);
    Lower(argument);

    // safety : skip empty string
    if (argument.size()==0) continue;

    // Is it an option?
    if (argument.size()>=2 && argument[0]=='-' && argument[1]=='-')
    {
      // check event
      if (argument=="--check_event") check_event_ = true;

      // weighted event
      else if (argument=="--no_event_weight") no_event_weight_ = true;

      // version
      else if (argument.find("--ma5_version=")==0) DecodeMA5version(argument);

      // other = mistake
      else
      {
        ERROR << "option '" << argument << "' is unknown !!!" << endmsg;
        PrintSyntax();
        return false;
      }
    } 

    // It is not an option? So it is a list of samples
    else
    {
      if (input_list_name_=="" || input_list_name_==std::string(argv[i]))
      {
        // Extracting the input list
        input_list_name_ = std::string(argv[i]);
      }
      else
      {
        // only one list of samples is required
        ERROR << "several list of samples have been declared: '" 
              << input_list_name_ << "' and '" << argv[i] 
              << "'. Only one is required." << endmsg;
        return false;
      }
    }
  }

  // Check that the input list is supplied
  if (input_list_name_=="")
  {
    ERROR << "no list of samples has been provided." << endmsg;
    PrintSyntax();
    return false;
  }

  // Ok
  return true;
}


// -----------------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------------
void Configuration::Display()
{
  INFO << "      - version: " << sampleanalyzer_version_ << " (" << sampleanalyzer_date_ << ") ";
  if ((sampleanalyzer_version_!=pythoninterface_version_ && pythoninterface_version_!="") || 
      (sampleanalyzer_date_!=pythoninterface_date_ && pythoninterface_version_!=""))
    INFO << "[ python interface version: " << pythoninterface_version_ 
         << " (" << pythoninterface_date_ << ") ]";
  INFO << endmsg;
 
  INFO << "      - general: ";

  // Is there option ?
  if (!check_event_ && !no_event_weight_)
  {
    INFO << "everything is default." << endmsg;
    return;
  }
  else
  {
    INFO << endmsg;
  }

  // Displaying options
  if (check_event_) 
    INFO << "     -> checking the event file format." << endmsg;
  if (no_event_weight_) 
    INFO << "     -> event weights are not used." << endmsg;
}
