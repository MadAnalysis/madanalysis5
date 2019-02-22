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


#ifndef PDGTABLE_H
#define PDGTABLE_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/PdgDataFormat.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

// STL headers
#include <string>
#include <map>
#include <cmath>


namespace MA5
{

class PdgTable {
  public:
  PdgTable()
  {};

  ~PdgTable()
  {};

  PdgTable(const PdgTable& Table);

  PdgTable& operator=(const PdgTable& Table);

  const std::map<MAint32, PdgDataFormat>& Table() 
  { return Table_; }

  void Insert(const MAint32 Pdgid, const PdgDataFormat &p);

  void Print() const;

  const PdgDataFormat& operator[](const MAint32 Pdgid) const;

  private:

  std::map<MAint32, PdgDataFormat> Table_;
  PdgDataFormat empty_;
};

}

#endif
