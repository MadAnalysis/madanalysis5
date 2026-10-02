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
 * @file AnalyzerManager.h
 * @brief Registry of the analyses.
 */

#ifndef ANALYSIS_MANAGER_h
#define ANALYSIS_MANAGER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"
#include "SampleAnalyzer/Process/Core/ManagerBase.h"


namespace MA5
{

/** @brief Registry of the analyses available in the job (user analyses are added by main.cpp). */
class AnalyzerManager : public ManagerBase<AnalyzerBase>
{
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /** @brief Constructor. */
  AnalyzerManager() : ManagerBase<AnalyzerBase>()
  { }

  /** @brief Destructor. */
  ~AnalyzerManager()
  { }

  /**
   * @brief Ask the user to choose an analysis on the standard input (exits on a wrong choice).
   *
   * @return the chosen analysis.
   */
  AnalyzerBase* ChoiceAnalyzer();

  /** @brief Register the predefined analyses (MergingPlots when FastJet is used). */
  void BuildPredefinedTable();

  /**
   * @brief Print the registered analyses.
   *
   * @param os logger.
   */
  void Print(LogStream& os=INFO) const
  { ManagerBase<AnalyzerBase>::Print(Objects_, Names_, os); }

};

}

#endif
