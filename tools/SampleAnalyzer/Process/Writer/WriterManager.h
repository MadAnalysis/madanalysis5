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
 * @file WriterManager.h
 * @brief Registry of the event-file writers, selected by file extension.
 */

#ifndef WRITER_MANAGER_h
#define WRITER_MANAGER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterBase.h"
#include "SampleAnalyzer/Process/Core/ManagerBase.h"


namespace MA5
{

/** @brief Registry of the writers (lhe and lhco, and their .gz versions with zlib). */
class WriterManager : public ManagerBase<WriterBase>
{
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  WriterManager() : ManagerBase<WriterBase>()
  { }

  /** @brief Destructor. */
  ~WriterManager()
  { }

  /** @brief Register the available writers. */
  void BuildTable();

  /**
   * @brief Find the writer corresponding to the extension of a file.
   *
   * @param filename file name.
   * @return the writer, or 0 if none.
   */
  WriterBase* GetByFileExtension(std::string filename);

  /**
   * @brief Print the registered writers.
   *
   * @param os logger.
   */
  void Print(LogStream& os=INFO) const
  { ManagerBase<WriterBase>::Print(Objects_, Names_, os); }

};


}

#endif
