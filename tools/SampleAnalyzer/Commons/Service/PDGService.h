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


#ifndef PDGSERVICE_h
#define PDGSERVICE_h

// STL headers
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/PdgTable.h"
#include "SampleAnalyzer/Commons/DataFormat/MCParticleFormat.h"

#define PDG PDGService::GetInstance()

namespace MA5
{

class PDGService
{

  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
 protected:

  PdgTable* Table_;  
  std::set<Int_t> NeutralTable_;  
  static PDGService* service_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// GetInstance
  static PDGService* GetInstance()
  {
    if (service_==0) service_ = new PDGService;
    return service_;
  }

  /// Kill
  static void Kill()
  {
    if (service_!=0) delete service_;
    service_=0;
  }

  /// Is charged ?
  Bool_t IsCharged (Int_t pdgid)
  {
    std::set<Int_t>::const_iterator it = NeutralTable_.find(pdgid);
    if(it==NeutralTable_.end()) return true;
    else return false; 
  }

  /// Get charge
  Int_t GetCharge (Int_t pdgid)
  {
    return (*Table_)[pdgid].Charge();
  }

  /// Get charge
  Int_t GetCharge (const MCParticleFormat& part)
  {
    return GetCharge(part.pdgid());
  }

  /// Get charge
  Int_t GetCharge (const MCParticleFormat* part)
  {
    if (part==0) return 0;
    return GetCharge(part->pdgid());
  }

 private:

  /// Constructor
  PDGService()  
  {
    Table_ = new PdgTable;
    NeutralTable_.clear();


    std::string temp_string;
    std::istringstream curstring;

    std::string ma5dir = std::getenv("MA5_BASE");
    std::ifstream table ((ma5dir+"/tools/SampleAnalyzer/particle.tbl").c_str());

    if(!table.good()) 
    {
      //ERROR <<"PDG Table not found! exit." << endmsg;
      //exit(1);
      return;
    }

    // first three lines of the file are useless
    getline(table,temp_string);
    getline(table,temp_string);
    getline(table,temp_string);

    while (getline(table,temp_string)) 
    {
      curstring.clear(); // needed when using several times istringstream::str(string)
      curstring.str(temp_string);
      Int_t ID;
      std::string name;
      Int_t charge;
      Float_t mass; 
      Float_t width; 
      Float_t lifetime;

      // ID name   chg       mass    total width   lifetime
      //  1 d      -1      0.33000     0.00000   0.00000E+00
      //  in the table, the charge is in units of e+/3
      //  the total width is in GeV
      //  the lifetime is ctau in mm

      curstring >> ID >> name >> charge >> mass >> width >> lifetime;

      PdgDataFormat particle(ID,name,mass,charge,width,lifetime/1000.);

      Table_->Insert(ID,particle);
      if (charge==0) NeutralTable_.insert(ID);
    }
  }

  /// Destructor
  ~PDGService()
  {delete Table_;}
};

}

#endif
