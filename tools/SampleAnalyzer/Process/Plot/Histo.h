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


#ifndef HISTO_H
#define HISTO_H


// STL headers
#include <map>
#include <cmath>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/PlotBase.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

class Histo : public PlotBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  /// Histogram arrays
  std::vector< std::pair<MAfloat64,MAfloat64> > histo_;
  std::pair<MAfloat64, MAfloat64> underflow_;
  std::pair<MAfloat64, MAfloat64> overflow_;

  /// Histogram description
  MAuint32  nbins_;
  MAfloat64 xmin_;
  MAfloat64 xmax_;
  MAfloat64 step_;

  /// Sum of event-weights over entries
  std::pair<MAfloat64,MAfloat64> sum_w_;

  /// Sum of squared weights
  std::pair<MAfloat64,MAfloat64> sum_ww_;

  /// Sum of value * weight
  std::pair<MAfloat64,MAfloat64> sum_xw_;

  /// Sum of value * value * weight
  std::pair<MAfloat64,MAfloat64> sum_xxw_;

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
    step_ = (xmax_ - xmin_)/static_cast<MAfloat64>(nbins_);
  }

  /// Constructor with argument 
  Histo(const std::string& name) : PlotBase(name)
  { }

  /// Constructor with argument 
  Histo(const std::string& name, MAuint32 nbins, MAfloat64 xmin, MAfloat64 xmax) :
     PlotBase(name)
  { 
    // Setting the description: nbins
    try
    {
      if (nbins==0) throw EXCEPTION_WARNING("nbins cannot be equal to 0. Set 100.","",0);
      nbins_ = nbins;
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      nbins_ = 100;
    }

    // Setting the description: min & max
    try
    {
      if (xmin>=xmax) throw EXCEPTION_WARNING("xmin cannot be equal to or greater than xmax. Setting xmin to 0 and xmax to 100.","",0);
      xmin_ = xmin;
      xmax_ = xmax;
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      xmin_=0.;
      xmax_=100.;
    }    

    step_ = (xmax_ - xmin_)/static_cast<MAfloat64>(nbins_);

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
  MAint32 AllSurviving()
  {
    if (regions_.size() == 0) return 0;
    MAbool FirstRegionSurvival = regions_[0]->IsSurviving();
    for(MAuint32 ii=1; ii < regions_.size(); ii++)
      if(regions_[ii]->IsSurviving() != FirstRegionSurvival) return -1;
    if(FirstRegionSurvival) return 1;
    else                    return 0;
  }

  /// Filling histogram
  void Fill(MAfloat64 value, MAfloat64 weight=1.0)
  {
    // Safety : nan or isinf
    try
    {
      if (std::isnan(value)) throw EXCEPTION_WARNING("Skipping a NaN (Not a Number) value in an histogram.","",0); 
      if (std::isinf(value)) throw EXCEPTION_WARNING("Skipping a Infinity value in an histogram.","",0); 
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }

    // Positive weight
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

    // Negative weight
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
  //  virtual void Write_RootFormat(std::pair<TH1F*,TH1F*>& histos);

 protected:

  /// Write the plot in a ROOT file
  virtual void Write_TextFormatBody(std::ostream* output);

};

}

#endif
