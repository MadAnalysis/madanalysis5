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


#ifndef HISTO_FREQUENCY_H
#define HISTO_FREQUENCY_H

// STL headers
#include <map>
#include <string>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/PlotBase.h"

namespace MA5
{

template <typename T> 
class HistoFrequency : public PlotBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  /// Collection of observables
  std::map<T, std::pair<Double_t,Double_t> > stack_;

  /// Sum of event-weights over entries
  std::pair<Double_t,Double_t> sum_w_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  typedef typename std::map<T,std::pair<Double_t,Double_t> >::iterator
       iterator; 
  typedef typename std::map<T,std::pair<Double_t,Double_t> >::const_iterator
       const_iterator; 
  typedef typename std::map<T,std::pair<Double_t,Double_t> >::size_type
       size_type;

  /// Constructor with argument 
  HistoFrequency(const std::string& name) : PlotBase(name)
  {     
    // Reseting statistical counters
    sum_w_ = std::make_pair(0.,0.);
  }

  /// Destructor
  virtual ~HistoFrequency()
  { }

  /// Adding an entry for a given observable
  void Fill(const T& obs, Double_t weight=1.0)
  {
    // Looking for the value
    iterator it = stack_.find(obs);

    // Value not found
    if (it==stack_.end())
    {
      stack_[obs]=std::make_pair(0.,0.);
    }

    // Value found
    else
    {
      if (weight>=0)
      {
        nentries_.first++;
        sum_w_.first+=weight;
        stack_[obs].first+=weight;
      }
      else 
      {
        nentries_.second++; 
        weight=std::abs(weight);
        sum_w_.second+=weight;
        stack_[obs].second+=weight;
      }
    }
  }

  /// Write the plot in a ROOT file
  virtual void Write_TextFormat(std::ostream* output)
  {
  // Header
	*output << "<HistoFrequency>" << std::endl;

  // Description
	*output << "<Description>" << std::endl;
	*output << "\"" << name_ << "\"" << std::endl;
	*output << "</Description>" << std::endl;

  // Statistics
  *output << "<Statistics>" << std::endl;
  *output << nevents_.first << " " 
          << nevents_.second << " # nevents" << std::endl;
  *output << nevents_w_.first << " " 
          << nevents_w_.second 
          << " # sum of event-weights over events" << std::endl;
  *output << nentries_.first << " " 
          << nentries_.second << " # nentries" << std::endl;
  *output << sum_w_.first << " " 
          << sum_w_.second 
          << " # sum of event-weights over events" << std::endl;
  *output << "</Statistics>" << std::endl;

  // Data
  *output << "<Data>" << std::endl;
  unsigned int i=0;
  for (const_iterator it = stack_.begin(); it!=stack_.end(); it++)
  {
    output->width(15);
    *output << std::left << it->first;
    output->width(15);
    *output << std::left << it->second.first;
    output->width(15);
    *output << std::left << it->second.second;
    if (i<2 || i>=(stack_.size()-2)) 
       *output << " # bin " << i+1 << " / " << stack_.size();
    *output << std::endl;
    i++;
  }
  *output << "</Data>" << std::endl;

  // Foot
  *output << "</HistoFrequency>" << std::endl;
  *output << std::endl;
  }

  /// Write the plot in a ROOT file
  virtual void Write_RootFormat(std::pair<TH1F*,TH1F*>& histo)
  {

    if (stack_.size()==0)
    {
    // Creating ROOT histograms
      histo.first  -> SetBins(1,0.,1.);
      histo.second -> SetBins(1,0.,1.);
      histo.first -> SetBinContent(1,0);
      histo.first->GetXaxis()->SetBinLabel(1,"666");
      histo.second -> SetBinContent(1,0);
      histo.second->GetXaxis()->SetBinLabel(1,"666");
      return;
    }

    // Creating ROOT histograms
    histo.first  -> SetBins(stack_.size(),0.,
                            static_cast<Double_t>(stack_.size()));
    histo.second -> SetBins(stack_.size(),0.,
                            static_cast<Double_t>(stack_.size()));
 
    // Layouting the histogram
    unsigned int i=0;
    for (const_iterator it=stack_.begin();it!=stack_.end();it++)
    {
      std::string tmp;
      std::stringstream str;
      str << it->first;
      str >> tmp;

      histo.first -> SetBinContent(i+1,it->second.first);
      histo.first->GetXaxis()->SetBinLabel(i+1,tmp.c_str());

      histo.second -> SetBinContent(i+1,it->second.second);
      histo.second->GetXaxis()->SetBinLabel(i+1,tmp.c_str());

      i++;
    }
  }

};

}


#endif
