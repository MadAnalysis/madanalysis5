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
 * @file DJRextractor.h
 * @brief Computation of the differential jet rates (kT algorithm) for merging validation.
 */

#ifndef DJR_EXTRACTOR_H
#define DJR_EXTRACTOR_H


// STL headers
#include <vector>
#include <map>
#include <string>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"


/** @brief Forward declarations of FastJet classes. */
namespace fastjet
{
  class JetDefinition;
  class PseudoJet;
}

namespace MA5
{
/** @brief Computation of the differential jet rates from the partons of the parton shower. */
class DJRextractor
{

//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

  /** @brief kT jet definition with R = 1 (owned). */
  fastjet::JetDefinition* JetDefinition_;

  /** @brief Maximum number of extra jets, flavour of the matched quarks, no-single-radiation flag. */
  MAuint32  merging_njets_;
  MAuint8   merging_nqmatch_;
  MAbool    merging_nosingrad_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
 public : 

  /** @brief Constructor. */
  DJRextractor() 
  {
    // Jet algo
    JetDefinition_=0;
    // Options
    merging_nqmatch_=4;
    merging_nosingrad_=false;
  }

  /** @brief Destructor. */
  ~DJRextractor() {}

  /**
   * @brief Create the kT jet definition.
   *
   * @return true.
   */
  MAbool Initialize();

  /** @brief Delete the jet definition (see the FIXME in the source). */
  void Finalize();

  /**
   * @brief Compute the DJR values of an event.
   *
   * @param sample sample.
   * @param event event.
   * @param DJR values to fill (the size gives the number of DJRs).
   * @return false if the Monte Carlo information is missing.
   */
  MAbool Execute(SampleFormat& sample, const EventFormat& event, std::vector<MAdouble64>& DJR);

  /**
   * @brief Number of additional jets of the event (declared but not defined).
   *
   * @param myEvent Monte Carlo event.
   * @param mySample Monte Carlo sample.
   * @return the number of jets.
   */
  // NOTE: this method has no definition.
  MAuint32 ExtractJetNumber(const MCEventFormat* myEvent, MCSampleFormat* mySample);

  /**
   * @brief Select the partons of the parton shower (no beam remnants, |y| < 5, no radiation duplicates).
   *
   * @param inputs FastJet inputs to fill.
   * @param myEvent Monte Carlo event.
   */
  void SelectParticles(std::vector<fastjet::PseudoJet>& inputs, const MCEventFormat* myEvent);

  /**
   * @brief Compute the DJR values (merging scales of the exclusive kT clustering).
   *
   * @param inputs FastJet inputs.
   * @param DJRvalues values to fill.
   */
  void ExtractDJR(const std::vector<fastjet::PseudoJet>& inputs,std::vector<MAdouble64>& DJRvalues);

  /**
   * @brief Absolute rapidity-like variable used for the cut on the partons (capped).
   *
   * @param px px.
   * @param py py.
   * @param pz pz.
   * @return the value.
   */
  MAdouble64 rapidity(MAdouble64 px, MAdouble64 py, MAdouble64 pz);


};

}

#endif

