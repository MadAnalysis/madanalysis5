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

// SampleHeader header
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoStandard.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoSISCone.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFMidpoint.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFJetClu.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoGridJet.h"

// STL header
#include <iostream>
#include <vector>
using namespace MA5;

// -----------------------------------------------------------------------
// main program
// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
  std::cout << "BEGIN-SAMPLEANALYZER-TEST" << std::endl;
  std::cout << std::endl;

  EventFormat event;
  SampleFormat sample;
  std::vector<ClusterAlgoBase*> tests;
  tests.push_back(new ClusterAlgoStandard("kt"));
  tests.push_back(new ClusterAlgoStandard("antikt"));
  tests.push_back(new ClusterAlgoStandard("genkt"));
  tests.push_back(new ClusterAlgoStandard("cambridge"));
  tests.push_back(new ClusterAlgoSISCone());
  tests.push_back(new ClusterAlgoCDFMidpoint());
  tests.push_back(new ClusterAlgoCDFJetClu());
  tests.push_back(new ClusterAlgoGridJet());

  for (unsigned int i=0;i<tests.size();i++) delete tests[i];

  std::cout << "END-SAMPLEANALYZER-TEST" << std::endl;
  return 0;
}
