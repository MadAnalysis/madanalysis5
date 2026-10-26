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
 * @file PdgTable.h
 * @brief Table of particle properties indexed by PDG code.
 */

#ifndef PDGTABLE_H
#define PDGTABLE_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/PdgDataFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

// STL headers
#include <string>
#include <map>
#include <cmath>


namespace MA5
{

/** @brief Table of particle properties indexed by PDG code. */
class PdgTable {
  public:
  /** @brief Constructor (empty table). */
  PdgTable()
  {};

  /** @brief Destructor. */
  ~PdgTable()
  {};

  /**
   * @brief Copy constructor.
   *
   * @param Table table to copy.
   */
  PdgTable(const PdgTable& Table);

  /**
   * @brief Merge another table into this one (existing entries are kept).
   *
   * @param Table table to copy.
   * @return this table.
   */
  PdgTable& operator=(const PdgTable& Table);

  /**
   * @brief Accessor to the table.
   *
   * @return the entries, by PDG code.
   */
  const std::map<MAint32, PdgDataFormat>& Table() 
  { return Table_; }

  /**
   * @brief Insert an entry (ignored if the PDG code already exists).
   *
   * @param Pdgid PDG code.
   * @param p properties.
   */
  void Insert(const MAint32 Pdgid, const PdgDataFormat &p);

  /** @brief Print the table. */
  void Print() const;

  /**
   * @brief Get the properties of a particle.
   *
   * @param Pdgid PDG code.
   * @return the properties, or an 'Unknown' entry (with a warning) if not found.
   */
  const PdgDataFormat& operator[](const MAint32 Pdgid) const;

  /**
   * @brief Get the properties of a particle.
   *
   * @param Pdgid PDG code.
   * @param verbose print a warning if the PDG code is unknown.
   * @return the properties, or an 'Unknown' entry if not found.
   */
  const PdgDataFormat& GetParticle(const MAint32 Pdgid, MAbool verbose=true) const;

  private:

  /** @brief Entries, by PDG code, and the entry returned for unknown codes. */
  std::map<MAint32, PdgDataFormat> Table_;
  PdgDataFormat empty_;
};

}

#endif
