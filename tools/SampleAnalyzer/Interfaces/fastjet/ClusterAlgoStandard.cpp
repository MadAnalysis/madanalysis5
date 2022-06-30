////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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


// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoStandard.h"

// FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>


using namespace MA5;

MAbool ClusterAlgoStandard::SetParameter(const std::string& key, const std::string& value)
{
  // radius
  if (key=="r")
  {
    MAfloat32 tmp=0;
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
    MAfloat32 tmp=0;
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
    MAfloat32 tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    p_=tmp;
  }

  // p
  else if (key=="collision")
  {
    std::string tmp;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp=="pp") collision_=true;
    else if (tmp=="ee") collision_=false;
    else WARNING << "The type of collisions in the events must be pp or ee. Using default value = " 
                       << collision_ << endmsg; 
  }

  // exclusive
  else if (key=="exclusive")
  {
    MAint32 tmp=0;
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

  // isolation radius for track
  else if (key.substr(0,9)=="isolation")
  {
    std::stringstream str(value);
    for (MAfloat64 tmp; str >> tmp;)
    {
        if (tmp>0. && key.substr(10) == "track.radius")    isocone_track_radius_.push_back(tmp);
        if (tmp>0. && key.substr(10) == "electron.radius") isocone_electron_radius_.push_back(tmp);
        if (tmp>0. && key.substr(10) == "muon.radius")     isocone_muon_radius_.push_back(tmp);
        if (tmp>0. && key.substr(10) == "photon.radius")   isocone_photon_radius_.push_back(tmp);
        if (str.peek() == ',' || str.peek() == ' ') str.ignore();
    }
  }

  // other
  else return false;

  return true;
}


MAbool ClusterAlgoStandard::Initialize()
{ 
  // Creating Jet Definition
  if (JetAlgorithm_=="kt")
  {
    if (collision_) JetDefinition_ = new fastjet::JetDefinition(fastjet::kt_algorithm, R_);
    else JetDefinition_ = new fastjet::JetDefinition(fastjet::ee_kt_algorithm, R_);
  }
  else if (JetAlgorithm_=="antikt")
  {
    if (collision_) JetDefinition_ = new fastjet::JetDefinition(fastjet::antikt_algorithm, R_);
    else JetDefinition_ = new fastjet::JetDefinition(fastjet::ee_genkt_algorithm, R_, -1.);
  }
  else if (JetAlgorithm_=="cambridge")
  {
    if (collision_) JetDefinition_ = new fastjet::JetDefinition(fastjet::cambridge_algorithm, R_);
    else JetDefinition_ = new fastjet::JetDefinition(fastjet::ee_genkt_algorithm, R_, 0.);
  }
  else if (JetAlgorithm_== "genkt") 
  {
    if (collision_) JetDefinition_ = new fastjet::JetDefinition(fastjet::genkt_algorithm, R_, p_);
    else JetDefinition_ = new fastjet::JetDefinition(fastjet::ee_genkt_algorithm, R_, p_);
  }
  else
  {
    JetDefinition_ = 0;
    ERROR << "No FastJet algorithm found with the name " << JetAlgorithm_ << endmsg;
    return false;
  }

  return true;
}


std::string ClusterAlgoStandard::GetCollisionType() const
{
  if (collision_) return "pp";
  else return "ee";
}

void ClusterAlgoStandard::PrintParam()
{
  if (JetAlgorithm_=="kt") INFO << "Algorithm : kt" << endmsg;
  else if (JetAlgorithm_=="cambridge") INFO << "Algorithm : Cambridge/Aachen" << endmsg;
  else if (JetAlgorithm_=="antikt") INFO << "Algorithm : anti kt" << endmsg;
  else if (JetAlgorithm_=="genkt") INFO << "Algorithm : generalized kt" << endmsg;
  INFO << "Parameters used :" << endmsg; 
  INFO << "R = " << R_ << "; p = " << p_ << "; Ptmin = " << Ptmin_ << "; collision type =" << collision_ << endmsg;
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
  str << "R=" << R_ << " ; p=" << p_ << " ; PTmin=" << Ptmin_ << " ; collision type =" << collision_;
  return str.str();
}


