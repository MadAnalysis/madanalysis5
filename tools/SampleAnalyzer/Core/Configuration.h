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


#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// STL headers
#include <iostream>
#include <string>

// ROOT headers
#include <TLorentzVector.h>


namespace MA5
{

class Configuration
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
  private:

    /// SampleAnalyzer version
    static const std::string sampleanalyzer_version_;
    static const std::string sampleanalyzer_date_;  

    /// Python interface version
    std::string pythoninterface_version_;
    std::string pythoninterface_date_;

    /// option : check event mode
    Bool_t check_event_;

    /// option : veto to event weights
    Bool_t no_event_weight_;

    /// input list name
    std::string input_list_name_;

    /// Use the RegionSelectionManager structure and its output
    bool useRSM_;
  

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

    /// Constructor without arguments
    Configuration()
    { Reset(); }

    /// Destructor
    ~Configuration()
    { }

    /// Initialization
    bool Initialize(int&argc, char *argv[], const bool&);
 
    /// Printing the configuration status
    void Display();

    /// Help message
    void PrintSyntax();

    /// Accessor to the sampleanalyzer date
    const std::string& GetSampleAnalyzerDate() const
    {return sampleanalyzer_date_;}

    /// Accessor to the sampleanalyzer version
    const std::string& GetSampleAnalyzerVersion() const
    {return sampleanalyzer_version_;}

    /// Accessor to the python interface date
    const std::string& GetPythonInterfaceDate() const
    {return sampleanalyzer_date_;}

    /// Accessor to the python interface version
    const std::string& GetPythonInterfaceVersion() const
    {return sampleanalyzer_version_;}

    /// Accessor to the  useRSM flag
    const bool& useRSM() const
    { return useRSM_; }

    /// Accessor to the input name
    const std::string& GetInputFileName() const
    {return input_list_name_;}

    /// Accessor to the input name
    const std::string GetInputName() const
    {
      std::string name;

      // removing path
      size_t found = input_list_name_.find_last_of("/");
      if (found==std::string::npos) name = input_list_name_;
      else name = input_list_name_.substr(found+1,std::string::npos);

      // removing extension
      found = name.find_last_of(".");
      if (found==std::string::npos) return name;
      else return name.substr(0,found);
    }

    /// Function to write a string in lower case
    static void Lower(std::string& word);

    /// Reset
    void Reset()
    {
      no_event_weight_ = false;
      check_event_     = false;
      input_list_name_ = "";
    }
 
    /// Accessor to Input Name
    const std::string& GetInputListName() const
    { return input_list_name_; }

    /// Accessor to NoEventWeight
    Bool_t IsNoEventWeight() const
    { return no_event_weight_; }

    /// Accessor to CheckEvent
    Bool_t IsCheckEvent() const
    { return check_event_; }

};

}

#endif
