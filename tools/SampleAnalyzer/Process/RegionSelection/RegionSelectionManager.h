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


#ifndef __REGIONSELECTIONMANAGER_H
#define __REGIONSELECTIONMANAGER_H

// STL headers
#include <vector>
#include <string>
#include <sstream>

// SampleAnalyzer
#include "SampleAnalyzer/Process/Counter/MultiRegionCounterManager.h"
#include "SampleAnalyzer/Process/Plot/PlotManager.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelection.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"

namespace MA5
{

class RegionSelectionManager
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /// Collection of Region selections
  std::vector<RegionSelection*> regions_;

  /// Collection of plots that will be generated during the analysis
  PlotManager plotmanager_;

  /// Collection of cuts that will be applied to the analysis
  MultiRegionCounterManager cutmanager_;

  /// Index related to the number of surviving regions in an analysis
  unsigned int NumberOfSurvivingRegions_;

  /// Weight associated with the processed event
  double weight_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public:
  /// constructor
  RegionSelectionManager() {};

  /// Destructor
  ~RegionSelectionManager() { };

  /// Reset
  void Reset()
  {
    for (unsigned int i=0;i<regions_.size();i++)
      { if (regions_[i]!=0) delete regions_[i]; }
    regions_.clear();
    cutmanager_.Finalize();
    plotmanager_.Finalize();
  }

  /// Finalizing
  void Finalize() { Reset(); }

  /// Get methods
  std::vector<RegionSelection*> Regions()
    { return regions_; }

  MultiRegionCounterManager* GetCutManager()
    { return &cutmanager_; }

  PlotManager* GetPlotManager()
    { return &plotmanager_; }

  double GetCurrentEventWeight()
    { return weight_; }

  /// Set method
  void SetCurrentEventWeight(double weight)
    { weight_ = weight; }

  /// Adding a RegionSelection to the manager
  void AddRegionSelection(const std::string& name)
  {
    std::string myname=name;
    if(myname.compare("")==0)
    {
      std::stringstream numstream;
      numstream << regions_.size();
      myname = "RegionSelection" + numstream.str();
    }
    RegionSelection* myregion = new RegionSelection(name);
    regions_.push_back(myregion);
  }

  /// Getting ready for a new event
  void InitializeForNewEvent(double EventWeight)
  {
    weight_ = EventWeight;
    NumberOfSurvivingRegions_ = regions_.size();
    for (unsigned int i=0; i<regions_.size(); i++ )
      regions_[i]->InitializeForNewEvent(EventWeight);
  }

  /// This method associates all regions with a cut
  void AddCut(const std::string&name)
  {
    // The name of the cut
    std::string myname=name;
    if(myname.compare("")==0)
    {
      std::stringstream numstream;
      numstream << cutmanager_.GetCuts().size();
      myname = "Cut" + numstream.str();
    }
    // Adding the cut to all the regions
    cutmanager_.AddCut(myname,regions_);
  }


  /// This method associates one single region with a cut
  void AddCut(const std::string&name, const std::string &RSname)
  {
    std::string RSnameA[] = {RSname};
    AddCut(name, RSnameA);
  }



  /// this method associates an arbitrary number of RS with a cut
  template <int NRS> void AddCut(const std::string&name, std::string const(&RSnames)[NRS])
  {
    // The name of the cut
    std::string myname=name;
    if(myname.compare("")==0)
    {
      std::stringstream numstream;
      numstream << cutmanager_.GetCuts().size();
      myname = "Cut" + numstream.str();
    }

    // Creating the vector of SR of interests
    std::vector<RegionSelection*> myregions;
    for(unsigned int i=0; i<NRS; i++)
    {
      for(unsigned int j=0; j<regions_.size(); j++)
      {
        if(regions_[j]->GetName().compare(RSnames[i])==0)
        {
          myregions.push_back(regions_[j]);
          break;
        }
      }
      if(myregions.size()==i)
        WARNING << "Assigning the cut \"" << name
                << "\" to the non-existing signal region \"" << RSnames[i]
                << "\"" << endmsg;
    }

    // Creating the cut
    cutmanager_.AddCut(myname,myregions);
  }

  /// Apply a cut
  bool ApplyCut(bool, std::string const&);

  /// This method associates all signal regions with an histo
  void AddHisto(const std::string&name,unsigned int nb,double xmin,double xmax)
  {
    // The name of the histo
    std::string myname=name;
    if(myname.compare("")==0)
    {
      std::stringstream numstream;
      numstream << plotmanager_.GetHistos().size();
      myname = "Histo" + numstream.str();
    }
    // Adding the histo and linking all regions to the histo
    plotmanager_.Add_Histo(myname,nb,xmin,xmax,regions_);
  }

  /// This method associates one single signal region with an histo
  void AddHisto(const std::string&name,unsigned int nb,double xmin,double xmax,
    const std::string &RSname)
  {
    std::string RSnameA[] = {RSname};
    AddHisto(name, nb, xmin, xmax, RSnameA);
  }

  /// this method associates an arbitrary number of RS with an histo
  template <int NRS> void AddHisto(const std::string&name, unsigned int nb,
    double xmin,double xmax, std::string const(&RSnames)[NRS])
  {
    // The name of the histo
    std::string myname=name;
    if(myname.compare("")==0)
    {
      std::stringstream numstream;
      numstream << plotmanager_.GetNplots();
      myname = "Histo" + numstream.str();
    }
     // Creating the vector of SR of interests
    std::vector<RegionSelection*> myregions;
    for(unsigned int i=0; i<NRS; i++)
    {
      for(unsigned int j=0; j<regions_.size(); j++)
      {
        if(regions_[j]->GetName().compare(RSnames[i])==0)
        {
          myregions.push_back(regions_[j]);
          break;
        }
      }
      if(myregions.size()==i)
        WARNING << "Assigning the histo \"" << name
                << "\" to the non-existing signal region \"" << RSnames[i]
                << "\"" << endmsg;
    }

    // Creating the histo
    plotmanager_.Add_Histo(myname, nb, xmin, xmax,myregions);
  }

  /// Filling an histo with a value val
  void FillHisto(std::string const&, double val);

  /// Writing the definition saf file
  void WriteHistoDefinition(SAFWriter& output);

  /// Checking if a given RS is surviging
  bool IsSurviving(const std::string &RSname)
  {
    // Looking for the region and checking its status
    for(unsigned int i=0; i<regions_.size(); i++)
    {
      if(regions_[i]->GetName().compare(RSname) == 0)
        return regions_[i]->IsSurviving();
    }

    // The region has not been found
    WARNING << "Checking whether the non-declared region \""
            << RSname << "\" is surviving the applied cuts." << endmsg;
    return false;
  }

};

}
#endif
