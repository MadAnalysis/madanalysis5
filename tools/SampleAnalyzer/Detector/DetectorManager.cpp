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


// SampleAnalyzer headers
#include "SampleAnalyzer/Detector/DetectorManager.h"
#ifdef DELPHES_USE
  #include "SampleAnalyzer/Detector/DetectorDelphes.h"
#endif
#ifdef DELFES_USE
  #include "SampleAnalyzer/Detector/DetectorDelfes.h"
#endif

using namespace MA5;

// -----------------------------------------------------------------------------
// BuildTable
// -----------------------------------------------------------------------------
void DetectorManager::BuildTable()
{
  #ifdef DELPHES_USE
  Add("delphes",new DetectorDelphes());
  #endif
  #ifdef DELFES_USE
  Add("delfes",new DetectorDelfes());
  #endif
}

