////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


#ifndef PLOT_MANAGER_H
#define PLOT_MANAGER_H

// STL headers
#include <iostream>
#include <ostream>
#include <vector>

// ROOT headers
#include <TFile.h>

// SampleAnalyzer
#include "SampleAnalyzer/Plot/PlotBase.h"
#include "SampleAnalyzer/Plot/Histo.h"
#include "SampleAnalyzer/Plot/HistoLogX.h"
#include "SampleAnalyzer/Plot/HistoFrequency.h"
#include "SampleAnalyzer/Writer/SAFWriter.h"

namespace MA5
{

class PlotManager
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /// Collection of plots
  std::vector<PlotBase*> plots_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  PlotManager()
  { }

  /// Destructor
  ~PlotManager()
  { }

  /// Reset
  void Reset()
  { 
    for (unsigned int i=0;i<plots_.size();i++) 
    { if (plots_[i]==0) delete plots_[i]; }
    plots_.clear(); 
  }

  /// Adding a 1D histogram with fixed bins
  Histo* Add_Histo(const std::string& name, UInt_t bins, 
                   Float_t xmin, Float_t xmax)
  {
    Histo* myhisto = new Histo(name, bins,  xmin, xmax);
    plots_.push_back(myhisto);
    return myhisto;
  }

  /// Adding a 1D histogram with a log binning
  HistoLogX* Add_HistoLogX(const std::string& name, UInt_t bins, 
                           Float_t xmin, Float_t xmax)
  {
    HistoLogX* myhisto = new HistoLogX(name, bins,  xmin, xmax);
    plots_.push_back(myhisto);
    return myhisto;
  }

  /// Adding a 1D histogram for frequency
  template <typename T> 
  HistoFrequency<T>* Add_HistoFrequency(const std::string& name)
  {
    HistoFrequency<T>* myhisto = new HistoFrequency<T>(name);
    plots_.push_back(myhisto);
    return myhisto;
  }

  /// Write the counters in a Text file
  void Write_TextFormat(SAFWriter& output);

  /// Write the counters in a ROOT file
  void Write_RootFormat(TFile* output);

  /// Finalizing
  void Finalize()
  { Reset(); }

};

}

#endif
