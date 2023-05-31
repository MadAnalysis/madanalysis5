////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


// STL headers
#include <iostream> 
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/PdgTable.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/ConvertService.h"


using namespace MA5;

PdgTable::PdgTable(const PdgTable& Table) 
{
  std::map<MAint32, PdgDataFormat>::const_iterator i;
  for(i = Table.Table_.begin(); i != Table.Table_.end(); i++)
  {
    Table_.insert(std::map<MAint32,PdgDataFormat>::value_type(i->first,i->second));
  }
}

PdgTable& PdgTable::operator=(const PdgTable& Table) 
{
  if(this == &Table) return *this;
  std::map<MAint32, PdgDataFormat>::const_iterator i;
  for(i = Table.Table_.begin(); i != Table.Table_.end(); i++)
  {
    Table_.insert(std::map<MAint32,PdgDataFormat>::value_type(i->first,i->second));
  }
  return *this;
}

void PdgTable::Insert(const MAint32 Pdgid, const PdgDataFormat &p) 
{
  Table_.insert(std::map<MAint32,PdgDataFormat>::value_type(Pdgid,p));
}

void PdgTable::Print() const 
{
  std::map<MAint32, PdgDataFormat>::const_iterator i;
  for(i = Table_.begin(); i != Table_.end(); i++)
  {
    INFO << "Name = " << /*set::setw(20)*/"" << std::left << i->second.Name() << endmsg; 
    INFO << "Pdg Id = "  << /*set::setw(10)*/"" << i->first << endmsg;
    INFO << "\t M="   << /*set::setw(10)*/"" << i->second.Mass() << "GeV/c^2" << endmsg;
    INFO << "\t Q="   << /*set::setw(6)*/""  << i->second.Charge() << "e+" << endmsg;
    INFO << "\t ctau=" << i->second.Ctau() << " m" << endmsg;
    if(i->second.IsInvisible()) INFO << "\t invisible" << endmsg;
  }
}

const PdgDataFormat& PdgTable::operator[](const MAint32 Pdgid) const 
{
  std::map<MAint32, PdgDataFormat>::const_iterator it = Table_.find(Pdgid);
  try
  {
    if (it==Table_.end()) throw EXCEPTION_WARNING("PDG ID not found ["+CONVERT->ToString(Pdgid)+"]","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
    return empty_;
  }    
   
  return it->second;
}


const PdgDataFormat& PdgTable::GetParticle(const MAint32 Pdgid, MAbool verbose) const
{
    std::map<MAint32, PdgDataFormat>::const_iterator it = Table_.find(Pdgid);
    try
    {
        if (it==Table_.end()) throw EXCEPTION_WARNING_VERBOSE("PDG ID not found ["+CONVERT->ToString(Pdgid)+"]","",0,verbose);
    }
    catch(const std::exception& e)
    {
        MANAGE_EXCEPTION(e);
        return empty_;
    }

    return it->second;
}