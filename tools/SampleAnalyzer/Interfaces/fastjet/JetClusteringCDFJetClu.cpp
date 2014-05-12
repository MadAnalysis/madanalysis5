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


#include "SampleAnalyzer/Interfaces/fastjet/JetClusteringCDFJetClu.h"
#ifdef FASTJET_USE

using namespace MA5;

bool JetClusteringCDFJetClu::Initialize(const std::map<std::string,std::string>& options)
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

    // OverlapThreshold
    else if (key=="overlapthreshold")
    {
      float tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp<0) WARNING << "Overlap Threshold must be positive. "
                         << "Using default value Overlap Threshold = " 
                         << OverlapThreshold_ << endmsg;
      else OverlapThreshold_=tmp;
    }

    // SeedThreshold
    else if (key=="seedthreshold")
    {
      float tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp<0) WARNING << "Seed Threshold must be positive. Using default value Seed Threshold = " 
                         << SeedThreshold_ << endmsg;
      else SeedThreshold_=tmp;
    }

    // Iratch
    else if (key=="iratch")
    {
      Int_t tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp<0) WARNING << "Iratch must be positive. Using default value Iratch = " 
                         << Iratch_ << endmsg;
      else Iratch_=tmp;
    }

    // other
    else SettingsCommonPart(key,it->second);
  }

  // Creating plugin
  Plugin_ = new fastjet::CDFJetCluPlugin(R_, OverlapThreshold_, SeedThreshold_, Iratch_);

  // Creating jet definition
  JetDefinition_ = fastjet::JetDefinition(Plugin_);  

  return true;
}


/// Print Parameters
void JetClusteringCDFJetClu::PrintParam() 
{
  INFO << "Algorithm : CDF JetClu" << endmsg; 
  INFO << "Parameters used :" << endmsg; 
  INFO << "R = " << R_ 
       << "; Overlap Threshold = " << OverlapThreshold_ << "; Seed Threshold = " 
       << SeedThreshold_ << "; iratch = " << Iratch_ << endmsg;
}


std::string JetClusteringCDFJetClu::GetName()
{
  return "CDF JetClu";
}

std::string JetClusteringCDFJetClu::GetParameters()
{
  std::stringstream str;
  str << "R=" << R_  << " ; OverlapThreshold=" 
      << OverlapThreshold_ << " ; SeedThreshold.=" 
      << SeedThreshold_ << " ; iratch=" << Iratch_;
  return str.str();
}


#endif
