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


// STL headeres
#include <stdlib.h>

// SampleAnalyzer headers
#ifdef FASTJET_USE
  #include "SampleAnalyzer/Process/Analyzer/MergingPlots.h"
#endif
#include "SampleAnalyzer/Process/Analyzer/AnalyzerManager.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

using namespace MA5;

// -----------------------------------------------------------------------------
// ChoiceAnalyzer
// -----------------------------------------------------------------------------
AnalyzerBase* AnalyzerManager::ChoiceAnalyzer()
{
  // Display the list of analyses
  INFO << endmsg;
  Print();
  INFO << endmsg;

  // Choose an analysis
  INFO << "Please, choose an analysis (0.."
       << Objects_.size()-1 << ") : "
       << endmsg;
  std::cout << "answer: ";
  unsigned int n=0;
  std::cin >> n;

  // Check the choice
  if (n>=Objects_.size())
    {
      ERROR << "wrong analysis" << endmsg;
      exit(1);
    }

  // Return the analysis
  return Objects_[n];
}


// -----------------------------------------------------------------------------
// BuildPredefinedTable
// -----------------------------------------------------------------------------
void AnalyzerManager::BuildPredefinedTable()
{
#ifdef FASTJET_USE
    Add("MergingPlots", new MergingPlots);
#endif
}
