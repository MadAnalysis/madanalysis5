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


#include "SampleAnalyzer/Commons/Base/TaggerBase.h"
using namespace MA5;

Bool_t TaggerBase::IsLast(MCParticleFormat* part, EventFormat& myEvent)
{
  for (unsigned int i=0; i<part->daughters().size(); i++)
  {
    if (part->daughters()[i]->pdgid()==part->pdgid()) return false;
  }
  return true;
}




bool TaggerBase::SetParameter(const std::string& key, 
                              const std::string& value,
                              std::string header)
{
  // Method
  if (key=="method")
  {
    Int_t tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0 || tmp>3)  WARNING << "Available methods are 1, 2 and 3. Using the default value = " 
                                 << Method_ << endmsg;
    else Method_=tmp;
  }

  // deltaR
  else if (key=="matching_dr")
  {
    Float_t tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0)  WARNING << "DeltaRmax must be a positive value. Using the default value = " 
                                 << DeltaRmax_ << endmsg;
    else DeltaRmax_=tmp;
  }

  // exclusive
  else if (key=="exclusive")
  {
    Int_t tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0)  WARNING << "DeltaRmax must be equal to 0 or 1. Using the default value = " 
                        << static_cast<Int_t>(Exclusive_) << endmsg;
    else Exclusive_=(tmp==1);
  }

  // efficiency
  else if (key=="efficiency")
  {
    Float_t tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    if (tmp<0)
    {
      WARNING << "Efficiency must be a positive value. "
              << "Using the default value = " 
              << Efficiency_ << endmsg;
    }
    else if (tmp>1) 
    {
      WARNING << "Efficiency cannot be greater than 1. "
              << "Using the default value = " 
              << Efficiency_ << endmsg;
    }
    else Efficiency_=tmp;
    if (Efficiency_!=1) doEfficiency_=true; else doEfficiency_=false;
  }

  // Other
  else return false;

  return true;

}
