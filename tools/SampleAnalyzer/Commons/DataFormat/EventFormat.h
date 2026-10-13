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
 * @file EventFormat.h
 * @brief Container of an event: Monte Carlo record and/or reconstructed objects.
 */

#ifndef EventFormat_h
#define EventFormat_h


// STL headers
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHEReader;
class LHCOReader;
class HEPMCReader;
class ROOTReader;

/**
 * @brief Event, made of the Monte Carlo record (mc()) and/or the reconstructed objects (rec()).
 *
 * Either pointer can be null depending on the input format: e.g. LHCO files only
 * provide reconstructed objects, while LHE files only provide Monte Carlo particles.
 */
class EventFormat
{
  friend class LHEReader;
  friend class LHCOReader;
  friend class HEPMCReader;
  friend class ROOTReader;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private : 

  /** @brief Reconstructed objects (null if not available). */
  RecEventFormat * rec_;

  /** @brief Monte Carlo record (null if not available). */
  MCEventFormat  * mc_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /** @brief Constructor (both pointers are null). */
  EventFormat()
  { 
    rec_=0;
    mc_=0; 
  }

  /** @brief Destructor (the pointers are not freed: see Delete()). */
  ~EventFormat()
  { 
  }

  /**
   * @brief Accessor to the Monte Carlo record (read-only).
   *
   * @return the record, or 0 if not available.
   */
  const MCEventFormat  * mc()   const {return mc_; }

  /**
   * @brief Accessor to the reconstructed objects (read-only).
   *
   * @return the objects, or 0 if not available.
   */
  const RecEventFormat * rec()  const {return rec_;}

  /**
   * @brief Accessor to the Monte Carlo record.
   *
   * @return the record, or 0 if not available.
   */
  MCEventFormat  * mc()   {return mc_; }

  /**
   * @brief Accessor to the reconstructed objects.
   *
   * @return the objects, or 0 if not available.
   */
  RecEventFormat * rec()  {return rec_;}

  /** @brief Create (or reset) the Monte Carlo record. */
  void InitializeMC()
  {
    if (mc_!=0) 
    {
      mc_->Reset();
    }
    else mc_=new MCEventFormat();
  }

  /** @brief Create (or reset) the container of reconstructed objects. */
  void InitializeRec()
  {
    if (rec_!=0) 
    {
      rec_->Reset();
    }
    else rec_=new RecEventFormat();
  }

  /** @brief Free the Monte Carlo record and the reconstructed objects. */
  void Delete()
  {
    // NOTE: the pointers are not reset to 0: calling Delete() twice would free them twice.
    if (rec_!=0) delete rec_;
    if (mc_!=0)  delete mc_;
  }

};

}

#endif
