////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef PLOT_MANAGER_H
#define PLOT_MANAGER_H


// STL headers
#include <iostream>
#include <ostream>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/PlotBase.h"
#include "SampleAnalyzer/Process/Plot/Histo.h"
#include "SampleAnalyzer/Process/Plot/HistoLogX.h"
#include "SampleAnalyzer/Process/Plot/HistoFrequency.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"


namespace MA5
{

class PlotManager
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

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
    for (MAuint32 i=0;i<plots_.size();i++) 
    { if (plots_[i]!=0) delete plots_[i]; }
    plots_.clear();
  }

  /// Get method
  std::vector<PlotBase*> GetHistos()
    { return plots_; }

  /// Getting thenumber of plots
  MAuint32 GetNplots()
    { return plots_.size(); }

  /// Adding a 1D histogram with fixed bins
  Histo* Add_Histo(const std::string& name, MAuint32 bins, 
                   MAfloat64 xmin, MAfloat64 xmax)
  {
    Histo* myhisto = new Histo(name, bins,  xmin, xmax);
    plots_.push_back(myhisto);
    return myhisto;
  }

  Histo* Add_Histo(const std::string& name, MAuint32 bins, 
                   MAfloat64 xmin, MAfloat64 xmax, std::vector<RegionSelection*> regions)
  {
    Histo* myhisto = new Histo(name, bins,  xmin, xmax);
    myhisto->SetSelectionRegions(regions);
    plots_.push_back(myhisto);
    return myhisto;
  }

  /// Adding a 1D histogram with a log binning
  HistoLogX* Add_HistoLogX(const std::string& name, MAuint32 bins, 
                           MAfloat64 xmin, MAfloat64 xmax)
  {
    HistoLogX* myhisto = new HistoLogX(name, bins,  xmin, xmax);
    plots_.push_back(myhisto);
    return myhisto;
  }

  HistoLogX* Add_HistoLogX(const std::string& name, MAuint32 bins,
                   MAfloat64 xmin, MAfloat64 xmax, std::vector<RegionSelection*> regions)
  {
    HistoLogX* myhisto = new HistoLogX(name, bins,  xmin, xmax);
    myhisto->SetSelectionRegions(regions);
    plots_.push_back(myhisto);
    return myhisto;
  }

  /// Adding a 1D histogram for frequency
  HistoFrequency* Add_HistoFrequency(const std::string& name)
  {
    HistoFrequency* myhisto = new HistoFrequency(name);
    plots_.push_back(myhisto);
    return myhisto;
  }

  HistoFrequency* Add_HistoFrequency(const std::string& name, std::vector<RegionSelection*> regions)
  {
    HistoFrequency* myhisto = new HistoFrequency(name);
    myhisto->SetSelectionRegions(regions);
    plots_.push_back(myhisto);
    return myhisto;
  }

  /// Write the counters in a Text file
  void Write_TextFormat(SAFWriter& output);

  /// Finalizing
  void Finalize()
  { Reset(); }

};

}

#endif
