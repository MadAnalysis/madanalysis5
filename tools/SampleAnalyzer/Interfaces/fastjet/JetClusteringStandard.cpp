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


#include "SampleAnalyzer/Interfaces/fastjet/JetClusteringStandard.h"
#ifdef FASTJET_USE

using namespace MA5;

bool JetClusteringStandard::Initialize(const std::map<std::string,std::string>& options)
{ 

  TaggerInitialize();

  for (std::map<std::string,std::string>::const_iterator
       it=options.begin();it!=options.end();it++)
  {
    std::string key = JetClustererBase::Lower(it->first);

    // radius
    if (key=="r")
    {
      float tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp<0) WARNING << "R must be positive. Using default value R = " 
                         << R_ << endmsg;
      else R_=tmp;
    }

    // ptmin
    else if (key=="ptmin")
    {
      float tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp<0) WARNING << "Ptmin must be positive. Using default value Ptmin = " 
                         << Ptmin_ << endmsg;
      else Ptmin_=tmp;
    }

    // p
    else if (key=="p")
    {
      float tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      p_=tmp;
    }

    // exclusive
    else if (key=="exclusive")
    {
      int tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp==1) Exclusive_=true;
      else if (tmp==0) Exclusive_=false;
      else
      {
        WARNING << "Exclusive must be equal to 0 or 1. Using default value Exclusive = " 
                << Exclusive_ << endmsg;
      }
    }

    // other
    else SettingsCommonPart(key,it->second);

  }

  // Creating Jet Definition
  if (JetAlgorithm_==fastjet::genkt_algorithm) 
      JetDefinition_ = fastjet::JetDefinition(JetAlgorithm_, R_, p_);
  else JetDefinition_ = fastjet::JetDefinition(JetAlgorithm_, R_);

  return true;
}


void JetClusteringStandard::PrintParam()
{
  if (JetAlgorithm_==0) INFO << "Algorithm : kt" << endmsg;
  else if (JetAlgorithm_==1) INFO << "Algorithm : Cambridge/Aachen" << endmsg;
  else if (JetAlgorithm_==2) INFO << "Algorithm : anti kt" << endmsg;
  else if (JetAlgorithm_==3) INFO << "Algorithm : generalized kt" << endmsg;
  INFO << "Parameters used :" << endmsg; 
  INFO << "R = " << R_ << "; p = " << p_ << "; Ptmin = " << Ptmin_ << endmsg;
}

std::string JetClusteringStandard::GetName()
{
  if (JetAlgorithm_==0) return "kt";
  else if (JetAlgorithm_==1) return "Cambridge/Aachen";
  else if (JetAlgorithm_==2) return "anti kt";
  else if (JetAlgorithm_==3) return "generalized kt";
  return "unknown";
}

std::string JetClusteringStandard::GetParameters()
{
  std::stringstream str;
  str << "R=" << R_ << " ; p=" << p_ << " ; PTmin=" << Ptmin_;
  return str.str();
}


#endif
