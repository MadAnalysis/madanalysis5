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

#ifndef ISOLATION_SERVICE_h
#define ISOLATION_SERVICE_h

// STL headers

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/IsolationTracker.h"
#include "SampleAnalyzer/Commons/Service/IsolationCalorimeter.h"
#include "SampleAnalyzer/Commons/Service/IsolationEFlow.h"


namespace MA5
{

class Isolation
{

  public:


  IsolationTracker     *tracker;
  IsolationCalorimeter *calorimeter;
  IsolationCalorimeter *combined;
  IsolationEFlow       *eflow;

  Isolation()
  {
    tracker     = new IsolationTracker;
    calorimeter = new IsolationCalorimeter;
    combined    = new IsolationCalorimeter;
    eflow       = new IsolationEFlow;
  }

  ~Isolation()
  {
    delete tracker;
    delete calorimeter;
    delete combined;
    delete eflow;
  }

};

}

#endif
