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
#include "SampleAnalyzer/Process/JetClustering/JetClustererManager.h"
#ifdef FASTJET_USE
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoStandard.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoSISCone.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFMidpoint.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFJetClu.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoGridJet.h"
#endif

using namespace MA5;

// -----------------------------------------------------------------------------
// BuildTable
// -----------------------------------------------------------------------------
void JetClustererManager::BuildTable()
{
  #ifdef FASTJET_USE
    Add("kt",          new JetClusterer(new ClusterAlgoStandard("kt")));
    Add("antikt",      new JetClusterer(new ClusterAlgoStandard("antikt")));
    Add("genkt",       new JetClusterer(new ClusterAlgoStandard("genkt")));
    Add("cambridge",   new JetClusterer(new ClusterAlgoStandard("cambridge")));
    Add("SISCone",     new JetClusterer(new ClusterAlgoSISCone()));
    Add("CDFMidpoint", new JetClusterer(new ClusterAlgoCDFMidpoint()));
    Add("CDFJetClu",   new JetClusterer(new ClusterAlgoCDFJetClu()));
    Add("GridJet",     new JetClusterer(new ClusterAlgoGridJet()));
  #endif
}

