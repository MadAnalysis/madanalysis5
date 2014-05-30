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
    virtual bool Execute(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Initialization
    virtual bool Initialize(const std::map<std::string,std::string>& options)=0;

    /// Putting the string in lower case
    static std::string Lower(const std::string& word)
    {
      std::string result;
      std::transform(word.begin(), word.end(), 
                     std::back_inserter(result), 
                     (int (*)(int))std::tolower);
      return result;
    }

    /// 
    void SettingsCommonPart(const std::string& key, const std::string& value)
    {
      // exclusive_id
      if (key=="exclusive_id")
      {
        int tmp=0;
        std::stringstream str;
        str << value;
        str >> tmp;
        if (tmp==1) ExclusiveId_=true;
        else if (tmp==0) ExclusiveId_=false;
        else
        {
          WARNING << "'exclusive_id' must be equal to 0 or 1. "
                  << "Using default value 'exclusive_id' = " 
                  << ExclusiveId_ << endmsg;
        }
      }

      // Other
      else WARNING << "Parameter " << key	<< " unknown." << endmsg;
    }

};

}

#endif
