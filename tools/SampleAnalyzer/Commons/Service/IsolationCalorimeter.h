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
 * @file IsolationCalorimeter.h
 * @brief Isolation based on the calorimeter towers.
 */

#ifndef ISOLATIONCALORIMETER_SERVICE_h
#define ISOLATIONCALORIMETER_SERVICE_h


// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/IsolationBase.h"


namespace MA5
{

/**
 * @brief Isolation based on the calorimeter towers.
 *
 * The pT of the particle is subtracted from the sum for photons and electrons
 * (muons deposit little energy in the calorimeter).
 */
// NOTE: private inheritance (no 'public'): the object cannot be used through an IsolationBase pointer.
class IsolationCalorimeter : IsolationBase
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:


  public:

    /** @brief Constructor. */
    IsolationCalorimeter() {}

    /** @brief Destructor. */
    virtual ~IsolationCalorimeter() {}

    // -------------------------------------------------------------
    //                Isolation of one particle
    // -------------------------------------------------------------

    /**
     * @brief Relative isolation of a lepton (sumIsolation / pT).
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    { return relIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Relative isolation of a lepton (sumIsolation / pT).
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin)/part->pt();
    }

    /**
     * @brief Absolute isolation of a lepton.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    { return sumIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Absolute isolation of a lepton.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      sum += sumPT(part,event->towers(),DR,PTmin);
      if (part->isElectron()) sum -= part->pt();
      return sum;
    }

    /**
     * @brief Relative isolation of a photon (sumIsolation / pT).
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    { return relIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Relative isolation of a photon (sumIsolation / pT).
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation (0 for null pointers, 999 for pT = 0).
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return (sumIsolation(part,event,DR,PTmin)/part->pt());
    }

    /**
     * @brief Absolute isolation of a photon.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    { return sumIsolation(&part, event, DR, PTmin); }

    /**
     * @brief Absolute isolation of a photon.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation (0 for null pointers).
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      MAfloat64 sum=0.;
      sum += sumPT(part,event->towers(),DR,PTmin);
      sum -= part->pt();
      return sum;
    }

    // -------------------------------------------------------------
    //                Isolation of one collection
    // -------------------------------------------------------------

    /**
     * @brief Select the isolated leptons.
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
     * @brief Select the isolated leptons.
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
     * @brief Select the isolated photons.
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
     * @brief Select the isolated photons.
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
