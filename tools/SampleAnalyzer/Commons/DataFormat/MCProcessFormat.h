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


#ifndef PROCESS_FORMAT_H
#define PROCESS_FORMAT_H


// STL headers
#include <map>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class HEPMCReader;
class STDHEPReader;
class STDHEPreader;
class ROOTReader;
class LHEWriter;

class ProcessFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class HEPMCReader;
  friend class ROOTReader;
  friend class LHEWriter;
  friend class STDHEPReader;
  friend class STDHEPreader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  MAfloat64 xsectionMean_;    /// cross-section (pb)
  MAfloat64 xsectionError_;   /// statistical error on the cross-section
  MAfloat64 weightMax_;       /// maximum weight encountered in the events
  MAuint32   processId_;       /// number identifying the process


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /// Constructor without arguments
  ProcessFormat() {Reset();}

  /// Destructor
  ~ProcessFormat() {}

  /// Accessor to the mean value of the process cross section
  const MAfloat64&  xsection()      const {return xsectionMean_;  }

  /// Accessor to the mean value of the process cross section
  const MAfloat64&  xsectionMean()  const {return xsectionMean_;  }

  /// Accessor to the error value of the process cross section
  const MAfloat64&  xsectionError() const {return xsectionError_; }

  /// Accessor to the highest weight
  const MAfloat64&  weightMax()     const {return weightMax_;     }

  /// Accessor to the process identity
  const MAuint32&   processId()      const {return processId_;     }

  /// Clearing all information
  void Reset()
  {
     xsectionMean_=0.; xsectionError_=0.;
     weightMax_=0.;    processId_=0;
  }

  /// Displaying data member values
  void Print() const
  {
    INFO << "processId="        << processId_
         << " - xsectionMean="  << xsectionMean_
         << " - xsectionError=" << xsectionError_
         << " - weightMax="     << weightMax_ << endmsg;
  }

};

}

#endif
