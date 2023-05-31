////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef JET_CLUSTERING_MANAGER_h
#define JET_CLUSTERING_MANAGER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/JetClustering/JetClusterer.h"
#include "SampleAnalyzer/Process/Core/ManagerBase.h"


namespace MA5
{

class JetClustererManager : public ManagerBase<JetClusterer>
{
  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
  public :

   /// Constructor without argument
   JetClustererManager() : ManagerBase<JetClusterer>()
   { }

   /// Destructor
   ~JetClustererManager()
   { }

   /// Build the table
  void BuildTable(); 

  /// Print the list of items in the collection
  void Print(LogStream& os=INFO) const
  { ManagerBase<JetClusterer>::Print(Objects_, Names_, os); }

};

}

#endif
