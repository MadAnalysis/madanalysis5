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
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoSISCone.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>
#include <fastjet/SISConePlugin.hh>
typedef fastjet::JetDefinition::Plugin FastJetPlugin;



using namespace MA5;

bool ClusterAlgoSISCone::SetParameter(const std::string& key, const std::string& value)
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
    if (tmp<0) WARNING << "Overlap Threshold must be positive. Using default value Overlap Threshold = " 
                       << OverlapThreshold_ << endmsg;
    else OverlapThreshold_=tmp;
  }

  // NPassMax
  else if (key=="npassmax")
  {
    Int_t tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0) WARNING << "NPassMax must be positive. Using default value NPassMax = " 
                       << NPassMax_ << endmsg;
    else NPassMax_=tmp;
  }

  // Protojet_ptmin
  else if (key=="protojet_ptmin")
  {
    float tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0) WARNING << "Protojet Ptmin must be positive. Using default value Protojet_ptmin = " 
                       << Protojet_ptmin_ << endmsg;
    else Protojet_ptmin_=tmp;
  }

  // other
  else return false;
  return true;
}


bool ClusterAlgoSISCone::Initialize()
{
  // Creating plugin
  fastjet::JetDefinition::Plugin* Plugin_ = new fastjet::SISConePlugin(R_, OverlapThreshold_, NPassMax_, Protojet_ptmin_);

  // Creating jet definition
  JetDefinition_ = new fastjet::JetDefinition(Plugin_);  

  return true;
}

    /// Print parameters
void ClusterAlgoSISCone::PrintParam()
{
  INFO << "Algorithm : SIS Cone" << endmsg; 
  INFO << "Parameters used :" << endmsg; 
  INFO << "R = " << R_ << "; Overlap Threshold = " << OverlapThreshold_ 
       << "; N Pass Max = " << NPassMax_ << "; Minimal Pt for protojet = "
       << Protojet_ptmin_ << "; Ptmin = " << Ptmin_ << endmsg;
}

std::string ClusterAlgoSISCone::GetName()
{
  return "SIS Cone";
}

std::string ClusterAlgoSISCone::GetParameters()
{
  std::stringstream str;
  str << "R=" << R_ << " ; OverlapThreshold=" << OverlapThreshold_ 
      << " ; NPassMax=" << NPassMax_ << " ; PTmin_AlgoInputs="
      << Protojet_ptmin_ << " ; PTmin_AlgoOutputs=" << Ptmin_;
  return str.str();
}


