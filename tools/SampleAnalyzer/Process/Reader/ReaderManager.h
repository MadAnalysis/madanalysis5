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
 * @file ReaderManager.h
 * @brief Registry of the event-file readers, selected by file extension.
 */

#ifndef READER_MANAGER_h
#define READER_MANAGER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/ReaderBase.h"
#include "SampleAnalyzer/Process/Core/ManagerBase.h"


namespace MA5
{

/** @brief Registry of the readers (lhe, lhco, hep, hepmc, their .gz versions, and root with ROOT). */
class ReaderManager : public ManagerBase<ReaderBase>
{
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  ReaderManager() : ManagerBase<ReaderBase>()
  { }

  /** @brief Destructor. */
  ~ReaderManager()
  { }

  /** @brief Register the available readers (the .gz extensions require zlib, root requires ROOT). */
  void BuildTable();

  /**
   * @brief Find the reader corresponding to the extension of a file (a `.fifo` suffix is ignored).
   *
   * @param filename file name.
   * @return the reader, or 0 (with an error message for a forbidden extension).
   */
  ReaderBase* GetByFileExtension(std::string filename);

  /**
   * @brief Print the registered readers.
   *
   * @param os logger.
   */
  void Print(LogStream& os=INFO) const
  { ManagerBase<ReaderBase>::Print(Objects_, Names_, os); }

};

}

#endif
