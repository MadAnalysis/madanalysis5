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


#ifndef RANDOM_SERVICE_H
#define RANDOM_SERVICE_H


// STL headers 
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


// ShortCut to access to RandomService
#define RANDOM MA5::RandomService::GetInstance()   


namespace MA5
{

//////////////////////////////////////////////////////////////////////////////
/// The class RandomService contains static methods used for producing random
/// numbers according to a given pdf.
///
/// RandomService is a singleton-pattern-based class : only one instance.
/// Getting the only one instance : RandomService::GetInstance()
//////////////////////////////////////////////////////////////////////////////
class RandomService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /// Pointer to the unique instance of RandomService
  static RandomService* Service_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /// Constructor without argument
  RandomService() 
  {
    // select a random seed according to the time
    std::srand(time(0));
  }

  /// Destructor
  ~RandomService()
  {}

  /// (Re)initialzing the streamer
  void Initialize()
  { }

 public:

  /// Getting the unique instance of RandomService
  static RandomService* GetInstance()
  {
    if (Service_==0) Service_ = new RandomService;
    return Service_;
  }

  /// Deleting the unique instance of Convert Service
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }

  /// Random a number between 0 and 1 according to a flat pdf
  MAdouble64 flat() const
  {
    return static_cast<MAdouble64>(std::rand()) /
           static_cast<MAdouble64>(RAND_MAX); 
  }

};

}

#endif
