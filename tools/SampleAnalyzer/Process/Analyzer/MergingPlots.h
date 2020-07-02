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


#ifndef MERGING_PLOTS_H
#define MERGING_PLOTS_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/MergingPlotType.h"
#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"


#ifdef FASTJET_USE


namespace MA5
{

class DJRextractor;

class MergingPlots : public AnalyzerBase
{
  INIT_ANALYSIS(MergingPlots,"MergingPlots")

//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

  /// Algo based on FastJet
  DJRextractor* algo_;

  /// DJR plots
  std::vector<MergingPlotType> DJR_;

  /// User configuration
  MAuint32 merging_njets_;
  MAuint8  merging_nqmatch_;
  MAbool   merging_nosingrad_;
  MAbool   ma5_mode_;

  /// Saving merging plots in the output file
  void Write_TextFormat(SAFWriter& output);

  /// Extracting the number of additionnal jets contained in the event 
  MAuint32 ExtractHardJetNumber(const MCEventFormat* myEvent, MCSampleFormat* mySample);


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
 public : 

  /// Initialization
  virtual MAbool Initialize(const Configuration& cfg,
             const std::map<std::string,std::string>& parameters);

  /// Finalization
  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);

  /// Execution
  virtual MAbool Execute(SampleFormat& sample, const EventFormat& event);

};
}

#endif
#endif

