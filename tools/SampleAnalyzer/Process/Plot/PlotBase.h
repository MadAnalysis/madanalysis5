////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


#ifndef PLOT_BASE_H
#define PLOT_BASE_H


// STL headers
#include <iostream>
#include <map>
#include <string>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Process/Writer/DatabaseManager.h"


struct MultiweightEvents {
	/// Number of events
  std::pair<MAint64,MAint64> nevents_;

  /// Number of entries
  std::pair<MAint64,MAint64> nentries_;

  /// Sum of event-weight over events
  std::pair<MAfloat64,MAfloat64> nevents_w_;

  MAbool fresh_event_;

};


namespace MA5
{

class PlotBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  /// Name of the plots
  std::string name_;

  /// Number of events
  std::pair<MAint64,MAint64> nevents_;

  /// Number of entries
  std::pair<MAint64,MAint64> nentries_;

  /// Sum of event-weight over events
  std::pair<MAfloat64,MAfloat64> nevents_w_;

  /// Flag telling whether a given histo has already been modified for an event
  MAbool fresh_event_;

  //map containing event meta data for each weight id
  std::map<MAuint32, MultiweightEvents*> multiweight_event_info; 


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  PlotBase()
  {
    // Reseting statistical counters
    nevents_     = std::make_pair(0,0);
    nentries_    = std::make_pair(0,0);
    nevents_w_   = std::make_pair(0,0);
    fresh_event_ = true;
	
  }

  /// Constructor with argument 
  PlotBase(const std::string& name)
  {
    name_        = name;
    nevents_     = std::make_pair(0,0);
    nevents_w_   = std::make_pair(0,0);
    nentries_    = std::make_pair(0,0);
    fresh_event_ = true;
	
  }

  /// Destructor
  virtual ~PlotBase()
  { 
	  for(auto &p : multiweight_event_info){
		  delete p.second;
	  }
  }

  /// Accesor for fresh_event
  MAbool FreshEvent() { return fresh_event_;}

  /// Accessor for multiweight fresh_event
  MAbool MultiweightFreshEvent(int index) {
	  return multiweight_event_info[index]->fresh_event_;
  }

  /// Modifier for fresh_event
  void SetFreshEvent(MAbool tag) { 
	  fresh_event_ = tag;
  }

  /// Modifier for Multiweight fresh_event
  void SetMultiweightFreshEvent(MAbool tag) {
	  for(auto &weight_id : multiweight_event_info){
		  weight_id.second->fresh_event_ = tag;
	  }
  }

  /// Write the plot in a ROOT file
  virtual void Write_TextFormat(std::ostream* output) = 0;

  virtual void WriteSQL(DatabaseManager &db) {};

  /// Increment number of events
  void IncrementNEvents(MAfloat64 weight=1.0)
  {
    if (weight>=0) 
    {
      nevents_.first++;
      nevents_w_.first+=weight;
    }
    else
    {
      weight = std::abs(weight);
      nevents_.second++;
      nevents_w_.second+=weight;
    }
    SetFreshEvent(false);
  }

  void IncrementNEvents(std::map<MAuint32, MAfloat64> &weights){
	  for(const auto &id_weight : weights){
		  if(multiweight_event_info.find(id_weight.first) == multiweight_event_info.end()){
			  multiweight_event_info[id_weight.first] = new MultiweightEvents();
			  multiweight_event_info[id_weight.first]->fresh_event_ = true;	
		  }

		  if(MultiweightFreshEvent(id_weight.first)){
			if(id_weight.second >= 0){
				multiweight_event_info[id_weight.first]->nevents_.first++;
				multiweight_event_info[id_weight.first]->nevents_w_.first+=id_weight.second;		
			} else {
				multiweight_event_info[id_weight.first]->nevents_.second++;
				multiweight_event_info[id_weight.first]->nevents_w_.second+=std::abs(id_weight.second);
			}

		  }
		  multiweight_event_info[id_weight.first]->fresh_event_ = false;
	  }
  }

  /// Return Number of events
  const std::pair<MAint64,MAint64>& GetNEvents()
  { return nevents_; }

  const std::pair<MAint64, MAint64>& GetNEvents(int index){
	return multiweight_event_info[index]->nevents_;
  }

  // Return the name
  std::string GetName()
    { return name_; }

};

}

#endif
