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


#include "SampleAnalyzer/Detector/DetectorDelphes.h"

#ifdef DELPHES_USE

using namespace MA5;

bool DetectorDelphes::Initialize(const std::string& configFile, const std::map<std::string,std::string>& options)
{ 
  configFile_ = configFile;

  for (std::map<std::string,std::string>::const_iterator
       it=options.begin();it!=options.end();it++)
  {
    std::string key = DetectorBase::Lower(it->first);

    /*
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
      }*/
  }

  return true;
}


void DetectorDelphes::PrintParam()
{
  INFO << "" << endmsg; 
}


std::string DetectorDelphes::GetParameters()
{
  std::stringstream str;
  return str.str();
}


/// Jet clustering
bool DetectorDelphes::Execute(SampleFormat& mySample, EventFormat& myEvent)
{
  return true;
}


#endif
