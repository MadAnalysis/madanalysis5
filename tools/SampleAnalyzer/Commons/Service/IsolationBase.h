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


#ifndef ISOLATIONBASE_SERVICE_h
#define ISOLATIONBASE_SERVICE_h


// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"


namespace MA5
{

class IsolationBase
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  protected:

    MAfloat64 sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecTrackFormat>& tracks,
                   const MAfloat64& DR,MAfloat64 PTmin) const; 

    MAfloat64 sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecParticleFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

    MAfloat64 sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecTowerFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

    MAfloat64 sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecTrackFormat>& tracks,
                   const MAfloat64& DR,MAfloat64 PTmin) const; 

    MAfloat64 sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecParticleFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

    MAfloat64 sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecTowerFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

  public:

    /// Constructor
    IsolationBase() {}

    /// Destructor
    virtual ~IsolationBase() {}


    // -------------------------------------------------------------
    //                Isolation of one particle
    // -------------------------------------------------------------

    /// Methods for leptons
    virtual MAfloat64 relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    virtual MAfloat64 relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    virtual MAfloat64 sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    virtual MAfloat64 sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /// Methods for photons
    virtual MAfloat64 relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    virtual MAfloat64 relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    virtual MAfloat64 sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    virtual MAfloat64 sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;


    // -------------------------------------------------------------
    //                Isolation of one collection
    // -------------------------------------------------------------

    virtual std::vector<const RecLeptonFormat*> getRelIsolated(const std::vector<RecLeptonFormat>& leptons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

    virtual std::vector<const RecLeptonFormat*> getRelIsolated(const std::vector<const RecLeptonFormat*>& leptons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

    virtual std::vector<const RecPhotonFormat*> getRelIsolated(const std::vector<RecPhotonFormat>& photons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

    virtual std::vector<const RecPhotonFormat*> getRelIsolated(const std::vector<const RecPhotonFormat*>& photons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

};

}

#endif
