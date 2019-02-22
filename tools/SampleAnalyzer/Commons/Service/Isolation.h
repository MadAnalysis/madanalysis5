////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/IsolationTracker.h"
#include "SampleAnalyzer/Commons/Service/IsolationCalorimeter.h"
#include "SampleAnalyzer/Commons/Service/IsolationCombined.h"
#include "SampleAnalyzer/Commons/Service/IsolationEFlow.h"


namespace MA5
{

class Isolation
{

  public:


  IsolationTracker     *tracker;
  IsolationCalorimeter *calorimeter;
  IsolationCombined    *combined;
  IsolationEFlow       *eflow;

  Isolation()
  {
    tracker     = new IsolationTracker;
    calorimeter = new IsolationCalorimeter;
    combined    = new IsolationCombined;
    eflow       = new IsolationEFlow;
  }

  ~Isolation()
  {
    delete tracker;
    delete calorimeter;
    delete combined;
    delete eflow;
  }


  // -----------------------------------------------------------------------------
  // JetCleaning
  // -----------------------------------------------------------------------------
  std::vector<const RecJetFormat*>
    JetCleaning(const std::vector<const RecJetFormat*>& uncleaned_jets,
                const std::vector<const RecLeptonFormat*>& leptons,
                MAfloat64 DeltaRmax = 0.1, MAfloat64 PTmin = 0.5) const;

  std::vector<const RecJetFormat*>
    JetCleaning(const std::vector<const RecJetFormat*>& uncleaned_jets,
                const std::vector<const RecPhotonFormat*>& photons,
                MAfloat64 DeltaRmax = 0.1, MAfloat64 PTmin = 0.5) const;

  std::vector<const RecJetFormat*>
    JetCleaning(const std::vector<RecJetFormat>& uncleaned_jets,
                const std::vector<const RecLeptonFormat*>& leptons,
                MAfloat64 DeltaRmax = 0.1, MAfloat64 PTmin = 0.5) const
  {
    std::vector<const RecJetFormat*> uncleaned_jets2(uncleaned_jets.size());
    for (MAuint32 i=0;i<uncleaned_jets.size();i++) uncleaned_jets2[i]=&(uncleaned_jets[i]); 
    return JetCleaning(uncleaned_jets2,leptons,DeltaRmax,PTmin);
  }

  std::vector<const RecJetFormat*>
    JetCleaning(const std::vector<RecJetFormat>& uncleaned_jets,
                const std::vector<const RecPhotonFormat*>& photons,
                MAfloat64 DeltaRmax = 0.1, MAfloat64 PTmin = 0.5) const
  {
    std::vector<const RecJetFormat*> uncleaned_jets2(uncleaned_jets.size());
    for (MAuint32 i=0;i<uncleaned_jets.size();i++) uncleaned_jets2[i]=&(uncleaned_jets[i]); 
    return JetCleaning(uncleaned_jets2,photons,DeltaRmax,PTmin);
  }



};

}

#endif
