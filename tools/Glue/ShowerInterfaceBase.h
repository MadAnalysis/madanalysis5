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


#ifndef ShowerInterfaceBase_h
#define ShowerInterfaceBase_h

// STL headers
#include <iostream>
#include <string>
#include <map>

// SampleAnalyzer headers
#include "SampleAnalyzer/DataFormat/EventFormat.h"
#include "SampleAnalyzer/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Core/StatusCode.h"


namespace MA5
{

class ShowerInterfaceBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected :

  /// Name identifying the shower 
  std::string name_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /// Constructor without arguments
  ShowerInterfaceBase()
  {}

  /// Destructor
  virtual ~ShowerInterfaceBase()
  {}

  /// Initializing procedure (virtual pure)
  virtual bool Initialize(const std::map<std::string,std::string>& config) = 0;
  
  /// Finalizing procedure (virtual pure)
  virtual bool Finalize() = 0;

  /// Executing procedure 
  /// arguments in read/write access
  virtual StatusCode::Type Execute(SampleFormat& mysample, 
                                   EventFormat & myevent) = 0;

  /// Accessor to name
  const std::string& GetName() const 
  { return name_; }

  /// Accessor to name
  void SetName(const std::string& name)
  { name_=name; }


  // -------------------------------------------------------------
  //                  protected method members
  // -------------------------------------------------------------
 protected :

  /// Preventing default copy constructor
  ShowerInterfaceBase(const ShowerInterfaceBase&);

  /// Preventing assignment
  ShowerInterfaceBase& operator=(const ShowerInterfaceBase&);


};

}

#endif
