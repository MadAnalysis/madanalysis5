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
 * @file Configuration.h
 * @brief Run-time configuration of SampleAnalyzer (command-line options and versions).
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H


// STL headers
#include <iostream>
#include <string>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"


namespace MA5
{

/**
 * @brief Configuration of a SampleAnalyzer run, decoded from the command line.
 *
 * Syntax: `SampleAnalyzer [options] <filelist>`. Recognised options are
 * `--check_event`, `--no_event_weight`, `--ma5_version="<version>;<date>"` and any
 * `--<name>=<value>` pair, stored in the option map passed to the analyses.
 */
class Configuration
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
  private:

    /** @brief Version of SampleAnalyzer (defined in Configuration.cpp). */
    static const std::string sampleanalyzer_version_;
    static const std::string sampleanalyzer_date_;  

    /** @brief Version of the Python interface (from `--ma5_version`). */
    std::string pythoninterface_version_;
    std::string pythoninterface_date_;

    /** @brief Option `--check_event`: check the compliance of the event file. */
    MAbool check_event_;

    /** @brief Option `--no_event_weight`: ignore the event weights. */
    MAbool no_event_weight_;

    /** @brief Name of the file containing the list of samples. */
    std::string input_list_name_;

    /** @brief Additional `--<name>=<value>` options. */
    std::map<std::string, std::string> options_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

    /** @brief Constructor (resets the options). */
    Configuration()
    { Reset(); }

    /** @brief Destructor. */
    ~Configuration()
    { }

    /**
     * @brief Decode the command-line arguments.
     *
     * @param argc number of arguments.
     * @param argv arguments (argv[0] is the program name).
     * @return false if the syntax is wrong or no (or several) sample lists are given, true otherwise.
     */
    MAbool Initialize(MAint32 &argc, MAchar *argv[]);
 
    /** @brief Print the versions and the non-default options. */
    void Display();

    /** @brief Print the command-line syntax. */
    void PrintSyntax();

    /**
     * @brief Accessor to the release date of SampleAnalyzer.
     *
     * @return the date.
     */
    const std::string& GetSampleAnalyzerDate() const
    {return sampleanalyzer_date_;}

    /**
     * @brief Accessor to the version of SampleAnalyzer.
     *
     * @return the version.
     */
    const std::string& GetSampleAnalyzerVersion() const
    {return sampleanalyzer_version_;}

    /**
     * @brief Accessor to the release date of the Python interface.
     *
     * @return the date (see the FIXME).
     */
    const std::string& GetPythonInterfaceDate() const
    // FIXME: returns sampleanalyzer_date_ instead of pythoninterface_date_.
    {return sampleanalyzer_date_;}

    /**
     * @brief Accessor to the version of the Python interface.
     *
     * @return the version (see the FIXME).
     */
    const std::string& GetPythonInterfaceVersion() const
    // FIXME: returns sampleanalyzer_version_ instead of pythoninterface_version_.
    {return sampleanalyzer_version_;}

    /**
     * @brief Accessor to the name of the sample-list file.
     *
     * @return the file name, as given on the command line.
     */
    const std::string& GetInputFileName() const
    {return input_list_name_;}

    /**
     * @brief Get the name of the sample list without path and extension.
     *
     * @return the base name.
     */
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

    /**
     * @brief Convert a string to lower case (in place).
     *
     * @param word string to convert.
     */
    static void Lower(std::string& word);

    /**
     * @brief Decode the option `--ma5_version="<version>;<date>"`.
     *
     * @param option the full option string.
     */
    void DecodeMA5version(const std::string& option);

    /** @brief Reset the options to their default values. */
    void Reset()
    {
      no_event_weight_ = false;
      check_event_     = false;
      input_list_name_ = "";
    }
 
    /**
     * @brief Accessor to the name of the sample-list file.
     *
     * @return the file name.
     */
    const std::string& GetInputListName() const
    { return input_list_name_; }

    /**
     * @brief Is the option `--no_event_weight` set?
     *
     * @return true if the event weights must be ignored.
     */
    MAbool IsNoEventWeight() const
    { return no_event_weight_; }

    /**
     * @brief Is the option `--check_event` set?
     *
     * @return true if the event file must be checked.
     */
    MAbool IsCheckEvent() const
    { return check_event_; }

    /**
     * @brief Accessor to the additional `--<name>=<value>` options.
     *
     * @return a copy of the option map.
     */
    std::map<std::string, std::string> Options() const {return options_;}

};

}

#endif
