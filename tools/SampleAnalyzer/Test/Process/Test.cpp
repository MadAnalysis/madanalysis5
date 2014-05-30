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
#include "SampleAnalyzer/Process/Core/SampleAnalyzer.h"
using namespace MA5;

// -----------------------------------------------------------------------
// main program
// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
  std::cout << "BEGIN-SAMPLEANALYZER-TEST" << std::endl;
  std::cout << std::endl;

  // Creating a manager
  SampleAnalyzer manager;

  // List of available analyzers
  INFO << "List of available analyzers:" << endmsg;
  manager.AnalyzerList().Print();
  INFO << endmsg;

  // List of available readers
  INFO << "List of available readers:" << endmsg;
  manager.ReaderList().Print();
  INFO << endmsg;

  // List of available writers
  INFO << "List of available writers:" << endmsg;
  manager.WriterList().Print();
  INFO << endmsg;

  // List of available JetClusterer
  INFO << "List of available JetClusterer:" << endmsg;
  manager.JetClustererList().Print();
  INFO << endmsg;

  // List of available detector simulation
  INFO << "List of available DetectorSimList:" << endmsg;
  manager.DetectorSimList().Print();
  INFO << endmsg;

  std::cout << "END-SAMPLEANALYZER-TEST" << std::endl;
  return 0;
}
