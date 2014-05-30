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
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFMidpoint.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>
#include <fastjet/CDFMidPointPlugin.hh>
typedef fastjet::JetDefinition::Plugin FastJetPlugin;


using namespace MA5;

bool ClusterAlgoCDFMidpoint::SetParameter(const std::string& key, const std::string& value)
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

  // OverlapThreshold
  else if (key=="overlapthreshold")
  {
    float tmp=0;
    std::stringstream str;
    str << value;
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
    str << value;
    str >> tmp;
    if (tmp<0) WARNING << "Seed Threshold must be positive. Using default value Seed Threshold = " 
                       << SeedThreshold_ << endmsg;
    else SeedThreshold_=tmp;
  }

  // ConeArea
  else if (key=="conearea")
  {
    float tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0) WARNING << "Cone Area Fraction must be positive. Using default value ConeAreaFraction = " 
                       << ConeAreaFraction_ << endmsg;
    else ConeAreaFraction_=tmp;
  }

  // other
  else return false;

  return true;
}


bool ClusterAlgoCDFMidpoint::Initialize()
{
  // Creating plugin
  fastjet::JetDefinition::Plugin* Plugin_ = new fastjet::CDFMidPointPlugin(R_, OverlapThreshold_, 
                                           SeedThreshold_, ConeAreaFraction_);

  // Creating jet definition
  JetDefinition_ = new fastjet::JetDefinition(Plugin_);

  return true;
}


void ClusterAlgoCDFMidpoint::PrintParam()
{
  INFO << "Algorithm : CDF Midpoint" << endmsg; 
  INFO << "Parameters used :" << endmsg; INFO << "R = " << R_ 
       << "; Overlap Threshold = " << OverlapThreshold_ << "; Seed Threshold = " 
       << SeedThreshold_ << "; Cone Area Fraction = " << ConeAreaFraction_ << endmsg;
}

std::string ClusterAlgoCDFMidpoint::GetName()
{
  return "CDF Midpoint";
}

std::string ClusterAlgoCDFMidpoint::GetParameters()
{
  std::stringstream str;
  str << "R=" << R_  << " ; OverlapThreshold=" 
      << OverlapThreshold_ << " ; SeedThreshold=" 
      << SeedThreshold_ << " ; ConeAreaFraction=" << ConeAreaFraction_;
  return str.str();
}

