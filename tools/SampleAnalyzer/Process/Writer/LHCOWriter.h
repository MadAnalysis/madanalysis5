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
 * @file LHCOWriter.h
 * @brief Writer of LHCO files (reconstructed objects).
 */

#ifndef LHCO_WRITER_BASE_h
#define LHCO_WRITER_BASE_h


// STL headers
#include <fstream>
#include <iostream>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/WriterTextBase.h"
#include "SampleAnalyzer/Process/Writer/LHCOParticleFormat.h"


namespace MA5
{

/** @brief Writer of LHC Olympics files. */
class LHCOWriter : public WriterTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  /** @brief Number of events written. */
  MAuint32 counter_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /** @brief Constructor. */
  LHCOWriter() : WriterTextBase()
  { counter_=0; }

  /** @brief Destructor. */
  virtual ~LHCOWriter()
  {}

  /**
   * @brief Write the header of the file (format description and original header).
   *
   * @param mySample sample.
   * @return true.
   */
  virtual MAbool WriteHeader(const SampleFormat& mySample);

  /**
   * @brief Write an event (events without reconstructed objects are skipped).
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
   * @brief Fill an LHCO line from a jet.
   *
   * @param jet jet.
   * @param lhco line to fill.
   */
  void WriteJet(const RecJetFormat& jet, LHCOParticleFormat* lhco);
  /**
   * @brief Fill an LHCO line from a muon (the isolation variables are encoded in the HAD/EM column).
   *
   * @param muon muon.
   * @param lhco line to fill.
   * @param myEvent reconstructed event.
   * @param npart number of objects written before the jets.
   */
  void WriteMuon(const RecLeptonFormat& muon, LHCOParticleFormat* lhco, const RecEventFormat* myEvent, MAuint32 npart);
  /**
   * @brief Fill an LHCO line from an electron.
   *
   * @param electron electron.
   * @param lhco line to fill.
   */
  void WriteElectron(const RecLeptonFormat& electron, LHCOParticleFormat* lhco);
  /**
   * @brief Fill an LHCO line from a photon.
   *
   * @param photon photon.
   * @param lhco line to fill.
   */
  void WritePhoton(const RecPhotonFormat& photon, LHCOParticleFormat* lhco);
  /**
   * @brief Fill an LHCO line from a tau.
   *
   * @param tau tau.
   * @param lhco line to fill.
   */
  void WriteTau(const RecTauFormat& tau, LHCOParticleFormat* lhco);
  /**
   * @brief Fill an LHCO line from the missing transverse momentum.
   *
   * @param met missing transverse momentum.
   * @param lhco line to fill.
   */
  void WriteMET(const ParticleBaseFormat& met, LHCOParticleFormat* lhco);


};

}

#endif
