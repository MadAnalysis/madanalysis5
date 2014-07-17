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


#ifndef HISTO_LOGX_H
#define HISTO_LOGX_H

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/Histo.h"

// ROOT headers
#include <TH1F.h>

// STL headers
#include <cmath>

namespace MA5
{

class HistoLogX : public Histo
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  // Histogram boundaries in Log scale
  Double_t log_xmin_;
  Double_t log_xmax_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout argument
  HistoLogX()
  { }

  /// Constructor with argument 
  HistoLogX(const std::string& name, UInt_t nbins, 
            Double_t xmin, Double_t xmax) : Histo(name)
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
    if (xmin<=0)
    {
      std::cout << "WARNING xmin cannot be less than or equal to zero" << std::endl;
      std::cout << "Setting xmin to 1." << std::endl;
      xmin_=1.;
    }
    if (xmin_>=xmax)
    {
      std::cout << "WARNING: xmin cannot be equal to or greater than xmax" << std::endl;
      std::cout << "Setting xmin to 1. and xmax to 100." << std::endl;
      xmin_=1.;
      xmax_=100.;
    }
    log_xmin_=std::log10(xmin);
    log_xmax_=std::log10(xmax);

    step_ = (log_xmax_ - log_xmin_)/static_cast<Double_t>(nbins_);

    // Reseting the histogram array
    histo_.resize(nbins_,std::make_pair(0.,0.));
    underflow_ = std::make_pair(0.,0.);
    overflow_  = std::make_pair(0.,0.);
	
    // Reseting statistical counters
    sum_w_    = std::make_pair(0.,0.);
    sum_ww_   = std::make_pair(0.,0.);
    sum_xw_   = std::make_pair(0.,0.);
  }

  /// Destructor
  virtual ~HistoLogX()
  { }

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
      sum_w_.first  +=weight;
      sum_ww_.first +=weight*weight;
      sum_xw_.first +=value*weight;
      if (value < xmin_) underflow_.first+=weight;
      else if (value >= xmax_) overflow_.first+=weight;
      else
      {
        histo_[std::floor((std::log10(value)-log_xmin_)/step_)].first+=weight;
      }
    }
    else
    {
      nentries_.second++;
      weight=fabs(weight);
      sum_w_.second  += weight;
      sum_ww_.second += weight*weight;
      sum_xw_.second += value*weight;
      if (value < xmin_) underflow_.second+=weight;
      else if (value >= xmax_) overflow_.second+=weight;
      else
      {
        histo_[std::floor((std::log10(value)-log_xmin_)/step_)].second+=weight;
      }
    }
  }


  /// Write the plot in a Text file
  virtual void Write_TextFormat(std::ostream* output);

  // Write the plot in a ROOT file
  virtual void Write_RootFormat(std::pair<TH1F*,TH1F*>& histos);

};

}

#endif
