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
 * @file ClusterAlgoBase.h
 * @brief Interface of the jet-clustering algorithms.
 */

#ifndef CLUSTER_ALGO_BASE_H
#define CLUSTER_ALGO_BASE_H


// STL headers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <algorithm>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"


namespace MA5
{

/** @brief Abstract base class of the jet-clustering algorithms (FastJet plugins). */
class ClusterAlgoBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  protected :

    /** @brief Name of the jet clustering algorithm. */
    std::string JetAlgorithm_;

    /** @brief Minimum transverse momentum of the jets. */
    MAfloat64 Ptmin_;

    /** @brief Is the clustering exclusive? */
    MAbool Exclusive_;

    /** @brief Exclusive identification (objects used as taus, electrons or photons are removed from the jets). */
    MAbool ExclusiveId_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /**
     * @brief Constructor.
     *
     * @param Algo name of the clustering algorithm.
     */
    ClusterAlgoBase(std::string Algo)
    {
      JetAlgorithm_=Algo;
      // Initializing common parameters
      Ptmin_       = 0.;
      Exclusive_   = false;
      ExclusiveId_ = false;
    }

    /** @brief Destructor. */
    virtual ~ClusterAlgoBase() {}

    /**
     * @brief Cluster the jets of an event (primary jet collection).
     *
     * @param mySample current sample.
     * @param myEvent event (the reconstructed jets are filled).
     * @param smearer detector smearer applied to the jets or their constituents.
     * @return false in case of error.
     */
    virtual MAbool Execute(SampleFormat& mySample, EventFormat& myEvent,
                           SmearerBase* smearer)=0;

    /**
     * @brief Cluster an additional jet collection.
     *
     * @param myEvent event.
     * @param JetID identifier of the jet collection.
     * @return false in case of error.
     */
    virtual MAbool Cluster(EventFormat& myEvent, std::string JetID)=0;

    /**
     * @brief Set a parameter of the algorithm.
     *
     * @param key parameter name.
     * @param value parameter value.
     * @return false if the parameter is unknown or invalid.
     */
    virtual MAbool SetParameter(const std::string& key, const std::string& value)=0;

    /**
     * @brief Initialise the algorithm.
     *
     * @return false in case of error.
     */
    virtual MAbool Initialize()=0;

    /**
     * @brief Convert a string to lower case.
     *
     * @param word string.
     * @return the lower-case string.
     */
    static std::string Lower(const std::string& word)
    {
      std::string result;
      std::transform(word.begin(), word.end(), 
                     std::back_inserter(result), 
                     (MAint32(*)(MAint32))std::tolower);
      return result;
    }

    /**
     * @brief Accessor to the name of the algorithm.
     *
     * @return the name.
     */
    virtual std::string GetName()=0;

    /** @brief Print the parameters of the algorithm. */
    virtual void PrintParam()=0;

    /**
     * @brief Accessor to the parameters of the algorithm.
     *
     * @return a printable summary of the parameters.
     */
    virtual std::string GetParameters()=0;


};

}

#endif
