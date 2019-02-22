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


#ifndef CLUSTER_ALGO_BASE_H
#define CLUSTER_ALGO_BASE_H


// STL headers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <algorithm>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"


namespace MA5
{

class ClusterAlgoBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  protected :

    /// Jet clustering algorithm
    std::string JetAlgorithm_;

    /// Pt min for the jets
    MAfloat64 Ptmin_;

    /// Is the jet clustering exclusive ?
    MAbool Exclusive_;

    /// Exclusive id for tau-elec-photon-jet
    MAbool ExclusiveId_;




//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor with algorithm
    ClusterAlgoBase(std::string Algo)
    {
      JetAlgorithm_=Algo;
      // Initializing common parameters
      Ptmin_       = 0.;
      Exclusive_   = false;
      ExclusiveId_ = false;
    }

    /// Destructor
    virtual ~ClusterAlgoBase() {}

    /// Jet clustering
    virtual MAbool Execute(SampleFormat& mySample, EventFormat& myEvent, 
                         MAbool ExclusiveId, const std::vector<bool>& vetos,
                         const std::set<const MCParticleFormat*> vetos2)=0;

    /// Set parameter
    virtual MAbool SetParameter(const std::string& key, const std::string& value)=0;

    /// Initialization
    virtual MAbool Initialize()=0;

    /// Putting the string in lower case
    static std::string Lower(const std::string& word)
    {
      std::string result;
      std::transform(word.begin(), word.end(), 
                     std::back_inserter(result), 
                     (MAint32(*)(MAint32))std::tolower);
      return result;
    }

    /// GetName
    virtual std::string GetName()=0;

    /// Print parameters
    virtual void PrintParam()=0;

    /// Accessor to the jet clusterer parameters
    virtual std::string GetParameters()=0;


};

}

#endif
