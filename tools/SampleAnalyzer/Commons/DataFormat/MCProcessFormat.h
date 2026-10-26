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
 * @file MCProcessFormat.h
 * @brief Properties of a generated process (LHE <init> block).
 */

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

/** @brief Cross section, uncertainty and maximum weight of a generated process. */
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

  /** @brief Cross section [pb], its statistical uncertainty, the maximum weight and the process identifier. */
  MAfloat64 xsectionMean_;    /// cross-section (pb)
  MAfloat64 xsectionError_;   /// statistical error on the cross-section
  MAfloat64 weightMax_;       /// maximum weight encountered in the events
  MAuint32   processId_;       /// number identifying the process


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor (members reset). */
  ProcessFormat() {Reset();}

  /** @brief Destructor. */
  ~ProcessFormat() {}

  /**
   * @brief Accessor to the cross section.
   *
   * @return the cross section [pb].
   */
  const MAfloat64&  xsection()      const {return xsectionMean_;  }

  /**
   * @brief Accessor to the cross section.
   *
   * @return the cross section [pb].
   */
  const MAfloat64&  xsectionMean()  const {return xsectionMean_;  }

  /**
   * @brief Accessor to the uncertainty on the cross section.
   *
   * @return the uncertainty [pb].
   */
  const MAfloat64&  xsectionError() const {return xsectionError_; }

  /**
   * @brief Accessor to the maximum event weight.
   *
   * @return the maximum weight.
   */
  const MAfloat64&  weightMax()     const {return weightMax_;     }

  /**
   * @brief Accessor to the process identifier.
   *
   * @return the identifier.
   */
  const MAuint32&   processId()      const {return processId_;     }

  /** @brief Reset the members. */
  void Reset()
  {
     xsectionMean_=0.; xsectionError_=0.;
     weightMax_=0.;    processId_=0;
  }

  /** @brief Print the process properties. */
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
