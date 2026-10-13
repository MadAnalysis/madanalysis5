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
 * @file IsolationBase.h
 * @brief Interface of the isolation tools.
 */

#ifndef ISOLATIONBASE_SERVICE_h
#define ISOLATIONBASE_SERVICE_h


// STL headers
#include <iostream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"


namespace MA5
{

/**
 * @brief Interface of the isolation tools.
 *
 * The absolute isolation (sumIsolation) is the scalar sum of the transverse momenta of
 * the objects in a cone of radius DR around the particle (objects below PTmin are
 * ignored); the relative isolation (relIsolation) is divided by the pT of the
 * particle.
 */
class IsolationBase
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  protected:

    /**
     * @brief Sum of the pT of the tracks in a cone around a lepton.
     *
     * @param part lepton.
     * @param tracks tracks.
     * @param DR cone radius.
     * @param PTmin minimum pT of the tracks.
     * @return the sum.
     */
    MAfloat64 sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecTrackFormat>& tracks,
                   const MAfloat64& DR,MAfloat64 PTmin) const; 

    /**
     * @brief Sum of the pT of the energy-flow objects in a cone around a lepton.
     *
     * @param part lepton.
     * @param towers objects.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects.
     * @return the sum.
     */
    MAfloat64 sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecParticleFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

    /**
     * @brief Sum of the pT of the calorimeter towers in a cone around a lepton.
     *
     * @param part lepton.
     * @param towers towers.
     * @param DR cone radius.
     * @param PTmin minimum pT of the towers.
     * @return the sum.
     */
    MAfloat64 sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecTowerFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

    /**
     * @brief Sum of the pT of the tracks in a cone around a photon.
     *
     * @param part photon.
     * @param tracks tracks.
     * @param DR cone radius.
     * @param PTmin minimum pT of the tracks.
     * @return the sum.
     */
    MAfloat64 sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecTrackFormat>& tracks,
                   const MAfloat64& DR,MAfloat64 PTmin) const; 

    /**
     * @brief Sum of the pT of the energy-flow objects in a cone around a photon.
     *
     * @param part photon.
     * @param towers objects.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects.
     * @return the sum.
     */
    MAfloat64 sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecParticleFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

    /**
     * @brief Sum of the pT of the calorimeter towers in a cone around a photon.
     *
     * @param part photon.
     * @param towers towers.
     * @param DR cone radius.
     * @param PTmin minimum pT of the towers.
     * @return the sum.
     */
    MAfloat64 sumPT(const RecPhotonFormat* part, 
                   const std::vector<RecTowerFormat>& towers,
                   const MAfloat64& DR,MAfloat64 PTmin) const;

  public:

    /** @brief Constructor. */
    IsolationBase() {}

    /** @brief Destructor. */
    virtual ~IsolationBase() {}


    // -------------------------------------------------------------
    //                Isolation of one particle
    // -------------------------------------------------------------

    /**
     * @brief Relative isolation of a lepton.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation.
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /**
     * @brief Relative isolation of a lepton.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation.
     */
    virtual MAfloat64 relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /**
     * @brief Absolute isolation of a lepton.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation.
     */
    virtual MAfloat64 sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /**
     * @brief Absolute isolation of a lepton.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation.
     */
    virtual MAfloat64 sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /**
     * @brief Relative isolation of a photon.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation.
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /**
     * @brief Relative isolation of a photon.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the relative isolation.
     */
    virtual MAfloat64 relIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /**
     * @brief Absolute isolation of a photon.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation.
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat& part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;

    /**
     * @brief Absolute isolation of a photon.
     *
     * @param part particle.
     * @param event reconstructed event.
     * @param DR cone radius.
     * @param PTmin minimum pT of the objects in the cone.
     * @return the absolute isolation.
     */
    virtual MAfloat64 sumIsolation(const RecPhotonFormat* part, const RecEventFormat* event, const MAfloat64& DR, MAfloat64 PTmin) const = 0;


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
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

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
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

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
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

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
                                                               const MAfloat64& threshold, const MAfloat64& DR, MAfloat64 PTmin=0.5) const = 0;

};

}

#endif
