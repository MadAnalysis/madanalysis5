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


#ifndef FortranShowerInterfaceBase_h
#define FortranShowerInterfaceBase_h

// STL headers
#include <iostream>
#include <string>

// Glue headers
#include "Glue/ShowerInterfaceBase.h"


namespace MA5
{

class FortranShowerInterfaceBase : public ShowerInterfaceBase
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
  FortranShowerInterfaceBase()
  {}

  /// Destructor
  virtual ~FortranShowerInterfaceBase()
  {}

  /// Initializing procedure (virtual pure)
  virtual bool Initialize(const std::map<std::string,std::string>& config);
  
  /// Finalizing procedure (virtual pure)
  virtual bool Finalize();

  /// Executing procedure 
  /// arguments in read/write access
  virtual StatusCode::Type Execute(SampleFormat& mysample, 
                                   EventFormat & myevent) = 0;


  // -------------------------------------------------------------
  //                  protected method members
  // -------------------------------------------------------------
 protected :

  /// Preventing default copy constructor
  FortranShowerInterfaceBase(const FortranShowerInterfaceBase&);

  /// Preventing assignment
  FortranShowerInterfaceBase& operator=(const FortranShowerInterfaceBase&);


};

}

#endif
