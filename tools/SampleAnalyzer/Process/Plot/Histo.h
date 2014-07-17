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


#ifndef HISTO_H
#define HISTO_H

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/PlotBase.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"

// ROOT headers
#include <TH1F.h>

// STL headers
#include <map>
#include <cmath>
#include <vector>

namespace MA5
{

class Histo : public PlotBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  /// Histogram arrays
  std::vector< std::pair<Double_t,Double_t> > histo_;
  std::pair<Double_t, Double_t> underflow_;
  std::pair<Double_t, Double_t> overflow_;

  /// Histogram description
  UInt_t   nbins_;
  Double_t xmin_;
  Double_t xmax_;
  Double_t step_;

  /// Sum of event-weights over entries
  std::pair<Double_t,Double_t> sum_w_;

  /// Sum of squared weights
  std::pair<Double_t,Double_t> sum_ww_;

  /// Sum of value * weight
  std::pair<Double_t,Double_t> sum_xw_;

  /// Sum of value * value * weight
  std::pair<Double_t,Double_t> sum_xxw_;

  /// RegionSelections attached to the histo
  std::vector<RegionSelection*> regions_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument
  Histo() : PlotBase()
  {
    nbins_=100; xmin_=0; xmax_=100;
    step_ = (xmax_ - xmin_)/static_cast<Double_t>(nbins_);
  }

  /// Constructor with argument 
  Histo(const std::string& name) : PlotBase(name)
  { }

  /// Constructor with argument 
  Histo(const std::string& name, UInt_t nbins, Double_t xmin, Double_t xmax) :
		PlotBase(name)
  { 
    // Setting the description
    nbins_ = nbins;
    if (nbins_==0)
    {
      std::cout << "WARNING: nbins cannot be equal to 0. Set 100" << std::endl;
      nbins_ = 100;
    }

    xmin_ = xmin;
    xmax_ = xmax;
    if (xmin_>=xmax)
    {
      std::cout << "WARNING: xmin cannot be equal to or greater than xmax" << std::endl;
      std::cout << "Setting xmin to 0 and xmax to 100" << std::endl;
      xmin_=0.;
      xmax_=100.;
    }

    step_ = (xmax_ - xmin_)/static_cast<Double_t>(nbins_);

    // Reseting the histogram array
    histo_.resize(nbins_,std::make_pair(0.,0.));
    underflow_ = std::make_pair(0.,0.);
    overflow_  = std::make_pair(0.,0.);
	
    // Reseting statistical counters
    sum_w_    = std::make_pair(0.,0.);
    sum_ww_   = std::make_pair(0.,0.);
    sum_xw_   = std::make_pair(0.,0.);
    sum_xxw_  = std::make_pair(0.,0.);
  }

  /// Destructor
  virtual ~Histo()
  { }

  /// Setting the linked regions
  void SetSelectionRegions(std::vector<RegionSelection*> myregions)
    { regions_.insert(regions_.end(), myregions.begin(), myregions.end()); }

  /// Checking that all regions of the histo are surviving
  /// Returns 0 if all regions are failing (includes te case with 0 SR)
  /// Returns 1 if all regions are passing 
  // returns -1 otherwise
  int AllSurviving()
  {
    if (regions_.size() == 0) return 0;
    bool FirstRegionSurvival = regions_[0]->IsSurviving();
    for(unsigned int ii=1; ii < regions_.size(); ii++)
      if(regions_[ii]->IsSurviving() != FirstRegionSurvival) return -1;
    if(FirstRegionSurvival) return 1;
    else                    return 0;
  }

  /// Filling histogram
  void Fill(Double_t value, Double_t weight=1.0)
  {
    if (std::isnan(value))
    {
      WARNING << "Skipping a NaN (Not a Number) value in an histogram." 
              << endmsg;
      return;
    }

    if (std::isinf(value))
    {
      WARNING << "Skipping a Infinity value in an histogram." 
              << endmsg;
      return;
    }

    if (weight>=0)
    {
      nentries_.first++;
      sum_w_.first   +=weight;
      sum_ww_.first  +=weight*weight;
      sum_xw_.first  +=value*weight;
      sum_xxw_.first +=value*value*weight;
      if (value < xmin_) underflow_.first+=weight;
      else if (value >= xmax_) overflow_.first+=weight;
      else
      {
        histo_[std::floor((value-xmin_)/step_)].first+=weight;
      }
    }
    else
    {
      nentries_.second++;
      weight=std::abs(weight);
      sum_w_.second   += weight;
      sum_ww_.second  += weight*weight;
      sum_xw_.second  += value*weight;
      sum_xxw_.second += value*value*weight;
      if (value < xmin_) underflow_.second+=weight;
      else if (value >= xmax_) overflow_.second+=weight;
      else
      {
        histo_[std::floor((value-xmin_)/step_)].second+=weight;
      }
    }
  }

  /// Write the plot in a ROOT file
  virtual void Write_TextFormat(std::ostream* output);

  /// Write the plot in a ROOT file
  virtual void Write_RootFormat(std::pair<TH1F*,TH1F*>& histos);

 protected:

  /// Write the plot in a ROOT file
  virtual void Write_TextFormatBody(std::ostream* output);

};

}

#endif
