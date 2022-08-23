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


#ifndef COUNTER_h
#define COUNTER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// STL headers
#include <iostream>
#include <string>
#include <map>
#include <fstream>

struct multiWeightEntry {
	  std::pair<MAint64, MAint64> nentries_;
	  std::pair<MAfloat64, MAfloat64> sumweight_;
	  std::pair<MAfloat64, MAfloat64> sumweight2_;
	  multiWeightEntry(){
		nentries_.first = 0;
		nentries_.second = 0;
		sumweight_.first = 0.;
		sumweight_.second = 0.;
		sumweight2_.first = 0.;
		sumweight2_.second = 0.;
	  }
};


namespace MA5
{

class CounterCollection;

class Counter
{
  friend class CounterCollection;

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public :



  /// name of the analysis
  std::string name_;

  /// number of times the function Increment is called
  /// first = positive weight ; second = negative weight
  std::pair<MAint64,MAint64> nentries_;

  /// sum of weights
  /// first = positive weight ; second = negative weight
  std::pair<MAfloat64,MAfloat64> sumweight_;

  /// sum of squared weights
  /// first = positive weight ; second = negative weight
  std::pair<MAfloat64,MAfloat64> sumweight2_;

  std::map<MAuint32, multiWeightEntry*> multiweight_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  Counter(const std::string& name = "unknown")
  { 
    name_       = name;
    nentries_   = std::make_pair(0,0); 
    sumweight_  = std::make_pair(0.,0.);
    sumweight2_ = std::make_pair(0.,0.);
  }

  /// Destructor
  ~Counter()
  { 
	  for(auto &p : multiweight_){
		  delete p.second;
	  } 
  }

  /// Reset
  void Reset()
  {
    nentries_   = std::make_pair(0,0); 
    sumweight_  = std::make_pair(0.,0.);
    sumweight2_ = std::make_pair(0.,0.);
	for(auto &p : multiweight_){
		delete p.second;
	}
	multiweight_.clear();
  }

  void Debug(int debugCount, int weightprocessed){
		for(auto &p : multiweight_){
			std::ofstream output;
			output.open("/Users/kfan/desktop/output.txt", std::fstream::app);
			output << "ID : " << p.first << " increment multiweight call number : " << debugCount << " weight Processed :"<< weightprocessed;
			output << "\n";
			output << "pos entries : " <<  p.second->nentries_.first << " --  " << nentries_.first << " neg entries : " << p.second->nentries_.second <<   " -- "  << nentries_.second << "\n";
			output << "pos sum : " << p.second->sumweight_.first <<  " -- " << sumweight_.first << " neg sum : " << p.second->sumweight_.second <<  " -- " << sumweight_.second << "\n";
			output << "pos squared : " <<p.second->sumweight2_.first <<  " -- " << sumweight2_.first << " neg squared : " << p.second->sumweight2_.second <<  " -- " << sumweight2_.second << "\n";
			output << "----------------------------------------------------------";
			output << "\n";
			output.close();

	  }

  }


  void Increment(const std::map<MAuint32, MAfloat64> &multiweights){
	//	static int incrementDebugCount = 0;
		for(const auto &weight : multiweights){
			if(multiweight_.find(weight.first) == multiweight_.end()){
				multiweight_[weight.first] = new multiWeightEntry();
			}
			if(weight.second > 0){
				multiweight_[weight.first]->nentries_.first++;
				multiweight_[weight.first]->sumweight_.first+=weight.second;
				multiweight_[weight.first]->sumweight2_.first+=weight.second*weight.second;
			}
			else if (weight.second < 0){
				multiweight_[weight.first]->nentries_.second++;
				multiweight_[weight.first]->sumweight_.second+=weight.second;
				multiweight_[weight.first]->sumweight2_.second+=weight.second*weight.second;
			}
	  }
	
	  //Debug testing
	//  Debug(incrementDebugCount, multiweights.begin()->second);	
	//  incrementDebugCount++;	

  }


  /// Increment the counter
  void Increment(const MAfloat32& weight)
  {
    if (weight>0)
    {
      nentries_.first++;
      sumweight_.first+=weight;
      sumweight2_.first+=weight*weight;
    }
    else if (weight<0)
    {
      nentries_.second++;
      sumweight_.second+=weight;
      sumweight2_.second+=weight*weight;
    }

  }

  

  


};

}

#endif
