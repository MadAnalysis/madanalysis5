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


#ifndef CLUSTER_ALGO_BASE_H
#define CLUSTER_ALGO_BASE_H


//SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"

//STL headers
#include <vector>
#include <map>
#include <string>
#include <set>

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
    Double_t Ptmin_;

    /// Is the jet clustering exclusive ?
    Bool_t Exclusive_;

    /// Exclusive id for tau-elec-photon-jet
    Bool_t ExclusiveId_;




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
    virtual bool Execute(SampleFormat& mySample, EventFormat& myEvent, 
                         bool ExclusiveId, const std::vector<bool>& vetos,
                         const std::set<const MCParticleFormat*> vetos2)=0;

    /// Set parameter
    virtual bool SetParameter(const std::string& key, const std::string& value)=0;

    /// Initialization
    virtual bool Initialize()=0;

    /// Putting the string in lower case
    static std::string Lower(const std::string& word)
    {
      std::string result;
      std::transform(word.begin(), word.end(), 
                     std::back_inserter(result), 
                     (int (*)(int))std::tolower);
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
