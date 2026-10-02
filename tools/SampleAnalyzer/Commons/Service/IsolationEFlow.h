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
 * @file IsolationEFlow.h
 * @brief Isolation based on the energy-flow objects (tracks, photons, neutral hadrons).
 */

#ifndef ISOLATIONEFLOW_SERVICE_h
#define ISOLATIONEFLOW_SERVICE_h


// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/IsolationBase.h"


namespace MA5
{

/**
 * @brief Isolation based on the energy-flow objects (Delphes).
 *
 * The versions with a ComponentType argument restrict the sum to one component.
 */
class IsolationEFlow : public IsolationBase
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:


  public:

    /** @brief Constructor. */
    IsolationEFlow() {}

    /** @brief Destructor. */
    virtual ~IsolationEFlow() {}

    /** @brief Energy-flow components entering the isolation sum. */
    enum ComponentType {ALL_COMPONENTS,TRACK_COMPONENT,PHOTON_COMPONENT,NEUTRAL_COMPONENT};

    /**
     * @brief Relative isolation of a lepton (sumIsolation / pT), restricted to some components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return relIsolation(&part, event, DR, PTmin, type); }

    /**
     * @brief Relative isolation of a lepton (sumIsolation / pT), restricted to some components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin,type)/part->pt();
    }

    /**
     * @brief Absolute isolation of a lepton, restricted to some components (the lepton pT is subtracted).
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return sumIsolation(&part, event, DR, PTmin,type); }

    /**
     * @brief Absolute isolation of a lepton, restricted to some components (the lepton pT is subtracted).
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the absolute isolation (0 for null pointers).
     */
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

    /**
     * @brief Relative isolation of a photon (sumIsolation / pT), restricted to some components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return relIsolation(&part, event, DR, PTmin, type); }

    /**
     * @brief Relative isolation of a photon (sumIsolation / pT), restricted to some components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin,type)/part->pt();
    }

    /**
     * @brief Absolute isolation of a photon, restricted to some components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    { return sumIsolation(&part, event, DR, PTmin, type); }

    /**
     * @brief Absolute isolation of a photon, restricted to some components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @param type component(s) entering the sum.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin, ComponentType type) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      if (type==TRACK_COMPONENT || type==ALL_COMPONENTS)
          sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      // NOTE: the photon component is not included for ALL_COMPONENTS.
      if (type==PHOTON_COMPONENT /*|| type==ALL_COMPONENTS*/)
          sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      if (type==NEUTRAL_COMPONENT || type==ALL_COMPONENTS)
          sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      return sum;
    }

    /**
     * @brief Relative isolation of a lepton (sumIsolation / pT) with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return relIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Relative isolation of a lepton (sumIsolation / pT) with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin)/part->pt();
    }

    /**
     * @brief Absolute isolation of a lepton with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return sumIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Absolute isolation of a lepton with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      // NOTE: unlike the ComponentType version, the lepton pT is not subtracted here.
      return sum;
    }

    /**
     * @brief Relative isolation of a photon (sumIsolation / pT) with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return relIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Relative isolation of a photon (sumIsolation / pT) with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin)/part->pt();
    }

    /**
     * @brief Absolute isolation of a photon with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    { return sumIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Absolute isolation of a photon with all the components.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      // NOTE: unlike the ComponentType version, the photon component is included here.
      sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      return sum;
    }

    // -------------------------------------------------------------
    //                Isolation of one collection
    // -------------------------------------------------------------

    /**
     * @brief Select the isolated leptons (all the components).
     *
     * @param leptons collection.
     * @param event reconstructed event.
     * @param threshold maximum relative isolation.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the leptons with a relative isolation below the threshold.
     */
    virtual std::vector<const RecLeptonFormat*> getRelIsolated(const std::vector<RecLeptonFormat>& leptons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated(leptons.size());
      for (MAuint32 i=0;i<leptons.size();i++) isolated[i]=&(leptons[i]);
      return getRelIsolated(isolated, event, threshold, DR, PTmin);
    }

    /**
     * @brief Select the isolated leptons (all the components).
     *
     * @param leptons collection.
     * @param event reconstructed event.
     * @param threshold maximum relative isolation.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the leptons with a relative isolation below the threshold.
     */
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


    /**
     * @brief Select the isolated photons (all the components).
     *
     * @param photons collection.
     * @param event reconstructed event.
     * @param threshold maximum relative isolation.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the photons with a relative isolation below the threshold.
     */
    virtual std::vector<const RecPhotonFormat*> getRelIsolated(const std::vector<RecPhotonFormat>& photons, 
                                                               const RecEventFormat* event, 
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      std::vector<const RecPhotonFormat*> isolated(photons.size());
      for (MAuint32 i=0;i<photons.size();i++) isolated[i]=&(photons[i]);
      return getRelIsolated(isolated, event, threshold, DR, PTmin);
    }

    /**
     * @brief Select the isolated photons (all the components).
     *
     * @param photons collection.
     * @param event reconstructed event.
     * @param threshold maximum relative isolation.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the photons with a relative isolation below the threshold.
     */
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
