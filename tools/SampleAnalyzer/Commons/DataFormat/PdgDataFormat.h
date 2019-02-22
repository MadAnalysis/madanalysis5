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


#ifndef PDGDATAFORMAT_H
#define PDGDATAFORMAT_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// STL headers
#include <string>
#include <cmath>


namespace MA5
{

class PdgDataFormat {
  public:
  PdgDataFormat(): Pdgid_(-999), Mass_(0), Charge_(0), GammaTot_(0), Ctau_(0), Name_("Unknown"), IsInvisible_(false) {};
  PdgDataFormat(const MAint32 Pdgid, const std::string& Name, const MAfloat32 m, const MAint32 q, const MAfloat32 Gamma, const MAfloat32 ctau);
  PdgDataFormat(const PdgDataFormat& p);
  PdgDataFormat& operator=(const PdgDataFormat& p);
  ~PdgDataFormat() {};
  MAint32 Pdgid() const {return Pdgid_;};
  MAfloat32 Mass() const {return Mass_;};
  MAint32 Charge() const {return Charge_;};
  MAfloat32 GammaTot() const {return GammaTot_;};
  MAfloat64 Ctau() const {return Ctau_;};
  MAbool IsInvisible() const {return IsInvisible_;};
  std::string Name() const {return Name_;};

  private:
  MAint32 Pdgid_;
  MAfloat32 Mass_;        // GeV
  MAint32 Charge_;      // in e+/3
  MAfloat32 GammaTot_;   // GeV
  MAfloat64 Ctau_;       // in m
  std::string Name_;
  MAbool IsInvisible_;
};

}

#endif
