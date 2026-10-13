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
 * @file Isolation.h
 * @brief Isolation toolbox (PHYSICS->Isol) and jet cleaning.
 */

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

/** @brief Isolation toolbox: isolation based on tracks, calorimeter towers, both, or energy-flow objects, and jet cleaning. */
class Isolation
{

  public:


  /** @brief Isolation based on the tracks. */
  IsolationTracker     *tracker;
  /** @brief Isolation based on the calorimeter towers. */
  IsolationCalorimeter *calorimeter;
  /** @brief Isolation based on the tracks and the calorimeter towers. */
  IsolationCombined    *combined;
  /** @brief Isolation based on the energy-flow objects. */
  IsolationEFlow       *eflow;

  /** @brief Constructor (creates the isolation tools). */
  Isolation()
  {
    tracker     = new IsolationTracker;
    calorimeter = new IsolationCalorimeter;
    combined    = new IsolationCombined;
    eflow       = new IsolationEFlow;
  }

  /** @brief Destructor. */
  ~Isolation()
  {
    delete tracker;
    delete calorimeter;
    delete combined;
    delete eflow;
  }


  /**
   * @brief Remove from a jet collection the soft jets and the jets overlapping with leptons.
   *
   * For each lepton, the closest jet within DeltaRmax is removed.
   *
   * @param uncleaned_jets jets.
   * @param leptons leptons.
   * @param DeltaRmax maximum distance for the overlap removal.
   * @param PTmin minimum jet pT.
   * @return the cleaned jets.
   */
  std::vector<const RecJetFormat*>
    JetCleaning(const std::vector<const RecJetFormat*>& uncleaned_jets,
                const std::vector<const RecLeptonFormat*>& leptons,
                MAfloat64 DeltaRmax = 0.1, MAfloat64 PTmin = 0.5) const;

  /**
   * @brief Remove from a jet collection the soft jets and the jets overlapping with photons.
   *
   * For each photon, the closest jet within DeltaRmax is removed.
   *
   * @param uncleaned_jets jets.
   * @param photons photons.
   * @param DeltaRmax maximum distance for the overlap removal.
   * @param PTmin minimum jet pT.
   * @return the cleaned jets.
   */
  std::vector<const RecJetFormat*>
    JetCleaning(const std::vector<const RecJetFormat*>& uncleaned_jets,
                const std::vector<const RecPhotonFormat*>& photons,
                MAfloat64 DeltaRmax = 0.1, MAfloat64 PTmin = 0.5) const;

  /**
   * @brief Remove from a jet collection the soft jets and the jets overlapping with leptons.
   *
   * @param uncleaned_jets jets.
   * @param leptons leptons.
   * @param DeltaRmax maximum distance for the overlap removal.
   * @param PTmin minimum jet pT.
   * @return the cleaned jets (pointers into uncleaned_jets).
   */
  std::vector<const RecJetFormat*>
    JetCleaning(const std::vector<RecJetFormat>& uncleaned_jets,
                const std::vector<const RecLeptonFormat*>& leptons,
                MAfloat64 DeltaRmax = 0.1, MAfloat64 PTmin = 0.5) const
  {
    std::vector<const RecJetFormat*> uncleaned_jets2(uncleaned_jets.size());
    for (MAuint32 i=0;i<uncleaned_jets.size();i++) uncleaned_jets2[i]=&(uncleaned_jets[i]); 
    return JetCleaning(uncleaned_jets2,leptons,DeltaRmax,PTmin);
  }

  /**
   * @brief Remove from a jet collection the soft jets and the jets overlapping with photons.
   *
   * @param uncleaned_jets jets.
   * @param photons photons.
   * @param DeltaRmax maximum distance for the overlap removal.
   * @param PTmin minimum jet pT.
   * @return the cleaned jets (pointers into uncleaned_jets).
   */
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
