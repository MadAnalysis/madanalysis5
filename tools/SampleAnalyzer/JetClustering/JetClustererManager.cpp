////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
#include "SampleAnalyzer/JetClustering/JetClustererManager.h"
#ifdef FASTJET_USE
  #include "SampleAnalyzer/JetClustering/JetClusteringStandard.h"
  #include "SampleAnalyzer/JetClustering/JetClusteringSISCone.h"
  #include "SampleAnalyzer/JetClustering/JetClusteringCDFMidpoint.h"
  #include "SampleAnalyzer/JetClustering/JetClusteringCDFJetClu.h"
  #include "SampleAnalyzer/JetClustering/JetClusteringGridJet.h"
#endif

using namespace MA5;

// -----------------------------------------------------------------------------
// BuildTable
// -----------------------------------------------------------------------------
void JetClustererManager::BuildTable()
{
    #ifdef FASTJET_USE
    Add("kt",          new JetClusteringStandard(fastjet::kt_algorithm));
    Add("antikt",      new JetClusteringStandard(fastjet::antikt_algorithm));
    Add("genkt",       new JetClusteringStandard(fastjet::genkt_algorithm));
    Add("cambridge",   new JetClusteringStandard(fastjet::cambridge_algorithm));
    Add("SISCone",     new JetClusteringSISCone());
    Add("CDFMidpoint", new JetClusteringCDFMidpoint());
    Add("CDFJetClu",   new JetClusteringCDFJetClu());
    Add("GridJet",     new JetClusteringGridJet());
    #endif
}

