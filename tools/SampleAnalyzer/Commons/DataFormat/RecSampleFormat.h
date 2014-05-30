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


#ifndef RECSAMPLE_DATAFORMAT_H
#define RECSAMPLE_DATAFORMAT_H

// STL headers
#include <map>
#include <iostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class SampleAnalyzer;

class RecSampleFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class SampleAnalyzer;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout arguments
  RecSampleFormat()
  { Reset(); }

  /// Destructor
  ~RecSampleFormat()
  { }

  /// Clear all the content
  void Reset()
  {
  }

  /// Displaying subtitle for file
  void printSubtitle() const
  {
    INFO << "Printing Subtitles ... " << endmsg;
  }


};

}

#endif
