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


#ifndef COUNTER_h
#define COUNTER_h

// STL headers
#include <iostream>
#include <string>
#include <map>


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
  std::pair<Long64_t,Long64_t> nentries_;

  /// sum of weights
  /// first = positive weight ; second = negative weight
  std::pair<Double_t,Double_t> sumweight_;

  /// sum of squared weights
  /// first = positive weight ; second = negative weight
  std::pair<Double_t,Double_t> sumweight2_;


  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  Counter(const std::string& name = "unkwown")
  { 
    name_       = name;
    nentries_   = std::make_pair(0,0); 
    sumweight_  = std::make_pair(0.,0.);
    sumweight2_ = std::make_pair(0.,0.);
  }

  /// Destructor
  ~Counter()
  { }

  /// Reset
  void Reset()
  {
    nentries_   = std::make_pair(0,0); 
    sumweight_  = std::make_pair(0.,0.);
    sumweight2_ = std::make_pair(0.,0.);
  }

  /// Increment the counter
  void Increment(const Float_t& weight=1.)
  {
    if (weight>=0)
    {
      nentries_.first++;
      sumweight_.first+=weight;
      sumweight2_.first+=weight*weight;
    }
    else
    {
      nentries_.second++;
      sumweight_.second+=weight;
      sumweight2_.second+=weight*weight;
    }
  }

};

}

#endif
