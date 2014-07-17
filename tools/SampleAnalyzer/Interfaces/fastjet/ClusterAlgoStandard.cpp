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


//SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoStandard.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>


using namespace MA5;

bool ClusterAlgoStandard::SetParameter(const std::string& key, const std::string& value)
{
  // radius
  if (key=="r")
  {
    float tmp=0;
    std::stringstream str;
    str << value;
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
    str << value;
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
    str << value;
    str >> tmp;
    p_=tmp;
  }

  // exclusive
  else if (key=="exclusive")
  {
    int tmp=0;
    std::stringstream str;
    str << value;
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
  else return false;

  return true;
}


bool ClusterAlgoStandard::Initialize()
{ 
  // Creating Jet Definition
  if (JetAlgorithm_=="kt")
  {
      JetDefinition_ = new fastjet::JetDefinition(fastjet::kt_algorithm, R_);
  }
  else if (JetAlgorithm_=="antikt")
  {
      JetDefinition_ = new fastjet::JetDefinition(fastjet::antikt_algorithm, R_);
  }
  else if (JetAlgorithm_=="cambridge")
  {
      JetDefinition_ = new fastjet::JetDefinition(fastjet::cambridge_algorithm, R_);
  }
  else if (JetAlgorithm_== "genkt") 
  {
      JetDefinition_ = new fastjet::JetDefinition(fastjet::genkt_algorithm, R_, p_);
  }
  else
  {
    JetDefinition_ = 0;
    ERROR << "No FastJet algorithm found with the name " << JetAlgorithm_ << endmsg;
    return false;
  }

  return true;
}


void ClusterAlgoStandard::PrintParam()
{
  if (JetAlgorithm_=="kt") INFO << "Algorithm : kt" << endmsg;
  else if (JetAlgorithm_=="cambridge") INFO << "Algorithm : Cambridge/Aachen" << endmsg;
  else if (JetAlgorithm_=="antikt") INFO << "Algorithm : anti kt" << endmsg;
  else if (JetAlgorithm_=="genkt") INFO << "Algorithm : generalized kt" << endmsg;
  INFO << "Parameters used :" << endmsg; 
  INFO << "R = " << R_ << "; p = " << p_ << "; Ptmin = " << Ptmin_ << endmsg;
}

std::string ClusterAlgoStandard::GetName()
{
  if (JetAlgorithm_=="kt") return "kt";
  else if (JetAlgorithm_=="cambridge") return "Cambridge/Aachen";
  else if (JetAlgorithm_=="antikt") return "anti kt";
  else if (JetAlgorithm_=="genkt") return "generalized kt";
  return "unknown";
}

std::string ClusterAlgoStandard::GetParameters()
{
  std::stringstream str;
  str << "R=" << R_ << " ; p=" << p_ << " ; PTmin=" << Ptmin_;
  return str.str();
}


