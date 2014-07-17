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


#ifndef JET_CLUSTERING_FASTJET_H
#define JET_CLUSTERING_FASTJET_H


//SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Base/ClusterAlgoBase.h"

//STL header
#include <set>
#include <map>
#include <vector>
#include <string>


namespace fastjet
{
  class JetDefinition;
}


namespace MA5
{

class ClusterAlgoFastJet: public ClusterAlgoBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  protected :

    /// Jet definition
    fastjet::JetDefinition* JetDefinition_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor with algorithm
    ClusterAlgoFastJet(std::string algo);

    /// Destructor
    virtual ~ClusterAlgoFastJet(); 

    /// Jet clustering
    virtual bool Execute(SampleFormat& mySample, EventFormat& myEvent, bool ExclusiveId,   
                         const std::vector<bool>& vetos,
                         const std::set<const MCParticleFormat*> vetos2);

    /// Initialization
    virtual bool Initialize()=0;
 
};

}

#endif
