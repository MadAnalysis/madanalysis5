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
 * @file DelphesMemoryInterface.h
 * @brief Transfer of the Delphes output (in memory) to the MA5 data format.
 */

#ifndef DELPHES_MEMORY_INTERFACE_h
#define DELPHES_MEMORY_INTERFACE_h


// STL headers
#include <string>
#include <map>
#include <iostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Base/StatusCode.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Base/DetectorBase.h"


class TObjArray;
class Delphes;
class Candidate;

namespace MA5
{

/** @brief Reads the Delphes output arrays (TFolder "Delphes/Export") of DetectorDelphes and fills RecEventFormat. */
class DelphesMemoryInterface
{
 public : 

  /// Original Delphes candidates mapped to MC particle indices for this event.
  std::map<const Candidate*, MAuint32> MCParticleIndices_;

  /// Pointers to data
  TObjArray* Jet_;
  TObjArray* FatJet_;
  TObjArray* Electron_;
  TObjArray* Photon_;
  TObjArray* Muon_;
  TObjArray* MET_;
  TObjArray* HT_;
  TObjArray* GenParticle_;
  TObjArray* Track_;
  TObjArray* Vertex_;
  TObjArray* Tower_;
  TObjArray* Event_;
  TObjArray* EFlowTrack_;
  TObjArray* EFlowPhoton_;
  TObjArray* EFlowNeutral_;

  /** @brief Whether the MA5-tuned collections (JetMA5, ElectronMA5, ...) are used. */
  MAbool delphesMA5card_;

  /** @brief Constructor (all pointers set to 0). */
  DelphesMemoryInterface();

  /** @brief Destructor (the arrays belong to Delphes). */
  ~DelphesMemoryInterface();

  /// Initialize access to the collections produced by Delphes
  void Initialize(Delphes* delphes, const std::map<std::string,std::string>& table, MAbool MA5card);

  TObjArray* GetCollection(Delphes* delphes, const std::map<std::string,std::string>& table, const std::string& name);


  /**
   * @brief Fill the reconstructed event (jets, taus, leptons, photons, tracks, towers, e-flow objects, MET and HT).
   *
   * @param mySample current sample.
   * @param myEvent current event.
   * @return true.
   */
  MAbool TransfertDELPHEStoMA5(SampleFormat& mySample, EventFormat& myEvent);

};

}

#endif
