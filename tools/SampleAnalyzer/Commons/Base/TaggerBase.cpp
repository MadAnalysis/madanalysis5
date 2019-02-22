////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
#include "SampleAnalyzer/Commons/Base/TaggerBase.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/ConvertService.h"


using namespace MA5;

MAbool TaggerBase::IsLast(MCParticleFormat* part, EventFormat& myEvent)
{
  for (MAuint32 i=0; i<part->daughters().size(); i++)
  {
    if (part->daughters()[i]->pdgid()==part->pdgid()) return false;
  }
  return true;
}




MAbool TaggerBase::SetParameter(const std::string& key, 
                              const std::string& value,
                              std::string header)
{
  // Method
  if (key=="method")
  {
    MAint32 tmp=0;
    std::stringstream str,str2;
    std::string Method_str;
    str << value;
    str >> tmp;
    str2 << Method_;
    str2 >> Method_str;
    try
    {
      if (tmp<0 || tmp>3) throw EXCEPTION_WARNING("Available methods are 1, 2 and 3. Using the default value = "+Method_str,"",0);
      Method_=tmp;
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
  }

  // deltaR
  else if (key=="matching_dr")
  {
    MAfloat32 tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    try
    {
      if (tmp<0) throw EXCEPTION_WARNING("DeltaRmax must be a positive value. Using the default value = "+CONVERT->ToString(DeltaRmax_),"",0);
      DeltaRmax_=tmp;
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
  }

  // exclusive
  else if (key=="exclusive")
  {
    MAint32 tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    try
    {
      if (tmp<0) throw EXCEPTION_WARNING("Exclusive_ must be equal to 0 or 1.value. Using the default value = "+CONVERT->ToString(Exclusive_),"",0);
      Exclusive_=(tmp==1);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
  }

  // efficiency
  else if (key=="efficiency")
  {
    MAfloat32 tmp=0;
    std::stringstream str;
    str << value;
    str >> tmp;
    try
    {
      if (tmp<0) throw EXCEPTION_WARNING("Efficiency must be a positive value. Using the default value = "+CONVERT->ToString(Efficiency_),"",0);
      if (tmp>1) throw EXCEPTION_WARNING("Efficiency cannot be greater than 1. Using the default value = "+CONVERT->ToString(Efficiency_),"",0); 
      Efficiency_=tmp;
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
    if (Efficiency_!=1) doEfficiency_=true; else doEfficiency_=false;
  }

  // Other
  else return false;

  return true;

}
