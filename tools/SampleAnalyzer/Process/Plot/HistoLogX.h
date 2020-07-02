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


#ifndef HISTO_LOGX_H
#define HISTO_LOGX_H


// STL headers
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/Histo.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

class HistoLogX : public Histo
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  // Histogram boundaries in Log scale
  MAfloat64 log_xmin_;
  MAfloat64 log_xmax_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor withtout argument
  HistoLogX()
  { }

  /// Constructor with argument 
  HistoLogX(const std::string& name, MAuint32 nbins, 
            MAfloat64 xmin, MAfloat64 xmax) : Histo(name)
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

    // Setting the description: min
    try
    {
      if (xmin<=0) throw EXCEPTION_WARNING("xmin cannot be less than or equal to zero. Setting xmin to 0.1","",0);
      xmin_ = xmin;
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      xmin_=.1;
    }

    // Setting the description: max
    try
    {
      if (xmin>=xmax) throw EXCEPTION_WARNING("xmin cannot be equal to or greater than xmax. Setting xmin to 0.1 and xmax to 100.","",0);
      xmax_ = xmax;
    }
    catch (const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      xmin_=.1;
      xmax_=100.;
    }


    log_xmin_=std::log10(xmin_);
    log_xmax_=std::log10(xmax_);
    step_ = (log_xmax_ - log_xmin_)/static_cast<MAfloat64>(nbins_);

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

    // Negative weight
    else
    {
      nentries_.second++;
      weight=std::abs(weight);
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
  //  virtual void Write_RootFormat(std::pair<TH1F*,TH1F*>& histos);

};

}

#endif
