////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file PDGService.h
 * @brief Singleton giving access to the particle properties (charge, ...), available as PDG.
 */

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


/** @brief Shortcut to the PDGService singleton. */
#define PDG PDGService::GetInstance()

namespace MA5
{

/** @brief Singleton giving access to the particle properties read from `tools/SampleAnalyzer/particle.tbl`. */
class PDGService
{

  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
 protected:

  /** @brief Table of the particle properties. */
  PdgTable* Table_;  
  /** @brief PDG codes of the neutral particles. */
  std::set<MAint32> NeutralTable_;  
  /** @brief Unique instance. */
  static PDGService* service_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static PDGService* GetInstance()
  {
    if (service_==0) service_ = new PDGService;
    return service_;
  }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (service_!=0) delete service_;
    service_=0;
  }

  /**
   * @brief Is a particle charged?
   *
   * @param pdgid PDG code.
   * @return false for the neutral particles of the table (true for unknown codes).
   */
  MAbool IsCharged (MAint32 pdgid)
  {
    std::set<MAint32>::const_iterator it = NeutralTable_.find(pdgid);
    if(it==NeutralTable_.end()) return true;
    else return false; 
  }

  /**
   * @brief Get the electric charge of a particle.
   *
   * @param pdgid PDG code.
   * @param verbose warn if the code is unknown.
   * @return the charge in units of e/3 (0 if unknown).
   */
  MAint32 GetCharge (MAint32 pdgid, MAbool verbose = true)
  {
    return (*Table_).GetParticle(pdgid, verbose).Charge();
  }

  /**
   * @brief Get the electric charge of a particle.
   *
   * @param part particle.
   * @return the charge in units of e/3.
   */
  MAint32 GetCharge (const MCParticleFormat& part)
  {
    return GetCharge(part.pdgid());
  }

  /**
   * @brief Get the electric charge of a particle.
   *
   * @param part particle (0 allowed).
   * @return the charge in units of e/3 (0 for a null pointer).
   */
  MAint32 GetCharge (const MCParticleFormat* part)
  {
    if (part==0) return 0;
    return GetCharge(part->pdgid());
  }

 private:

  /** @brief Constructor: read `$MA5_BASE/tools/SampleAnalyzer/particle.tbl` (silently empty if missing). */
  PDGService()  
  {
    Table_ = new PdgTable;
    NeutralTable_.clear();


    std::string temp_string;
    std::istringstream curstring;

    // FIXME: std::getenv returns a null pointer if MA5_BASE is not set: constructing a std::string from it
    // is undefined behaviour (crash).
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
      MAint32 ID;
      std::string name;
      MAint32 charge;
      MAfloat32 mass; 
      MAfloat32 width; 
      MAfloat32 lifetime;

      // ID name   chg       mass    total width   lifetime
      //  1 d      -1      0.33000     0.00000   0.00000E+00
      //  in the table, the charge is in units of e+/3
      //  the total width is in GeV
      //  the lifetime is ctau in mm

      curstring >> ID >> name >> charge >> mass >> width >> lifetime;

      // the table gives c*tau in mm; it is stored in m
      PdgDataFormat particle(ID,name,mass,charge,width,lifetime/1000.);

      Table_->Insert(ID,particle);
      if (charge==0) NeutralTable_.insert(ID);
    }
  }

  /** @brief Destructor. */
  ~PDGService()
  {delete Table_;}
};

}

#endif
