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
 * @file MCconfig.h
 * @brief Monte Carlo configuration: PDG codes of the hadronic and invisible particles.
 */

#ifndef MCTOOLCONFIG_h
#define MCTOOLCONFIG_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

// STL headers
#include <set>
#include <string>


namespace MA5
{

class Tools;

/** @brief PDG codes defining the `hadronic` and `invisible` multiparticles (set by the generated analysis). */
struct MCconfig
{
  friend class PhysicsService;
  friend class Identification;

  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  protected:

  /** @brief PDG codes of the invisible particles. */
  std::set<MAint32> invisible_ids_;

  /** @brief PDG codes of the hadronic particles. */
  std::set<MAint32> hadronic_ids_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
  public:

  /** @brief Constructor. */
  MCconfig()
  { }

  /** @brief Destructor. */
  ~MCconfig()
  { }

  /** @brief Remove all the PDG codes. */
  void Reset()
  {
    invisible_ids_.clear();
    hadronic_ids_.clear();
  } 

  /**
   * @brief Add a hadronic PDG code.
   *
   * @param id PDG code.
   */
  void AddHadronicId(MAint32 id)
  {
    hadronic_ids_.insert(id);
  } 

  /**
   * @brief Remove a hadronic PDG code.
   *
   * @param id PDG code.
   */
  void RemoveHadronicId(MAint32 id)
  {
    hadronic_ids_.erase(id);
  }

  /**
   * @brief Add an invisible PDG code.
   *
   * @param id PDG code.
   */
  void AddInvisibleId(MAint32 id)
  {
    invisible_ids_.insert(id);
  } 

  /**
   * @brief Remove an invisible PDG code.
   *
   * @param id PDG code.
   */
  void RemoveInvisibleId(MAint32 id)
  {
    invisible_ids_.erase(id);
  }

  /** @brief Print the PDG codes. */
  void Print()
  {
    INFO << "Hadronic IDs " ;
    for (auto id : hadronic_ids_)
        INFO << id << ", ";
    INFO << endmsg;

    INFO << "\nInvisible IDs ";
    for (auto id : invisible_ids_)
        INFO << id << ", ";
    INFO << endmsg;
  }

};

}

#endif
