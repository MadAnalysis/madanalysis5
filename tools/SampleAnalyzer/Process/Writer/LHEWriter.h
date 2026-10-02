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
 * @file LHEWriter.h
 * @brief Writer of LHE files (Monte Carlo and/or reconstructed events).
 */

#ifndef LHE_WRITER_BASE_h
#define LHE_WRITER_BASE_h


// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterTextBase.h"
#include "SampleAnalyzer/Process/Writer/LHEParticleFormat.h"


namespace MA5
{

/**
 * @brief Writer of LHE files.
 *
 * Monte Carlo events are written as they are; for events with reconstructed objects,
 * the hard-process particles are written together with the reconstructed objects
 * (jets, leptons, photons, taus and MET), following the simplified LHE format of
 * MadAnalysis 5.
 */
class LHEWriter : public WriterTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:



  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  LHEWriter() : WriterTextBase()
  {}

  /** @brief Destructor. */
  virtual ~LHEWriter()
  {}

  /**
   * @brief Write the header of the file (with the <init> block).
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool WriteHeader(const SampleFormat& mySample);

  /**
   * @brief Write an event (the header is written before the first event).
   *
   * @param myEvent event.
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool WriteEvent(const EventFormat& myEvent, 
                          const SampleFormat& mySample);

  /**
   * @brief Write the footer of the file.
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool WriteFoot(const SampleFormat& mySample);
 
 private:

  /**
   * @brief Write the first line of an event.
   *
   * @param myEvent event.
   * @param nevents number of particles in the event.
   * @return true.
   */
  MAbool WriteEventHeader(const EventFormat& myEvent,
                        MAuint32 nevents);

  /**
   * @brief Fill an LHE particle line from a Monte Carlo particle.
   *
   * @param myPart particle.
   * @param mother1 index of the first mother.
   * @param mother2 index of the second mother.
   * @param statuscode status code (0: keep the original one).
   * @param lhe line to fill.
   */
  void WriteParticle(const MCParticleFormat& myPart, MAint32 mother1, MAint32 mother2, 
                     MAint32 statuscode, LHEParticleFormat& lhe);

  /**
   * @brief Format a number in the Fortran style.
   *
   * @param value number.
   * @param precision number of digits.
   * @return the formatted number.
   */
  static std::string FortranFormat_SimplePrecision(MAfloat32 value,MAuint32 precision=7); 
  /**
   * @brief Format a number in the Fortran style.
   *
   * @param value number.
   * @param precision number of digits.
   * @return the formatted number.
   */
  static std::string FortranFormat_DoublePrecision(MAfloat64 value,MAuint32 precision=11); 

  /**
   * @brief Fill an LHE particle line from a reconstructed jet.
   *
   * @param jet jet.
   * @param lhe line to fill.
   * @param mother index of the mother.
   */
  void WriteJet(const RecJetFormat& jet, LHEParticleFormat& lhe, MAint32& mother);
  /**
   * @brief Fill an LHE particle line from a reconstructed muon.
   *
   * @param muon muon.
   * @param lhe line to fill.
   * @param mother index of the mother.
   */
  void WriteMuon(const RecLeptonFormat& muon, LHEParticleFormat& lhe, MAint32& mother);
  /**
   * @brief Fill an LHE particle line from a reconstructed electron.
   *
   * @param electron electron.
   * @param lhe line to fill.
   * @param mother index of the mother.
   */
  void WriteElectron(const RecLeptonFormat& electron, LHEParticleFormat& lhe, MAint32& mother);
  /**
   * @brief Fill an LHE particle line from a reconstructed photon.
   *
   * @param photon photon.
   * @param lhe line to fill.
   * @param mother index of the mother.
   */
  void WritePhoton(const RecPhotonFormat& photon, LHEParticleFormat& lhe, MAint32& mother);
  /**
   * @brief Fill an LHE particle line from a reconstructed tau.
   *
   * @param tau tau.
   * @param lhe line to fill.
   * @param mother index of the mother.
   */
  void WriteTau(const RecTauFormat& tau, LHEParticleFormat& lhe, MAint32& mother);
  /**
   * @brief Fill an LHE particle line from the missing transverse momentum.
   *
   * @param met missing transverse momentum.
   * @param lhe line to fill.
   */
  void WriteMET(const ParticleBaseFormat& met, LHEParticleFormat& lhe);


};

}

#endif
