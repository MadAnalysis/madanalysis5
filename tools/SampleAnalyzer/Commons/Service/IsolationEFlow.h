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


#ifndef ISOLATIONEFLOW_SERVICE_h
#define ISOLATIONEFLOW_SERVICE_h


// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/IsolationBase.h"


namespace MA5
{

class IsolationEFlow : public IsolationBase
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:


  public:

    /// Constructor
    IsolationEFlow() {}

    /// Destructor
    virtual ~IsolationEFlow() {}

    /// Isolation component
    enum ComponentType {ALL_COMPONENTS,TRACK_COMPONENT,PHOTON_COMPONENT,NEUTRAL_COMPONENT};

    virtual MAfloat64 relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return relIsolation(&part, event, DR, PTmin, type); }

    virtual MAfloat64 relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin,type)/part->pt();
    }

    virtual MAfloat64 sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return sumIsolation(&part, event, DR, PTmin,type); }

    virtual MAfloat64 sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      if (type==TRACK_COMPONENT || type==ALL_COMPONENTS)
          sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      if (type==PHOTON_COMPONENT || type==ALL_COMPONENTS)
          sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      if (type==NEUTRAL_COMPONENT || type==ALL_COMPONENTS)
          sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      sum -= part->pt();
      return sum;
    }

    virtual MAfloat64 relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return relIsolation(&part, event, DR, PTmin, type); }

    virtual MAfloat64 relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin,type)/part->pt();
    }

    virtual MAfloat64 sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return sumIsolation(&part, event, DR, PTmin, type); }

    virtual MAfloat64 sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      if (type==TRACK_COMPONENT || type==ALL_COMPONENTS)
          sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      if (type==PHOTON_COMPONENT /*|| type==ALL_COMPONENTS*/)
          sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      if (type==NEUTRAL_COMPONENT || type==ALL_COMPONENTS)
          sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      return sum;
    }

    virtual MAfloat64 relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return relIsolation(&part, event, DR, PTmin); }

    virtual MAfloat64 relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin)/part->pt();
    }

    virtual MAfloat64 sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return sumIsolation(&part, event, DR, PTmin); }

    virtual MAfloat64 sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      return sum;
    }

    virtual MAfloat64 relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return relIsolation(&part, event, DR, PTmin); }

    virtual MAfloat64 relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin)/part->pt();
    }

    virtual MAfloat64 sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return sumIsolation(&part, event, DR, PTmin); }

    virtual MAfloat64 sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      return sum;
    }

    // -------------------------------------------------------------
    //                Isolation of one collection
    // -------------------------------------------------------------

    virtual std::vector<const RecLeptonFormat*> getRelIsolated(const std::vector<RecLeptonFormat>& leptons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated(leptons.size());
      for (MAuint32 i=0;i<leptons.size();i++) isolated[i]=&(leptons[i]);
      return getRelIsolated(isolated, event, threshold, DR, PTmin);
    }

    virtual std::vector<const RecLeptonFormat*> getRelIsolated(const std::vector<const RecLeptonFormat*>& leptons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated;
      for (MAuint32 i=0;i<leptons.size();i++)
      {
        if (relIsolation(leptons[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(leptons[i]);
      }
      return isolated;
    }


    virtual std::vector<const RecPhotonFormat*> getRelIsolated(const std::vector<RecPhotonFormat>& photons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      std::vector<const RecPhotonFormat*> isolated(photons.size());
      for (MAuint32 i=0;i<photons.size();i++) isolated[i]=&(photons[i]);
      return getRelIsolated(isolated, event, threshold, DR, PTmin);
    }

    virtual std::vector<const RecPhotonFormat*> getRelIsolated(const std::vector<const RecPhotonFormat*>& photons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      std::vector<const RecPhotonFormat*> isolated;
      for (MAuint32 i=0;i<photons.size();i++)
      {
        if (relIsolation(photons[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(photons[i]);
      }
      return isolated;
    }



};

}

#endif
