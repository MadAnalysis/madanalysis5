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


#ifndef DETECTOR_MANAGER_h
#define DETECTOR_MANAGER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/DetectorBase.h"
#include "SampleAnalyzer/Process/Core/ManagerBase.h"

namespace MA5
{

class DetectorManager : public ManagerBase<DetectorBase>
{
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
  public :

   /// Constructor without argument
   DetectorManager() : ManagerBase<DetectorBase>()
   { }

   /// Destructor
   ~DetectorManager()
   { }

   /// Build the table
  void BuildTable(); 
};

}

#endif
