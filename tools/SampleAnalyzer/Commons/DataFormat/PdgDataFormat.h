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
 * @file PdgDataFormat.h
 * @brief Properties of a particle species (PDG table entry).
 */

#ifndef PDGDATAFORMAT_H
#define PDGDATAFORMAT_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// STL headers
#include <string>
#include <cmath>


namespace MA5
{

/** @brief Properties of a particle species: PDG code, name, mass, charge, width and c*tau. */
class PdgDataFormat {
  public:
  /** @brief Default constructor (unknown particle, PDG code -999). */
  PdgDataFormat(): Pdgid_(-999), Mass_(0), Charge_(0), GammaTot_(0), Ctau_(0), Name_("Unknown"), IsInvisible_(false) {};
  /**
   * @brief Constructor (neutrinos and neutralinos are flagged as invisible).
   *
   * @param Pdgid PDG code.
   * @param Name name.
   * @param m mass [GeV].
   * @param q electric charge in units of e/3.
   * @param Gamma total width [GeV].
   * @param ctau c*tau [m].
   */
  PdgDataFormat(const MAint32 Pdgid, const std::string& Name, const MAfloat32 m, const MAint32 q, const MAfloat32 Gamma, const MAfloat32 ctau);
  /**
   * @brief Copy constructor.
   *
   * @param p entry to copy.
   */
  PdgDataFormat(const PdgDataFormat& p);
  /**
   * @brief Copy assignment.
   *
   * @param p entry to copy.
   * @return this entry.
   */
  PdgDataFormat& operator=(const PdgDataFormat& p);
  /** @brief Destructor. */
  ~PdgDataFormat() {};
  /**
   * @brief Accessor to the PDG code.
   *
   * @return the PDG code.
   */
  MAint32 Pdgid() const {return Pdgid_;};
  /**
   * @brief Accessor to the mass.
   *
   * @return the mass [GeV].
   */
  MAfloat32 Mass() const {return Mass_;};
  /**
   * @brief Accessor to the electric charge.
   *
   * @return the charge in units of e/3.
   */
  MAint32 Charge() const {return Charge_;};
  /**
   * @brief Accessor to the total width.
   *
   * @return the width [GeV].
   */
  MAfloat32 GammaTot() const {return GammaTot_;};
  /**
   * @brief Accessor to c*tau.
   *
   * @return c*tau [m].
   */
  MAfloat64 Ctau() const {return Ctau_;};
  /**
   * @brief Is the particle invisible (neutrino or neutralino)?
   *
   * @return true if invisible.
   */
  MAbool IsInvisible() const {return IsInvisible_;};
  /**
   * @brief Accessor to the name.
   *
   * @return the name.
   */
  std::string Name() const {return Name_;};

  private:
  MAint32 Pdgid_;
  MAfloat32 Mass_;        // GeV
  MAint32 Charge_;      // in e+/3
  MAfloat32 GammaTot_;   // GeV
  MAfloat64 Ctau_;       // in m
  std::string Name_;
  MAbool IsInvisible_;
};

}

#endif
