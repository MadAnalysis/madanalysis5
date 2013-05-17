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


#ifndef Herwig6Interface_h
#define Herwig6Interface_h

// STL headers
#include <iostream>
#include <string>

// Glue headers
#include "Glue/FortranShowerInterfaceBase.h"


namespace MA5
{

class Herwig6Interface : public FortranShowerInterfaceBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  /// To declare : event data in array format


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor without arguments
  Herwig6Interface()
  { name_="Herwig6"; }

  /// Destructor
  virtual ~Herwig6Interface()
  {}

  /// Initializing procedure (virtual pure)
  virtual bool Initialize(const std::map<std::string,std::string>& config);
  
  /// Finalizing procedure (virtual pure)
  virtual bool Finalize();

  /// Executing procedure 
  /// arguments in read/write access
  virtual StatusCode::Type Execute(SampleFormat& mysample, 
                                   EventFormat & myevent);


  // -------------------------------------------------------------
  //                  protected method members
  // -------------------------------------------------------------
 protected :

  /// Preventing default copy constructor
  Herwig6Interface(const Herwig6Interface&);

  /// Preventing assignment
  Herwig6Interface& operator=(const Herwig6Interface&);


};

}

#endif
