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
 * @file RECconfig.h
 * @brief Reconstruction configuration: muon isolation algorithm.
 */

#ifndef RECTOOLCONFIG_h
#define RECTOOLCONFIG_h


// STL headers
#include <set>
#include <string>


namespace MA5
{

class Tools;

/** @brief Configuration of the muon isolation used by Identification::IsIsolatedMuon. */
struct RECconfig
{
  friend class PhysicsService;
  friend class Identification;
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  protected:

  /** @brief Use the DeltaR algorithm (true) or the SumPT algorithm (false). */
  MAbool deltaRalgo_;

  /** @brief DeltaR algorithm: minimum distance to the jets. */
  MAfloat32 deltaR_;

  /** @brief SumPT algorithm: maximum sum of the track transverse momenta. */
  MAfloat32 sumPT_;

  /** @brief SumPT algorithm: maximum ratio sumET/sumPT. */
  MAfloat32 ET_PT_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
  public:

  /** @brief Constructor (DeltaR algorithm with DeltaR = 0.5). */
  RECconfig()
  { deltaRalgo_=true; deltaR_=0.5; sumPT_=1.; ET_PT_=1.; }

  /** @brief Destructor. */
  ~RECconfig()
  { }

  /** @brief Reset to the DeltaR algorithm with DeltaR = 0.5. */
  void Reset()
  { deltaRalgo_=true; deltaR_=0.5; sumPT_=1.; ET_PT_=1.; }

  /**
   * @brief Use the DeltaR isolation algorithm.
   *
   * @param deltaR minimum distance between the muon and the jets.
   */
  void UseDeltaRIsolation(MAfloat32 deltaR=0.5)
  {
    deltaRalgo_=true; deltaR_=deltaR;
  } 

  /**
   * @brief Use the SumPT isolation algorithm.
   *
   * @param sumPT maximum sum of track pT.
   * @param ET_PT maximum ratio sumET/sumPT.
   */
  void UseSumPTIsolation(MAfloat32 sumPT, MAfloat32 ET_PT)
  {
    deltaRalgo_=false; sumPT_=sumPT; ET_PT_=ET_PT;
  } 

};

}

#endif
