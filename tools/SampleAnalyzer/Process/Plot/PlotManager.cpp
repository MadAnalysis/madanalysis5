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
#include "SampleAnalyzer/Process/Plot/PlotManager.h"

// STL headers
#include <map>
#include <sstream>

// Root headers
#include <TH1F.h>
#include <TClonesArray.h>
#include <TVector.h>

using namespace MA5;

/// Write the counters in a Text file
void PlotManager::Write_TextFormat(SAFWriter& output)
{
  for (unsigned int i=0;i<plots_.size();i++)
    plots_[i]->Write_TextFormat(output.GetStream());
}


/// Write the counters in a ROOT file
void PlotManager::Write_RootFormat(TFile* output)
{
  // Initializing histogram array
  TClonesArray * plots_array_positive_weight_=new TClonesArray("TH1F",plots_.size());
  TClonesArray * plots_array_negative_weight_=new TClonesArray("TH1F",plots_.size());
  for (unsigned int i=0;i<plots_.size();i++)
  {
    std::stringstream str;
    str << "selection_positiveweights_" << i;
    new ((*plots_array_positive_weight_)[i]) TH1F(str.str().c_str(),"",100,0.0,1000.0);
    std::stringstream str2;
    str2 << "selection_negativeweights_" << i;
    new ((*plots_array_negative_weight_)[i]) TH1F(str2.str().c_str(),"",100,0.0,1000.0);
    std::pair<TH1F*,TH1F*> mypair = std::make_pair ( dynamic_cast<TH1F*>((*plots_array_positive_weight_)[i]),
                                                     dynamic_cast<TH1F*>((*plots_array_negative_weight_)[i])  );
    plots_[i]->Write_RootFormat(mypair);
  }

  // Filling number of events
  TVector plots2save_nevents_positive_weight_(plots_.size());
  TVector plots2save_nevents_negative_weight_(plots_.size());
  for (unsigned int i=0;i<plots_.size();i++)
  {
    plots2save_nevents_positive_weight_[i]=plots_[i]->GetNEvents().first;
    plots2save_nevents_negative_weight_[i]=plots_[i]->GetNEvents().second;
  }

  // Write
  plots_array_positive_weight_->Write("plots_array_positive_weight",TObject::kSingleKey);
  plots_array_negative_weight_->Write("plots_array_negative_weight",TObject::kSingleKey);
  plots2save_nevents_positive_weight_.Write("nevents_positive_weight");
  plots2save_nevents_negative_weight_.Write("nevents_negative_weight");

}
