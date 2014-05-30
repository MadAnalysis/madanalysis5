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


//SampleAnalyzer headers
#include "SampleAnalyzer/Process/Analyzer/MergingPlots.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/CompilationService.h"


//STL headers
#include <sstream>


using namespace MA5;

/*
extern"C"
{
  void ktclus_(int *imode, double PP[512][4], int* NN, double* ECUT, double Y[512]);
}
*/


bool MergingPlots::Initialize(const Configuration& cfg,
             const std::map<std::string,std::string>& parameters)
{
  return true;
}


bool MergingPlots::Execute(SampleFormat& mySample, const EventFormat& myEvent)
{

  return true;
}

void MergingPlots::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
}




