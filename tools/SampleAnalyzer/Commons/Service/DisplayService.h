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
 * @file DisplayService.h
 * @brief Redirection of std::cout/std::cerr (DISPLAY).
 */

#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H


// STL headers
#include <iostream>
#include <string>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogReport.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionType.h"


/** @brief Shortcut to the DisplayService singleton. */
#define DISPLAY MA5::DisplayService::GetInstance()

namespace MA5
{

/** @brief Singleton redirecting std::cout and std::cerr to string streams (e.g. to silence external libraries). */
class DisplayService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /** @brief Unique instance. */
  static DisplayService* Service_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /** @brief Constructor. */
  DisplayService() 
  {
    oldCoutStreamBuf_=0;
    oldCerrStreamBuf_=0;
  }

  /** @brief Destructor. */
  ~DisplayService()
  {}

  /** @brief Original stream buffers of std::cout and std::cerr during a redirection. */
  std::streambuf* oldCoutStreamBuf_;
  std::streambuf* oldCerrStreamBuf_;

 public:

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static DisplayService* GetInstance()
  {
    if (Service_==0) Service_ = new DisplayService;
    return Service_;
  }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }
  
  /**
   * @brief Redirect std::cout to a string stream.
   *
   * @param str destination.
   */
  void beginCoutRedirection(std::stringstream& str);
  /** @brief Restore std::cout. */
  void endCoutRedirection();

  /**
   * @brief Redirect std::cerr to a string stream.
   *
   * @param str destination.
   */
  void beginCerrRedirection(std::stringstream& str);
  /** @brief Restore std::cerr. */
  void endCerrRedirection();

  /**
   * @brief Write a string stream to a file (declared but never defined).
   *
   * @param str content.
   * @param filename file name.
   * @param recreate overwrite the file.
   * @return success flag.
   */
  // NOTE: this method has no definition: calling it gives a link error.
  MAbool redirectToFile(std::stringstream& str,
                      const std::string& filename,
                      MAbool recreate=true);

};

}

#endif
