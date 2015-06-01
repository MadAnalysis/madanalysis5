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

#ifndef ISOLATIONBASE_SERVICE_h
#define ISOLATIONBASE_SERVICE_h

// STL headers

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

    Double_t sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecTrackFormat>& tracks,
                   const double& DR,double PTmin) const; 

    Double_t sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecParticleFormat>& towers,
                   const double& DR,double PTmin) const;

    Double_t sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecTowerFormat>& towers,
                   const double& DR,double PTmin) const;

    Double_t sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecTrackFormat>& tracks,
                   const double& DR,double PTmin) const; 

    Double_t sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecParticleFormat>& towers,
                   const double& DR,double PTmin) const;

    Double_t sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecTowerFormat>& towers,
                   const double& DR,double PTmin) const;

  public:

    /// Constructor
    IsolationBase() {}

    /// Destructor
    virtual ~IsolationBase() {}


    /// Methods for leptons
    virtual Double_t relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;

    virtual Double_t relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;

    virtual Double_t sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;

    virtual Double_t sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;

    /// Methods for photons
    virtual Double_t relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;

    virtual Double_t relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;

    virtual Double_t sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;

    virtual Double_t sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const double& DR, double PTmin) const = 0;


    virtual std::vector<const RecLeptonFormat*> getRelIsolatedMuons(const RecEventFormat* event, 
                                                            const double& threshold, 
                                                            const double& DR, double PTmin) const = 0;

    virtual std::vector<const RecLeptonFormat*> getRelIsolatedElectrons(const RecEventFormat* event, 
                                                                        const double& threshold,
                                                                        const double& DR, double PTmin) const = 0;

    virtual std::vector<const RecPhotonFormat*> getRelIsolatedPhotons(const RecEventFormat* event, 
                                                                      const double& threshold, 
                                                                      const double& DR, double PTmin) const = 0;

    /*
    std::vector<const RecJetFormat*> cleanJets(const RecEventFormat* event,
                                               const std::vector<const RecLeptonFormat*>& muons, 
                                               const std::vector<const RecLeptonFormat*>& electrons,
                                               const double& threshold) const
    {
      std::vector<const RecJetFormat*> cleaned;
      for (unsigned int i=0;i<event->jets().size();i++)
      {
        const RecJetFormat& jet = event->jets()[i];
        bool isolated = true;
  
        for (unsigned int j=0;j<muons.size();j++)
        {
          const RecLeptonFormat* muon = muons[j];
          if (muon->momentum().DeltaR(jet.momentum())<threshold) isolated=false;
        }
        if (!isolated) continue;

        for (unsigned int j=0;j<electrons.size();j++)
        {
          const RecLeptonFormat* elec = electrons[j];
          if (elec->momentum().DeltaR(jet.momentum())<threshold) isolated=false;
        }
        if (!isolated) continue;
        
        cleaned.push_back(&jet);
      }
      return cleaned;
    }
    */

};

}

#endif
