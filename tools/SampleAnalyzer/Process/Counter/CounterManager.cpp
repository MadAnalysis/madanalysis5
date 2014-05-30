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


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Counter/CounterManager.h"

// ROOT headers
#include <TVector.h>


using namespace MA5;

/// Write the counters in a ROOT file
void CounterManager::Write_RootFormat(TFile* output) const
{
  // Creating ROOT containers
  TVector nentries_pos   (counters_.size());
  TVector sumweight_pos  (counters_.size());
  TVector sumweight2_pos (counters_.size());
  TVector nentries_neg   (counters_.size());
  TVector sumweight_neg  (counters_.size());
  TVector sumweight2_neg (counters_.size());
  TVector initial_pos    (1);
  TVector initial_neg    (1);
  TVector initial2_pos    (1);
  TVector initial2_neg    (1);

  // Filling ROOT containers with info
  for (unsigned int i=0; i<counters_.size();i++)
  {
    nentries_pos[i]   = counters_[i].nentries_.first;
    sumweight_pos[i]  = counters_[i].sumweight_.first;
    sumweight2_pos[i] = counters_[i].sumweight2_.first;
    nentries_neg[i]   = counters_[i].nentries_.second;
    sumweight_neg[i]  = counters_[i].sumweight_.second;
    sumweight2_neg[i] = counters_[i].sumweight2_.second;
  }
  initial_pos[0]  = initial_.sumweight_.first;
  initial_neg[0]  = initial_.sumweight_.second;
  initial2_pos[0] = initial_.sumweight2_.first;
  initial2_neg[0] = initial_.sumweight2_.second;

  // Saving info
  initial_pos.Write("initial_sumw_positive_weight");
  initial_neg.Write("initial_sumw_negative_weight");
  initial2_pos.Write("initial_sumw2_positive_weight");
  initial2_neg.Write("initial_sumw2_negative_weight");
  nentries_pos.Write("nentries_pos");
  sumweight_pos.Write("accepted_sumw_positive_weight");
  sumweight2_pos.Write("accepted_sumw2_positive_weight");
  nentries_neg.Write("nentries_neg");
  sumweight_neg.Write("accepted_sumw_negative_weight");
  sumweight2_neg.Write("accepted_sumw2_negative_weight");
}


/// Write the counters in a TEXT file
void CounterManager::Write_TextFormat(SAFWriter& output) const
{
  // header
  *output.GetStream() << "<InitialCounter>" << std::endl;

  // name
  *output.GetStream() << "\"Initial number of events\"      #" << std::endl;

  // nentries
  output.GetStream()->width(15);
  *output.GetStream() << std::left << std::scientific << initial_.nentries_.first;
  *output.GetStream() << " ";
  output.GetStream()->width(15);
  *output.GetStream() << std::left << std::scientific << initial_.nentries_.second;
  *output.GetStream() << " # nentries" << std::endl;

  // sum of weights
  output.GetStream()->width(15);
  *output.GetStream() << std::left << std::scientific << initial_.sumweight_.first;
  *output.GetStream() << " ";
  output.GetStream()->width(15);
  *output.GetStream() << std::left << std::scientific << initial_.sumweight_.second;
  *output.GetStream() << " # sum of weights" << std::endl;

  // sum of weights^2
  output.GetStream()->width(15);
  *output.GetStream() << std::left << std::scientific << initial_.sumweight2_.first;
  *output.GetStream() << " ";
  output.GetStream()->width(15);
  *output.GetStream() << std::left << std::scientific << initial_.sumweight2_.second;
  *output.GetStream() << " # sum of weights^2" << std::endl;

  // foot
  *output.GetStream() << "</InitialCounter>" << std::endl;
  *output.GetStream() << std::endl;



  // Loop over the counters
  for (unsigned int i=0;i<counters_.size();i++)
  {
    // header
    *output.GetStream() << "<Counter>" << std::endl;

    // name
    int nsp = 30-counters_[i].name_.size();
    if(nsp<0) nsp=0;
    *output.GetStream() << "\"" << counters_[i].name_  << "\"";
    for (unsigned int jj=0; jj<static_cast<unsigned int>(nsp);jj++) *output.GetStream() << " ";
    *output.GetStream() << "# " << i+1 <<"st cut" << std::endl;

    // nentries
    output.GetStream()->width(15);
    *output.GetStream() << std::left << std::scientific << counters_[i].nentries_.first;
    *output.GetStream() << " ";
    output.GetStream()->width(15);
    *output.GetStream() << std::left << std::scientific << counters_[i].nentries_.second;
    *output.GetStream() << " # nentries" << std::endl;

    // sum of weights
    output.GetStream()->width(15);
    *output.GetStream() << std::left << std::scientific << counters_[i].sumweight_.first;
    *output.GetStream() << " ";
    output.GetStream()->width(15);
    *output.GetStream() << std::left << std::scientific << counters_[i].sumweight_.second;
    *output.GetStream() << " # sum of weights" << std::endl;

    // sum of weights^2
    output.GetStream()->width(15);
    *output.GetStream() << std::left << std::scientific << counters_[i].sumweight2_.first;
    *output.GetStream() << " ";
    output.GetStream()->width(15);
    *output.GetStream() << std::left << std::scientific << counters_[i].sumweight2_.second;
    *output.GetStream() << " # sum of weights^2" << std::endl;

    // foot
    *output.GetStream() << "</Counter>" << std::endl;
    *output.GetStream() << std::endl;
  }
}
