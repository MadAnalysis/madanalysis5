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


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/JetClustering/JetClusterer.h"

using namespace MA5;

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
bool JetClusterer::Initialize(const std::map<std::string,std::string>& options)
{
  // algo defined ?
  if (algo_==0) return false;

  // Loop over options
  for (std::map<std::string,std::string>::const_iterator
       it=options.begin();it!=options.end();it++)
  {
    std::string key = ClusterAlgoBase::Lower(it->first);
    bool result=false;

    // exclusive_id
    if (key=="exclusive_id")
    {
      int tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp==1) ExclusiveId_=true;
      else if (tmp==0) ExclusiveId_=false;
      else
      {
        WARNING << "'exclusive_id' must be equal to 0 or 1. "
                << "Using default value 'exclusive_id' = " 
                << ExclusiveId_ << endmsg;
      }
      result=true;
    }

    // b-tagging
    else if (key.find("bjet_id.")==0)
    {
      result=myBtagger_->SetParameter(key.substr(8),it->second,"bjet_id.");
    }

    // c-tagging
    //    else if (key.find("cjet_id.")==0)
    //    {
    //      result=myCtagger_->SetParameter(key.substr(8),it->second,"cjet_id.");
    //    }

    // tau-tagging
    else if (key.find("tau_id.")==0)
    {
      result=myTautagger_->SetParameter(key.substr(7),it->second,"tau_id.");
    }

    // clustering algo
    else if (key.find("cluster.")==0)
    {
      result=algo_->SetParameter(key.substr(8),it->second);
    }
   
    // Other
    if (!result) WARNING << "Parameter " << key << " unknown. It will be skipped." << endmsg;

  }

  // configure algo
  algo_->Initialize();

  // configure tagger
  myBtagger_   = new bTagger();
  myCtagger_   = new cTagger();
  myTautagger_ = new TauTagger();

  return true;
}


// -----------------------------------------------------------------------------
// Finalize
// -----------------------------------------------------------------------------
void JetClusterer::Finalize()
{
  if (algo_!=0)        delete algo_;
  if (myBtagger_!=0)   delete myBtagger_;
  if (myCtagger_!=0)   delete myCtagger_;
  if (myTautagger_!=0) delete myTautagger_;
}
