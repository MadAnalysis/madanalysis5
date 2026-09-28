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
 * @file MergingPlots.h
 * @brief Predefined analysis producing the differential jet rate (DJR) plots (FastJet only).
 */

#ifndef MERGING_PLOTS_H
#define MERGING_PLOTS_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/MergingPlotType.h"
#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"


#ifdef FASTJET_USE


namespace MA5
{

class DJRextractor;

/**
 * @brief Analysis filling the DJR distributions used to validate the ME/PS merging.
 *
 * For each DJR, the total distribution and the contribution of the events with 0, 1, ...
 * extra partons are filled (histograms `DJR<i>_total` and `DJR<i>_<n>jet`).
 */
class MergingPlots : public AnalyzerBase
{
  INIT_ANALYSIS(MergingPlots,"MergingPlots")

//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

  /** @brief Algorithm computing the DJR values (kT clustering). */
  DJRextractor* algo_;

  /** @brief DJR plots. */
  std::vector<MergingPlotType> DJR_;

  /** @brief Maximum number of extra jets, flavour of the matched quarks, no-single-radiation flag, MA5 mode. */
  MAuint32 merging_njets_;
  MAuint8  merging_nqmatch_;
  MAbool   merging_nosingrad_;
  MAbool   ma5_mode_;

  /** @brief Write the plots in the SAF file (empty: the plots are written by the region manager). */
  void Write_TextFormat(SAFWriter& output);

  /**
   * @brief Number of extra partons produced by the matrix element.
   *
   * @param myEvent Monte Carlo event.
   * @param mySample Monte Carlo sample.
   * @return the number of extra partons.
   */
  MAuint32 ExtractHardJetNumber(const MCEventFormat* myEvent, MCSampleFormat* mySample);


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
 public : 

  /**
   * @brief Initialise the plots (parameter `njets`: number of extra jets; `ma5_mode`: 1 to take the number of jets from the process identifier).
   *
   * @param cfg run configuration.
   * @param parameters parameters.
   * @return false if njets is 0.
   */
  virtual MAbool Initialize(const Configuration& cfg,
             const std::map<std::string,std::string>& parameters);

  /**
   * @brief Finalise the algorithm and the plots.
   *
   * @param summary summary of the samples.
   * @param files samples.
   */
  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);

  /**
   * @brief Fill the DJR plots for an event.
   *
   * @param sample current sample.
   * @param event current event.
   * @return false if the event is skipped.
   */
  virtual MAbool Execute(SampleFormat& sample, const EventFormat& event);

};
}

#endif
#endif

