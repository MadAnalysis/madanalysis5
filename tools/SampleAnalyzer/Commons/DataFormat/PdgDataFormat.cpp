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


#include "SampleAnalyzer/Commons/DataFormat/PdgDataFormat.h" 
using namespace MA5;


PdgDataFormat::PdgDataFormat(const Int_t Pdgid, const std::string& Name, const Float_t m, const Int_t q, const Float_t Gamma, const Float_t ctau) :
  Pdgid_(Pdgid), Mass_(m), Charge_(q), GammaTot_(Gamma), Ctau_(ctau), Name_(Name) 
{
  if( (fabs(Pdgid) == 12)       || (fabs(Pdgid) == 14)       || (fabs(Pdgid) == 16)      || 
      (fabs(Pdgid) == 1000022 ) || (fabs(Pdgid) == 1000023 ) || (fabs(Pdgid) == 1000025) || 
      (fabs(Pdgid) == 1000035 ) || (fabs(Pdgid) == 1000045 ) ) IsInvisible_ = true;
  else IsInvisible_ = false;
}


PdgDataFormat::PdgDataFormat(const PdgDataFormat& p) :
  Pdgid_(p.Pdgid_), Mass_(p.Mass_), Charge_(p.Charge_), GammaTot_(p.GammaTot_), Ctau_(p.Ctau_), Name_(p.Name_), IsInvisible_(p.IsInvisible_) {}

PdgDataFormat& PdgDataFormat::operator=(const PdgDataFormat& p) 
{
  if(this == &p) return *this;
  Pdgid_ = p.Pdgid_;
  Name_= p.Name_;
  Mass_=p.Mass_; 
  Charge_=p.Charge_;
  GammaTot_=p.GammaTot_;
  Ctau_ = p.Ctau_;
  IsInvisible_ = p.IsInvisible_;
  return *this;
}
