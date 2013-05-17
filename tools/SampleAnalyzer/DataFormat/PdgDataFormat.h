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


#ifndef PDGDATAFORMAT_H
#define PDGDATAFORMAT_H

#include <string>
#include <cmath>
#include <Rtypes.h>


namespace MA5
{

class PdgDataFormat {
  public:
	PdgDataFormat(): Pdgid_(-999), Mass_(0), Charge_(0), GammaTot_(0), Ctau_(0), Name_("Unknown"), IsInvisible_(false) {};
	PdgDataFormat(const Int_t Pdgid, const std::string& Name, const Float_t m, const Int_t q, const Float_t Gamma, const Float_t ctau);
	PdgDataFormat(const PdgDataFormat& p);
	PdgDataFormat& operator=(const PdgDataFormat& p);
	~PdgDataFormat() {};
	Int_t Pdgid() const {return Pdgid_;};
	Float_t Mass() const {return Mass_;};
	Int_t Charge() const {return Charge_;};
	Float_t GammaTot() const {return GammaTot_;};
	Double_t Ctau() const {return Ctau_;};
	Bool_t IsInvisible() const {return IsInvisible_;};
        std::string Name() const {return Name_;};

  private:
	Int_t Pdgid_;
	Float_t Mass_;        // GeV
	Int_t Charge_;      // in e+/3
	Float_t GammaTot_;   // GeV
	Double_t Ctau_;       // in m
	std::string Name_;
	Bool_t IsInvisible_;
};

}

#endif
