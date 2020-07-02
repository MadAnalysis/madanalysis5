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


// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/RegionSelection/RegionSelectionManager.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


using namespace MA5;

/// Apply a cut
MAbool RegionSelectionManager::ApplyCut(MAbool condition, std::string const &cut)
{
  /// Skip the cut if all regions are already failing the previous cut
  if (NumberOfSurvivingRegions_==0) { return false; }

  /// Get the cut under consideration
  MultiRegionCounter *mycut=0;
  for(MAuint32 i=0; i<cutmanager_.GetNcuts(); i++)
  {
    if(cut.compare(cutmanager_.GetCuts()[i]->GetName())==0)
    {
      mycut=cutmanager_.GetCuts()[i];
      break;
    }
  }
  // Trying to apply a non-existing cut
  try
  {  
    if(mycut==0) throw EXCEPTION_WARNING("Trying to apply the non-declared cut \""+ cut + "\"","",0);
  }
  catch (const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
    return true;
  }

  // Looping over all regions the cut needs to be applied
  std::vector<RegionSelection*> RegionsForThisCut = mycut->Regions();
  for (MAuint32 i=0; i<RegionsForThisCut.size(); i++)
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
void RegionSelectionManager::FillHisto(std::string const&histname, MAfloat64 val)
{
  // Current histo
  Histo *myhisto=0;
  HistoFrequency *myhistof=0;
  HistoLogX *myhistoX=0;
  // Looping over all histos
  for(MAuint32 i=0; i<plotmanager_.GetNplots(); i++)
  {
    if(histname.compare(plotmanager_.GetHistos()[i]->GetName())==0)
    {
        // HistoFrequency
        if(dynamic_cast<HistoFrequency*>(plotmanager_.GetHistos()[i])!=0)
        {
          myhistof = dynamic_cast<HistoFrequency*>(plotmanager_.GetHistos()[i]);
          if(myhistof->AllSurviving()==0) return;
          try
          {
            if(myhistof->AllSurviving()==-1) throw
              EXCEPTION_WARNING("Filling an histogram with not all SRs surviving the cuts applied so far","",0);
          }
          catch (const std::exception& e)  { MANAGE_EXCEPTION(e); }
          // Filling the histo
          if (myhistof->FreshEvent())  myhistof->IncrementNEvents(weight_);
          myhistof->Fill(val,weight_);
       }
      // LogX histo
      else if(dynamic_cast<HistoLogX*>(plotmanager_.GetHistos()[i])!=0)
      {
        myhistoX = dynamic_cast<HistoLogX*>(plotmanager_.GetHistos()[i]);
        if(myhistoX->AllSurviving()==0) return;
        try
        {
          if(myhistoX->AllSurviving()==-1) throw
            EXCEPTION_WARNING("Filling an histogram with not all SRs surviving the cuts applied so far","",0);
        }
        catch (const std::exception& e)  { MANAGE_EXCEPTION(e); }
        // Filling the histo
        if (myhistoX->FreshEvent())  myhistoX->IncrementNEvents(weight_);
        myhistoX->Fill(val,weight_);
      }
      // Normal histo
      else if(dynamic_cast<Histo*>(plotmanager_.GetHistos()[i])!=0)
      {
        myhisto = dynamic_cast<Histo*>(plotmanager_.GetHistos()[i]);
        if(myhisto->AllSurviving()==0) return;
        try
        {
          if(myhisto->AllSurviving()==-1) throw
            EXCEPTION_WARNING("Filling an histogram with not all SRs surviving the cuts applied so far","",0);
        }
        catch (const std::exception& e)  { MANAGE_EXCEPTION(e); }
        // Filling the histo
        if (myhisto->FreshEvent())  myhisto->IncrementNEvents(weight_);
        myhisto->Fill(val,weight_);
      }
      break;
    }
  }
  // Trying to fill a non-existing histo
  try
  {
    if( (myhisto==0) && (myhistof==0) && (myhistoX==0) ) throw
       EXCEPTION_WARNING("Trying to fill non-declared histogram \""+ histname + "\"","",0);
  }
  catch (const std::exception& e)    { MANAGE_EXCEPTION(e); return; }
}



void RegionSelectionManager::WriteHistoDefinition(SAFWriter& output)
{
  *output.GetStream() << "<RegionSelection>" << std::endl;
  for(MAuint32 i=0; i<regions_.size();i++)
    regions_[i]->WriteDefinition(output);
  *output.GetStream() << "</RegionSelection>" << std::endl << std::endl;
}



void RegionSelectionManager::HeadSR(std::ostream &outwriter, const std::string &ananame)
{
  for (MAuint32 i=0;i<regions_.size();i++)
    outwriter <<  " " << ananame << "-" << regions_[i]->GetName();
}


void RegionSelectionManager::DumpSR(std::ostream &outwriter)
{
  for (MAuint32 i=0;i<regions_.size();i++)
    outwriter<< "  " << regions_[i]->IsSurviving();
}

