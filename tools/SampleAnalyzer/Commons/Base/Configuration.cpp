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


// STL headers
#include <algorithm>
#include <fstream>
#include <locale>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


using namespace MA5;


// -----------------------------------------------------------------------------
// Initializing static data members
// -----------------------------------------------------------------------------
// DO NOT TOUCH THESE LINES
const std::string Configuration::sampleanalyzer_version_ = "1.1.11";
const std::string Configuration::sampleanalyzer_date_    = "2014/09/15";
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
                 (int (*)(int))std::tolower);
}


// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
bool Configuration::Initialize(int &argc, char *argv[], const bool &useRSM)
{
  // Using a Region Selection Manager or not
  useRSM_=useRSM;

  // Checking number of arguments
  // <filelist> is compulsory
  if (argc<2)
  {
    ERROR << "number of arguments is incorrect." << endmsg;
    PrintSyntax();
    return false;
  }

  // Decoding options
  if (argc>=3) for (unsigned int i=1;i<static_cast<unsigned int>(argc-1);i++)
  {
    // converting const char to string
    std::string option = std::string(argv[i]);
    Lower(option);

    // safety : skip empty string
    if (option.size()==0) continue;

    // check event
    if (option=="--check_event") check_event_ = true;

    // weighted event
    else if (option=="--no_event_weight") no_event_weight_ = true;

    // version
    else if (option.find("--ma5_version=")==0)
    {
      std::string stamp = option.substr(14,std::string::npos);
      std::size_t result = stamp.find(";");
      if (result==std::string::npos)
      {
        WARNING << "MA5 version '" << stamp << "' is not valid." << std::endl;
      }
      else
      {
        pythoninterface_version_ = stamp.substr(0,result);
        if (pythoninterface_version_.find("\"")==0)
          pythoninterface_version_ = pythoninterface_version_.substr(1,std::string::npos);
        if (pythoninterface_version_.size()>=2) 
          if (pythoninterface_version_.find("\"")==(pythoninterface_version_.size()-1))
            pythoninterface_version_ = pythoninterface_version_.substr(0,(pythoninterface_version_.size()-1));

        pythoninterface_date_ = stamp.substr(result+1,std::string::npos);
        if (pythoninterface_date_.find("\"")==0)
          pythoninterface_date_ = pythoninterface_date_.substr(1,std::string::npos);
        if (pythoninterface_date_.size()>=2) 
          if (pythoninterface_date_.find("\"")==(pythoninterface_date_.size()-1))
            pythoninterface_date_ = pythoninterface_date_.substr(0,(pythoninterface_date_.size()-1));
      }
    }

    // unknown option
    else
    {
      ERROR << "argument '" << option << "' is unknown !!!" << endmsg;
      PrintSyntax();
      return false;
    }
  }

  // Extracting the input list
  input_list_name_ = std::string(argv[static_cast<unsigned int>(argc-1)]);

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
