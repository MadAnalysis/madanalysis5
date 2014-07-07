////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2014 Eric Conte, Benjamin Fuks, Chris Wymant
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

// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/RegionSelection/RegionSelectionManager.h"

using namespace MA5;

/// Apply a cut
bool RegionSelectionManager::ApplyCut(bool condition, std::string const &cut)
{
  /// Skip the cut if all regions are already failing the previous cut
  if (NumberOfSurvivingRegions_==0) { return false; }

  /// Get the cut under consideration
  MultiRegionCounter *mycut=0;
  for(unsigned int i=0; i<cutmanager_.GetNcuts(); i++)
  {
    if(cut.compare(cutmanager_.GetCuts()[i]->GetName())==0)
    {
      mycut=cutmanager_.GetCuts()[i];
      break;
    }
  }
  // Trying to apply a non-existing cut
  if(mycut==0)
  {
    WARNING << "Trying to apply the non-declared cut \""
            << cut << "\"" << endmsg;
    return true;
  }

  // Looping over all regions the cut needs to be applied
  std::vector<RegionSelection*> RegionsForThisCut = mycut->Regions();
  for (unsigned int i=0; i<RegionsForThisCut.size(); i++)
  {
    RegionSelection* ThisRegion = RegionsForThisCut[i];

    /// Skip the current region if it has failed a previous cut
    if(!ThisRegion->IsSurviving() ) { continue; }

    /// Check the current cut:
    if(condition) { ThisRegion->IncrementCutFlow(weight_); }
    else
    {
      ThisRegion->SetSurvivingTest(false);
      NumberOfSurvivingRegions_--;
      if (NumberOfSurvivingRegions_==0) {return false;}
    }
  }

  /// If we're here, we've looped through all RegionsForThisCut and
  /// NumberOfSurvivingRegions is still greater than zero, so return true.
  return true;
}

/// Filling an histo with a value val
void RegionSelectionManager::FillHisto(std::string const&histname, double val)
{
  /// Get the histo under consideration
  Histo *myhisto=0;
  for(unsigned int i=0; i<plotmanager_.GetNplots(); i++)
  {
    if(histname.compare(plotmanager_.GetHistos()[i]->GetName())==0)
    {
      myhisto=dynamic_cast<Histo*>(plotmanager_.GetHistos()[i]);
      break;
    }
  }
  // Trying to fill a non-existing histo
  if(myhisto==0)
  {
    WARNING << "Trying to fill a non-declared histogram \""
            << histname << "\"" << endmsg;
    return;
  }

  // Checking if each region is surviving
  if(myhisto->AllSurviving()==0) return;
  if(myhisto->AllSurviving()==-1)
  {
    WARNING << "Trying to fill an histogram for which at least one (but"
     << " not all) SRs is not surviving the cuts applied so far."
     << endmsg;
    WARNING << "Please modify the analysis and declare different histograms"
      << endmsg;
  }

  // Filling the histo
  myhisto->IncrementNEvents();
  myhisto->Fill(val,weight_);
}

void RegionSelectionManager::WriteHistoDefinition(SAFWriter& output)
{
  *output.GetStream() << "<RegionSelection>" << std::endl;
  for(unsigned int i=0; i<regions_.size();i++)
    regions_[i]->WriteDefinition(output);
  *output.GetStream() << "</RegionSelection>" << std::endl << std::endl;
}


