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
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoGridJet.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>
#include <fastjet/GridJetPlugin.hh>
typedef fastjet::JetDefinition::Plugin FastJetPlugin;


using namespace MA5;

bool ClusterAlgoGridJet::SetParameter(const std::string& key, const std::string& value)
{ 
  // radius
  if (key=="ymax")
  {
    float tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0) WARNING << "Ymax must be positive. Using default value Ymax = " 
                       << Ymax_ << endmsg;
    else Ymax_=tmp;
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

  // Requested Grid Spacing
  else if (key=="requestedgridspacing")
  {
    float tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0) WARNING << "RequestedGridSpacing must be positive. "
                       << "Using default value RequestedGridSpacing = " 
                       << RequestedGridSpacing_ << endmsg;
    else RequestedGridSpacing_=tmp;
  }

  // other
  else return false;
  return true;
}


bool ClusterAlgoGridJet::Initialize()
{ 
  // Creating Plugin
  fastjet::JetDefinition::Plugin* Plugin_ = new fastjet::GridJetPlugin(Ymax_, RequestedGridSpacing_);

  // Creating jet definition
  JetDefinition_ = new fastjet::JetDefinition(Plugin_);  

  return true;
}


/// Print parameters
void ClusterAlgoGridJet::PrintParam() 
{
  INFO << "Algorithm : Grid Jet" << endmsg; 
  INFO << "Parameters used :" << endmsg; 
  INFO << "Ymax = " << Ymax_ 
       << "; RequestedGridSpacing = " << RequestedGridSpacing_ 
       << "; Ptmin = " << Ptmin_ << endmsg;
}

std::string ClusterAlgoGridJet::GetName()
{
  return "Grid Jet";
}

std::string ClusterAlgoGridJet::GetParameters()
{
  std::stringstream str;
  str << "Ymax=" << Ymax_ 
      << " ; GridSpacing=" << RequestedGridSpacing_ 
      << " ; PTmin=" << Ptmin_;
  return str.str();
}


