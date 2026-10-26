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
 * @file IsolationConeType.h
 * @brief Isolation variables computed in a cone around an object.
 */

#ifndef IsolationConeType_h
#define IsolationConeType_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

class LHCOReader;
class ROOTReader;
class DelphesTreeReader;
class DelphesMA5tuneTreeReader;
class DetectorDelphes;
class DetectorDelphesMA5tune;
class ClusterAlgoBase;

/**
 * @brief Isolation variables in a cone of given radius around an object.
 *
 * The contribution of the object itself (selfPT_, selfET_) is subtracted by the
 * accessors sumPT() and sumET().
 */
class IsolationConeType
{

  friend class LHCOReader;
  friend class ROOTReader;
  friend class JetClusteringFastJet;
  friend class bTagger;
  friend class TauTagger;
  friend class cTagger;
  friend class DetectorDelphes;
  friend class DetectorDelphesMA5tune;
  friend class DelphesTreeReader;
  friend class DelphesMA5tuneTreeReader;
  friend class ClusterAlgoBase;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief Number of tracks in the cone. */
  MAuint16 ntracks_;    /// number of tracks
  /** @brief Scalar sum of the transverse momenta of the tracks. */
  MAfloat32 sumPT_;       /// sum PT
  /** @brief Scalar sum of the transverse momenta of the energy-flow objects. */
  MAfloat32 eflow_sumPT_; /// sum PT eflow
  /** @brief Scalar sum of the transverse energies of the calorimeter deposits. */
  MAfloat32 sumET_;       /// sum ET
  /** @brief Radius of the cone. */
  MAfloat32 deltaR_;      /// deltaR of the cone
  /** @brief Transverse energy and momentum of the object itself. */
  MAfloat32 selfET_;
  MAfloat32 selfPT_;

  // -------------------------------------------------------------
  //                        method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor (members reset). */
  IsolationConeType()
  { Reset(); }

  /** @brief Destructor. */
  virtual ~IsolationConeType()
  {}

  /** @brief Print (nothing is printed). */
  virtual void Print() const
  {
  }

  /** @brief Reset all the members. */
  virtual void Reset()
  {
    ntracks_     = 0; 
    sumPT_       = 0.;
    sumET_       = 0.;
    eflow_sumPT_ = 0.;
    deltaR_      = 0.;
    selfET_      = 0.;
    selfPT_      = 0.;
  }

  /**
   * @brief Accessor to the number of tracks.
   *
   * @return the number of tracks.
   */
  const MAuint16 ntracks() const
  {return ntracks_;}

  /**
   * @brief Accessor to the sum of the track transverse momenta, without the object itself.
   *
   * @return the sum.
   */
  const MAfloat32 sumPT() const
  { return sumPT_ - selfPT_; }

  /**
   * @brief Accessor to the sum of the transverse energies, without the object itself.
   *
   * @return the sum.
   */
  const MAfloat32 sumET() const
  { return sumET_ - selfET_; }

  /**
   * @brief Accessor to the radius of the cone.
   *
   * @return the radius.
   */
  const MAfloat32& deltaR() const
  {return deltaR_;}

  /**
   * @brief Accessor to the sum of the energy-flow transverse momenta.
   *
   * @return the sum.
   */
  const MAfloat32& sumPTeflow() const
  {return eflow_sumPT_;}

  /**
   * @brief Set the number of tracks.
   *
   * @param tracks number of tracks.
   */
  void setNtracks(MAuint16 tracks)
  {ntracks_=tracks;}

  /**
   * @brief Add tracks.
   *
   * @param tracks number of tracks to add.
   */
  void addNtracks(MAuint16 tracks)
  {ntracks_+=tracks;}

  /**
   * @brief Set the sum of the track transverse momenta.
   *
   * @param sumPT sum.
   */
  void setsumPT(MAfloat32 sumPT)
  {sumPT_=sumPT;}

  /**
   * @brief Add to the sum of the track transverse momenta.
   *
   * @param sumPT value to add.
   */
  void addsumPT(MAfloat32 sumPT)
  {sumPT_+=sumPT;}

  /**
   * @brief Set the sum of the transverse energies.
   *
   * @param sumET sum.
   */
  void setSumET(MAfloat32 sumET)
  {sumET_=sumET;}

  /**
   * @brief Add to the sum of the transverse energies.
   *
   * @param sumET value to add.
   */
  void addSumET(MAfloat32 sumET)
  {sumET_+=sumET;}

  /**
   * @brief Set the radius of the cone.
   *
   * @param deltaR radius.
   */
  void setDeltaR(MAfloat32 deltaR)
  {deltaR_=deltaR;}

  /**
   * @brief Set the transverse momentum of the object itself.
   *
   * @param PT transverse momentum.
   */
  void setSelfPT(MAfloat32 PT)
  {selfPT_=PT;}

  /**
   * @brief Set the transverse energy of the object itself.
   *
   * @param ET transverse energy.
   */
  void setSelfET(MAfloat32 ET)
  {selfET_=ET;}

  /**
   * @brief Set the sum of the energy-flow transverse momenta.
   *
   * @param eflow_sumPT sum.
   */
  void setSumPTeflow(MAfloat32 eflow_sumPT)
  {eflow_sumPT_=eflow_sumPT;}

};

}

#endif
