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
 * @file DetectorManager.h
 * @brief Registry of the detector simulations.
 */

#ifndef DETECTOR_MANAGER_h
#define DETECTOR_MANAGER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/DetectorBase.h"
#include "SampleAnalyzer/Process/Core/ManagerBase.h"


namespace MA5
{

/** @brief Registry of the detector simulations. */
class DetectorManager : public ManagerBase<DetectorBase>
{
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
  public :

   /** @brief Constructor. */
   DetectorManager() : ManagerBase<DetectorBase>()
   { }

   /** @brief Destructor. */
   ~DetectorManager()
   { }

  /** @brief Register the available detector simulations (Delphes and Delphes-MA5tune, when compiled with DELPHES_USE/DELPHESMA5TUNE_USE). */
  void BuildTable(); 

  /**
   * @brief Print the registered detector simulations.
   *
   * @param os logger.
   */
  void Print(LogStream& os=INFO) const
  { ManagerBase<DetectorBase>::Print(Objects_, Names_, os); }

};

}

#endif
